#include "main.h"
#include <crypt.h>

// ------------------------------------------------------------------
// 비밀번호 찾기 (로그인 화면에서 pass)
//   아이디 -> 등록된 이메일로 6 자리 인증 번호 (15 분) -> 번호를 넣으면 새 비밀번호를 직접 정한다.
//   번호를 확인하기 전에는 비밀번호를 바꾸지 않는다 (예전에는 아이디만 넣고 y 면 곧바로 바뀌어,
//   남이 아무 회원 (운영자도) 의 비밀번호를 바꿔 들어가지 못하게 할 수 있었다).
//   인증 번호는 해시로만 저장하고, 5 번 틀리면 무효, 메일은 같은 아이디에 10 분에 한 번만.
// ------------------------------------------------------------------

struct termio sys_term;

char tty[10];

#define P_W		"\033[=15F"
#define P_Y		"\033[=14F"
#define P_C		"\033[=11F"
#define P_G		"\033[=7F"
#define P_R		"\033[=12F"
#define P_GR	"\033[=10F"

#define CODE_MINUTES	15
#define RESEND_MINUTES	10
#define MAX_TRIES		5

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

static void enter_and_back(void)
{
	printf("\r\n\r\n [Enter] 를 누르세요.");
	press_enter();
	return_login();
}

static std::string ask(const char *q, int len, int echo = 1)
{
	char buf[128];
	printf(ESC_ENG);
	printf("\r\n %s", q);
	if ( echo == 3 ) line_input_echo(buf, len);
	else line_input(buf, len);
	std::string s = trim(buf);
	if ( !strcasecmp(s.c_str(), "/x") ) return_login();
	return s;
}

static int query_int(const std::string &q)
{
	bool ok;
	return atoi(database::fetch((char*)q.c_str(), &ok).c_str());
}

// 이메일을 가린다: abcdef@example.com -> ab****@example.com
static std::string mask_email(const std::string &e)
{
	std::string::size_type at = e.find('@');
	if ( at == std::string::npos ) return "****";
	std::string user = e.substr(0, at);
	std::string keep = user.substr(0, user.size() > 2 ? 2 : 1);
	return keep + std::string(user.size() - keep.size(), '*') + e.substr(at);
}

// /dev/urandom 에서 6 자리 숫자
static std::string make_code(void)
{
	unsigned char r[8];
	int n = 0;
	int fd = open("/dev/urandom", O_RDONLY);
	if ( fd >= 0 ) { n = read(fd, r, sizeof(r)); close(fd); }
	unsigned long v = 0;
	for ( int i = 0; i < 8; i++ ) v = (v << 8) | (n == 8 ? r[i] : (unsigned char)rand());
	char b[8];
	snprintf(b, sizeof(b), "%06lu", v % 1000000UL);
	return b;
}

// 인증 번호 메일. 성공하면 true
static bool send_code_mail(const std::string &email, const std::string &nick, const std::string &code)
{
	std::string p1 = make_private_tmpfile("pass");
	std::string p2 = make_private_tmpfile("pass_utf8");
	if ( p1.empty() || p2.empty() ) {
		if ( !p1.empty() ) unlink(p1.c_str());
		if ( !p2.empty() ) unlink(p2.c_str());
		return false;
	}
	FILE *fp = fopen(p1.c_str(), "w");
	if ( fp == NULL ) { unlink(p1.c_str()); unlink(p2.c_str()); return false; }
	fprintf(fp, "'%s'님 안녕하세요.\r\n\r\n", nick.c_str());
	fprintf(fp, "%s BBS 비밀번호 찾기 인증 번호는 %s 입니다.\r\n", host_name, code.c_str());
	fprintf(fp, "%d 분 안에 BBS 화면에 넣고 새 비밀번호를 정해 주세요.\r\n\r\n", CODE_MINUTES);
	fprintf(fp, "직접 요청하지 않으셨다면 이 메일은 무시하셔도 됩니다. 비밀번호는 바뀌지 않았습니다.\r\n");
	fclose(fp);

	char cmd[9072];
	bool success;
	snprintf(cmd, sizeof(cmd), "iconv -f EUC-KR -t UTF8 %s > %s", shell_quote(p1).c_str(), shell_quote(p2).c_str());
	exec_command(cmd, &success);

	// SMTP 비밀번호는 ps 에 보이지 않도록 환경변수로 전달 (mailsend 가 SMTP_USER_PASS 를 읽음)
	setenv("SMTP_USER_PASS", mailserver_passwd, 1);
	std::string subject = std::string("[") + host_name + "] 비밀번호 찾기 인증 번호";
	std::string log_path = std::string(getenv("HANULSO")) + "/tmp/mailsend.log";
	snprintf(cmd, sizeof(cmd), "%s/bin/mailsend -to %s -from %s"
		" -sub %s"
		" -starttls -port %s -auth -smtp %s -user %s"
		" -M \"`cat %s`\" -log %s",
		getenv("HANULSO"), shell_quote(email).c_str(),
		shell_quote(mailserver_user).c_str(), shell_quote(subject).c_str(),
		shell_quote(mailserver_port).c_str(), shell_quote(mailserver_host).c_str(),
		shell_quote(mailserver_user).c_str(), shell_quote(p2).c_str(),
		shell_quote(log_path).c_str());
	exec_command(cmd, &success);
	unsetenv("SMTP_USER_PASS");

	unlink(p1.c_str());
	unlink(p2.c_str());
	return success;
}

// 새 비밀번호를 정한다 (가입과 같은 규칙)
static std::string new_password(const std::string &user_id)
{
	printf("\r\n\r\n " P_Y "새 비밀번호를 정해 주세요." P_W "  " P_G "8 자 이상, 영문과 특수 문자를 섞어서" P_W);
	while ( 1 ) {
		std::string p1 = ask(">> ", 40, 3);
		if ( p1.size() < 8 ) { printf("\r\n " P_R "8 자 이상으로 해 주세요." P_W); continue; }
		if ( check_password_strongness(p1) == 0 ) { printf("\r\n " P_R "너무 약합니다. 영문과 특수 문자를 섞어 주세요." P_W); continue; }
		if ( !strcasecmp(p1.c_str(), user_id.c_str()) ) { printf("\r\n " P_R "아이디와 같은 비밀번호는 쓸 수 없습니다." P_W); continue; }
		printf("\r\n " P_G "확인을 위해 한 번 더" P_W);
		std::string p2 = ask(">> ", 40, 3);
		if ( p1 != p2 ) { printf("\r\n " P_R "두 번 넣은 것이 다릅니다." P_W); continue; }
		return p1;
	}
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

    umask(0077);

	// DB open ...
	if ( database::open() == false )
		exit(1);

	mysql_query(mysql, "CREATE TABLE IF NOT EXISTS pass_token ( "
			"USER_ID VARCHAR(50) NOT NULL PRIMARY KEY, "
			"CODE_HASH VARCHAR(128) NOT NULL, "
			"EXPIRES DATETIME NOT NULL, "
			"TRIES INT NOT NULL DEFAULT 0, "
			"SENT DATETIME NOT NULL )");

	if ( file_exists("txt/pass.txt") ) {
		print_file("txt/pass.txt");
	}

	printf("\r\n " P_Y "비밀번호 찾기" P_W "  " P_G "(어느 칸에서나 /x 면 그만둡니다)" P_W "\r\n");

	// 1. 아이디
	std::string user_id;
	for ( int t = 0; ; t++ ) {
		if ( t >= 3 ) return_login();
		std::string s = ask("아이디 >> ", 40);
		if ( s.empty() ) continue;
		if ( !database::exist_user_id((char*)s.c_str()) ) {
			printf("\r\n " P_R "가입되지 않은 아이디입니다." P_W);
			sleep(1);
			continue;
		}
		user_id = s;
		break;
	}

	bool exist;
	std::map<std::string, std::string> user = database::user_info((char*)user_id.c_str(), &exist);
	user_id = user["USER_ID"];		// DB 에 적힌 그대로 (대소문자)
	std::string email = user["EMAIL"];
	if ( email.empty() || !is_email_valid(email.c_str()) ) {
		printf("\r\n\r\n 등록된 이메일 주소가 없거나 올바르지 않아 인증 번호를 보낼 수 없습니다.");
		printf("\r\n 운영자에게 쪽지나 건의하기 게시판으로 알려 주세요.");
		enter_and_back();
	}
	std::string uid = database::escape(user_id.c_str());

	// 2. 인증 번호: 아직 유효한 번호가 있으면 그것을 넣거나, 없으면 새로 보낸다
	bool have = query_int("SELECT COUNT(*) FROM pass_token WHERE USER_ID='" + uid + "' AND EXPIRES > NOW() AND TRIES < " +
			TO_STRING(MAX_TRIES)) > 0;
	if ( !have ) {
		int recent = query_int("SELECT COUNT(*) FROM pass_token WHERE USER_ID='" + uid + "' AND SENT > NOW() - INTERVAL " +
				TO_STRING(RESEND_MINUTES) + " MINUTE");
		if ( recent > 0 ) {
			printf("\r\n\r\n " P_R "조금 전에 인증 번호를 보냈습니다. %d 분 뒤에 다시 해 주세요." P_W, RESEND_MINUTES);
			enter_and_back();
		}
		printf("\r\n\r\n 등록된 이메일 " P_Y "%s" P_W " 로 6 자리 인증 번호를 보냅니다.", mask_email(email).c_str());
		printf("\r\n 보낼까요? (y/N) ");
		char yn[4];
		line_input(yn, 1);
		if ( strcasecmp(yn, "y") ) return_login();

		std::string code = make_code();
		std::string q = "REPLACE INTO pass_token (USER_ID, CODE_HASH, EXPIRES, TRIES, SENT) VALUES ('" + uid + "', '" +
			database::escape(database::hash_password(code.c_str()).c_str()) + "', NOW() + INTERVAL " +
			TO_STRING(CODE_MINUTES) + " MINUTE, 0, NOW())";
		mysql_query(mysql, q.c_str());

		printf("\r\n\r\n 메일을 보내는 중입니다..."); fflush(stdout);
		if ( !send_code_mail(email, display_text(user["NICK_NAME"]), code) ) {
			mysql_query(mysql, ("DELETE FROM pass_token WHERE USER_ID='" + uid + "'").c_str());
			printf("\r\n " P_R "메일을 보내지 못했습니다. 잠시 뒤에 다시 해 주세요." P_W);
			enter_and_back();
		}
		printf("\r\n " P_GR "인증 번호를 보냈습니다." P_W " %d 분 안에 넣어 주세요. 메일이 늦으면 다시 PASS 로 들어와 넣어도 됩니다.", CODE_MINUTES);
	} else {
		printf("\r\n\r\n 조금 전에 " P_Y "%s" P_W " 로 보낸 인증 번호가 아직 유효합니다.", mask_email(email).c_str());
	}

	// 3. 번호 확인 (이 접속에서 3 번, 번호마다 모두 5 번까지)
	for ( int t = 0; t < 3; t++ ) {
		std::string code = ask("인증 번호 6 자리 >> ", 6);
		if ( code.empty() ) { t--; continue; }
		std::vector<std::map<std::string, std::string> > r = database::fetch_rows((char*)("SELECT CODE_HASH FROM pass_token "
					"WHERE USER_ID='" + uid + "' AND EXPIRES > NOW() AND TRIES < " + TO_STRING(MAX_TRIES)).c_str());
		if ( r.empty() ) {
			printf("\r\n " P_R "인증 번호가 만료되었거나 너무 여러 번 틀렸습니다. 다시 PASS 로 받아 주세요." P_W);
			enter_and_back();
		}
		std::string stored = r[0]["CODE_HASH"];
		char *h = crypt(code.c_str(), stored.c_str());
		if ( h != NULL && stored == h ) {
			std::string pw = new_password(user_id);
			database::set_user_password((char*)user_id.c_str(), (char*)pw.c_str());
			mysql_query(mysql, ("DELETE FROM pass_token WHERE USER_ID='" + uid + "'").c_str());
			printf("\r\n\r\n " P_GR "비밀번호를 바꿨습니다." P_W " 새 비밀번호로 로그인하세요.");
			enter_and_back();
		}
		mysql_query(mysql, ("UPDATE pass_token SET TRIES = TRIES + 1 WHERE USER_ID='" + uid + "'").c_str());
		printf("\r\n " P_R "인증 번호가 다릅니다." P_W);
		sleep(2);
	}
	return_login();
	return 0;
}
