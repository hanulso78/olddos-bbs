#include "main.h"
#include <sys/wait.h>

// ------------------------------------------------------------------
// 운영자 메뉴 (SYSOP)
//   bin/sysop <tty>
//   접속자: 지금 접속자 / 강제 종료, 전체 공지 방송, 로그인 공지
//   회원  : 찾기 / 등급 / 비밀번호, 이용 정지, 가입 / 접속 통계, 삭제
//   게시판: 옮기기, 지우기, 공지 고정, 꼬리말 지우기, 투표 관리
//   서버  : 임시 파일, 주인 없는 첨부, 디스크, AI 사용량
// ------------------------------------------------------------------

struct termio sys_term;

char tty[10];

char login_user_id[50];		// 이 접속의 운영자 아이디 (정지한 사람으로 적는다)

#define S_WHITE		"\033[=15F"
#define S_YELLOW	"\033[=14F"
#define S_RED		"\033[=12F"
#define S_GREEN		"\033[=10F"
#define S_CYAN		"\033[=11F"
#define S_GRAY		"\033[=7F"
#define S_MAGENTA	"\033[=13F"

#define MAX_PINS	3		// 게시판마다 고정할 수 있는 글

typedef std::vector<std::map<std::string, std::string> > rows_t;

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
	printf(S_WHITE);
	fflush(stdout);
	database::close();
    ioctl(0, TCSETAF, &sys_term);
    exit(1);
}

static void print_header(const char *head_title)
{
	printf(ESC_CLEAR);
    printf("\033[1;1H");
	printf("\033[=9F\033[=1G%s\033[=15F\033[=1G", repeat("─", 40).c_str());
    printf("\033[1;1H");
	printf("\033[1A\033[7m%s\033[0m", host_name);
	int center = (80 - strlen(strip_ansi_codes(head_title))) / 2;
	if ( center < 0 ) center = 0;
    printf("\033[2;1H");
	printf("\r\033[%dC%s", center, head_title);
    printf("\033[3;1H");
	printf("\033[=0F\033[=1G%s\033[=15F\033[=1G", repeat("━", 40).c_str());
    printf("\033[4;1H");
}

static void wait_enter(void)
{
	printf("\r\n " S_GRAY "[Enter] 를 누르세요." S_WHITE);
	press_enter();
}

static void msg(const char *color, const std::string &m)
{
	printf("\r\n  %s%s" S_WHITE, color, m.c_str());
}

// 한 줄 묻기. 한글이면 han
static std::string ask(const char *q, int len, bool han = false)
{
	char buf[256];
	if ( len > 200 ) len = 200;
	printf(han ? ESC_HAN : ESC_ENG);
	printf("\r\n  %s", q);
	line_input(buf, len);
	printf(ESC_ENG);
	return trim(buf);
}

static bool confirm(const std::string &q)
{
	printf(ESC_ENG);
	printf("\r\n  " S_YELLOW "%s" S_WHITE " (y/N) ", q.c_str());
	return yesno(NO) == YES;
}

static int query_int(const std::string &q)
{
	bool ok;
	return atoi(database::fetch((char*)q.c_str(), &ok).c_str());
}

static rows_t rows_of(const std::string &q)
{
	return database::fetch_rows((char*)q.c_str());
}

static std::string esc(const std::string &s)
{
	return database::escape(s.c_str());
}

static std::string nick_of(const std::string &id)
{
	bool exist;
	std::map<std::string, std::string> u = database::user_info((char*)id.c_str(), &exist);
	std::string n = exist ? display_text(u["NICK_NAME"]) : "";
	return n.empty() ? id : n;
}

static bool is_sysop(const std::string &id)
{
	return std::find(sysop_users.begin(), sysop_users.end(), id) != sysop_users.end();
}

static std::string hanulso(void)
{
	return getenv("HANULSO") ? getenv("HANULSO") : ".";
}

static void create_tables(void)
{
	mysql_query(mysql, "CREATE TABLE IF NOT EXISTS member_suspend ( "
			"USER_ID VARCHAR(50) NOT NULL PRIMARY KEY, "
			"UNTIL DATETIME NULL, "				/* NULL 이면 영구 */
			"REASON VARCHAR(255) NOT NULL, "
			"BY_ID VARCHAR(50) NOT NULL, "
			"DATE_TIME DATETIME NOT NULL )");
	mysql_query(mysql, "CREATE TABLE IF NOT EXISTS login_log ( "
			"NO INTEGER NOT NULL AUTO_INCREMENT PRIMARY KEY, "
			"USER_ID VARCHAR(50) NOT NULL, "
			"NODE VARCHAR(16) NOT NULL, "
			"DATE_TIME DATETIME NOT NULL, "
			"KEY IDX_DATE (DATE_TIME) )");
	mysql_query(mysql, "CREATE TABLE IF NOT EXISTS board_pin ( "
			"BOARD VARCHAR(64) NOT NULL, "
			"NO INT NOT NULL, "
			"DATE_TIME DATETIME NOT NULL, "
			"PRIMARY KEY (BOARD, NO) )");
	mysql_query(mysql, "CREATE TABLE IF NOT EXISTS stat_online ( "
			"DAY DATE NOT NULL PRIMARY KEY, "
			"MAX_ONLINE INT NOT NULL, "
			"MAX_TIME DATETIME NOT NULL )");
}

// ------------------------------------------------------------------
// 접속자
// ------------------------------------------------------------------
struct online_user {
	std::string id, node, path;
	int pid;
	long ms;		// 접속한 지
};

static std::vector<online_user> online_users(void)
{
	std::vector<online_user> r;
	std::vector<std::string> files = find_files_time_sorted((char*)(hanulso() + "/tmp/*.tty").c_str());
	for ( unsigned int i = 0; i < files.size(); i++ ) {
		online_user u;
		if ( !read_tty_file(files[i], u.id) ) continue;
		char sid[256] = "";
		u.pid = 0;
		sscanf(trim(read_file(files[i].c_str())).c_str(), "%255s %d", sid, &u.pid);
		u.node = split_string(split_file_name(files[i]), '.')[0];
		u.path = files[i];
		u.ms = ms_time_now() - file_ms_mtime(files[i]);
		r.push_back(u);
	}
	return r;
}

// 접속을 끊는다: 알림을 띄우고 그 접속의 프로세스 묶음에 SIGHUP
static void disconnect(const online_user &u, const std::string &why)
{
	notify_online(u.id, why, true);
	fflush(stdout);
	sleep(1);
	pid_t pg = getpgid(u.pid);
	if ( pg > 1 && pg != getpgrp() ) killpg(pg, SIGHUP);
	else kill(u.pid, SIGHUP);
}

static void disconnect_user(const std::string &id, const std::string &why)
{
	std::vector<online_user> us = online_users();
	for ( unsigned int i = 0; i < us.size(); i++ ) {
		if ( us[i].id == id && us[i].node != tty ) disconnect(us[i], why);
	}
}

static void show_online(void)
{
	while ( 1 ) {
		std::vector<online_user> us = online_users();
		print_header(S_CYAN "운영자 - 지금 접속자" S_WHITE);
		printf("\r\n  " S_GRAY "%3s  %-24s %-24s %-8s %s" S_WHITE "\r\n", "", "이름 (아이디)", "있는 곳", "노드", "접속한 지");
		for ( unsigned int i = 0; i < us.size() && i < 15; i++ ) {
			std::string name = string_truncate(nick_of(us[i].id), 12, "") + " (" + us[i].id + ")";
			bool me = us[i].node == tty;
			std::string where = display_text(trim(read_file((us[i].path.substr(0, us[i].path.size() - 4) + ".where").c_str())));
			printf("  %s%3d  %-24s %-24s pts/%-4s %s%s" S_WHITE "\r\n", me ? S_YELLOW : S_WHITE, i + 1,
					string_truncate(name, 24, "").c_str(), string_truncate(where.empty() ? "-" : where, 24, "").c_str(), us[i].node.c_str(),
					ms_time_to_string(us[i].ms).substr(0, 8).c_str(), me ? "  (나)" : "");
		}
		if ( us.size() > 15 ) printf("  " S_GRAY "... 그 밖에 %d 명" S_WHITE "\r\n", (int)us.size() - 15);
		printf("\r\n  " S_GRAY "모두 %d 명" S_WHITE, (int)us.size());

		std::string c = ask("강제로 끊을 번호 (Enter: 돌아가기, R: 새로 보기) >> ", 3);
		if ( c.empty() ) return;
		if ( !strcasecmp(c.c_str(), "r") ) continue;
		int k = atoi(c.c_str());
		if ( k < 1 || k > (int)us.size() ) continue;
		online_user &u = us[k - 1];
		if ( u.node == tty ) { msg(S_RED, "자기 접속은 끊을 수 없습니다."); wait_enter(); continue; }
		if ( !confirm(nick_of(u.id) + " (" + u.id + ") 님의 접속을 끊을까요?") ) continue;
		disconnect(u, "◆ 운영자가 접속을 끊습니다.");
		msg(S_GREEN, "접속을 끊었습니다.");
		wait_enter();
	}
}

static void broadcast(void)
{
	print_header(S_CYAN "운영자 - 전체 공지 방송" S_WHITE);
	printf("\r\n  지금 접속한 모든 회원의 화면에 한 줄 알림을 띄웁니다.\r\n");
	printf("  " S_GRAY "예) 10 분 뒤 서버 점검으로 잠시 접속이 끊깁니다." S_WHITE "\r\n");
	std::string m = display_text(ask("알림 (Enter: 취소) >> ", 60, true));
	if ( m.empty() ) return;
	if ( !confirm("모두에게 보낼까요?") ) return;
	std::vector<online_user> us = online_users();
	std::set<std::string> done;
	int n = 0;
	for ( unsigned int i = 0; i < us.size(); i++ ) {
		if ( done.count(us[i].id) ) continue;
		done.insert(us[i].id);
		n += notify_online(us[i].id, "◆ 운영자 공지: " + m, true) > 0 ? 1 : 0;
	}
	msg(S_GREEN, TO_STRING(n) + " 명에게 보냈습니다.");
	wait_enter();
}

static std::string notice_path(void)
{
	return hanulso() + "/txt/login_notice.txt";
}

static void login_notice(void)
{
	while ( 1 ) {
		std::vector<std::string> lines = split_string(read_file(notice_path().c_str()), '\n');
		while ( !lines.empty() && trim(lines.back()).empty() ) lines.pop_back();
		for ( unsigned int i = 0; i < lines.size(); i++ ) {
			if ( !lines[i].empty() && lines[i][lines[i].size() - 1] == '\r' ) lines[i].erase(lines[i].size() - 1);
		}

		print_header(S_CYAN "운영자 - 로그인 공지" S_WHITE);
		printf("\r\n  로그인할 때 회원 정보 아래에 보이는 공지입니다. (앞의 5 줄까지)\r\n\r\n");
		if ( lines.empty() ) printf("  " S_GRAY "(지금은 공지가 없습니다)" S_WHITE "\r\n");
		for ( unsigned int i = 0; i < lines.size() && i < 8; i++ ) {
			printf("  " S_YELLOW "%s" S_WHITE "\r\n", string_truncate(lines[i], 74, "").c_str());
		}
		std::string c = ask("E: 고치기  D: 지우기  Enter: 돌아가기 >> ", 2);
		if ( c.empty() ) return;
		if ( !strcasecmp(c.c_str(), "d") ) {
			if ( confirm("로그인 공지를 지울까요?") ) unlink(notice_path().c_str());
		} else if ( !strcasecmp(c.c_str(), "e") ) {
			printf("\r\n");
			line_editor_layout(2, 72);
			if ( line_editor(lines, !lines.empty()) ) {
				FILE *fp = fopen(notice_path().c_str(), "w");
				if ( fp != NULL ) {
					for ( unsigned int i = 0; i < lines.size(); i++ ) fprintf(fp, "%s\n", lines[i].c_str());
					fclose(fp);
					chmod(notice_path().c_str(), 0644);
				}
			}
		}
	}
}

// ------------------------------------------------------------------
// 회원
// ------------------------------------------------------------------

// 정지 중이면 정지 줄 (아니면 빈 map)
static std::map<std::string, std::string> suspension_of(const std::string &id)
{
	rows_t r = rows_of("SELECT *, IFNULL(UNTIL, '영구') AS UNTIL_S FROM member_suspend WHERE USER_ID='" + esc(id) +
			"' AND (UNTIL IS NULL OR UNTIL > NOW())");
	return r.empty() ? std::map<std::string, std::string>() : r[0];
}

// 아이디나 닉네임으로 찾는다
static std::string find_member(const std::string &key)
{
	if ( key.empty() ) return "";
	if ( database::exist_user_id((char*)key.c_str()) ) return key;
	bool exist;
	std::map<std::string, std::string> u = database::user_info_by_nick_name((char*)key.c_str(), &exist);
	return exist ? u["USER_ID"] : "";
}

// ------------------------------------------------------------------
// 아이디 바꾸기
// 카페에서 가져온 회원(coma****, 비밀번호 '!')을 본인이 요청하면 진짜 아이디로 바꿔 줄 때 쓴다.
// 아이디가 들어 있는 모든 테이블의 칸을 함께 바꾼다 (information_schema 에서 찾는다:
// USER_ID, ..._USER_ID, BY_ID, BLACK_ID, WHITE_ID). 게시판, 첨부, 쪽지, 꼬리말, 게임 기록 등.
// 새 아이디가 이미 BBS 회원이면, 카페에서 가져온 회원만 그 회원에 합칠 수 있다.
// ------------------------------------------------------------------
static rows_t id_columns(void)
{
	return rows_of("SELECT TABLE_NAME, COLUMN_NAME FROM information_schema.COLUMNS WHERE TABLE_SCHEMA = DATABASE() "
			"AND (COLUMN_NAME = 'USER_ID' OR COLUMN_NAME LIKE '%\\_USER\\_ID' OR COLUMN_NAME IN ('BY_ID', 'BLACK_ID', 'WHITE_ID', 'FRIEND_ID'))");
}

static bool valid_new_id(const std::string &id)
{
	if ( id.size() < 5 || id.size() > 40 ) return false;
	for ( unsigned int i = 0; i < id.size(); i++ ) {
		if ( !isalnum((unsigned char)id[i]) && id[i] != '_' ) return false;
	}
	return true;
}

static void change_id(const std::string &from)
{
	if ( is_sysop(from) ) { msg(S_RED, "운영자 아이디는 바꿀 수 없습니다 (hanulso.cfg 의 sysop)."); wait_enter(); return; }
	bool cafe = query_int("SELECT COUNT(*) FROM member WHERE USER_ID='" + esc(from) + "' AND PASSWORD='!'") > 0;
	printf("\r\n  %s (%s)%s", from.c_str(), nick_of(from).c_str(), cafe ? " " S_GRAY "- 카페에서 가져온 회원" S_WHITE : "");
	printf("\r\n  " S_GRAY "새 아이디: 영문, 숫자, _ 로 5 자 이상.");
	if ( cafe ) printf("\r\n  이미 가입한 BBS 아이디를 넣으면 이 회원의 글을 그 회원에 합칩니다.");
	printf(S_WHITE "\r\n");
	std::string to = trim(ask("새 아이디 (Enter: 취소) >> ", 40));
	if ( to.empty() || to == from ) return;
	if ( !valid_new_id(to) ) { msg(S_RED, "영문, 숫자, _ 로 5 자 이상이어야 합니다."); wait_enter(); return; }

	rows_t cols = id_columns();
	if ( cols.empty() ) { msg(S_RED, "아이디 칸을 찾지 못했습니다 (information_schema)."); wait_enter(); return; }
	bool merge = database::exist_user_id((char*)to.c_str());
	if ( merge ) {
		if ( !cafe ) { msg(S_RED, to + " 는 이미 있는 아이디입니다. (합치기는 카페에서 가져온 회원만 됩니다)"); wait_enter(); return; }
		if ( !confirm(from + " 의 글/첨부를 모두 " + nick_of(to) + " (" + to + ") 회원 것으로 합치고 " + from + " 은 지울까요?") ) return;
	} else {
		// 지운 회원이 남긴 기록이 새 아이디로 남아 있으면 섞이므로 멈춘다
		std::string left;
		for ( unsigned int i = 0; i < cols.size(); i++ ) {
			if ( cols[i]["TABLE_NAME"] == "member" ) continue;
			if ( query_int("SELECT COUNT(*) FROM " + cols[i]["TABLE_NAME"] + " WHERE " + cols[i]["COLUMN_NAME"] + "='" + esc(to) + "'") > 0 )
				left += " " + cols[i]["TABLE_NAME"];
		}
		if ( !left.empty() ) { msg(S_RED, "새 아이디로 남아 있는 기록이 있어 바꿀 수 없습니다:" + left); wait_enter(); return; }
		if ( !confirm(from + " -> " + to + " 로 아이디를 바꿀까요?") ) return;
	}
	disconnect_user(from, "◆ 운영자가 아이디를 바꿉니다. 새 아이디로 다시 접속해 주세요.");

	int n = 0;
	for ( unsigned int i = 0; i < cols.size(); i++ ) {
		const std::string &t = cols[i]["TABLE_NAME"], &c = cols[i]["COLUMN_NAME"];
		if ( merge && t == "member" ) continue;
		// IGNORE: 합칠 때 같은 기록(게임 점수 등)이 겹치면 새 아이디 것을 남긴다
		std::string q = "UPDATE IGNORE " + t + " SET " + c + "='" + esc(to) + "' WHERE " + c + "='" + esc(from) + "'";
		if ( mysql_query(mysql, q.c_str()) == 0 && t != "member" && t != "login_log" ) n += (int)mysql_affected_rows(mysql);
	}
	if ( merge ) {
		// 겹쳐서 못 옮긴 기록과 회원 정보
		for ( unsigned int i = 0; i < cols.size(); i++ ) {
			mysql_query(mysql, ("DELETE FROM " + cols[i]["TABLE_NAME"] + " WHERE " + cols[i]["COLUMN_NAME"] + "='" + esc(from) + "'").c_str());
		}
	}

	// 파일: AI 대화 기록
	std::string ai = hanulso() + "/data/aichat/";
	if ( !merge && access((ai + from).c_str(), F_OK) == 0 && access((ai + to).c_str(), F_OK) != 0 ) rename((ai + from).c_str(), (ai + to).c_str());
	// 카페: 이 회원으로 연결된 카페 회원은 새 아이디로 (다음에 받는 글도 새 아이디로 올라간다)
	std::string cd = hanulso() + "/data/cafe";
	bool ok;
	std::vector<std::string> fs = exec_command((char*)("grep -lxF -e " + shell_quote(from) + " " + cd + "/links/* " + cd + "/members/* 2>/dev/null").c_str(), &ok);
	for ( unsigned int i = 0; i < fs.size(); i++ ) {
		std::string f = trim(fs[i]);
		size_t s = f.rfind('/');
		if ( f.empty() || s == std::string::npos ) continue;
		std::string link = cd + "/links/" + f.substr(s + 1);
		if ( f != link && access(link.c_str(), F_OK) == 0 ) continue;	// 이미 다른 회원과 연결됨
		FILE *fp = fopen(link.c_str(), "w");
		if ( fp ) { fputs(to.c_str(), fp); fclose(fp); }
	}

	msg(S_GREEN, std::string(merge ? "합쳤습니다." : "바꿨습니다.") + " 옮긴 기록 " + TO_STRING(n) + " 개 (게시글, 첨부, 꼬리말, 쪽지 등)");
	if ( !merge && cafe ) {
		printf("\r\n  " S_GRAY "카페에서 가져온 회원은 비밀번호가 없어 로그인할 수 없습니다." S_WHITE);
		std::string pw = ask("새 비밀번호 (Enter: 나중에) >> ", 20);
		if ( !pw.empty() ) {
			database::set_user_password((char*)to.c_str(), (char*)pw.c_str());
			msg(S_GREEN, "비밀번호를 정했습니다. 이제 " + to + " 로 로그인할 수 있습니다.");
		}
	}
	wait_enter();
}

// 운영자 메뉴: 아이디나 닉네임으로 찾아서 아이디 바꾸기
static void rename_member(void)
{
	print_header(S_CYAN "운영자 - 아이디 바꾸기" S_WHITE);
	printf("\r\n  아이디나 닉네임으로 찾습니다. 일부만 넣으면 비슷한 회원을 보여 줍니다.");
	printf("\r\n  " S_GRAY "카페에서 가져온 회원(coma****)을 본인 요청에 따라 진짜 아이디로 바꿀 때 씁니다." S_WHITE "\r\n");
	std::string key = ask("아이디 / 닉네임 (Enter: 돌아가기) >> ", 30, true);
	if ( key.empty() ) return;
	std::string id = find_member(key);
	if ( id.empty() ) {
		rows_t r = rows_of("SELECT USER_ID, NICK_NAME, PASSWORD FROM member WHERE USER_ID LIKE '%" + esc(key) + "%' OR NICK_NAME LIKE '%" +
				esc(key) + "%' ORDER BY USER_ID LIMIT 15");
		if ( r.empty() ) { msg(S_RED, "찾는 회원이 없습니다."); wait_enter(); return; }
		printf("\r\n");
		for ( unsigned int i = 0; i < r.size(); i++ ) {
			printf("  %3d. %-20s %s%s\r\n", i + 1, r[i]["USER_ID"].c_str(), display_text(r[i]["NICK_NAME"]).c_str(),
					r[i]["PASSWORD"] == "!" ? " " S_GRAY "(카페)" S_WHITE : "");
		}
		int k = atoi(ask("번호 (Enter: 돌아가기) >> ", 3).c_str());
		if ( k < 1 || k > (int)r.size() ) return;
		id = r[k - 1]["USER_ID"];
	}
	change_id(id);
}

static void suspend_member(const std::string &id)
{
	if ( is_sysop(id) ) { msg(S_RED, "운영자는 정지할 수 없습니다."); return; }
	std::string d = ask("정지 기간 (일 수, 0: 영구, Enter: 취소) >> ", 4);
	if ( d.empty() || !is_number((char*)d.c_str()) ) return;
	std::string reason = display_text(ask("사유 >> ", 60, true));
	if ( reason.empty() ) reason = "운영 원칙 위반";
	int days = atoi(d.c_str());
	if ( !confirm(nick_of(id) + " (" + id + ") 님을 " + (days ? TO_STRING(days) + " 일" : std::string("영구")) + " 정지할까요?") ) return;
	std::string until = days ? "NOW() + INTERVAL " + TO_STRING(days) + " DAY" : "NULL";
	std::string q = "REPLACE INTO member_suspend (USER_ID, UNTIL, REASON, BY_ID, DATE_TIME) VALUES ('" + esc(id) + "', " +
		until + ", '" + esc(reason) + "', '" + esc(login_user_id) + "', NOW())";
	mysql_query(mysql, q.c_str());
	disconnect_user(id, "◆ 운영자가 이용을 정지했습니다: " + reason);
	msg(S_GREEN, "정지했습니다. 접속해 있었다면 끊었습니다.");
}

static void member_card(const std::string &id)
{
	while ( 1 ) {
		bool exist;
		std::map<std::string, std::string> u = database::user_info((char*)id.c_str(), &exist);
		if ( !exist ) return;
		std::map<std::string, std::string> s = suspension_of(id);
		int articles = 0;

		print_header(S_CYAN "운영자 - 회원 정보" S_WHITE);
		printf("\r\n");
		printf("  %-10s: %s\r\n", "아이디", id.c_str());
		printf("  %-10s: %s\r\n", "닉네임", display_text(u["NICK_NAME"]).c_str());
		printf("  %-10s: %s%s\r\n", "등급", is_sysop(id) ? "운영자 / " : "", get_level_name(atoi(u["LEVEL"].c_str())).c_str());
		printf("  %-10s: %s\r\n", "가입일", u["REGISTRATION_DATETIME"].c_str());
		printf("  %-10s: %s\r\n", "최근 접속", u["LASTLOGIN_DATETIME"].c_str());
		printf("  %-10s: %s\r\n", "생일", u["BIRTHDAY"].c_str());
		printf("  %-10s: %s\r\n", "이메일", display_text(u["EMAIL"]).c_str());
		printf("  %-10s: %d 번\r\n", "로그인", query_int("SELECT COUNT(*) FROM login_log WHERE USER_ID='" + esc(id) + "'"));
		printf("  %-10s: %d 개\r\n", "꼬리말", query_int("SELECT COUNT(*) FROM comments WHERE USER_ID='" + esc(id) + "'"));
		(void)articles;
		if ( !s.empty() ) {
			printf("  %-10s: " S_RED "정지 중 (%s 까지) %s" S_WHITE "\r\n", "상태", s["UNTIL_S"].c_str(), display_text(s["REASON"]).c_str());
		} else {
			printf("  %-10s: " S_GREEN "정상" S_WHITE "\r\n", "상태");
		}

		std::string c = ask("L: 등급  W: 비밀번호  I: 아이디  S: 정지  U: 정지 풀기  Enter: 돌아가기 >> ", 2);
		if ( c.empty() ) return;
		if ( !strcasecmp(c.c_str(), "i") ) {
			change_id(id);
			return;	// 아이디가 바뀌었을 수 있다
		}
		if ( !strcasecmp(c.c_str(), "l") ) {
			printf("\r\n  ");
			for ( unsigned int i = 0; i < level_nums.size(); i++ ) printf("[%d]%s ", level_nums[i], level_names[i].c_str());
			std::string lv = ask("새 등급 >> ", 2);
			if ( is_number((char*)lv.c_str()) && check_exist_level(atoi(lv.c_str())) ) {
				database::set_user_level((char*)id.c_str(), atoi(lv.c_str()));
				msg(S_GREEN, "바꿨습니다.");
				wait_enter();
			}
		} else if ( !strcasecmp(c.c_str(), "w") ) {
			std::string pw = ask("새 비밀번호 (Enter: 취소) >> ", 20);
			if ( !pw.empty() && confirm("비밀번호를 바꿀까요?") ) {
				database::set_user_password((char*)id.c_str(), (char*)pw.c_str());
				msg(S_GREEN, "바꿨습니다.");
				wait_enter();
			}
		} else if ( !strcasecmp(c.c_str(), "s") ) {
			suspend_member(id);
			wait_enter();
		} else if ( !strcasecmp(c.c_str(), "u") ) {
			if ( !s.empty() && confirm("정지를 풀까요?") ) {
				mysql_query(mysql, ("DELETE FROM member_suspend WHERE USER_ID='" + esc(id) + "'").c_str());
				msg(S_GREEN, "정지를 풀었습니다.");
				wait_enter();
			}
		}
	}
}

static void members(void)
{
	print_header(S_CYAN "운영자 - 회원 찾기" S_WHITE);
	printf("\r\n  아이디나 닉네임으로 찾습니다. 닉네임 일부를 넣으면 비슷한 회원을 보여 줍니다.\r\n");
	std::string key = ask("아이디 / 닉네임 (Enter: 돌아가기) >> ", 30, true);
	if ( key.empty() ) return;
	std::string id = find_member(key);
	if ( !id.empty() ) { member_card(id); return; }

	rows_t r = rows_of("SELECT USER_ID, NICK_NAME FROM member WHERE USER_ID LIKE '%" + esc(key) + "%' OR NICK_NAME LIKE '%" +
			esc(key) + "%' ORDER BY USER_ID LIMIT 15");
	if ( r.empty() ) { msg(S_RED, "찾는 회원이 없습니다."); wait_enter(); return; }
	printf("\r\n");
	for ( unsigned int i = 0; i < r.size(); i++ ) {
		printf("  %3d. %-20s %s\r\n", i + 1, r[i]["USER_ID"].c_str(), display_text(r[i]["NICK_NAME"]).c_str());
	}
	std::string c = ask("번호 (Enter: 돌아가기) >> ", 3);
	int k = atoi(c.c_str());
	if ( k >= 1 && k <= (int)r.size() ) member_card(r[k - 1]["USER_ID"]);
}

static void suspensions(void)
{
	while ( 1 ) {
		print_header(S_CYAN "운영자 - 이용 정지" S_WHITE);
		rows_t r = rows_of("SELECT *, IFNULL(DATE_FORMAT(UNTIL, '%Y-%m-%d %H:%i'), '영구') AS UNTIL_S FROM member_suspend "
				"WHERE UNTIL IS NULL OR UNTIL > NOW() ORDER BY DATE_TIME DESC LIMIT 14");
		printf("\r\n  " S_GRAY "%3s  %-20s %-17s %s" S_WHITE "\r\n", "", "아이디", "언제까지", "사유");
		if ( r.empty() ) printf("  " S_GRAY "     정지된 회원이 없습니다." S_WHITE "\r\n");
		for ( unsigned int i = 0; i < r.size(); i++ ) {
			printf("  %3d  %-20s %-17s %s\r\n", i + 1, r[i]["USER_ID"].c_str(), r[i]["UNTIL_S"].c_str(),
					string_truncate(display_text(r[i]["REASON"]), 30, "").c_str());
		}
		std::string c = ask("A: 정지하기  번호: 정지 풀기  Enter: 돌아가기 >> ", 3);
		if ( c.empty() ) return;
		if ( !strcasecmp(c.c_str(), "a") ) {
			std::string id = find_member(ask("아이디 / 닉네임 >> ", 30, true));
			if ( id.empty() ) { msg(S_RED, "찾는 회원이 없습니다."); wait_enter(); continue; }
			suspend_member(id);
			wait_enter();
		} else {
			int k = atoi(c.c_str());
			if ( k >= 1 && k <= (int)r.size() && confirm(r[k - 1]["USER_ID"] + " 님의 정지를 풀까요?") ) {
				mysql_query(mysql, ("DELETE FROM member_suspend WHERE USER_ID='" + esc(r[k - 1]["USER_ID"]) + "'").c_str());
			}
		}
	}
}

static void member_stats(void)
{
	print_header(S_CYAN "운영자 - 가입 / 접속 통계" S_WHITE);
	printf("  " S_GRAY "%-12s %8s %8s %8s %8s" S_WHITE "\r\n", "날짜", "로그인", "회원 수", "최고동시", "새 회원");
	for ( int d = 0; d < 14; d++ ) {
		std::string day = "CURDATE() - INTERVAL " + TO_STRING(d) + " DAY";
		rows_t r = rows_of("SELECT DATE_FORMAT(" + day + ", '%m-%d (%a)') AS D, "
				"(SELECT COUNT(*) FROM login_log WHERE DATE(DATE_TIME) = " + day + ") AS L, "
				"(SELECT COUNT(DISTINCT USER_ID) FROM login_log WHERE DATE(DATE_TIME) = " + day + ") AS U, "
				"(SELECT IFNULL(MAX(MAX_ONLINE), 0) FROM stat_online WHERE DAY = " + day + ") AS M, "
				"(SELECT COUNT(*) FROM member WHERE DATE(REGISTRATION_DATETIME) = " + day + ") AS N");
		if ( r.empty() ) continue;
		printf("  %s%-12s %8s %8s %8s %8s" S_WHITE "\r\n", d == 0 ? S_YELLOW : S_WHITE, r[0]["D"].c_str(),
				r[0]["L"].c_str(), r[0]["U"].c_str(), r[0]["M"].c_str(), r[0]["N"].c_str());
	}
	int total = query_int("SELECT COUNT(*) FROM member");
	int idle = query_int("SELECT COUNT(*) FROM member WHERE LASTLOGIN_DATETIME < NOW() - INTERVAL 1 YEAR");
	printf("\r\n  전체 회원 " S_CYAN "%d" S_WHITE " 명,  1 년 넘게 접속하지 않은 회원 " S_CYAN "%d" S_WHITE " 명\r\n", total, idle);
	printf("  " S_GRAY "로그인 기록은 이 기능을 넣은 뒤부터 쌓입니다." S_WHITE);
	printf(ESC_ENG);
	printf("\r\n  N: 새 회원 목록  I: 오래 쉰 회원  Enter: 돌아가기 >> ");
	char c[4];
	line_input(c, 1);
	if ( c[0] == 'n' || c[0] == 'N' || c[0] == 'i' || c[0] == 'I' ) {
		bool news = c[0] == 'n' || c[0] == 'N';
		print_header(news ? S_CYAN "운영자 - 새 회원" S_WHITE : S_CYAN "운영자 - 오래 쉰 회원" S_WHITE);
		rows_t r = rows_of(news ? "SELECT * FROM member ORDER BY REGISTRATION_DATETIME DESC LIMIT 16"
				: "SELECT * FROM member ORDER BY LASTLOGIN_DATETIME ASC LIMIT 16");
		printf("  " S_GRAY "%-20s %-16s %-19s %s" S_WHITE "\r\n", "아이디", "닉네임", "가입", "최근 접속");
		for ( unsigned int i = 0; i < r.size(); i++ ) {
			printf("  %-20s %-16s %-19s %s\r\n", string_truncate(r[i]["USER_ID"], 20, "").c_str(),
					string_truncate(display_text(r[i]["NICK_NAME"]), 16, "").c_str(),
					r[i]["REGISTRATION_DATETIME"].substr(0, 16).c_str(), r[i]["LASTLOGIN_DATETIME"].substr(0, 16).c_str());
		}
		wait_enter();
	}
}

static void delete_member(void)
{
	print_header(S_CYAN "운영자 - 회원 삭제" S_WHITE);
	printf("\r\n  " S_RED "지운 회원은 되살릴 수 없습니다." S_WHITE " 잠시 막으려면 이용 정지를 쓰세요.\r\n");
	std::string id = find_member(ask("아이디 / 닉네임 (Enter: 돌아가기) >> ", 30, true));
	if ( id.empty() ) return;
	if ( is_sysop(id) ) { msg(S_RED, "운영자는 지울 수 없습니다."); wait_enter(); return; }
	if ( !confirm(nick_of(id) + " (" + id + ") 님을 정말 지울까요?") ) return;
	disconnect_user(id, "◆ 운영자가 접속을 끊습니다.");
	if ( database::delete_user((char*)id.c_str()) ) msg(S_GREEN, "지웠습니다.");
	wait_enter();
}

// ------------------------------------------------------------------
// 게시판
// ------------------------------------------------------------------
struct board_info { std::string id, go, name; };

static void collect_boards(pugi::xml_node node, std::vector<board_info> &list)
{
	for ( pugi::xml_node n = node.first_child(); n; n = n.next_sibling() ) {
		if ( !strcasecmp(n.attribute("type").value(), "board") ) {
			board_info b;
			b.id = n.attribute("id").value();
			b.go = n.attribute("go").value();
			b.name = trim(n.child("name").child_value());
			if ( !b.id.empty() ) list.push_back(b);
		}
		collect_boards(n, list);
	}
}

static std::vector<board_info> all_boards(void)
{
	static std::vector<board_info> list;
	if ( list.empty() ) {
		pugi::xml_document doc;
		if ( doc.load_file((hanulso() + "/hanulso.mnu").c_str()) ) collect_boards(doc.first_child(), list);
	}
	return list;
}

// 게시판 고르기: 아이디 (bbs_...), GO 이름, ? 로 목록에서 번호
static bool choose_board(const char *what, board_info &out)
{
	std::vector<board_info> list = all_boards();
	while ( 1 ) {
		std::string c = ask((std::string(what) + " (GO 이름 / 아이디, ?: 목록, Enter: 취소) >> ").c_str(), 30);
		if ( c.empty() ) return false;
		if ( c == "?" ) {
			for ( unsigned int i = 0; i < list.size(); i += 32 ) {
				print_header(S_CYAN "운영자 - 게시판 목록" S_WHITE);
				for ( unsigned int k = i; k < list.size() && k < i + 32; k += 2 ) {
					for ( unsigned int j = k; j < k + 2 && j < list.size() && j < i + 32; j++ ) {
						// 한 칸 38 자: 번호. 이름 (회색 GO 이름)
						std::string go = string_truncate(list[j].go, 10, "");
						std::string num = TO_STRING(j + 1) + ". ";
						int room = 36 - num.size() - (go.empty() ? 0 : go.size() + 1);
						std::string s = num + string_truncate(display_text(list[j].name), room, "");
						int pad = 36 - (int)s.size() - (go.empty() ? 0 : (int)go.size() + 1);
						printf("  %s%s" S_GRAY "%s" S_WHITE "%s", s.c_str(), go.empty() ? "" : " ", go.c_str(),
								std::string(pad > 0 ? pad : 0, ' ').c_str());
					}
					printf("\r\n");
				}
				if ( i + 32 < list.size() ) {
					printf(ESC_ENG);
					printf("  " S_GRAY "번호를 넣거나 Enter (다음 쪽) >> " S_WHITE);
					char b[8];
					line_input(b, 3);
					if ( strlen(b) ) { c = b; break; }
				} else {
					c = ask("번호 (Enter: 취소) >> ", 3);
				}
			}
			if ( c.empty() || c == "?" ) continue;		// 목록만 보고 다시 묻기
			int k = atoi(c.c_str());
			if ( k >= 1 && k <= (int)list.size() ) { out = list[k - 1]; return true; }
			continue;
		}
		for ( unsigned int i = 0; i < list.size(); i++ ) {
			if ( !strcasecmp(list[i].id.c_str(), c.c_str()) || !strcasecmp(list[i].go.c_str(), c.c_str()) ) {
				out = list[i];
				return true;
			}
		}
		msg(S_RED, "그런 게시판이 없습니다. ? 로 목록을 보세요.");
	}
}

static std::map<std::string, std::string> article(const board_info &b, int no)
{
	rows_t r = rows_of("SELECT NO, FAMILY, STEP, USER_ID, DATE_TIME, TITLE FROM " + b.id + " WHERE NO=" + TO_STRING(no));
	return r.empty() ? std::map<std::string, std::string>() : r[0];
}

static void print_article_line(const std::map<std::string, std::string> &a0)
{
	std::map<std::string, std::string> a = a0;
	printf("  %5s  %-12s %s  %s\r\n", a["NO"].c_str(), string_truncate(nick_of(a["USER_ID"]), 12, "").c_str(),
			a["DATE_TIME"].substr(2, 8).c_str(), string_truncate(display_text(a["TITLE"]), 44, "").c_str());
}

// 첨부 표의 파일 이름: "file..." 또는 번호 폴더까지 "001/file..." (그 밖은 건드리지 않는다)
static bool safe_attach_name(const std::string &f)
{
	if ( f.empty() || f[0] == '/' || f.find("..") != std::string::npos ) return false;
	std::string::size_type sl = f.find('/');
	return sl == std::string::npos || f.find('/', sl + 1) == std::string::npos;
}

// 글 하나와 그 꼬리말, 첨부 (파일까지), 고정을 지운다
static void remove_article(const board_info &b, int no)
{
	std::string n = TO_STRING(no);
	rows_t at = rows_of("SELECT FILENAME FROM attachment WHERE FAMILY_TABLE='" + esc(b.id) + "' AND FAMILY_ID=" + n);
	for ( unsigned int i = 0; i < at.size(); i++ ) {
		std::string f = at[i]["FILENAME"];
		if ( safe_attach_name(f) ) unlink((hanulso() + "/file/" + f).c_str());
	}
	mysql_query(mysql, ("DELETE FROM attachment WHERE FAMILY_TABLE='" + esc(b.id) + "' AND FAMILY_ID=" + n).c_str());
	mysql_query(mysql, ("DELETE FROM comments WHERE BOARD='" + esc(b.id) + "' AND ARTICLE=" + n).c_str());
	mysql_query(mysql, ("DELETE FROM board_pin WHERE BOARD='" + esc(b.id) + "' AND NO=" + n).c_str());
	mysql_query(mysql, ("DELETE FROM " + b.id + " WHERE NO=" + n).c_str());
}

static void move_articles(void)
{
	print_header(S_CYAN "운영자 - 게시물 옮기기" S_WHITE);
	printf("\r\n  글을 다른 게시판으로 옮깁니다. 꼬리말과 첨부도 함께 옮깁니다.\r\n");
	printf("  " S_GRAY "첫 글을 옮기면 그 글에 달린 답글도 함께 옮깁니다." S_WHITE "\r\n");
	board_info from, to;
	if ( !choose_board("어느 게시판에서", from) ) return;
	int no = atoi(ask("글 번호 >> ", 8).c_str());
	std::map<std::string, std::string> a = article(from, no);
	if ( a.empty() ) { msg(S_RED, "그런 글이 없습니다."); wait_enter(); return; }
	printf("\r\n");
	print_article_line(a);
	if ( !choose_board("어느 게시판으로", to) ) return;
	if ( to.id == from.id ) { msg(S_RED, "같은 게시판입니다."); wait_enter(); return; }
	if ( !database::create_board((char*)to.id.c_str()) ) return;

	// 옮길 글들: 첫 글이면 같은 묶음 전체, 답글이면 그 글만 (첫 글이 되어)
	bool root = atoi(a["STEP"].c_str()) == 0;
	rows_t list = root ? rows_of("SELECT NO FROM " + from.id + " WHERE FAMILY=" + a["FAMILY"] + " ORDER BY ORDERBY")
		: rows_of("SELECT NO FROM " + from.id + " WHERE NO=" + TO_STRING(no));
	if ( !confirm(TO_STRING(list.size()) + " 개의 글을 '" + display_text(to.name) + "' 로 옮길까요?") ) return;

	int new_family = 0;
	for ( unsigned int i = 0; i < list.size(); i++ ) {
		std::string o = list[i]["NO"];
		std::string q = "INSERT INTO " + to.id + " (FAMILY, ORDERBY, STEP, USER_ID, DATE_TIME, HIT, RECOMMEND, TITLE, CONTENT) "
			"SELECT 0, " + std::string(root ? "ORDERBY, STEP" : "0, 0") + ", USER_ID, DATE_TIME, HIT, RECOMMEND, TITLE, CONTENT FROM " +
			from.id + " WHERE NO=" + o;
		if ( mysql_query(mysql, q.c_str()) != 0 ) { msg(S_RED, "옮기지 못했습니다."); wait_enter(); return; }
		int nn = (int)mysql_insert_id(mysql);
		if ( i == 0 ) new_family = nn;
		std::string n = TO_STRING(nn);
		mysql_query(mysql, ("UPDATE " + to.id + " SET FAMILY=" + TO_STRING(new_family) + " WHERE NO=" + n).c_str());
		mysql_query(mysql, ("UPDATE comments SET BOARD='" + esc(to.id) + "', ARTICLE=" + n + " WHERE BOARD='" + esc(from.id) +
					"' AND ARTICLE=" + o).c_str());
		mysql_query(mysql, ("UPDATE attachment SET FAMILY_TABLE='" + esc(to.id) + "', FAMILY_ID=" + n + " WHERE FAMILY_TABLE='" +
					esc(from.id) + "' AND FAMILY_ID=" + o).c_str());
		mysql_query(mysql, ("DELETE FROM board_pin WHERE BOARD='" + esc(from.id) + "' AND NO=" + o).c_str());
		mysql_query(mysql, ("DELETE FROM " + from.id + " WHERE NO=" + o).c_str());
	}
	msg(S_GREEN, "옮겼습니다. 새 번호는 " + TO_STRING(new_family) + " 번입니다.");
	wait_enter();
}

static void delete_articles(void)
{
	print_header(S_CYAN "운영자 - 게시물 지우기" S_WHITE);
	printf("\r\n  " S_RED "지운 글은 되살릴 수 없습니다." S_WHITE " 꼬리말과 첨부 파일도 함께 지웁니다.\r\n");
	board_info b;
	if ( !choose_board("게시판", b) ) return;
	std::string r = ask("글 번호 (예: 3 또는 3-5) >> ", 15);
	int a = 0, z = 0;
	int n = sscanf(r.c_str(), "%d-%d", &a, &z);
	if ( n < 1 ) return;
	if ( n == 1 ) z = a;
	if ( z < a || z - a > 200 ) { msg(S_RED, "번호가 잘못되었습니다 (한 번에 200 개까지)."); wait_enter(); return; }
	rows_t list = rows_of("SELECT NO, FAMILY, STEP, USER_ID, DATE_TIME, TITLE FROM " + b.id + " WHERE NO BETWEEN " +
			TO_STRING(a) + " AND " + TO_STRING(z) + " ORDER BY NO");
	if ( list.empty() ) { msg(S_RED, "그런 글이 없습니다."); wait_enter(); return; }
	printf("\r\n");
	for ( unsigned int i = 0; i < list.size() && i < 10; i++ ) print_article_line(list[i]);
	if ( list.size() > 10 ) printf("  " S_GRAY "... 그 밖에 %d 개" S_WHITE "\r\n", (int)list.size() - 10);
	if ( !confirm(TO_STRING(list.size()) + " 개의 글을 지울까요?") ) return;
	for ( unsigned int i = 0; i < list.size(); i++ ) remove_article(b, atoi(list[i]["NO"].c_str()));
	msg(S_GREEN, "지웠습니다.");
	wait_enter();
}

static void pins(void)
{
	board_info b;
	print_header(S_CYAN "운영자 - 공지 고정" S_WHITE);
	printf("\r\n  고정한 글은 게시판 첫 쪽 맨 위에 [공지] 로 보입니다. 게시판마다 %d 개까지.\r\n", MAX_PINS);
	if ( !choose_board("게시판", b) ) return;
	while ( 1 ) {
		print_header((S_CYAN "운영자 - 공지 고정: " S_WHITE + display_text(b.name)).c_str());
		rows_t p = rows_of("SELECT P.NO, A.USER_ID, A.DATE_TIME, A.TITLE FROM board_pin P JOIN " + b.id +
				" A ON A.NO = P.NO WHERE P.BOARD='" + esc(b.id) + "' ORDER BY P.DATE_TIME DESC");
		printf("\r\n");
		if ( p.empty() ) printf("  " S_GRAY "고정한 글이 없습니다." S_WHITE "\r\n");
		for ( unsigned int i = 0; i < p.size(); i++ ) print_article_line(p[i]);
		std::string c = ask("A 번호: 고정하기  R 번호: 풀기  Enter: 돌아가기 >> ", 12);
		if ( c.empty() ) return;
		char op = toupper(c[0]);
		int no = atoi(trim(c.substr(1)).c_str());
		if ( no <= 0 ) no = atoi(ask("글 번호 >> ", 8).c_str());
		if ( op == 'A' ) {
			if ( (int)p.size() >= MAX_PINS ) { msg(S_RED, "더 고정할 수 없습니다. 먼저 하나를 푸세요."); wait_enter(); continue; }
			if ( article(b, no).empty() ) { msg(S_RED, "그런 글이 없습니다."); wait_enter(); continue; }
			mysql_query(mysql, ("REPLACE INTO board_pin (BOARD, NO, DATE_TIME) VALUES ('" + esc(b.id) + "', " +
						TO_STRING(no) + ", NOW())").c_str());
		} else if ( op == 'R' ) {
			mysql_query(mysql, ("DELETE FROM board_pin WHERE BOARD='" + esc(b.id) + "' AND NO=" + TO_STRING(no)).c_str());
		}
	}
}

static void delete_comments(void)
{
	print_header(S_CYAN "운영자 - 꼬리말 지우기" S_WHITE);
	board_info b;
	printf("\r\n");
	if ( !choose_board("게시판", b) ) return;
	int no = atoi(ask("글 번호 >> ", 8).c_str());
	std::map<std::string, std::string> a = article(b, no);
	if ( a.empty() ) { msg(S_RED, "그런 글이 없습니다."); wait_enter(); return; }
	while ( 1 ) {
		print_header(S_CYAN "운영자 - 꼬리말 지우기" S_WHITE);
		printf("\r\n");
		print_article_line(a);
		rows_t r = rows_of("SELECT * FROM comments WHERE BOARD='" + esc(b.id) + "' AND ARTICLE=" + TO_STRING(no) +
				" ORDER BY NO LIMIT 15");
		printf("\r\n");
		if ( r.empty() ) printf("  " S_GRAY "꼬리말이 없습니다." S_WHITE "\r\n");
		for ( unsigned int i = 0; i < r.size(); i++ ) {
			printf("  %3d. " S_CYAN "%-10s" S_WHITE " %s\r\n", i + 1, string_truncate(nick_of(r[i]["USER_ID"]), 10, "").c_str(),
					string_truncate(display_text(r[i]["TEXT"]), 58, "").c_str());
		}
		std::string c = ask("지울 번호 (Enter: 돌아가기) >> ", 3);
		int k = atoi(c.c_str());
		if ( k < 1 || k > (int)r.size() ) return;
		if ( confirm("이 꼬리말을 지울까요?") ) {
			mysql_query(mysql, ("DELETE FROM comments WHERE NO=" + r[k - 1]["NO"]).c_str());
		}
	}
}

static void polls(void)
{
	while ( 1 ) {
		print_header(S_CYAN "운영자 - 투표 관리" S_WHITE);
		rows_t r = rows_of("SELECT P.*, (SELECT COUNT(*) FROM poll_vote V WHERE V.POLL = P.NO) AS VOTES FROM poll P "
				"ORDER BY P.NO DESC LIMIT 15");
		printf("\r\n  " S_GRAY "%5s  %-44s %6s  %s" S_WHITE "\r\n", "번호", "질문", "표", "상태");
		if ( r.empty() ) printf("  " S_GRAY "      투표가 없습니다." S_WHITE "\r\n");
		for ( unsigned int i = 0; i < r.size(); i++ ) {
			bool closed = atoi(r[i]["CLOSED"].c_str()) != 0;
			printf("  %5s  %-44s %6s  %s\r\n", r[i]["NO"].c_str(), string_truncate(display_text(r[i]["QUESTION"]), 44, "").c_str(),
					r[i]["VOTES"].c_str(), closed ? S_GRAY "마감" S_WHITE : S_GREEN "진행" S_WHITE);
		}
		std::string c = ask("E 번호: 마감  D 번호: 지우기  Enter: 돌아가기 >> ", 10);
		if ( c.empty() ) return;
		char op = toupper(c[0]);
		std::string no = TO_STRING(atoi(trim(c.substr(1)).c_str()));
		if ( no == "0" ) continue;
		if ( op == 'E' ) {
			mysql_query(mysql, ("UPDATE poll SET CLOSED=1 WHERE NO=" + no).c_str());
		} else if ( op == 'D' && confirm(no + " 번 투표를 지울까요? (표도 함께)") ) {
			mysql_query(mysql, ("DELETE FROM poll_vote WHERE POLL=" + no).c_str());
			mysql_query(mysql, ("DELETE FROM poll WHERE NO=" + no).c_str());
		}
	}
}

// ------------------------------------------------------------------
// 서버 정리와 점검
// ------------------------------------------------------------------
static std::string run1(const std::string &cmd)
{
	bool ok;
	std::vector<std::string> l = exec_command((char*)cmd.c_str(), &ok);
	return l.empty() ? "" : trim(l[0]);
}

// file/ 에 있지만 첨부 표에 없는 파일들
static std::vector<std::string> orphan_files(long *bytes)
{
	std::set<std::string> known;
	rows_t r = rows_of("SELECT FILENAME FROM attachment");
	for ( unsigned int i = 0; i < r.size(); i++ ) known.insert(r[i]["FILENAME"]);
	std::vector<std::string> out;
	*bytes = 0;
	// file/ 바로 아래 (예전) 와 번호 폴더 file/000/ ... 안
	std::string base = hanulso() + "/file/";
	std::vector<std::string> files = find_files((char*)(base + "file*").c_str());
	std::vector<std::string> sub = find_files((char*)(base + "[0-9][0-9][0-9]*/file*").c_str());
	files.insert(files.end(), sub.begin(), sub.end());
	for ( unsigned int i = 0; i < files.size(); i++ ) {
		std::string name = files[i].substr(base.size());		// "file..." 또는 "001/file..."
		if ( known.count(name) ) continue;
		struct stat st;
		if ( stat(files[i].c_str(), &st) != 0 || !S_ISREG(st.st_mode) ) continue;
		// 지금 올리는 중일 수 있는 새 파일은 빼고 (하루 지난 것만)
		if ( time(NULL) - st.st_mtime < 86400 ) continue;
		*bytes += st.st_size;
		out.push_back(files[i]);
	}
	return out;
}

// 번호 폴더들에서 이름이 f 인 파일 ("NNN/f", 없으면 "")
static std::string find_in_buckets(const std::string &f)
{
	std::string base = hanulso() + "/file/";
	std::vector<std::string> hit = find_files((char*)(base + "[0-9][0-9][0-9]*/" + f).c_str());
	return hit.empty() ? "" : hit[0].substr(base.size());
}

static bool same_file(const std::string &a, const std::string &b)
{
	struct stat x, y;
	return stat(a.c_str(), &x) == 0 && stat(b.c_str(), &y) == 0 && x.st_dev == y.st_dev && x.st_ino == y.st_ino;
}

// 자동 끊기 (ctime: 10 분 동안 키 입력이 없으면 끊는다) 에 걸리지 않게 터미널의 마지막 입력 시각을 지금으로
static void keep_alive(void)
{
	struct stat st;
	if ( fstat(0, &st) != 0 ) return;
	struct timeval tv[2];
	gettimeofday(&tv[0], NULL);
	tv[1].tv_sec = st.st_mtime;
	tv[1].tv_usec = 0;
	futimes(0, tv);
}

// 예전처럼 file/ 바로 아래에 있는 첨부를 1000 개씩 번호 폴더로 옮기고 표의 이름도 고친다.
// 한 파일씩: 새 이름으로 link -> 표를 고침 -> 옛 이름을 지움. 표를 못 고치면 새 이름을 지운다.
// 한 파일을 처리하는 동안은 끊김 신호 (SIGHUP) 를 미뤄 파일 사이에서만 멈춘다.
// 예전 판 (rename 뒤 표 고치기) 이 그 사이에 끊겨 파일만 옮겨지고 표는 옛 이름인 것은 찾아서 표를 고친다.
static void split_attachments(int total)
{
	rows_t r = rows_of("SELECT NO, FILENAME FROM attachment WHERE FILENAME NOT LIKE '%/%' ORDER BY NO");
	int moved = 0, missing = 0, failed = 0, repaired = 0;
	std::string base = hanulso() + "/file/";
	sigset_t hup, old;
	sigemptyset(&hup);
	sigaddset(&hup, SIGHUP);
	printf("\r\n");
	for ( unsigned int i = 0; i < r.size(); i++ ) {
		if ( i % 50 == 0 ) keep_alive();
		std::string f = r[i]["FILENAME"];
		if ( !safe_attach_name(f) ) { failed++; continue; }
		std::string from = base + f;
		std::string found = find_in_buckets(f);
		struct stat st;

		sigprocmask(SIG_BLOCK, &hup, &old);
		if ( stat(from.c_str(), &st) != 0 ) {
			// 옛 자리에 없다: 번호 폴더로 이미 옮겨졌으면 표만 고친다
			if ( !found.empty() && mysql_query(mysql, ("UPDATE attachment SET FILENAME='" + esc(found) + "' WHERE NO=" +
						r[i]["NO"]).c_str()) == 0 ) repaired++;
			else missing++;
			sigprocmask(SIG_SETMASK, &old, NULL);
			continue;
		}
		std::string nf;
		bool linked_now = false;
		if ( !found.empty() && same_file(from, base + found) ) {
			nf = found;			// 지난번에 link 까지 하고 끊긴 것
		} else {
			std::string bucket = attachment_bucket();
			if ( bucket.empty() ) { failed++; sigprocmask(SIG_SETMASK, &old, NULL); break; }
			nf = bucket + "/" + f;
			// link 는 같은 이름이 이미 있으면 실패한다 (rename 처럼 덮지 않는다)
			if ( link(from.c_str(), (base + nf).c_str()) != 0 ) { failed++; sigprocmask(SIG_SETMASK, &old, NULL); continue; }
			linked_now = true;
		}
		std::string q = "UPDATE attachment SET FILENAME='" + esc(nf) + "' WHERE NO=" + r[i]["NO"];
		if ( mysql_query(mysql, q.c_str()) != 0 || mysql_affected_rows(mysql) != 1 ) {
			if ( linked_now ) unlink((base + nf).c_str());
			failed++;
			sigprocmask(SIG_SETMASK, &old, NULL);
			continue;
		}
		unlink(from.c_str());
		sigprocmask(SIG_SETMASK, &old, NULL);

		moved++;
		if ( moved % 100 == 0 ) {
			printf("\r  " S_GRAY "%d / %d 개 옮김..." S_WHITE, moved, total);
			fflush(stdout);
		}
	}
	printf("\r\033[K");
	msg(S_GREEN, TO_STRING(moved) + " 개를 번호 폴더로 옮겼습니다.");
	if ( repaired ) msg(S_GREEN, TO_STRING(repaired) + " 개는 지난번에 파일만 옮겨진 것이라 표를 고쳤습니다.");
	if ( missing ) msg(S_GRAY, TO_STRING(missing) + " 개는 파일이 없어 그대로 두었습니다 (표에만 남은 첨부).");
	if ( failed ) msg(S_RED, TO_STRING(failed) + " 개는 옮기지 못했습니다 (폴더 권한, 또는 번호 폴더에 같은 이름의 다른 파일).");
}

static std::string human(long b)
{
	char s[32];
	if ( b >= 1024L * 1024 * 1024 ) snprintf(s, sizeof(s), "%.1fG", b / 1024.0 / 1024 / 1024);
	else if ( b >= 1024L * 1024 ) snprintf(s, sizeof(s), "%.1fM", b / 1024.0 / 1024);
	else if ( b >= 1024 ) snprintf(s, sizeof(s), "%.1fK", b / 1024.0);
	else snprintf(s, sizeof(s), "%ldB", b);
	return s;
}

static void maintenance(void)
{
	while ( 1 ) {
		std::string h = shell_quote(hanulso());
		print_header(S_CYAN "운영자 - 정리와 점검" S_WHITE);

		printf("\r\n  " S_YELLOW "◆ 디스크" S_WHITE "\r\n");
		std::string df = run1("df -hP " + h + " | tail -1 | awk '{print $2\" 중 \"$3\" 사용 (\"$5\"), 남은 곳 \"$4}'");
		printf("      %s\r\n", df.c_str());
		printf("      BBS 폴더 %s,  첨부 (file) %s\r\n", run1("du -sh " + h + " 2>/dev/null | cut -f1").c_str(),
				run1("du -sh " + h + "/file 2>/dev/null | cut -f1").c_str());

		printf("\r\n  " S_YELLOW "◆ 임시 파일 (tmp)" S_WHITE "\r\n");
		std::string all = run1("find " + h + "/tmp -mindepth 1 -maxdepth 1 | wc -l");
		std::string old = run1("find " + h + "/tmp -mindepth 1 -maxdepth 1 ! -name '*.tty' ! -name '.*' ! -name 'stats.cache' "
				"! -name 'mailsend.log' -mmin +1440 | wc -l");
		printf("      모두 %s 개 (%s),  하루 지난 것 %s 개\r\n", all.c_str(),
				run1("du -sh " + h + "/tmp 2>/dev/null | cut -f1").c_str(), old.c_str());

		long ob;
		std::vector<std::string> orphans = orphan_files(&ob);
		printf("\r\n  " S_YELLOW "◆ 주인 없는 첨부 파일" S_WHITE "\r\n");
		printf("      %d 개 (%s)  " S_GRAY "글이 지워졌는데 남은 파일" S_WHITE "\r\n", (int)orphans.size(), human(ob).c_str());
		int flat = query_int("SELECT COUNT(*) FROM attachment WHERE FILENAME NOT LIKE '%/%'");
		int nb = (int)find_files((char*)(hanulso() + "/file/[0-9][0-9][0-9]*").c_str()).size();
		printf("      번호 폴더 %d 개" S_GRAY " (1000 개씩)" S_WHITE ",  아직 file/ 바로 아래에 있는 첨부 %d 개\r\n", nb, flat);

		// AI 와 이야기: 오늘 물은 수 (data/aichat/<아이디> 의 "날짜 수")
		std::vector<std::string> ai = find_files((hanulso() + "/data/aichat/*").c_str());
		char today[16];
		time_t t = time(NULL);
		strftime(today, sizeof(today), "%Y-%m-%d", localtime(&t));
		int ai_total = 0, ai_users = 0, ai_top = 0;
		std::string ai_top_id;
		for ( unsigned int i = 0; i < ai.size(); i++ ) {
			char d[16] = "";
			int n = 0;
			if ( sscanf(read_file(ai[i].c_str()).c_str(), "%15s %d", d, &n) == 2 && today == std::string(d) ) {
				ai_total += n;
				ai_users++;
				if ( n > ai_top ) { ai_top = n; ai_top_id = split_file_name(ai[i]); }
			}
		}
		printf("\r\n  " S_YELLOW "◆ AI 와 이야기 (오늘)" S_WHITE "\r\n");
		printf("      %d 명이 %d 번 물었습니다.", ai_users, ai_total);
		if ( ai_top > 0 ) printf("  가장 많이: %s (%d 번)", nick_of(ai_top_id).c_str(), ai_top);
		printf("\r\n");

		std::string c = ask("T: 임시 파일 정리  F: 주인 없는 첨부 지우기  M: 첨부 폴더 나누기  Enter >> ", 2);
		if ( c.empty() ) return;
		if ( !strcasecmp(c.c_str(), "t") ) {
			unlink((hanulso() + "/tmp/.sweep").c_str());		// 한 시간 막음 풀기
			sweep_stale_tmp();
			msg(S_GREEN, "하루 지난 임시 파일을 정리했습니다.");
			wait_enter();
		} else if ( !strcasecmp(c.c_str(), "m") && flat > 0 ) {
			printf("\r\n  첨부 %d 개를 1000 개씩 번호 폴더 (file/000, file/001 ...) 로 옮깁니다.", flat);
			printf("\r\n  " S_GRAY "옮기는 동안 그 파일을 받는 사람이 있으면 실패할 수 있으니 한가한 때에 하세요." S_WHITE);
			if ( confirm("옮길까요?") ) {
				split_attachments(flat);
				wait_enter();
			}
		} else if ( !strcasecmp(c.c_str(), "f") && !orphans.empty() ) {
			if ( confirm(TO_STRING(orphans.size()) + " 개 (" + human(ob) + ") 를 지울까요?") ) {
				for ( unsigned int i = 0; i < orphans.size(); i++ ) unlink(orphans[i].c_str());
				msg(S_GREEN, "지웠습니다.");
				wait_enter();
			}
		}
	}
}

// ------------------------------------------------------------------
// 차림표
// ------------------------------------------------------------------
// ------------------------------------------------------------------
// 네이버 카페 글 가져오기 (bin/cafeimport) / 카페 회원 연결
// ------------------------------------------------------------------
static std::string cafe_dir(void)
{
	return hanulso() + "/data/cafe";
}

// 카페 글이 올라가는 게시판들 (hanulso.cfg 옆의 table.txt)
static std::vector<std::string> cafe_tables(void)
{
	std::vector<std::string> out;
	std::vector<std::string> lines = split_string(read_file((hanulso() + "/table.txt").c_str()), '\n');
	for (unsigned int i=0; i<lines.size(); i++) {
		std::string l = lines[i];
		size_t c = l.find(';');
		if ( c != std::string::npos ) l = l.substr(0, c);
		int id; char t[256];
		if ( sscanf(l.c_str(), "%d %255s", &id, t) == 2 &&
		     std::find(out.begin(), out.end(), std::string(t)) == out.end() )
			out.push_back(t);
	}
	return out;
}

// from 아이디로 올라간 카페 글과 첨부를 to 회원으로 옮긴다. 옮긴 글 수
static int move_cafe_articles(const std::string &from, const std::string &to)
{
	std::vector<std::string> tables = cafe_tables();
	int n = 0;
	for (unsigned int i=0; i<tables.size(); i++) {
		std::string q = "UPDATE " + tables[i] + " SET USER_ID='" + esc(to) + "' WHERE USER_ID='" + esc(from) + "'";
		if ( mysql_query(mysql, q.c_str()) == 0 ) n += (int)mysql_affected_rows(mysql);
		q = "UPDATE attachment SET USER_ID='" + esc(to) + "' WHERE USER_ID='" + esc(from)
			+ "' AND FAMILY_TABLE='" + esc(tables[i]) + "'";
		mysql_query(mysql, q.c_str());
	}
	return n;
}

// 연결할 BBS 회원 묻기 (카페에서 가져온 회원이 아닌 진짜 회원만)
static std::string ask_bbs_member(const std::string &def)
{
	std::string prompt = "BBS 아이디" + (def.empty() ? std::string("") : " [" + def + "]") + " >> ";
	std::string id = ask(prompt.c_str(), 40);
	if ( id.empty() ) id = def;
	if ( id.empty() ) return "";
	if ( query_int("SELECT COUNT(*) FROM member WHERE USER_ID='" + esc(id) + "' AND PASSWORD <> '!'") != 1 ) {
		msg(S_RED, "'" + id + "' 는 BBS 회원이 아닙니다. (카페에서 가져온 회원에는 연결할 수 없습니다)");
		wait_enter();
		return "";
	}
	return id;
}

struct cafe_unlinked {
	std::string masked, nick, key, cands, now;
};

static void cafe_link(void)
{
	while ( 1 ) {
		// 연결 후보: 같은 회원은 마지막 줄만, 이미 연결된 회원은 빼고
		std::vector<std::string> lines = split_string(read_file((cafe_dir() + "/unlinked.log").c_str()), '\n');
		std::vector<cafe_unlinked> list;
		std::vector<std::string> seen;
		for (int i=(int)lines.size()-1; i>=0; i--) {
			std::vector<std::string> f = split_string(lines[i], '\t');
			if ( f.size() < 3 || std::find(seen.begin(), seen.end(), f[2]) != seen.end() ) continue;
			seen.push_back(f[2]);
			if ( !trim(read_file((cafe_dir() + "/links/" + f[2]).c_str())).empty() ) continue;
			cafe_unlinked u;
			u.masked = f[0]; u.nick = f[1]; u.key = f[2]; u.cands = (f.size() > 3) ? f[3] : "";
			u.now = trim(read_file((cafe_dir() + "/members/" + f[2]).c_str()));
			list.push_back(u);
		}

		print_header(S_CYAN "카페 회원 연결" S_WHITE);
		printf("\r\n  네이버는 매니저에게도 카페 아이디를 가려서 줍니다 (coma****).");
		printf("\r\n  같은 사람의 BBS 아이디를 정해 주면 그 사람의 카페 글이 BBS 회원 글로 올라갑니다.\r\n\r\n");
		if ( list.empty() ) printf("  " S_GRAY "연결을 기다리는 카페 회원이 없습니다." S_WHITE "\r\n");
		for (unsigned int i=0; i<list.size() && i<15; i++) {
			printf("  %2d. %-12s %-16s " S_GRAY "후보: %s" S_WHITE "\r\n", i + 1, list[i].now.c_str(),
				string_truncate(list[i].nick, 16, "").c_str(),
				list[i].cands.empty() ? "없음" : string_truncate(list[i].cands, 36, "...").c_str());
		}
		if ( list.size() > 15 ) printf("  " S_GRAY "... 외 %d 명 (연결하면 다음 사람이 보입니다)" S_WHITE "\r\n", (int)list.size() - 15);
		printf("\r\n  " S_GRAY "M: 예전에 가져온 글을 가려진 아이디로 옮기기 (juya**** -> BBS 아이디)" S_WHITE);

		std::string c = ask("번호 / M (끝: Enter) >> ", 3);
		if ( c.empty() ) return;

		if ( !strcasecmp(c.c_str(), "m") ) {
			std::string from = ask("옮길 카페 아이디 (예 juya****) >> ", 40);
			if ( from.empty() || from.find('*') == std::string::npos ) continue;
			std::string to = ask_bbs_member("");
			if ( to.empty() ) continue;
			if ( !confirm(from + " 의 카페 글을 " + to + " 회원 글로 옮길까요?") ) continue;
			char b[64]; snprintf(b, sizeof(b), "%d", move_cafe_articles(from, to));
			msg(S_GREEN, std::string("글 ") + b + " 개를 옮겼습니다.");
			wait_enter();
			continue;
		}

		int n = atoi(c.c_str());
		if ( n < 1 || n > (int)list.size() || n > 15 ) continue;
		cafe_unlinked &u = list[n - 1];
		std::string def = u.cands.substr(0, u.cands.find(','));
		printf("\r\n  %s (%s)", u.now.c_str(), u.nick.c_str());
		std::string to = ask_bbs_member(def);
		if ( to.empty() ) continue;
		if ( !confirm(u.now + " (" + u.nick + ") = BBS 회원 " + to + " 로 연결할까요?") ) continue;

		FILE *fp = fopen((cafe_dir() + "/links/" + u.key).c_str(), "w");
		if ( !fp ) { msg(S_RED, "기록하지 못했습니다: " + cafe_dir() + "/links"); wait_enter(); continue; }
		fputs(to.c_str(), fp);
		fclose(fp);
		int moved = u.now.empty() ? 0 : move_cafe_articles(u.now, to);
		char b[64]; snprintf(b, sizeof(b), "%d", moved);
		msg(S_GREEN, "연결했습니다. 이미 올라간 글 " + std::string(b) + " 개를 " + to + " 회원 글로 옮겼습니다.");
		wait_enter();
	}
}

static void cafe_menu(void);

static void cafe_import(void)
{
	print_header(S_CYAN "네이버 카페 글 받기" S_WHITE);
	printf("\r\n  카페 글 번호 범위를 넣으면 글/작성자/그림/첨부를 data/cafe/download/ 에 받아 둡니다.");
	printf("\r\n  BBS 는 아직 바뀌지 않습니다. '받은 글 확인' 에서 보고 'BBS 에 올리기' 로 올리세요.");
	printf("\r\n  " S_GRAY "이미 올린 글, 이미 받은 글은 건너뜁니다. 필요: hanulso.cfg 의 <naver><cookie>" S_WHITE "\r\n");

	printf("  " S_GRAY "103001-103050 처럼 범위, 103001- 처럼 끝을 비우면 최근 글까지" S_WHITE "\r\n");
	{
		// 어디서부터 받을지: 지금까지 올린 카페 글 번호 중 가장 큰 것
		int last = 0;
		const char *logs[] = { "/data/cafe/imported.log", "/restore.log", NULL };
		for (int k=0; logs[k]; k++) {
			std::vector<std::string> l = split_string(read_file((hanulso() + logs[k]).c_str()), '\n');
			for (unsigned int i=0; i<l.size(); i++) if ( atoi(l[i].c_str()) > last ) last = atoi(l[i].c_str());
		}
		if ( last ) printf("  지금까지 올린 카페 글의 마지막 번호: " S_CYAN "%d" S_WHITE "  (이어 받으려면 %d-)\r\n", last, last + 1);
	}
	std::string r = ask("글 번호 (예: 103001-103050 103100) >> ", 120);
	if ( r.empty() ) return;
	// 숫자, '-', 공백만
	for (unsigned int i=0; i<r.size(); i++) {
		if ( !isdigit((unsigned char)r[i]) && r[i] != '-' && r[i] != ' ' ) {
			msg(S_RED, "숫자와 '-' 만 쓸 수 있습니다.");
			wait_enter();
			return;
		}
	}
	if ( !confirm("받을까요?") ) return;

	std::string cmd = hanulso() + "/bin/cafeimport " + shell_quote(tty) + " fetch";
	std::vector<std::string> parts = split_string(r, ' ');
	for (unsigned int i=0; i<parts.size(); i++) {
		if ( !trim(parts[i]).empty() ) cmd += " " + shell_quote(trim(parts[i]));
	}
	printf("\r\n\r\n");
	fflush(stdout);
	system(cmd.c_str());
	wait_enter();
}

// 이미 올린 글의 본문을 카페에서 다시 받아 고치기 (예전 받기에서 본문 한글이 깨진 글)
static void cafe_redo(void)
{
	print_header(S_CYAN "올린 카페 글 본문 다시 받기" S_WHITE);
	printf("\r\n  BBS 에 올린 카페 글의 본문을 카페에서 다시 받아 그 자리에서 고칩니다.");
	printf("\r\n  글 번호, 조회수, 첨부 파일은 그대로입니다. 깨진 첨부 이름(%%EB%%8B..)도 풀어 줍니다.");
	printf("\r\n  " S_GRAY "번호를 비우면 올린 글 모두 (data/cafe/imported.log)" S_WHITE "\r\n");
	std::string r = ask("글 번호 (예: 90500-90600, 비우면 모두) >> ", 120);
	for (unsigned int i=0; i<r.size(); i++) {
		if ( !isdigit((unsigned char)r[i]) && r[i] != '-' && r[i] != ' ' ) {
			msg(S_RED, "숫자와 '-' 만 쓸 수 있습니다.");
			wait_enter();
			return;
		}
	}
	if ( !confirm("고칠까요?") ) return;
	std::string cmd = hanulso() + "/bin/cafeimport " + shell_quote(tty) + " redo";
	std::vector<std::string> parts = split_string(r, ' ');
	for (unsigned int i=0; i<parts.size(); i++) {
		if ( !trim(parts[i]).empty() ) cmd += " " + shell_quote(trim(parts[i]));
	}
	printf("\r\n\r\n");
	fflush(stdout);
	system(cmd.c_str());
	wait_enter();
}

static void cafeimport_run(const std::string &args)
{
	std::string cmd = hanulso() + "/bin/cafeimport " + shell_quote(tty) + " " + args;
	fflush(stdout);
	system(cmd.c_str());
}

// 받은 글 확인: 목록, 본문 보기, 빼기
static void cafe_review(void)
{
	while ( 1 ) {
		print_header(S_CYAN "받은 카페 글" S_WHITE);
		printf("\r\n");
		cafeimport_run("list");
		std::string c = ask("번호: 본문 보기, D 번호: 빼기 (끝: Enter) >> ", 12);
		if ( c.empty() ) return;
		if ( c[0] == 'd' || c[0] == 'D' ) {
			int n = atoi(c.c_str() + 1);
			if ( n <= 0 ) continue;
			char b[64]; snprintf(b, sizeof(b), "%d 번 글을 뺄까요? (BBS 에 올리지 않음)", n);
			if ( !confirm(b) ) continue;
			snprintf(b, sizeof(b), "drop %d", n);
			printf("\r\n");
			cafeimport_run(b);
			wait_enter();
			continue;
		}
		int n = atoi(c.c_str());
		if ( n <= 0 ) continue;
		print_header(S_CYAN "받은 카페 글" S_WHITE);
		printf("\r\n");
		char b[64]; snprintf(b, sizeof(b), "show %d", n);
		cafeimport_run(b);
		wait_enter();
	}
}

// 받아 둔 글을 BBS 에 올리기
static void cafe_apply(void)
{
	print_header(S_CYAN "받은 카페 글을 BBS 에 올리기" S_WHITE);
	printf("\r\n");
	cafeimport_run("list");
	printf("\r\n  " S_GRAY "작성자는 지금 연결 상태로 올립니다 ('카페 회원 연결' 을 먼저 하면 그 회원 글로)." S_WHITE);
	if ( !confirm("받아 둔 글을 모두 BBS 에 올릴까요?") ) return;
	printf("\r\n\r\n");
	cafeimport_run("apply");
	wait_enter();
}

static void cafe_menu(void)
{
	while ( 1 ) {
		print_header(S_CYAN "네이버 카페" S_WHITE);
		printf("\r\n       1. 카페 글 받기     " S_GRAY "(글 번호 범위, BBS 는 그대로)" S_WHITE "\r\n");
		printf("       2. 받은 글 확인     " S_GRAY "(보기, 빼기)" S_WHITE "\r\n");
		printf("       3. BBS 에 올리기    " S_GRAY "(받아 둔 글을 게시판에)" S_WHITE "\r\n");
		printf("       4. 카페 회원 연결   " S_GRAY "(카페 회원 = BBS 회원)" S_WHITE "\r\n");
		printf("       5. 올린 글 고치기   " S_GRAY "(본문을 카페에서 다시 받아 덮어씀)" S_WHITE "\r\n");
		std::string c = ask("번호 (끝: Enter) >> ", 2);
		if ( c == "1" ) cafe_import();
		else if ( c == "2" ) cafe_review();
		else if ( c == "3" ) cafe_apply();
		else if ( c == "4" ) cafe_link();
		else if ( c == "5" ) cafe_redo();
		else return;
	}
}

// ------------------------------------------------------------------
// DB 백업: $HANULSO/backup/bbs-YYYYMMDD-HHMM.sql.gz (mysqldump | gzip)
//   표는 latin1 에 완성형 바이트를 넣어 두므로 latin1 로 받아 바이트를 그대로 둔다.
//   비밀번호는 명령줄에 드러나지 않게 임시 설정 파일 (--defaults-extra-file) 로 넘긴다.
//   받는 동안은 뒤에서 돌리고 크기를 보여 주며, 자동 끊기에 걸리지 않게 한다.
// ------------------------------------------------------------------
static std::string backup_dir(void)
{
	return hanulso() + "/backup";
}

static long file_bytes(const std::string &path)
{
	struct stat st;
	return stat(path.c_str(), &st) == 0 ? (long)st.st_size : -1;
}

static std::vector<std::string> backup_files(void)
{
	std::vector<std::string> f = find_files((char*)(backup_dir() + "/bbs-*.sql.gz").c_str());
	std::sort(f.rbegin(), f.rend());		// 새것 먼저 (이름이 날짜순)
	return f;
}

static bool run_backup(std::string &out_path, std::string &err, const std::string &table = "")
{
	mkdir(backup_dir().c_str(), 0700);
	chmod(backup_dir().c_str(), 0700);		// 회원 정보가 들어 있으니 BBS 계정만

	char stamp[32];
	time_t t = time(NULL);
	strftime(stamp, sizeof(stamp), "%Y%m%d-%H%M%S", localtime(&t));
	out_path = backup_dir() + (table.empty() ? "/bbs-" : "/drop-" + table + "-") + stamp + ".sql.gz";
	std::string tmp = out_path + ".part";

	char conf[] = "/tmp/sysop_my.XXXXXX";
	int fd = mkstemp(conf);
	if ( fd < 0 ) { err = "임시 파일을 만들지 못했습니다"; return false; }
	std::string c = std::string("[client]\nuser=") + db_user + "\npassword=\"" + db_passwd + "\"\nhost=" + db_host + "\n";
	write(fd, c.data(), c.size());
	close(fd);

	std::string cmd = "set -o pipefail; mysqldump --defaults-extra-file=" + std::string(conf) +
		" --single-transaction --quick --default-character-set=latin1 " + shell_quote(db_name) +
		(table.empty() ? "" : " " + shell_quote(table)) +
		" 2>" + shell_quote(tmp + ".err") + " | gzip > " + shell_quote(tmp);

	pid_t pid = fork();
	if ( pid == 0 ) {
		umask(0077);
		execl("/bin/bash", "bash", "-c", cmd.c_str(), (char*)0);
		_exit(127);
	}
	int status = -1;
	long last = -1;
	time_t started = time(NULL);
	while ( pid > 0 ) {
		pid_t r = waitpid(pid, &status, WNOHANG);
		if ( r == pid ) break;
		if ( r < 0 ) { status = -1; break; }
		keep_alive();
		long b = file_bytes(tmp);
		if ( b != last ) {
			printf("\r  " S_GRAY "받는 중... %s (%ld 초)" S_WHITE "\033[K", human(b < 0 ? 0 : b).c_str(), (long)(time(NULL) - started));
			fflush(stdout);
			last = b;
		}
		usleep(500000);
	}
	unlink(conf);
	printf("\r\033[K");

	std::string e = trim(read_file((tmp + ".err").c_str()));
	unlink((tmp + ".err").c_str());
	if ( pid <= 0 || !WIFEXITED(status) || WEXITSTATUS(status) != 0 || file_bytes(tmp) <= 0 ) {
		unlink(tmp.c_str());
		err = e.empty() ? "mysqldump 가 실패했습니다" : string_truncate(e, 70, "");
		return false;
	}
	rename(tmp.c_str(), out_path.c_str());
	chmod(out_path.c_str(), 0600);
	return true;
}

// 백업 파일을 회원 쪽으로 보낸다 (자료실 받기와 같은 프로토콜). 임시 폴더에 본래 이름으로 링크를 걸고 보낸다
static int ask_send_protocol(void)
{
	std::string c = ask("보낼 프로토콜 (1:Xmodem 2:Ymodem 3:Zmodem 4:Kermit, Enter:Zmodem) >> ", 1);
	if ( c.empty() ) return 3;
	int p = atoi(c.c_str());
	return (p >= 1 && p <= 4) ? p : 0;
}

static bool send_backup(const std::string &path, int protocol)
{
	std::string name = split_file_name(path);
	char dir[1024];
	snprintf(dir, sizeof(dir), "%s/tmp/sysopdl.XXXXXX", hanulso().c_str());
	if ( mkdtemp(dir) == NULL ) return false;
	std::string link_path = std::string(dir) + "/" + name;
	if ( symlink(path.c_str(), link_path.c_str()) != 0 ) { rmdir(dir); return false; }

	std::string q = shell_quote("./" + name);
	std::string cmd;
	if ( protocol == 1 ) cmd = "sz --xmodem -e " + q;
	else if ( protocol == 2 ) cmd = "sz --ymodem -e " + q;
	else if ( protocol == 4 ) {
		char k[1024];
		snprintf(k, sizeof(k), KERMIT_PROG " -i -s %s", getenv("HANULSO"), q.c_str());
		cmd = k;
	}
	else cmd = "sz --zmodem -e " + q;

	printf("\r\n  %s (%s) 를 보냅니다. 받기를 시작하세요.\r\n", name.c_str(), human(file_bytes(path)).c_str());
	fflush(stdout);
	chdir(dir);
	ioctl(0, TCSETAF, &sys_term);
	int a = system(cmd.c_str());
	raw_mode();
	chdir(hanulso().c_str());
	unlink(link_path.c_str());
	rmdir(dir);
	return WIFEXITED(a) && WEXITSTATUS(a) == 0;
}

static void backup_menu(void)
{
	while ( 1 ) {
		print_header(S_CYAN "운영자 - DB 백업" S_WHITE);
		std::vector<std::string> files = backup_files();
		printf("\r\n  " S_GRAY "DB (%s) 전체를 backup/ 에 받습니다. 첨부 파일 (file/) 은 들어가지 않습니다." S_WHITE "\r\n\r\n", db_name);
		printf("  " S_GRAY "%3s  %-30s %10s" S_WHITE "\r\n", "", "파일", "크기");
		if ( files.empty() ) printf("  " S_GRAY "     아직 백업이 없습니다." S_WHITE "\r\n");
		long total = 0;
		for ( unsigned int i = 0; i < files.size(); i++ ) {
			long b = file_bytes(files[i]);
			total += b > 0 ? b : 0;
			if ( i < 10 ) printf("  %3d  %-30s %10s\r\n", i + 1, split_file_name(files[i]).c_str(), human(b).c_str());
		}
		if ( files.size() > 10 ) printf("  " S_GRAY "     ... 그 밖에 %d 개" S_WHITE "\r\n", (int)files.size() - 10);
		if ( !files.empty() ) printf("  " S_GRAY "     모두 %d 개, %s" S_WHITE "\r\n", (int)files.size(), human(total).c_str());
		printf("\r\n  " S_GRAY "되살리기 (서버에서): gunzip < backup/파일 | mysql -u 아이디 -p %s" S_WHITE "\r\n", db_name);

		std::string c = ask("B: 지금 백업  R 번호: 내려받기  D 번호: 지우기  Enter >> ", 6);
		if ( c.empty() ) return;
		if ( !strcasecmp(c.c_str(), "b") ) {
			if ( !confirm("지금 DB 를 백업할까요?") ) continue;
			printf("\r\n");
			std::string path, err;
			if ( run_backup(path, err) ) {
				msg(S_GREEN, "백업했습니다: " + split_file_name(path) + " (" + human(file_bytes(path)) + ")");
			} else {
				msg(S_RED, "백업하지 못했습니다: " + err);
			}
			wait_enter();
		} else if ( toupper(c[0]) == 'R' ) {
			int k = atoi(trim(c.substr(1)).c_str());
			if ( k < 1 || k > (int)files.size() ) {
				if ( files.empty() ) continue;
				k = atoi(ask("내려받을 번호 >> ", 3).c_str());
				if ( k < 1 || k > (int)files.size() ) continue;
			}
			int protocol = ask_send_protocol();
			if ( protocol == 0 ) continue;
			if ( send_backup(files[k - 1], protocol) ) msg(S_GREEN, "보냈습니다.");
			else msg(S_RED, "보내지 못했습니다.");
			wait_enter();
		} else if ( toupper(c[0]) == 'D' ) {
			int k = atoi(trim(c.substr(1)).c_str());
			if ( k >= 1 && k <= (int)files.size() && confirm(split_file_name(files[k - 1]) + " 을 지울까요?") ) {
				unlink(files[k - 1].c_str());
			}
		}
	}
}

// ------------------------------------------------------------------
// DB 테이블 정리: 소스 (src/) 와 메뉴 (*.mnu) 어디에도 이름이 나오지 않는 테이블을 찾아 지운다
// ------------------------------------------------------------------
struct table_info { std::string name, rows, bytes, updated; bool used; };

// src/ 의 소스와 *.mnu 에 나오는 낱말 (테이블 이름 후보) 을 모은다. 읽지 못하면 false
static bool used_words(std::set<std::string> &words)
{
	std::string home = shell_quote(hanulso());
	std::string cmd = "cd " + home + " && { grep -rhoE '[A-Za-z_][A-Za-z0-9_]*' "
		"--include='*.cpp' --include='*.c' --include='*.h' --include='*.py' --include='*.sh' src; "
		"cat *.mnu | grep -oE '[A-Za-z_][A-Za-z0-9_]*'; } 2>/dev/null | sort -u";
	FILE *fp = popen(cmd.c_str(), "r");
	if ( !fp ) return false;
	char buf[256];
	while ( fgets(buf, sizeof(buf), fp) ) words.insert(trim(buf));
	pclose(fp);
	return words.size() > 500 && words.count("member");		// 소스를 제대로 읽었는지
}

static bool safe_table_name(const std::string &t)
{
	if ( t.empty() || t.size() > 64 ) return false;
	for ( unsigned int i = 0; i < t.size(); i++ ) if ( !isalnum((unsigned char)t[i]) && t[i] != '_' ) return false;
	return true;
}

static void drop_tables(void)
{
	bool all = false;
	int page = 0;
	while ( 1 ) {
		std::set<std::string> words;
		bool ok = used_words(words);
		rows_t r = rows_of("SELECT TABLE_NAME AS N, IFNULL(TABLE_ROWS, 0) AS R, IFNULL(DATA_LENGTH + INDEX_LENGTH, 0) AS B, "
			"IFNULL(DATE_FORMAT(IFNULL(UPDATE_TIME, CREATE_TIME), '%Y-%m-%d'), '') AS U "
			"FROM information_schema.TABLES WHERE TABLE_SCHEMA = DATABASE() AND TABLE_TYPE = 'BASE TABLE' ORDER BY TABLE_NAME");
		std::vector<table_info> list;
		int unused = 0;
		for ( unsigned int i = 0; i < r.size(); i++ ) {
			table_info t;
			t.name = r[i]["N"];
			t.rows = r[i]["R"];
			t.bytes = human(atol(r[i]["B"].c_str()));
			t.updated = r[i]["U"];
			t.used = !ok || words.count(t.name);
			if ( !t.used ) unused++;
			if ( all || !t.used ) list.push_back(t);
		}
		const int PER = 13;
		int pages = list.empty() ? 1 : ((int)list.size() + PER - 1) / PER;
		if ( page >= pages ) page = pages - 1;
		if ( page < 0 ) page = 0;

		print_header(S_CYAN "운영자 - DB 테이블 정리" S_WHITE);
		if ( ok ) {
			printf("\r\n  " S_GRAY "테이블 %d 개 가운데 소스 (src/) 와 메뉴 (*.mnu) 에 이름이 없는 것: " S_YELLOW "%d" S_GRAY " 개" S_WHITE "\r\n",
				(int)r.size(), unused);
			printf("  " S_GRAY "지우기 전에 backup/ 에 받아 둡니다. 쓰는 테이블은 지울 수 없습니다." S_WHITE "\r\n\r\n");
		} else {
			printf("\r\n  " S_RED "src/ 를 읽지 못해 어떤 테이블을 쓰는지 알 수 없습니다. 지우기는 막아 둡니다." S_WHITE "\r\n\r\n\r\n");
		}
		printf("  " S_GRAY "%3s  %-34s %9s %9s  %-10s %s" S_WHITE "\r\n", "", "테이블", "행 (약)", "크기", "바뀐 날", "");
		if ( list.empty() ) printf("  " S_GRAY "     %s" S_WHITE "\r\n", all ? "테이블이 없습니다." : "지울 만한 테이블이 없습니다. (A: 모두 보기)");
		for ( int i = page * PER; i < (int)list.size() && i < (page + 1) * PER; i++ ) {
			const table_info &t = list[i];
			printf("  %3d  %s%-34s" S_WHITE " %9s %9s  %-10s %s\r\n", i + 1, t.used ? S_WHITE : S_YELLOW,
				string_truncate(t.name, 34, "").c_str(), t.rows.c_str(), t.bytes.c_str(), t.updated.c_str(),
				t.used ? "" : S_YELLOW "안 씀" S_WHITE);
		}

		std::string q = std::string("D 번호: 지우기  A: ") + (all ? "안 쓰는 것만" : "모두 보기");
		if ( pages > 1 ) q += "  N/P: 쪽 (" + TO_STRING(page + 1) + "/" + TO_STRING(pages) + ")";
		q += "  Enter >> ";
		std::string c = ask(q.c_str(), 6);
		if ( c.empty() ) return;
		if ( !strcasecmp(c.c_str(), "a") ) { all = !all; page = 0; continue; }
		if ( !strcasecmp(c.c_str(), "n") ) { page++; continue; }
		if ( !strcasecmp(c.c_str(), "p") ) { page--; continue; }
		if ( toupper(c[0]) != 'D' ) continue;

		int k = atoi(trim(c.substr(1)).c_str());
		if ( k < 1 || k > (int)list.size() ) {
			if ( list.empty() ) continue;
			k = atoi(ask("지울 번호 >> ", 3).c_str());
			if ( k < 1 || k > (int)list.size() ) continue;
		}
		table_info t = list[k - 1];
		if ( !ok ) { msg(S_RED, "src/ 를 읽지 못해 지울 수 없습니다."); wait_enter(); continue; }
		if ( t.used ) { msg(S_RED, t.name + " 는 소스나 메뉴에서 쓰는 테이블이라 지울 수 없습니다."); wait_enter(); continue; }
		if ( !safe_table_name(t.name) ) { msg(S_RED, "이름에 영문, 숫자, _ 가 아닌 글자가 있어 여기서는 지울 수 없습니다."); wait_enter(); continue; }

		int count = query_int("SELECT COUNT(*) FROM `" + t.name + "`");
		printf("\r\n  " S_YELLOW "%s" S_WHITE ": 행 %d 개, %s, 바뀐 날 %s\r\n", t.name.c_str(), count, t.bytes.c_str(), t.updated.c_str());
		printf("  " S_RED "지운 테이블은 backup/ 에 받아 둔 파일로만 되살릴 수 있습니다." S_WHITE "\r\n");
		std::string typed = ask("지우려면 테이블 이름을 그대로 치세요 (Enter: 취소) >> ", 64);
		if ( typed != t.name ) { if ( !typed.empty() ) { msg(S_RED, "이름이 달라 지우지 않았습니다."); wait_enter(); } continue; }

		printf("\r\n");
		std::string path, err;
		if ( !run_backup(path, err, t.name) ) {
			msg(S_RED, "백업하지 못해 지우지 않았습니다: " + err);
			wait_enter();
			continue;
		}
		std::string dq = "DROP TABLE `" + t.name + "`";
		if ( mysql_query(mysql, dq.c_str()) != 0 ) {
			msg(S_RED, std::string("지우지 못했습니다: ") + mysql_error(mysql));
		} else {
			msg(S_GREEN, "지웠습니다. 받아 둔 파일 (" + human(file_bytes(path)) + "):");
			printf("\r\n    backup/%s", split_file_name(path).c_str());
			printf("\r\n  " S_GRAY "되살리기 (서버에서): gunzip < backup/위 파일 | mysql -u 아이디 -p %s" S_WHITE, db_name);
		}
		wait_enter();
	}
}

static void main_menu(void)
{
	while ( 1 ) {
		print_header(S_CYAN "운영자 메뉴" S_WHITE);
		int online = online_users().size();
		int susp = query_int("SELECT COUNT(*) FROM member_suspend WHERE UNTIL IS NULL OR UNTIL > NOW()");
		printf("\r\n");
		printf("  " S_YELLOW "◆ 접속자" S_WHITE "  " S_GRAY "(지금 %d 명)" S_WHITE "\r\n", online);
		printf("       1. 지금 접속자 / 강제 종료       2. 전체 공지 방송\r\n");
		printf("       3. 로그인 공지 고치기\r\n");
		printf("  " S_YELLOW "◆ 회원" S_WHITE "  " S_GRAY "(정지 %d 명)" S_WHITE "\r\n", susp);
		printf("       4. 회원 찾기 / 등급 / 비밀번호   5. 이용 정지 / 풀기\r\n");
		printf("       6. 가입 / 접속 통계              7. 회원 삭제\r\n");
		printf("      15. 아이디 바꾸기 " S_GRAY "(아이디나 닉네임으로 찾기, 카페 회원을 진짜 아이디로)" S_WHITE "\r\n");
		printf("  " S_YELLOW "◆ 게시판" S_WHITE "\r\n");
		printf("       8. 게시물 옮기기                 9. 게시물 지우기\r\n");
		printf("      10. 공지 고정 / 풀기             11. 꼬리말 지우기\r\n");
		printf("      12. 투표 마감 / 지우기           14. 네이버 카페 글 가져오기\r\n");
		printf("  " S_YELLOW "◆ 서버" S_WHITE "\r\n");
		printf("      13. 정리와 점검 " S_GRAY "(디스크, 임시 파일, 주인 없는 첨부, AI 사용량)" S_WHITE "\r\n");
		{
			std::vector<std::string> bf = backup_files();
			std::string last = bf.empty() ? "아직 없음" : split_file_name(bf[0]).substr(4, 13);
			printf("      16. DB 백업 " S_GRAY "(마지막: %s)" S_WHITE "\r\n", last.c_str());
		}
		printf("      17. DB 테이블 정리 " S_GRAY "(쓰지 않는 테이블 지우기)" S_WHITE "\r\n");

		std::string c = ask("번호 (끝내기: X) >> ", 3);
		if ( !strcasecmp(c.c_str(), "x") || !strcasecmp(c.c_str(), "p") || !strcasecmp(c.c_str(), "q") ) return;
		switch ( atoi(c.c_str()) ) {
		case 1: show_online(); break;
		case 2: broadcast(); break;
		case 3: login_notice(); break;
		case 4: members(); break;
		case 5: suspensions(); break;
		case 6: member_stats(); break;
		case 7: delete_member(); break;
		case 8: move_articles(); break;
		case 9: delete_articles(); break;
		case 10: pins(); break;
		case 11: delete_comments(); break;
		case 12: polls(); break;
		case 13: maintenance(); break;
		case 14: cafe_menu(); break;
		case 15: rename_member(); break;
		case 16: backup_menu(); break;
		case 17: drop_tables(); break;
		}
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

    umask(0022);

	if ( database::open() == false )
		exit(1);
	create_tables();

	// 운영자 아이디: 이 접속의 접속자 파일에서
	std::string me;
	read_tty_file(hanulso() + "/tmp/" + tty + ".tty", me);
	snprintf(login_user_id, sizeof(login_user_id), "%s", me.c_str());
	if ( me.empty() || !is_sysop(me) ) {
		printf("\r\n운영자만 쓸 수 있습니다.");
		wait_enter();
		host_close();
	}

	main_menu();

	host_close();
	return 0;
}
