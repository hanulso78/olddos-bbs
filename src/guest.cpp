#include "main.h"

// ------------------------------------------------------------------
// 회원 가입 (로그인 화면에서 guest)
//   1/6 아이디 -> 2/6 비밀번호 (가린 채 두 번) -> 3/6 닉네임 -> 4/6 생일 -> 5/6 이메일 -> 6/6 성별
//   -> 넣은 것을 모아 보여 주고 번호로 고치거나 Y 로 가입. 어디서나 /x 면 그만둔다.
//   가입하면 운영자 이름으로 환영 쪽지, 접속해 있는 운영자에게 알림.
// ------------------------------------------------------------------

struct termio sys_term;

char tty[10];

#define G_W		"\033[=15F"
#define G_Y		"\033[=14F"
#define G_C		"\033[=11F"
#define G_G		"\033[=7F"
#define G_R		"\033[=12F"
#define G_GR	"\033[=10F"

#define ID_MIN		5
#define ID_MAX		20		// 로그인 화면의 아이디 칸과 같게
#define NICK_MAX	10		// 게시판 목록의 작성자 칸

static std::string user_id, user_passwd, nick_name, birthday, email_address, sex;

void raw_mode(void)
{
    struct termio tbuf;
    ioctl(0, TCGETA, &tbuf);
    tbuf.c_cc[4] = 1;
    tbuf.c_cc[5] = 0;
    tbuf.c_iflag = 0;
    tbuf.c_iflag |= IXON;
    tbuf.c_iflag |= IXANY;
    tbuf.c_oflag = 0;
    tbuf.c_oflag &= ~OPOST;
    tbuf.c_lflag &= ~(ICANON | ISIG | ECHO);
    tbuf.c_cflag &= ~PARENB;
    tbuf.c_cflag &= ~CSIZE;
    tbuf.c_cflag |= CS8;
    ioctl(0, TCSETAF, &tbuf);
}

/* 프로그램 종료 루틴 */
int host_close (void)
{
	// ctime 이 만든 (로그인 전) 접속자 파일
	char buf[1024];
	snprintf(buf, sizeof(buf), "%s/tmp/%s.tty", getenv("HANULSO"), tty);
	unlink(buf);

	database::close();

    ioctl(0, TCSETAF, &sys_term);
    exit(1);
}

void return_login ()
{
	database::close();
	ioctl(0, TCSETAF, &sys_term);
	execl("bin/main","main", tty, (char *)0);
	exit(1);
}

static void bad(const char *m)
{
	printf("\r\n    " G_R "%s" G_W, m);
}

// 한 줄 받기. /x 면 로그인 화면으로. echo 3 이면 * 로 가린다
static std::string ask(int step, const char *title, const char *rule, int len, bool han, int echo = 1)
{
	char buf[128];
	if ( title ) {
		printf("\r\n\r\n " G_C "%d/6" G_W " " G_Y "%s" G_W, step, title);
		if ( rule ) printf("  " G_G "%s" G_W, rule);
	}
	printf(han ? ESC_HAN : ESC_ENG);
	printf("\r\n  >> ");
	if ( echo == 3 ) line_input_echo(buf, len);
	else line_input(buf, len);
	printf(ESC_ENG);
	std::string s = trim(buf);
	if ( !strcasecmp(s.c_str(), "/x") ) return_login();
	return s;
}

// 로그인 화면에서 다른 뜻으로 쓰는 말과 운영 쪽 이름은 아이디로 못 쓴다
static bool reserved_id(const std::string &id)
{
	static const char *words[] = { "guest", "pass", "new", "sysop", "admin", "root", "operator", "bbs",
		"hanulso", "test", NULL };
	for ( int i = 0; words[i]; i++ ) if ( !strcasecmp(id.c_str(), words[i]) ) return true;
	return false;
}

static void get_id(void)
{
	bool first = true;
	while ( 1 ) {
		std::string s = ask(1, first ? "아이디" : NULL, "영문으로 시작, 영문 / 숫자 / _ 5~20 자", ID_MAX, false);
		first = false;
		if ( (int)s.size() < ID_MIN ) { bad("5 자 이상 넣어 주세요."); continue; }
		if ( !isalpha((unsigned char)s[0]) ) { bad("영문으로 시작해 주세요."); continue; }
		bool ok = true;
		for ( unsigned int i = 0; i < s.size(); i++ ) {
			if ( !isalnum((unsigned char)s[i]) && s[i] != '_' ) ok = false;
		}
		if ( !ok ) { bad("영문, 숫자, _ 만 쓸 수 있습니다."); continue; }
		if ( reserved_id(s) ) { bad("쓸 수 없는 아이디입니다. 다른 것을 넣어 주세요."); continue; }
		if ( database::exist_user_id((char*)s.c_str()) ) { bad("이미 가입된 아이디입니다."); continue; }
		user_id = s;
		return;
	}
}

static void get_password(void)
{
	bool first = true;
	while ( 1 ) {
		std::string p1 = ask(2, first ? "비밀번호" : NULL, "8 자 이상, 영문과 특수 문자 (!@#$ 등) 를 섞어서", 40, false, 3);
		first = false;
		if ( p1.size() < 8 ) { bad("8 자 이상으로 해 주세요."); continue; }
		if ( check_password_strongness(p1) == 0 ) { bad("너무 약합니다. 영문과 특수 문자를 섞어 주세요."); continue; }
		if ( !strcasecmp(p1.c_str(), user_id.c_str()) ) { bad("아이디와 같은 비밀번호는 쓸 수 없습니다."); continue; }
		printf("\r\n  " G_G "확인을 위해 한 번 더" G_W);
		std::string p2 = ask(2, NULL, NULL, 40, false, 3);
		if ( p1 != p2 ) { bad("두 번 넣은 것이 다릅니다. 처음부터 다시 넣어 주세요."); continue; }
		user_passwd = p1;
		return;
	}
}

static void get_nick(void)
{
	bool first = true;
	while ( 1 ) {
		std::string s = ask(3, first ? "닉네임" : NULL, "한글 2~5 자, 영문 4~10 자", NICK_MAX, true);
		first = false;
		if ( s.empty() ) { bad("닉네임을 넣어 주세요."); continue; }
		if ( display_text(s) != s ) { bad("화면에 보이지 않는 글자가 있습니다."); continue; }
		if ( s.size() < 4 ) { bad("너무 짧습니다. 한글 2 자, 영문 4 자 이상."); continue; }
		if ( database::exist_nick_name((char*)s.c_str()) ) { bad("이미 쓰는 닉네임입니다."); continue; }
		nick_name = s;
		return;
	}
}

static void get_birthday(void)
{
	time_t t = time(NULL);
	struct tm now;
	localtime_r(&t, &now);
	bool first = true;
	while ( 1 ) {
		std::string s = ask(4, first ? "생년월일" : NULL, "년-월-일, 예) 1975-3-21  (바이오리듬, 운세, 띠에 씀)", 12, false);
		first = false;
		int y = 0, m = 0, d = 0;
		if ( sscanf(s.c_str(), "%d-%d-%d", &y, &m, &d) != 3 || !is_date_valid(d, m, y) ) {
			bad("날짜가 바르지 않습니다. 년-월-일 로 넣어 주세요.");
			continue;
		}
		if ( y < 1900 || y * 10000 + m * 100 + d > (now.tm_year + 1900) * 10000 + (now.tm_mon + 1) * 100 + now.tm_mday ) {
			bad("1900 년 이후, 오늘까지의 날짜를 넣어 주세요.");
			continue;
		}
		char b[16];
		snprintf(b, sizeof(b), "%04d-%02d-%02d", y, m, d);
		birthday = b;
		return;
	}
}

static void get_email(void)
{
	bool first = true;
	while ( 1 ) {
		std::string s = ask(5, first ? "이메일" : NULL, "비밀번호를 잊었을 때 (PASS) 이 주소로 보냅니다", 50, false);
		first = false;
		if ( !is_email_valid(s.c_str()) ) { bad("이메일 주소가 바르지 않습니다."); continue; }
		if ( database::exist_email_address((char*)s.c_str()) ) { bad("이미 가입된 이메일 주소입니다. PASS 로 비밀번호를 찾을 수 있습니다."); continue; }
		email_address = s;
		return;
	}
}

static void get_sex(void)
{
	bool first = true;
	while ( 1 ) {
		std::string s = ask(6, first ? "성별" : NULL, "[1] 남자  [2] 여자", 1, false);
		first = false;
		if ( s == "1" || s == "2" ) { sex = s; return; }
		bad("1 또는 2 를 넣어 주세요.");
	}
}

// 넣은 것을 모아 보여 준다
static void summary(void)
{
	printf(ESC_CLEAR);
	printf("\r\n " G_Y "가입 내용을 확인해 주세요." G_W "\r\n\r\n");
	printf("   " G_C "1" G_W ". 아이디     %s\r\n", user_id.c_str());
	printf("   " G_C "2" G_W ". 비밀번호   %s\r\n", std::string(user_passwd.size(), '*').c_str());
	printf("   " G_C "3" G_W ". 닉네임     %s\r\n", nick_name.c_str());
	printf("   " G_C "4" G_W ". 생년월일   %s\r\n", birthday.c_str());
	printf("   " G_C "5" G_W ". 이메일     %s\r\n", email_address.c_str());
	printf("   " G_C "6" G_W ". 성별       %s\r\n", sex == "1" ? "남자" : "여자");
}

// 운영자 이름으로 환영 쪽지
static void welcome_memo(void)
{
	if ( sysop_users.empty() || sysop_users[0].empty() ) return;
	std::string from = sysop_users[0];
	std::string body = nick_name + " 님, " + host_name + " 에 오신 것을 환영합니다!\r\n\r\n"
		"처음 오셨다면 이렇게 둘러보세요.\r\n"
		"  - T : 맨 처음 화면,  H : 도움말\r\n"
		"  - GO 이름 : 바로 가기 (예: GO PLAZA 자유 게시판, GO LIFE 생활정보, GO GAME 게임 마당)\r\n"
		"  - NEW : 지난번 접속 뒤의 새 글,  MY : 내 글,  AT : 출석 도장\r\n"
		"  - PE : 내 정보 고치기 (자기소개, 정보 공개),  LOG : 내 접속 기록\r\n"
		"  - 가입인사 게시판 (GO WELCOME) 에 인사를 남겨 주시면 반갑게 맞이하겠습니다.\r\n\r\n"
		"궁금한 것은 이 쪽지에 답장하거나 건의하기 (GO TOSYSOP) 에 남겨 주세요.";
	std::string q = "INSERT INTO memo (SENDER_USER_ID, RECIPIENT_USER_ID, CREATION_DATETIME, TITLE, CONTENT) VALUES ('" +
		database::escape(from.c_str()) + "', '" + database::escape(user_id.c_str()) + "', NOW(), '" +
		database::escape((std::string("환영합니다! ") + host_name + " 이용 안내").c_str()) + "', '" +
		database::escape(body.c_str()) + "')";
	mysql_query(mysql, q.c_str());
}

int main(int argc, char **argv)
{
	snprintf(tty, sizeof(tty), "%s", argc > 1 ? argv[1] : "");

    signal(SIGQUIT, SIG_IGN);
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
    signal(SIGHUP, (__sighandler_t)host_close);
    signal(SIGSEGV, (__sighandler_t)host_close);
    signal(SIGBUS, (__sighandler_t)host_close);

	read_settings("hanulso.cfg");

    ioctl(0,TCGETA, &sys_term);
	raw_mode();

    umask(0111);

	// DB open ...
	if ( database::open() == false )
		exit(1);

	if ( file_exists("txt/register.txt") ) {
		print_file("txt/register.txt");

		printf("\r\n [Enter] 를 누르세요.");
		press_enter();
	}

    printf(ESC_CLEAR);

	// member, memo 테이블 생성
	database::create_member();
	database::create_memo();

	printf("\r\n " G_Y "회원 가입" G_W "  " G_G "(어느 칸에서나 /x 를 넣으면 그만둡니다)" G_W);

	get_id();
	get_password();
	get_nick();
	get_birthday();
	get_email();
	get_sex();

	while ( 1 ) {
		summary();
		printf(ESC_ENG);
		printf("\r\n " G_Y "Y" G_W ": 가입하기   " G_C "번호" G_W ": 고치기   /x: 그만두기 >> ");
		char buf[8];
		line_input(buf, 2);
		std::string c = trim(buf);
		if ( !strcasecmp(c.c_str(), "/x") ) return_login();
		if ( !strcasecmp(c.c_str(), "y") ) break;
		switch ( atoi(c.c_str()) ) {
		case 1: get_id(); break;
		case 2: get_password(); break;
		case 3: get_nick(); break;
		case 4: get_birthday(); break;
		case 5: get_email(); break;
		case 6: get_sex(); break;
		}
	}

	// 그 사이 다른 사람이 같은 아이디 / 닉네임 / 이메일로 가입했을 수 있다
	if ( database::exist_user_id((char*)user_id.c_str()) || database::exist_nick_name((char*)nick_name.c_str()) ||
			database::exist_email_address((char*)email_address.c_str()) ) {
		printf("\r\n\r\n " G_R "방금 다른 회원이 같은 아이디, 닉네임 또는 이메일로 가입했습니다. 처음부터 다시 해 주세요." G_W);
		printf("\r\n [Enter] 를 누르세요.");
		press_enter();
		return_login();
	}

	std::ostringstream query;
	query << "INSERT INTO member ( ";
	query << "		USER_ID, NICK_NAME, BIRTHDAY, PASSWORD, EMAIL, SEX, LEVEL, IS_OPEN,";
	query << "		REGISTRATION_DATETIME, LASTLOGIN_DATETIME ";
	query << ") VALUES (";
	query << "'" << database::escape(user_id.c_str()) << "', ";
	query << "'" << database::escape(nick_name.c_str()) << "', ";
	query << "'" << database::escape(birthday.c_str()) << "', ";
	// 단방향 패스워드 알고리즘 사용
	query << "'" << database::escape(database::hash_password((char*)user_passwd.c_str()).c_str()) << "', ";
	query << "'" << database::escape(email_address.c_str()) << "', ";
	query << "'" << database::escape(sex.c_str()) << "', ";
	query << "'" << 1 << "', ";
	query << "'" << 0 << "', ";
	query << "'" << datetime_now_string(false) << "', ";
	query << "'" << datetime_now_string(false) << "'";
	query << ")";
	if ( mysql_query(mysql, query.str().c_str()) != 0 ) {
		printf("\r\n\r\n " G_R "가입하지 못했습니다: %s" G_W, mysql_error(mysql));
		printf("\r\n [Enter] 를 누르세요.");
		press_enter();
		host_close();
	}

	welcome_memo();
	// 접속해 있는 운영자에게
	for ( unsigned int i = 0; i < sysop_users.size(); i++ ) {
		if ( !sysop_users[i].empty() ) notify_online(sysop_users[i], "◆ 새 회원 가입: " + nick_name + " (" + user_id + ")", true);
	}

	printf("\r\n\r\n " G_GR "가입을 축하합니다!" G_W " 이제 아이디 " G_Y "%s" G_W " 로 로그인하세요.", user_id.c_str());
	printf("\r\n " G_G "운영자가 보낸 환영 쪽지가 기다리고 있습니다 (로그인 뒤 MEMO)." G_W);
	printf("\r\n\r\n [Enter] 를 누르세요.");
	press_enter();

	return_login();
	return 0;
}
