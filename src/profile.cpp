#include "main.h"

// ------------------------------------------------------------------
// 내 정보 고치기 (PE)
//   지금 값을 늘어놓고 번호로 고친다: 비밀번호, 닉네임, 생일, 이메일, 성별, 정보 공개, 자기소개
//   비밀번호는 지금 비밀번호를 확인한 뒤, 가린 채 두 번 받는다 (가입과 같은 규칙: 8 자 이상, 영문 + 특수 문자)
//   정보 공개 (member.IS_OPEN): 1 이면 다른 회원의 PF 에 이메일이 보인다 (0 이면 본인과 운영자만)
//   자기소개 (member.INTRO): PF 에 한 줄
// ------------------------------------------------------------------

#define P_W		"\033[=15F"
#define P_Y		"\033[=14F"
#define P_C		"\033[=11F"
#define P_G		"\033[=7F"
#define P_R		"\033[=12F"
#define P_GR	"\033[=10F"

// 예전 member 표에 INTRO 칸을 더한다 (처음 한 번)
void profile_upgrade(void)
{
	bool ok;
	std::string n = database::fetch((char*)"SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA=DATABASE() "
			"AND TABLE_NAME='member' AND COLUMN_NAME='INTRO'", &ok);
	if ( ok && atoi(n.c_str()) == 0 ) {
		mysql_query(mysql, "ALTER TABLE member ADD COLUMN INTRO VARCHAR(100) NOT NULL DEFAULT ''");
	}
}

static void note(const char *color, const char *text)
{
	printf("\r\n  %s%s" P_W, color, text);
}

static std::string ask_line(const char *q, int len, bool han, int echo = 1)
{
	char buf[128];
	if ( len > 100 ) len = 100;
	printf(han ? ESC_HAN : ESC_ENG);
	printf("\r\n  %s", q);
	if ( echo == 3 ) line_input_echo(buf, len);
	else line_input(buf, len);
	printf(ESC_ENG);
	return trim(buf);
}

static void set_column(char *user_id, const char *col, const std::string &value)
{
	std::string q = std::string("UPDATE member SET ") + col + "='" + database::escape(value.c_str()) +
		"' WHERE USER_ID='" + database::escape(user_id) + "'";
	mysql_query(mysql, q.c_str());
}

static bool change_password(char *user_id)
{
	std::string now = ask_line("지금 비밀번호 (Enter: 취소) : ", 40, false, 3);
	if ( now.empty() ) return false;
	if ( !database::check_same_password(user_id, (char*)now.c_str()) ) {
		note(P_R, "비밀번호가 틀렸습니다.");
		return false;
	}
	note(P_G, "새 비밀번호: 8 자 이상, 영문과 특수 문자를 섞어 주세요.");
	while ( 1 ) {
		std::string p1 = ask_line("새 비밀번호 : ", 40, false, 3);
		if ( p1.empty() ) return false;
		if ( p1.size() < 8 ) { note(P_R, "8 자 이상으로 해 주세요."); continue; }
		if ( check_password_strongness(p1) == 0 ) { note(P_R, "너무 약합니다. 영문과 특수 문자를 섞어 주세요."); continue; }
		if ( p1 == now ) { note(P_R, "지금 비밀번호와 같습니다."); continue; }
		std::string p2 = ask_line("한 번 더    : ", 40, false, 3);
		if ( p1 != p2 ) { note(P_R, "두 번 넣은 것이 다릅니다."); continue; }
		if ( database::set_user_password(user_id, (char*)p1.c_str()) ) {
			note(P_GR, "비밀번호를 바꿨습니다.");
			return true;
		}
		note(P_R, "바꾸지 못했습니다.");
		return false;
	}
}

static bool change_nick(char *user_id, const std::string &cur)
{
	note(P_G, "한글 2~5 자, 영문 4~10 자.");
	while ( 1 ) {
		std::string n = ask_line("새 닉네임 (Enter: 취소) : ", 10, true);
		if ( n.empty() ) return false;
		if ( n == cur ) { note(P_G, "지금 닉네임과 같습니다."); return false; }
		if ( display_text(n) != n ) { note(P_R, "화면에 보이지 않는 글자가 있습니다."); continue; }
		if ( n.size() < 4 ) { note(P_R, "너무 짧습니다. 한글 2 자, 영문 4 자 이상."); continue; }
		if ( database::exist_nick_name((char*)n.c_str()) ) { note(P_R, "이미 쓰는 닉네임입니다."); continue; }
		database::set_user_nick_name(user_id, (char*)n.c_str());
		note(P_GR, "닉네임을 바꿨습니다.");
		return true;
	}
}

static bool change_birthday(char *user_id)
{
	while ( 1 ) {
		std::string b = ask_line("생일 (예: 1978-1-8, Enter: 취소) : ", 12, false);
		if ( b.empty() ) return false;
		int y = 0, m = 0, d = 0;
		if ( sscanf(b.c_str(), "%d-%d-%d", &y, &m, &d) != 3 || y < 1900 || !is_date_valid(d, m, y) ) {
			note(P_R, "날짜가 바르지 않습니다. 년-월-일 로 넣어 주세요.");
			continue;
		}
		char s[16];
		snprintf(s, sizeof(s), "%04d-%02d-%02d", y, m, d);
		database::set_user_birthday(user_id, s);
		note(P_GR, "생일을 바꿨습니다. 바이오리듬, 운세, 띠가 이 날짜로 나옵니다.");
		return true;
	}
}

static bool change_email(char *user_id, const std::string &cur)
{
	note(P_G, "비밀번호를 잊었을 때 (PASS) 이 주소로 보냅니다.");
	while ( 1 ) {
		std::string e = ask_line("새 이메일 (Enter: 취소) : ", 50, false);
		if ( e.empty() ) return false;
		if ( !strcasecmp(e.c_str(), cur.c_str()) ) { note(P_G, "지금 주소와 같습니다."); return false; }
		if ( !is_email_valid((char*)e.c_str()) ) { note(P_R, "이메일 주소가 바르지 않습니다."); continue; }
		if ( database::exist_email_address((char*)e.c_str()) ) { note(P_R, "다른 회원이 쓰는 주소입니다."); continue; }
		database::set_user_email(user_id, (char*)e.c_str());
		note(P_GR, "이메일을 바꿨습니다.");
		return true;
	}
}

bool edit_profile(char *user_id)
{
	profile_upgrade();
	while ( 1 ) {
		bool exist;
		std::map<std::string, std::string> u = database::user_info(user_id, &exist);
		if ( !exist ) {
			printf("\r\n회원 정보가 없습니다.\r\n[Enter] 를 누르세요.");
			press_enter();
			return false;
		}
		bool open = atoi(u["IS_OPEN"].c_str()) != 0;
		std::string intro = display_text(u["INTRO"]);

		printf(ESC_CLEAR);
		print_news_title("내 정보 고치기");
		printf("\033[4;1H\r\n");
		printf("  " P_G "아이디 %s,  가입 %s" P_W "\r\n\r\n", user_id, u["REGISTRATION_DATETIME"].substr(0, 10).c_str());
		printf("    " P_Y "1" P_W ". 비밀번호   " P_G "********" P_W "\r\n");
		printf("    " P_Y "2" P_W ". 닉네임     %s\r\n", display_text(u["NICK_NAME"]).c_str());
		printf("    " P_Y "3" P_W ". 생일       %s\r\n", u["BIRTHDAY"].c_str());
		printf("    " P_Y "4" P_W ". 이메일     %s\r\n", display_text(u["EMAIL"]).c_str());
		printf("    " P_Y "5" P_W ". 성별       %s\r\n", atoi(u["SEX"].c_str()) == 1 ? "남자" : "여자");
		printf("    " P_Y "6" P_W ". 정보 공개  %s\r\n", open ? P_GR "공개" P_W "  " P_G "(다른 회원의 PF 에 이메일이 보임)" P_W
				: P_G "비공개 (PF 에서 이메일은 나와 운영자만)" P_W);
		printf("    " P_Y "7" P_W ". 자기소개   %s\r\n", intro.empty() ? P_G "(없음, PF 에 한 줄로 보입니다)" P_W : intro.c_str());

		std::string c = ask_line("고칠 번호 (Enter: 그만) >> ", 1, false);
		if ( c.empty() || c == "0" || !strcasecmp(c.c_str(), "p") || !strcasecmp(c.c_str(), "q") ) return true;
		bool done = false;
		switch ( atoi(c.c_str()) ) {
		case 1: done = change_password(user_id); break;
		case 2: done = change_nick(user_id, u["NICK_NAME"]); break;
		case 3: done = change_birthday(user_id); break;
		case 4: done = change_email(user_id, u["EMAIL"]); break;
		case 5: {
			std::string s = ask_line("성별 [1]남자 [2]여자 (Enter: 취소) : ", 1, false);
			if ( s == "1" || s == "2" ) { database::set_user_sex(user_id, (char*)s.c_str()); done = true; note(P_GR, "바꿨습니다."); }
			break;
		}
		case 6:
			set_column(user_id, "IS_OPEN", open ? "0" : "1");
			note(P_GR, open ? "비공개로 바꿨습니다." : "공개로 바꿨습니다.");
			done = true;
			break;
		case 7: {
			printf("\r\n  " P_G "지금: %s" P_W, intro.empty() ? "(없음)" : intro.c_str());
			std::string t = ask_line("자기소개 한 줄 (Enter: 그대로, - : 지우기) : ", 60, true);
			if ( t == "-" ) { set_column(user_id, "INTRO", ""); note(P_GR, "지웠습니다."); done = true; }
			else if ( !t.empty() ) { set_column(user_id, "INTRO", display_text(t)); note(P_GR, "바꿨습니다."); done = true; }
			break;
		}
		default:
			continue;
		}
		(void)done;
		printf("\r\n\r\n  [Enter] 를 누르세요.");
		press_enter();
	}
}
