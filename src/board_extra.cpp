#include "main.h"
#include <sys/statvfs.h>
#include <utmp.h>

// ------------------------------------------------------------------
// 게시판 덧붙임: 꼬리말(한 줄 댓글), 새 글 모아보기(NEW), 전체 게시판 검색(FIND)
// ------------------------------------------------------------------

#define X_W		"\033[=15F"
#define X_Y		"\033[=14F"
#define X_C		"\033[=11F"
#define X_G		"\033[=7F"
#define X_R		"\033[=12F"

#define COMMENT_LEN	46		// 꼬리말 한 줄 (바이트, 한글 23 자)

void comments_init(void)
{
	mysql_query(mysql, "CREATE TABLE IF NOT EXISTS comments ( "
			"NO INTEGER NOT NULL AUTO_INCREMENT PRIMARY KEY, "
			"BOARD VARCHAR(64) NOT NULL, "
			"ARTICLE INT NOT NULL, "
			"USER_ID VARCHAR(50) NOT NULL, "
			"TEXT VARCHAR(255) NOT NULL, "
			"DATE_TIME DATETIME NOT NULL, "
			"KEY IDX_ARTICLE (BOARD, ARTICLE) )");
}

static std::string nick_or_id(const std::string &id)
{
	static std::map<std::string, std::string> cache;
	std::map<std::string, std::string>::iterator it = cache.find(id);
	if ( it != cache.end() ) return it->second;
	bool exist;
	std::map<std::string, std::string> u = database::user_info((char*)id.c_str(), &exist);
	std::string n = exist ? display_text(u["NICK_NAME"]) : "";
	if ( exist && n.empty() ) n = display_text(id);
	if ( n.empty() ) n = "탈퇴회원";
	cache[id] = n;
	return n;
}

// 글 목록에 쓸 꼬리말 수
int comment_count(const char *table, int no)
{
	char q[512];
	bool ok;
	snprintf(q, sizeof(q), "SELECT COUNT(*) FROM comments WHERE BOARD='%s' AND ARTICLE=%d",
			database::escape(table).c_str(), no);
	return atoi(database::fetch(q, &ok).c_str());
}

static std::vector<std::map<std::string, std::string> > fetch_comments(const char *table, int no)
{
	char q[512];
	snprintf(q, sizeof(q), "SELECT * FROM comments WHERE BOARD='%s' AND ARTICLE=%d ORDER BY NO",
			database::escape(table).c_str(), no);
	return database::fetch_rows(q);
}

// 글 아래에 이어 붙일 꼬리말 줄들 (본문처럼 쪽을 넘긴다)
// 한 줄: 1 + 번호 3 + 1 + 닉네임 12 + 1 + 꼬리말 46 + 2 + 날짜 11 = 77 칸
std::vector<std::string> comment_lines(const char *table, int no)
{
	std::vector<std::string> out;
	std::vector<std::map<std::string, std::string> > r = fetch_comments(table, no);
	out.push_back("");
	char head[128];
	if ( r.empty() ) snprintf(head, sizeof(head), X_G "── 꼬리말이 없습니다. CO 로 달아 보세요 ──" X_W);
	else snprintf(head, sizeof(head), X_G "── 꼬리말 %d개 (CO 내용: 달기, CD 번호: 내 꼬리말 지우기) ──" X_W, (int)r.size());
	out.push_back(head);
	for ( unsigned int i = 0; i < r.size(); i++ ) {
		std::string t = r[i]["DATE_TIME"];
		std::string date = t.size() >= 16 ? t.substr(5, 5) + " " + t.substr(11, 5) : "";
		char line[512];
		snprintf(line, sizeof(line), " %3d " X_C "%-12s" X_W " %-46s  " X_G "%s" X_W, (int)i + 1,
				string_truncate(nick_or_id(r[i]["USER_ID"]), 12, "").c_str(),
				string_truncate(display_text(r[i]["TEXT"]), COMMENT_LEN, "").c_str(), date.c_str());
		out.push_back(line);
	}
	return out;
}

// 꼬리말 달기 (text 가 비었으면 묻는다). 달았으면 true
bool add_comment(const char *table, int no, std::string text)
{
	if ( text.empty() ) {
		char buf[COMMENT_LEN + 8];
		printf(ESC_HAN);
		printf("\r\n꼬리말 (한 줄, Enter: 취소) >> ");
		line_input(buf, COMMENT_LEN);
		printf(ESC_ENG);
		text = buf;
	}
	text = string_truncate(trim(display_text(text)), COMMENT_LEN, "");
	if ( text.empty() ) return false;

	// 도배 막기: 5 초 안에 또
	bool ok;
	char q0[512];
	snprintf(q0, sizeof(q0), "SELECT COUNT(*) FROM comments WHERE USER_ID='%s' AND DATE_TIME > NOW() - INTERVAL 5 SECOND",
			database::escape(login_user_id).c_str());
	if ( atoi(database::fetch(q0, &ok).c_str()) > 0 ) {
		printf("\r\n잠시 후에 다시 달아 주세요.\r\n[Enter] 를 누르세요.");
		press_enter();
		return false;
	}

	std::string q = "INSERT INTO comments (BOARD, ARTICLE, USER_ID, TEXT, DATE_TIME) VALUES ('" +
		database::escape(table) + "', " + TO_STRING(no) + ", '" + database::escape(login_user_id) + "', '" +
		database::escape(text.c_str()) + "', NOW())";
	if ( mysql_query(mysql, q.c_str()) != 0 ) return false;

	// 글쓴이가 접속해 있으면 알린다 (대화방 안에서도)
	char q2[512];
	snprintf(q2, sizeof(q2), "SELECT CONCAT(USER_ID, CHAR(9), TITLE) FROM %s WHERE NO=%d", table, no);
	std::string r = database::fetch(q2, &ok);
	std::string::size_type tab = r.find('\t');
	if ( ok && tab != std::string::npos ) {
		std::string author = r.substr(0, tab);
		if ( author != login_user_id ) {
			notify_online(author, "◆ 꼬리말 ─ " + nick_or_id(login_user_id) + " 님이 '" +
					string_truncate(display_text(r.substr(tab + 1)), 30, "..") + "' 에 꼬리말을 달았습니다.", true);
		}
	}
	return true;
}

// 꼬리말 지우기 (index 는 화면의 번호). 내 것만, 운영자는 아무거나
void delete_comment(const char *table, int no, int index)
{
	std::vector<std::map<std::string, std::string> > r = fetch_comments(table, no);
	if ( index < 1 || index > (int)r.size() ) {
		printf("\r\n그런 꼬리말이 없습니다.\r\n[Enter] 를 누르세요.");
		press_enter();
		return;
	}
	if ( r[index - 1]["USER_ID"] != login_user_id && !login_user_is_admin ) {
		printf("\r\n내 꼬리말만 지울 수 있습니다.\r\n[Enter] 를 누르세요.");
		press_enter();
		return;
	}
	std::string q = "DELETE FROM comments WHERE NO=" + r[index - 1]["NO"];
	mysql_query(mysql, q.c_str());
}

// 글을 지울 때 그 글의 꼬리말도
void delete_comments_of(const char *table, int no)
{
	char q[512];
	snprintf(q, sizeof(q), "DELETE FROM comments WHERE BOARD='%s' AND ARTICLE=%d", database::escape(table).c_str(), no);
	mysql_query(mysql, q);
}

// ------------------------------------------------------------------
// 여러 게시판에서 모은 글 목록 (NEW, FIND, BEST, MY)
// ------------------------------------------------------------------
pugi::xml_node menu_root;
std::string current_board;		// 지금 들어가 있는 게시판 (SUB 로 구독할 때)

board_scope::board_scope(const char *id) : old(current_board) { current_board = id; }
board_scope::~board_scope() { current_board = old; }

// ------------------------------------------------------------------
// 지금 있는 곳 (US)
// ------------------------------------------------------------------
std::string current_where;

static void write_where(const std::string &w)
{
	std::string path = std::string(getenv("HANULSO")) + "/tmp/" + tty + ".where";
	FILE *fp = fopen(path.c_str(), "w");
	if ( fp ) { fprintf(fp, "%s\n", w.c_str()); fclose(fp); }
}

where_scope::where_scope(const std::string &where) : old(current_where)
{
	current_where = where;
	write_where(where);
}

where_scope::~where_scope()
{
	current_where = old;
	write_where(old);
}

// 메뉴 항목의 이름에서 뒤의 " (go 이름)" 은 뗀다. 이름이 없으면 go 이름 (top 은 첫 화면)
std::string node_where(pugi::xml_node node)
{
	std::string name = trim(node.child("name").child_value());
	std::string::size_type p = name.rfind(" (");
	if ( p != std::string::npos && p > 0 && name[name.size() - 1] == ')' ) name = name.substr(0, p);
	if ( !name.empty() ) return name;
	std::string go = node.attribute("go").value();
	if ( go == "top" || go.empty() ) return "첫 화면";
	return go;
}

// 화면 칸 폭에 맞춰 자르고 채운다 (완성형 한 글자 = 2 칸 = 2 바이트)
static std::string pad_to(const std::string &s, int width)
{
	std::string t = string_truncate(s, width, "");
	if ( (int)t.size() < width ) t += std::string(width - t.size(), ' ');
	return t;
}

static std::string short_time(long sec)
{
	char buf[32];
	if ( sec < 3600 ) snprintf(buf, sizeof(buf), "%ld분", sec / 60);
	else snprintf(buf, sizeof(buf), "%ld:%02ld", sec / 3600, (sec / 60) % 60);
	return buf;
}

struct online_row { std::string id, nick, where; long on, idle; bool me; };

// US: 접속중인 회원, 있는 곳, 접속한 지, 쉬고 있는 시간. 번호로 회원 정보
void show_online_users(void)
{
	int page = 0;
	while ( 1 ) {
		std::vector<online_row> list;
		char pattern[1024];
		snprintf(pattern, sizeof(pattern), "%s/tmp/*.tty", getenv("HANULSO"));
		std::vector<std::string> files = find_files_time_sorted(pattern);
		for ( unsigned int i = 0; i < files.size(); i++ ) {
			online_row u;
			if ( !read_tty_file(files[i], u.id) ) continue;
			bool exist;
			std::map<std::string, std::string> m = database::user_info((char*)u.id.c_str(), &exist);
			u.nick = exist ? display_text(m["NICK_NAME"]) : u.id;
			std::string node = split_string(split_file_name(files[i]), '.')[0];
			u.me = node == tty;
			u.on = (long)((ms_time_now() - file_ms_mtime(files[i])) / 1000);
			// 터미널의 마지막 입력 시각 (w 명령의 IDLE 과 같은 방법)
			struct stat st;
			u.idle = stat(("/dev/pts/" + node).c_str(), &st) == 0 ? (long)(time(NULL) - st.st_atime) : 0;
			std::string base = files[i].substr(0, files[i].size() - 4);		// .tty 를 뗀 것
			u.where = display_text(trim(read_file((base + ".where").c_str())));
			if ( u.where.empty() ) u.where = "-";
			list.push_back(u);
		}

		const int PER = 14;
		int pages = list.empty() ? 1 : ((int)list.size() + PER - 1) / PER;
		if ( page >= pages ) page = pages - 1;
		if ( page < 0 ) page = 0;

		printf(ESC_CLEAR);
		print_news_title("접속중인 회원");
		printf("\033[5;1H");
		printf("  \033[=7F지금 \033[=14F%d\033[=7F 명이 접속해 있습니다.\033[=15F\r\n\r\n", (int)list.size());
		printf("  \033[=7F%3s  %s %s %6s %6s\033[=15F\r\n", "", pad_to("회원", 24).c_str(), pad_to("있는 곳", 28).c_str(), "접속", "쉼");
		for ( int i = page * PER; i < (int)list.size() && i < (page + 1) * PER; i++ ) {
			const online_row &u = list[i];
			std::string name = string_truncate(u.nick, 12, "") + "(" + u.id + ")";
			printf("  %s%3d  %s \033[=11F%s\033[=15F %6s %6s%s\r\n", u.me ? "\033[=14F" : "\033[=15F", i + 1,
				pad_to(name, 24).c_str(), pad_to(u.where, 28).c_str(), short_time(u.on).c_str(),
				u.idle >= 60 ? short_time(u.idle).c_str() : "", u.me ? "" : "");
			printf("\033[=15F");
		}

		std::string q = "\r\n  번호: 회원 정보";
		if ( pages > 1 ) {
			char buf[64];
			snprintf(buf, sizeof(buf), "  N/P: 쪽 (%d/%d)", page + 1, pages);
			q += buf;
		}
		q += "  R: 새로 보기  Enter: 끝 >> ";
		printf(ESC_ENG);
		printf("%s", q.c_str());
		char cmd[16];
		line_input(cmd, 3);
		std::string c = trim(cmd);
		if ( c.empty() ) return;
		if ( !strcasecmp(c.c_str(), "n") ) { page++; continue; }
		if ( !strcasecmp(c.c_str(), "p") ) { page--; continue; }
		if ( !strcasecmp(c.c_str(), "r") ) continue;
		int k = atoi(c.c_str());
		if ( k < 1 || k > (int)list.size() ) continue;
		printf("\r\n");
		print_user_info((char*)list[k - 1].id.c_str());
		printf("\r\n[Enter] 를 누르세요.");
		press_enter();
	}
}

struct found_article {
	std::string table, board, user_id, date_time, title;
	std::string col;		// 날짜 칸 대신 보일 것 (BEST: 추천 / 조회)
	std::string mark;		// 제목 앞 표시 (MY: 새 꼬리말)
	int no, hit, recommend;
	pugi::xml_node node;
};

static bool newer_first(const found_article &a, const found_article &b)
{
	return a.date_time > b.date_time;
}

// 인기: 추천 하나를 조회 10 으로 친다
static bool best_first(const found_article &a, const found_article &b)
{
	int sa = a.recommend * 10 + a.hit, sb = b.recommend * 10 + b.hit;
	if ( sa != sb ) return sa > sb;
	return a.date_time > b.date_time;
}

// 이 회원이 들어갈 수 있는 게시판들 (노드)
static void readable_boards(pugi::xml_node node, std::vector<pugi::xml_node> &out)
{
	for ( pugi::xml_node c = node.first_child(); c; c = c.next_sibling() ) {
		if ( c.type() != pugi::node_element ) continue;
		if ( !strcmp(c.attribute("type").value(), "board") && !c.attribute("id").empty() ) {
			int level = atoi(c.attribute("access_level").value());
			if ( login_user_is_admin || level <= login_user_level ) out.push_back(c);
		}
		readable_boards(c, out);
	}
}

static pugi::xml_node board_node(const std::string &table)
{
	std::vector<pugi::xml_node> boards;
	if ( menu_root ) readable_boards(menu_root, boards);
	for ( unsigned int i = 0; i < boards.size(); i++ ) {
		if ( table == boards[i].attribute("id").value() ) return boards[i];
	}
	return pugi::xml_node();
}

// 메뉴에서 그 게시판이 운영자만 글을 쓰는 곳인지 (답글 RE 도 막는다)
bool board_sysop_only(const char *table)
{
	std::vector<pugi::xml_node> all;
	if ( !menu_root ) return false;
	// 등급과 상관없이 모든 게시판에서 찾는다
	std::vector<pugi::xml_node> stack(1, menu_root);
	while ( !stack.empty() ) {
		pugi::xml_node n = stack.back();
		stack.pop_back();
		for ( pugi::xml_node c = n.first_child(); c; c = c.next_sibling() ) {
			if ( c.type() != pugi::node_element ) continue;
			if ( !strcmp(c.attribute("type").value(), "board") && !strcmp(c.attribute("id").value(), table) ) {
				return !strcasecmp(c.child_value("write_sysop_only"), "yes");
			}
			stack.push_back(c);
		}
	}
	return false;
}

static std::string board_name(pugi::xml_node n)
{
	std::string s = trim(n.child_value("name"));
	std::string::size_type p = s.find(" (");
	if ( p != std::string::npos && p > 0 ) s = s.substr(0, p);
	return s.empty() ? n.attribute("id").value() : s;
}

static found_article to_found(const std::string &table, pugi::xml_node node, std::map<std::string, std::string> &row)
{
	found_article a;
	a.table = table;
	a.board = board_name(node);
	a.no = atoi(row["NO"].c_str());
	a.user_id = row["USER_ID"];
	a.date_time = row["DATE_TIME"];
	a.title = row["TITLE"];
	a.hit = atoi(row["HIT"].c_str());
	a.recommend = atoi(row["RECOMMEND"].c_str());
	a.node = node;
	return a;
}

// 게시판마다 where 조건으로 찾아 모은다 (게시판마다 limit 개까지)
static std::vector<found_article> collect(const std::string &where, int limit, const std::string &order = "NO DESC")
{
	std::vector<found_article> out;
	std::vector<pugi::xml_node> boards;
	if ( menu_root ) readable_boards(menu_root, boards);
	for ( unsigned int i = 0; i < boards.size(); i++ ) {
		std::string table = boards[i].attribute("id").value();
		std::string q = "SELECT NO, USER_ID, DATE_TIME, TITLE, HIT, RECOMMEND FROM " + table + " WHERE " + where +
			" ORDER BY " + order + " LIMIT " + TO_STRING(limit);
		std::vector<std::map<std::string, std::string> > r = database::fetch_rows((char*)q.c_str());
		for ( unsigned int k = 0; k < r.size(); k++ ) out.push_back(to_found(table, boards[i], r[k]));
	}
	std::sort(out.begin(), out.end(), newer_first);
	return out;
}

// 지난번 접속 시각 (접속해 있는 동안은 LASTLOGIN_DATETIME 이 지난번 것). 없으면 ""
static std::string previous_login(void)
{
	bool exist;
	std::map<std::string, std::string> u = database::user_info(login_user_id, &exist);
	std::string since = u["LASTLOGIN_DATETIME"];
	if ( since.size() < 10 || since.compare(0, 4, "0000") == 0 ) return "";
	return since;
}

// 그 글 읽기 (게시판의 첨부/답글 설정을 따라)
static void open_article(const found_article &a)
{
	board_scope scope(a.table.c_str());
	bool attachment = strcasecmp(a.node.child_value("attachment"), "no") != 0;
	bool reply = !strcasecmp(a.node.child_value("reply"), "yes");
	bool is_dir;
	std::string name = a.node.child_value("name");
	show_article((char*)a.table.c_str(), 1, 1, (char*)name.c_str(), a.no, attachment, reply, &is_dir);
}

// 화면 위 석 줄 (호스트 이름, 가운데 제목, 오른쪽 쪽 정보)
static void list_header(const std::string &head, const std::string &right)
{
	printf(ESC_CLEAR);
	printf("\033[1;1H");
	printf("\033[=9F\033[=1G%s\033[=15F\033[=1G", repeat("─", 40).c_str());
	printf("\033[1;1H");
	printf("\033[1A\033[7m%s\033[0m", host_name);
	int hw = strlen(strip_ansi_codes(head.c_str()));
	printf("\033[2;1H\r\033[%dC%s", (80 - hw) / 2, head.c_str());
	if ( !right.empty() ) {
		int rw = strlen(strip_ansi_codes(right.c_str()));
		printf("\r\033[%dC%s", 79 - rw, right.c_str());
	}
	printf("\033[3;1H");
	printf("\033[=0F\033[=1G%s\033[=15F\033[=1G", repeat("━", 40).c_str());
	printf("\033[4;1H");
}

// on_remove: 있으면 목록에서 "D 번호" 로 뺄 수 있다 (스크랩). 뺐으면 true
static void article_list(const std::string &head, const std::string &empty_msg, std::vector<found_article> &list,
		const std::string &col_label = "날짜", bool (*on_remove)(const found_article &) = NULL)
{
	unsigned int page = 0;
	const unsigned int per = 15;
	while ( 1 ) {
		unsigned int pages = list.empty() ? 1 : (list.size() + per - 1) / per;
		if ( page >= pages ) page = pages - 1;

		char pg[64];
		snprintf(pg, sizeof(pg), "%d/%d (총 %d건)", page + 1, pages, (int)list.size());
		list_header(head, pg);
		// 3 + 1 + 게시판 14 + 1 + 작성자 10 + 1 + 날짜 11 + 1 + 제목 35 = 77 칸
		printf("%3s %-14s %-10s %-11s %s\r\n", "", "게시판", "작성자", col_label.c_str(), "제목");
		printf("%s\r\n", repeat("─", 40).c_str());
		if ( list.empty() ) printf("%s\r\n", centered(empty_msg.c_str(), 80).c_str());
		for ( unsigned int i = page * per; i < list.size() && i < (page + 1) * per; i++ ) {
			const found_article &a = list[i];
			std::string d = !a.col.empty() ? a.col : a.date_time.size() >= 16 ? a.date_time.substr(5, 11) : a.date_time;
			int cc = comment_count(a.table.c_str(), a.no);
			std::string cs = cc > 0 ? " [" + TO_STRING(cc) + "]" : "";
			printf("%3d " X_C "%-14s" X_W " %-10s " X_G "%-11s" X_W " " X_Y "%s" X_W "%s" X_C "%s" X_W "\r\n", i + 1,
					string_truncate(a.board, 14, "").c_str(), string_truncate(nick_or_id(a.user_id), 10, "").c_str(),
					d.c_str(), a.mark.c_str(),
					string_truncate(display_text(a.title), 35 - cs.size() - a.mark.size(), "").c_str(), cs.c_str());
		}
		printf("%s\r\n", repeat("━", 40).c_str());

		char cmd[32];
		printf(ESC_ENG);
		printf(on_remove ? "읽기(번호) 빼기(D 번호) 다음(Enter/N) 이전(B) 나가기(P) >> " :
				"읽기(번호) 다음(Enter/N) 이전(B) 나가기(P) >> ");
		line_input(cmd, 10);
		std::string c = trim(cmd);
		if ( on_remove && c.size() > 1 && (c[0] == 'd' || c[0] == 'D') ) {
			int n = atoi(trim(c.substr(1)).c_str());
			if ( n >= 1 && n <= (int)list.size() && on_remove(list[n - 1]) ) list.erase(list.begin() + (n - 1));
			continue;
		}
		if ( !strcasecmp(c.c_str(), "p") || !strcasecmp(c.c_str(), "q") || !strcasecmp(c.c_str(), "x") ) return;
		if ( c.empty() || !strcasecmp(c.c_str(), "n") ) {
			if ( page + 1 < pages ) page++;
			else if ( c.empty() ) return;		// 마지막 쪽에서 Enter 면 나가기
		} else if ( !strcasecmp(c.c_str(), "b") ) {
			if ( page > 0 ) page--;
		} else if ( is_number((char*)c.c_str()) ) {
			int n = atoi(c.c_str());
			if ( n >= 1 && n <= (int)list.size() ) open_article(list[n - 1]);
		}
	}
}

// NEW : 지난번 접속 이후 새 글 (처음이면 최근 하루, 길어도 30 일)
void show_new_articles(void)
{
	std::string since = previous_login();
	std::string where;
	if ( !since.empty() ) {
		where = "DATE_TIME > GREATEST('" + database::escape(since.c_str()) + "', NOW() - INTERVAL 30 DAY)";
	} else {
		where = "DATE_TIME > NOW() - INTERVAL 1 DAY";
	}
	// 내 글은 빼고
	where += " AND USER_ID <> '" + database::escape(login_user_id) + "'";
	printf("\r\n새 글을 모으는 중입니다...");
	fflush(stdout);
	std::vector<found_article> list = collect(where, 100);
	std::string head = since.size() >= 16 ? "새 글 모아보기 (" + since.substr(5, 11) + " 이후)" : "새 글 모아보기 (최근 하루)";
	article_list(head, "지난번 접속 이후 올라온 새 글이 없습니다.", list);
}

// FIND 단어 : 모든 게시판의 제목/본문,  FIND @닉네임(아이디) : 글쓴이
void search_all_boards(std::string word)
{
	word = trim(word);
	if ( word.empty() ) {
		char buf[64];
		printf(ESC_HAN);
		printf("\r\n찾을 말 (@닉네임 이면 글쓴이로) >> ");
		line_input(buf, 40);
		printf(ESC_ENG);
		word = trim(buf);
		if ( word.empty() ) return;
	}

	std::string where, head;
	if ( word[0] == '@' ) {
		std::string who = word.substr(1);
		bool exist;
		std::string id;
		database::user_info((char*)who.c_str(), &exist);
		if ( exist ) id = who;
		else {
			std::map<std::string, std::string> u = database::user_info_by_nick_name((char*)who.c_str(), &exist);
			if ( exist ) id = u["USER_ID"];
		}
		if ( id.empty() ) {
			printf("\r\n'%s' 회원이 없습니다.\r\n[Enter] 를 누르세요.", string_truncate(display_text(who), 20, "").c_str());
			press_enter();
			return;
		}
		where = "USER_ID='" + database::escape(id.c_str()) + "'";
		head = "글쓴이 찾기: " + nick_or_id(id);
	} else {
		std::string w = database::escape(word.c_str());
		where = "(TITLE LIKE '%" + w + "%' OR CONTENT LIKE '%" + w + "%')";
		head = "모든 게시판에서 찾기: " + string_truncate(display_text(word), 30, "");
	}
	printf("\r\n모든 게시판에서 찾는 중입니다...");
	fflush(stdout);
	std::vector<found_article> list = collect(where, 50);
	article_list(head, "찾은 글이 없습니다.", list);
}

// BEST : 이번 주 인기 글,  BEST M : 이번 달,  BEST Y : 올해,  BEST A : 전체 기간
void show_best(std::string arg)
{
	std::string a = trim(arg);
	int range = 0;		// 0 주, 1 달, 2 올해, 3 전체
	if ( !strcasecmp(a.c_str(), "m") ) range = 1;
	else if ( !strcasecmp(a.c_str(), "y") ) range = 2;
	else if ( !strcasecmp(a.c_str(), "a") || !strcasecmp(a.c_str(), "all") ) range = 3;
	static const char *since[4] = { "DATE_TIME > NOW() - INTERVAL 7 DAY AND ", "DATE_TIME > NOW() - INTERVAL 30 DAY AND ",
		"DATE_TIME >= MAKEDATE(YEAR(NOW()), 1) AND ", "" };
	std::string where = std::string(since[range]) + "(HIT > 0 OR RECOMMEND > 0)";
	printf("\r\n인기 글을 모으는 중입니다...");
	fflush(stdout);
	std::vector<found_article> list = collect(where, 20, "RECOMMEND DESC, HIT DESC");
	std::sort(list.begin(), list.end(), best_first);
	if ( list.size() > 30 ) list.resize(30);
	for ( unsigned int i = 0; i < list.size(); i++ ) {
		char b[32];
		snprintf(b, sizeof(b), "%4d / %4d", list[i].recommend, list[i].hit);
		list[i].col = b;
	}
	static const char *head[4] = {
		"이번 주 인기 글 (최근 7일)  BEST M 달, Y 올해, A 전체",
		"이번 달 인기 글 (최근 30일)  BEST 주, Y 올해, A 전체",
		"올해 인기 글  BEST 주, M 달, A 전체",
		"전체 기간 인기 글  BEST 주, M 달, Y 올해" };
	article_list(head[range], "인기 글이 없습니다.", list, "추천 / 조회");
}

// 지난번 접속 이후 다른 사람이 단 꼬리말이 있으면 ◆
static void mark_new_comments(std::vector<found_article> &list)
{
	std::string since = previous_login();
	std::string after = since.empty() ? "NOW() - INTERVAL 1 DAY" : "'" + database::escape(since.c_str()) + "'";
	for ( unsigned int i = 0; i < list.size(); i++ ) {
		bool ok;
		std::string q = "SELECT COUNT(*) FROM comments WHERE BOARD='" + database::escape(list[i].table.c_str()) +
			"' AND ARTICLE=" + TO_STRING(list[i].no) + " AND USER_ID <> '" + database::escape(login_user_id) +
			"' AND DATE_TIME > " + after;
		if ( atoi(database::fetch((char*)q.c_str(), &ok).c_str()) > 0 ) list[i].mark = "◆ ";
	}
}

// ------------------------------------------------------------------
// 내 접속 기록 (LOG): 언제, 어느 노드로, 어디서 접속했고 얼마나 머물렀나
// ------------------------------------------------------------------
long login_log_no = 0;		// 이 접속의 login_log 번호 (끝낼 때 END_TIME)

// 예전 login_log 에 HOST, END_TIME 칸을 더한다 (이미 있으면 ALTER 가 실패할 뿐)
void login_log_upgrade(void)
{
	bool ok;
	std::string n = database::fetch((char*)"SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA=DATABASE() "
			"AND TABLE_NAME='login_log' AND COLUMN_NAME='HOST'", &ok);
	if ( ok && atoi(n.c_str()) == 0 ) {
		mysql_query(mysql, "ALTER TABLE login_log ADD COLUMN HOST VARCHAR(64) NOT NULL DEFAULT '', "
				"ADD COLUMN END_TIME DATETIME NULL, ADD KEY IDX_USER (USER_ID, DATE_TIME)");
	}
}

// 이 터미널 (pts/<tty>) 로 들어온 곳: 텔넷 로그인이 utmp 에 남긴 주소. 모르면 ""
std::string remote_host_of_tty(void)
{
	std::string line = std::string("pts/") + tty;
	std::string host;
	struct utmp *u;
	setutent();
	while ( (u = getutent()) != NULL ) {
		if ( u->ut_type != USER_PROCESS ) continue;
		if ( strncmp(u->ut_line, line.c_str(), sizeof(u->ut_line)) != 0 ) continue;
		host = std::string(u->ut_host, strnlen(u->ut_host, sizeof(u->ut_host)));
	}
	endutent();
	if ( host.empty() && getenv("REMOTEHOST") ) host = getenv("REMOTEHOST");
	return display_text(host);
}

void show_login_log(void)
{
	std::string id = database::escape(login_user_id);
	bool ok;
	int total = atoi(database::fetch((char*)("SELECT COUNT(*) FROM login_log WHERE USER_ID='" + id + "'").c_str(), &ok).c_str());
	int month = atoi(database::fetch((char*)("SELECT COUNT(*) FROM login_log WHERE USER_ID='" + id +
					"' AND DATE_TIME >= DATE_FORMAT(NOW(), '%Y-%m-01')").c_str(), &ok).c_str());
	std::vector<std::map<std::string, std::string> > r = database::fetch_rows((char*)("SELECT NO, NODE, HOST, "
				"DATE_FORMAT(DATE_TIME, '%Y-%m-%d %H:%i') AS T, DATE_FORMAT(DATE_TIME, '%w') AS W, "
				"TIMESTAMPDIFF(MINUTE, DATE_TIME, END_TIME) AS M FROM login_log WHERE USER_ID='" + id +
				"' ORDER BY NO DESC LIMIT 15").c_str());
	static const char *wd[] = { "일", "월", "화", "수", "목", "금", "토" };

	list_header(X_C "내 접속 기록" X_W, "");
	printf("  모두 " X_Y "%d" X_W " 번,  이번 달 " X_Y "%d" X_W " 번  " X_G "(최근 15 번)" X_W "\r\n\r\n", total, month);
	printf("  " X_G "%-21s %-8s %-30s %s" X_W "\r\n", "접속한 때", "노드", "접속한 곳", "머문 시간");
	for ( unsigned int i = 0; i < r.size(); i++ ) {
		std::string stay;
		long no = atol(r[i]["NO"].c_str());
		if ( no == login_log_no ) stay = X_Y "접속 중" X_W;
		else if ( r[i]["M"].empty() ) stay = X_G "-" X_W;		// 끊긴 때를 모름 (예전 기록, 강제 종료)
		else {
			int m = atoi(r[i]["M"].c_str());
			char b[32];
			if ( m >= 60 ) snprintf(b, sizeof(b), "%d시간 %d분", m / 60, m % 60);
			else snprintf(b, sizeof(b), "%d분", m < 1 ? 1 : m);
			stay = b;
		}
		int w = atoi(r[i]["W"].c_str());
		std::string when = r[i]["T"] + " (" + wd[w >= 0 && w < 7 ? w : 0] + ")";
		std::string host = r[i]["HOST"].empty() ? "-" : string_truncate(r[i]["HOST"], 30, "");
		printf("  %s%-21s %-8s %-30s %s" X_W "\r\n", no == login_log_no ? X_Y : X_W, when.c_str(),
				("pts/" + r[i]["NODE"]).c_str(), host.c_str(), stay.c_str());
	}
	if ( r.empty() ) printf("  " X_G "아직 기록이 없습니다." X_W "\r\n");
	printf("\r\n  " X_G "모르는 접속이 있으면 비밀번호를 바꾸고 (PE) 운영자에게 알려 주세요." X_W "\r\n");
	printf("\r\n[Enter] 를 누르세요.");
	press_enter();
}

// ------------------------------------------------------------------
// 스크랩 : 글 보기에서 SC 로 담기 / 빼기, MY S 로 모아 보기
// ------------------------------------------------------------------
void scrap_init(void)
{
	mysql_query(mysql, "CREATE TABLE IF NOT EXISTS scrap ( "
			"USER_ID VARCHAR(50) NOT NULL, "
			"BOARD VARCHAR(64) NOT NULL, "
			"ARTICLE INT NOT NULL, "
			"DATE_TIME DATETIME NOT NULL, "
			"PRIMARY KEY (USER_ID, BOARD, ARTICLE) )");
}

static std::string scrap_where(const std::string &table, int no)
{
	return "USER_ID='" + database::escape(login_user_id) + "' AND BOARD='" + database::escape(table.c_str()) +
		"' AND ARTICLE=" + TO_STRING(no);
}

// 글 보기의 SC: 스크랩에 없으면 담고, 있으면 뺀다
void scrap_toggle(const char *table, int no)
{
	bool ok;
	std::string w = scrap_where(table, no);
	int have = atoi(database::fetch((char*)("SELECT COUNT(*) FROM scrap WHERE " + w).c_str(), &ok).c_str());
	if ( have > 0 ) {
		mysql_query(mysql, ("DELETE FROM scrap WHERE " + w).c_str());
		printf("\r\n" X_C "스크랩에서 뺐습니다." X_W);
	} else {
		std::string q = "INSERT IGNORE INTO scrap (USER_ID, BOARD, ARTICLE, DATE_TIME) VALUES ('" +
			database::escape(login_user_id) + "', '" + database::escape(table) + "', " + TO_STRING(no) + ", NOW())";
		mysql_query(mysql, q.c_str());
		printf("\r\n" X_Y "스크랩했습니다." X_W " MY S 로 모아 볼 수 있습니다. (다시 SC 면 뺍니다)");
	}
	printf("\r\n[Enter] 를 누르세요.");
	press_enter();
}

static bool scrap_remove(const found_article &a)
{
	mysql_query(mysql, ("DELETE FROM scrap WHERE " + scrap_where(a.table, a.no)).c_str());
	return true;
}

static void show_scraps(void)
{
	std::vector<found_article> list;
	std::string q = "SELECT BOARD, ARTICLE FROM scrap WHERE USER_ID='" + database::escape(login_user_id) +
		"' ORDER BY DATE_TIME DESC LIMIT 200";
	std::vector<std::map<std::string, std::string> > r = database::fetch_rows((char*)q.c_str());
	for ( unsigned int i = 0; i < r.size(); i++ ) {
		pugi::xml_node node = board_node(r[i]["BOARD"]);
		if ( !node ) continue;		// 이제 들어갈 수 없는 게시판
		int no = atoi(r[i]["ARTICLE"].c_str());
		std::string q2 = "SELECT NO, USER_ID, DATE_TIME, TITLE, HIT, RECOMMEND FROM " + r[i]["BOARD"] + " WHERE NO=" + TO_STRING(no);
		std::vector<std::map<std::string, std::string> > a = database::fetch_rows((char*)q2.c_str());
		if ( a.empty() ) {
			// 지워진 글은 스크랩에서도 뺀다
			mysql_query(mysql, ("DELETE FROM scrap WHERE " + scrap_where(r[i]["BOARD"], no)).c_str());
			continue;
		}
		list.push_back(to_found(r[i]["BOARD"], node, a[0]));
	}
	article_list("스크랩한 글 (글 보기에서 SC 로 담기)", "스크랩한 글이 없습니다. 글 보기에서 SC 로 담으세요.", list,
			"날짜", scrap_remove);
}

// MY : 내가 쓴 글,  MY C : 내가 꼬리말을 단 글,  MY S : 스크랩한 글
void show_my(std::string arg)
{
	std::vector<found_article> list;
	printf("\r\n모으는 중입니다...");
	fflush(stdout);
	if ( !strcasecmp(trim(arg).c_str(), "s") ) {
		show_scraps();
		return;
	}
	if ( !strcasecmp(trim(arg).c_str(), "c") ) {
		std::string q = "SELECT BOARD, ARTICLE, MAX(NO) AS LAST FROM comments WHERE USER_ID='" +
			database::escape(login_user_id) + "' GROUP BY BOARD, ARTICLE ORDER BY LAST DESC LIMIT 60";
		std::vector<std::map<std::string, std::string> > r = database::fetch_rows((char*)q.c_str());
		for ( unsigned int i = 0; i < r.size(); i++ ) {
			pugi::xml_node node = board_node(r[i]["BOARD"]);
			if ( !node ) continue;
			std::string q2 = "SELECT NO, USER_ID, DATE_TIME, TITLE, HIT, RECOMMEND FROM " + r[i]["BOARD"] +
				" WHERE NO=" + TO_STRING(atoi(r[i]["ARTICLE"].c_str()));
			std::vector<std::map<std::string, std::string> > a = database::fetch_rows((char*)q2.c_str());
			if ( a.size() > 0 ) list.push_back(to_found(r[i]["BOARD"], node, a[0]));
		}
		mark_new_comments(list);
		article_list("내가 꼬리말을 단 글 (◆ 새 꼬리말, MY: 내 글, MY S: 스크랩)", "꼬리말을 단 글이 없습니다.", list);
		return;
	}
	list = collect("USER_ID='" + database::escape(login_user_id) + "'", 50);
	if ( list.size() > 100 ) list.resize(100);
	mark_new_comments(list);
	article_list("내가 쓴 글 (◆ 새 꼬리말, MY C: 꼬리말 단 글, MY S: 스크랩)", "쓴 글이 없습니다.", list);
}

// ------------------------------------------------------------------
// 게시판 구독 : 게시판 안에서 SUB 로 구독/해지, 밖에서 SUB 는 구독 목록
// 구독한 게시판에 새 글이 올라오면 접속해 있는 구독자에게 알린다
// ------------------------------------------------------------------
void subscribe_init(void)
{
	mysql_query(mysql, "CREATE TABLE IF NOT EXISTS subscribe ( "
			"USER_ID VARCHAR(50) NOT NULL, "
			"BOARD VARCHAR(64) NOT NULL, "
			"PRIMARY KEY (USER_ID, BOARD), "
			"KEY IDX_BOARD (BOARD) )");
}

static bool is_subscribed(const std::string &board)
{
	bool ok;
	std::string q = "SELECT COUNT(*) FROM subscribe WHERE USER_ID='" + database::escape(login_user_id) +
		"' AND BOARD='" + database::escape(board.c_str()) + "'";
	return atoi(database::fetch((char*)q.c_str(), &ok).c_str()) > 0;
}

void subscribe_command(void)
{
	std::string me = database::escape(login_user_id);

	// 게시판 안: 구독 / 해지
	if ( !current_board.empty() ) {
		pugi::xml_node n = board_node(current_board);
		std::string name = n ? board_name(n) : current_board;
		std::string b = database::escape(current_board.c_str());
		if ( is_subscribed(current_board) ) {
			std::string q = "DELETE FROM subscribe WHERE USER_ID='" + me + "' AND BOARD='" + b + "'";
			mysql_query(mysql, q.c_str());
			printf("\r\n'%s' 구독을 그만둡니다.", name.c_str());
		} else {
			std::string q = "INSERT IGNORE INTO subscribe (USER_ID, BOARD) VALUES ('" + me + "', '" + b + "')";
			mysql_query(mysql, q.c_str());
			printf("\r\n'%s' 을(를) 구독합니다.\r\n접속해 있을 때 새 글이 올라오면 바로 알려 드립니다.", name.c_str());
		}
		printf("\r\n[Enter] 를 누르세요.");
		press_enter();
		return;
	}

	// 밖: 구독 목록, 번호로 해지
	while ( 1 ) {
		std::string q = "SELECT BOARD FROM subscribe WHERE USER_ID='" + me + "'";
		std::vector<std::map<std::string, std::string> > r = database::fetch_rows((char*)q.c_str());
		list_header("게시판 구독", "");
		printf("\r\n");
		if ( r.empty() ) printf("%s\r\n", centered("구독한 게시판이 없습니다.", 80).c_str());
		for ( unsigned int i = 0; i < r.size() && i < 14; i++ ) {
			pugi::xml_node n = board_node(r[i]["BOARD"]);
			std::string go = n ? n.attribute("go").value() : "";
			printf("  %3d. " X_C "%-20s" X_W " %s\r\n", i + 1,
					string_truncate(n ? board_name(n) : r[i]["BOARD"], 20, "").c_str(),
					go.empty() ? "" : (X_G "GO " + go + X_W).c_str());
		}
		printf("\r\n  " X_G "게시판 안에서 SUB 를 치면 구독 / 해지. 새 글이 올라오면 바로 알려 드려요." X_W "\r\n");
		printf("%s\r\n", repeat("━", 40).c_str());
		char cmd[16];
		printf(ESC_ENG);
		printf("해지(번호) 나가기(Enter/P) >> ");
		line_input(cmd, 3);
		std::string c = trim(cmd);
		if ( c.empty() || !is_number(c) ) return;
		int k = atoi(c.c_str());
		if ( k < 1 || k > (int)r.size() ) continue;
		std::string d = "DELETE FROM subscribe WHERE USER_ID='" + me + "' AND BOARD='" +
			database::escape(r[k - 1]["BOARD"].c_str()) + "'";
		mysql_query(mysql, d.c_str());
	}
}

// 새 글이 올라왔을 때 (write_article 에서)
void notify_subscribers(const char *table, int no)
{
	std::string q = "SELECT USER_ID FROM subscribe WHERE BOARD='" + database::escape(table) +
		"' AND USER_ID <> '" + database::escape(login_user_id) + "'";
	std::vector<std::map<std::string, std::string> > r = database::fetch_rows((char*)q.c_str());
	if ( r.empty() ) return;
	bool ok;
	std::string t = database::fetch((char*)("SELECT TITLE FROM " + std::string(table) + " WHERE NO=" + TO_STRING(no)).c_str(), &ok);
	pugi::xml_node n = board_node(table);
	std::string name = n ? board_name(n) : table;
	std::string line = "◆ 새 글 ─ [" + name + "] " + nick_or_id(login_user_id) + ": " +
		string_truncate(display_text(t), 34, "..");
	for ( unsigned int i = 0; i < r.size(); i++ ) notify_online(r[i]["USER_ID"], line, true);
}

// ------------------------------------------------------------------
// 투표 : 운영자가 질문과 보기를 올리고, 회원은 한 번씩 투표 (POLL)
// ------------------------------------------------------------------
void poll_init(void)
{
	mysql_query(mysql, "CREATE TABLE IF NOT EXISTS poll ( "
			"NO INTEGER NOT NULL AUTO_INCREMENT PRIMARY KEY, "
			"USER_ID VARCHAR(50) NOT NULL, "
			"QUESTION VARCHAR(255) NOT NULL, "
			"OPTIONS TEXT NOT NULL, "
			"DATE_TIME DATETIME NOT NULL, "
			"CLOSED INT NOT NULL DEFAULT 0 )");
	mysql_query(mysql, "CREATE TABLE IF NOT EXISTS poll_vote ( "
			"POLL INT NOT NULL, "
			"USER_ID VARCHAR(50) NOT NULL, "
			"CHOICE INT NOT NULL, "
			"DATE_TIME DATETIME NOT NULL, "
			"PRIMARY KEY (POLL, USER_ID) )");
}

static void new_poll(void)
{
	char buf[128];
	printf(ESC_HAN);
	printf("\r\n\r\n질문 (Enter: 취소) >> ");
	line_input(buf, 60);
	std::string question = trim(display_text(buf));
	if ( question.empty() ) { printf(ESC_ENG); return; }

	std::vector<std::string> opts;
	printf("\r\n보기를 하나씩 넣으세요. (2~6개, 빈 줄이면 끝)");
	while ( opts.size() < 6 ) {
		printf("\r\n  %d. ", (int)opts.size() + 1);
		line_input(buf, 24);
		std::string o = trim(display_text(buf));
		if ( o.empty() ) break;
		opts.push_back(o);
	}
	printf(ESC_ENG);
	if ( opts.size() < 2 ) {
		printf("\r\n보기는 두 개 이상이어야 합니다.\r\n[Enter] 를 누르세요.");
		press_enter();
		return;
	}
	printf("\r\n이대로 올릴까요? (Y/n) ");
	if ( yesno(YES) != YES ) return;

	std::string all;
	for ( unsigned int i = 0; i < opts.size(); i++ ) all += (i ? "\n" : "") + opts[i];
	std::string q = "INSERT INTO poll (USER_ID, QUESTION, OPTIONS, DATE_TIME) VALUES ('" +
		database::escape(login_user_id) + "', '" + database::escape(question.c_str()) + "', '" +
		database::escape(all.c_str()) + "', NOW())";
	mysql_query(mysql, q.c_str());
}

static void view_poll(int no)
{
	std::string message;
	while ( 1 ) {
		std::string q = "SELECT * FROM poll WHERE NO=" + TO_STRING(no);
		std::vector<std::map<std::string, std::string> > p = database::fetch_rows((char*)q.c_str());
		if ( p.empty() ) {
			printf("\r\n그런 투표가 없습니다.\r\n[Enter] 를 누르세요.");
			press_enter();
			return;
		}
		std::vector<std::string> opts = split_string(p[0]["OPTIONS"], '\n');
		bool closed = atoi(p[0]["CLOSED"].c_str()) != 0;

		std::map<int, int> votes;
		int total = 0;
		q = "SELECT CHOICE, COUNT(*) AS N FROM poll_vote WHERE POLL=" + TO_STRING(no) + " GROUP BY CHOICE";
		std::vector<std::map<std::string, std::string> > v = database::fetch_rows((char*)q.c_str());
		for ( unsigned int i = 0; i < v.size(); i++ ) {
			votes[atoi(v[i]["CHOICE"].c_str())] = atoi(v[i]["N"].c_str());
			total += atoi(v[i]["N"].c_str());
		}
		bool ok;
		q = "SELECT CHOICE FROM poll_vote WHERE POLL=" + TO_STRING(no) + " AND USER_ID='" + database::escape(login_user_id) + "'";
		std::string mine_s = database::fetch((char*)q.c_str(), &ok);
		int mine = mine_s.empty() ? 0 : atoi(mine_s.c_str());

		list_header("투 표", closed ? X_G "마감" X_W : X_Y "진행 중" X_W);
		printf("\r\n  " X_Y "Q. %s" X_W "\r\n", string_truncate(display_text(p[0]["QUESTION"]), 74, "").c_str());
		std::string d = p[0]["DATE_TIME"].size() >= 16 ? p[0]["DATE_TIME"].substr(0, 16) : p[0]["DATE_TIME"];
		printf("     " X_G "%s 님이 %s 에 올림 / 모두 %d표" X_W "\r\n\r\n",
				nick_or_id(p[0]["USER_ID"]).c_str(), d.c_str(), total);
		// 2 + 번호 3 + 보기 24 + 1 + 막대 30 + 1 + 표 5 + 1 + 4 + 3 = 74 칸
		for ( unsigned int i = 0; i < opts.size(); i++ ) {
			int n = votes[i + 1];
			int pct = total ? (n * 100 + total / 2) / total : 0;
			int cells = total ? (n * 15 + total / 2) / total : 0;
			printf("  %d. %-24s " X_C "%s" X_G "%s" X_W " %3d표 %3d%%%s\r\n", i + 1,
					string_truncate(display_text(opts[i]), 24, "").c_str(),
					repeat("■", cells).c_str(), repeat("□", 15 - cells).c_str(), n, pct,
					mine == (int)i + 1 ? X_Y " ◀" X_W : "");
		}
		printf("\r\n");
		if ( mine ) printf("  " X_G "◀ 내가 고른 보기" X_W "\r\n");
		if ( !message.empty() ) printf("  %s\r\n", message.c_str());
		message.clear();
		printf("%s\r\n", repeat("━", 40).c_str());

		bool can_vote = !closed && !mine;
		std::string pr;
		if ( can_vote ) pr += "투표(보기 번호) ";
		if ( login_user_is_admin ) pr += closed ? "다시 열기(END) 삭제(DD) " : "마감(END) 삭제(DD) ";
		pr += "나가기(Enter/P) >> ";
		char cmd[16];
		printf(ESC_ENG);
		printf("%s", pr.c_str());
		line_input(cmd, 5);
		std::string c = trim(cmd);
		if ( c.empty() || !strcasecmp(c.c_str(), "p") ) return;

		if ( is_number(c) ) {
			int k = atoi(c.c_str());
			if ( closed ) message = X_R "마감된 투표입니다." X_W;
			else if ( mine ) message = X_R "이미 투표했습니다." X_W;
			else if ( k < 1 || k > (int)opts.size() ) message = X_R "보기 번호를 넣으세요." X_W;
			else {
				q = "INSERT IGNORE INTO poll_vote (POLL, USER_ID, CHOICE, DATE_TIME) VALUES (" + TO_STRING(no) + ", '" +
					database::escape(login_user_id) + "', " + TO_STRING(k) + ", NOW())";
				mysql_query(mysql, q.c_str());
				message = X_Y "투표했습니다. 고맙습니다!" X_W;
			}
		} else if ( login_user_is_admin && !strcasecmp(c.c_str(), "end") ) {
			q = "UPDATE poll SET CLOSED=" + std::string(closed ? "0" : "1") + " WHERE NO=" + TO_STRING(no);
			mysql_query(mysql, q.c_str());
		} else if ( login_user_is_admin && !strcasecmp(c.c_str(), "dd") ) {
			printf("\r\n이 투표를 지울까요? (y/N) ");
			if ( yesno(NO) == YES ) {
				q = "DELETE FROM poll_vote WHERE POLL=" + TO_STRING(no);
				mysql_query(mysql, q.c_str());
				q = "DELETE FROM poll WHERE NO=" + TO_STRING(no);
				mysql_query(mysql, q.c_str());
				return;
			}
		}
	}
}

void show_polls(void)
{
	unsigned int page = 0;
	const unsigned int per = 14;
	while ( 1 ) {
		std::string me = database::escape(login_user_id);
		std::string q = "SELECT p.NO, p.QUESTION, p.CLOSED, "
			"(SELECT COUNT(*) FROM poll_vote v WHERE v.POLL=p.NO) AS VOTES, "
			"(SELECT COUNT(*) FROM poll_vote v WHERE v.POLL=p.NO AND v.USER_ID='" + me + "') AS MINE "
			"FROM poll p ORDER BY p.CLOSED, p.NO DESC";
		std::vector<std::map<std::string, std::string> > r = database::fetch_rows((char*)q.c_str());
		unsigned int pages = r.empty() ? 1 : (r.size() + per - 1) / per;
		if ( page >= pages ) page = pages - 1;

		char pg[64];
		snprintf(pg, sizeof(pg), "%d/%d (총 %d건)", page + 1, pages, (int)r.size());
		list_header("투 표", pg);
		// 번호 4 + 1 + 상태 6 + 1 + 질문 48 + 1 + 표 6 + 1 + 참여 4 = 72 칸
		printf("%4s %-6s %-48s %6s %s\r\n", "번호", "상태", "질문", "투표수", "참여");
		printf("%s\r\n", repeat("─", 40).c_str());
		if ( r.empty() ) printf("%s\r\n", centered("아직 투표가 없습니다.", 80).c_str());
		for ( unsigned int i = page * per; i < r.size() && i < (page + 1) * per; i++ ) {
			bool closed = atoi(r[i]["CLOSED"].c_str()) != 0;
			printf("%4s %s %-48s %4s표 %s\r\n", r[i]["NO"].c_str(),
					closed ? X_G "마감  " X_W : X_Y "진행중" X_W,
					string_truncate(display_text(r[i]["QUESTION"]), 48, "").c_str(), r[i]["VOTES"].c_str(),
					atoi(r[i]["MINE"].c_str()) ? X_C " √" X_W : "");
		}
		printf("%s\r\n", repeat("━", 40).c_str());

		char cmd[16];
		printf(ESC_ENG);
		printf("보기(번호) 다음(N) 이전(B) %s나가기(Enter/P) >> ", login_user_is_admin ? "새 투표(W) " : "");
		line_input(cmd, 5);
		std::string c = trim(cmd);
		if ( c.empty() || !strcasecmp(c.c_str(), "p") ) return;
		if ( !strcasecmp(c.c_str(), "n") ) { if ( page + 1 < pages ) page++; }
		else if ( !strcasecmp(c.c_str(), "b") ) { if ( page > 0 ) page--; }
		else if ( !strcasecmp(c.c_str(), "w") ) {
			if ( login_user_is_admin ) new_poll();
		} else if ( is_number(c) ) view_poll(atoi(c.c_str()));
	}
}

// ------------------------------------------------------------------
// SYS: 서버와 BBS 현황
// ------------------------------------------------------------------
static std::string size_text(double bytes)
{
	char buf[32];
	if ( bytes >= 1024.0 * 1024 * 1024 * 1024 ) snprintf(buf, sizeof(buf), "%.1fT", bytes / (1024.0 * 1024 * 1024 * 1024));
	else if ( bytes >= 100.0 * 1024 * 1024 * 1024 ) snprintf(buf, sizeof(buf), "%.0fG", bytes / (1024.0 * 1024 * 1024));
	else if ( bytes >= 1024.0 * 1024 * 1024 ) snprintf(buf, sizeof(buf), "%.1fG", bytes / (1024.0 * 1024 * 1024));
	else if ( bytes >= 1024.0 * 1024 ) snprintf(buf, sizeof(buf), "%.0fM", bytes / (1024.0 * 1024));
	else snprintf(buf, sizeof(buf), "%.0fK", bytes / 1024.0);
	return buf;
}

// 막대 (16 칸 = 32 글자 폭): 쓴 만큼 ■, 80% 를 넘으면 빨강
static void usage_bar(const char *label, double used, double total, const std::string &extra)
{
	int pct = total > 0 ? (int)(used * 100 / total + 0.5) : 0;
	int n = total > 0 ? (int)(used * 16 / total + 0.5) : 0;
	if ( n > 16 ) n = 16;
	printf("    \033[=7F%-11s\033[=15F%s", label, pct >= 80 ? "\033[=12F" : "\033[=10F");
	for ( int i = 0; i < 16; i++ ) {
		if ( i == n ) printf("\033[=8F");
		printf(i < n ? "■" : "□");
	}
	printf("\033[=15F %s / %s (%d%%)%s\r\n", size_text(used).c_str(), size_text(total).c_str(), pct, extra.c_str());
}

static std::string first_line_of(const std::string &path)
{
	std::vector<std::string> l = split_string(read_file(path.c_str()), '\n');
	return l.empty() ? "" : trim(l[0]);
}

void show_system_info(void)
{
	where_scope where("시스템 정보");
	printf(ESC_CLEAR);
	print_news_title("시스템 정보");
	printf("\033[4;1H");

	// ---- 서버
	printf("  \033[=14F◆ 서버\033[=15F\r\n");
	std::string dist = first_line_of("/etc/system-release");
	if ( dist.empty() ) {
		// 다른 배포판: os-release 의 PRETTY_NAME="..."
		std::vector<std::string> os = split_string(read_file("/etc/os-release"), '\n');
		for ( unsigned int i = 0; i < os.size(); i++ ) {
			if ( os[i].compare(0, 12, "PRETTY_NAME=") != 0 ) continue;
			dist = os[i].substr(12);
			if ( dist.size() >= 2 && dist[0] == '"' ) dist = dist.substr(1, dist.size() - 2);
		}
	}
	printf("    \033[=7F%-11s\033[=15F%s\r\n", "배포판", string_truncate(dist, 64, "").c_str());

	std::vector<std::string> v = split_string(trim(read_file("/proc/version")), ' ');
	if ( v.size() >= 3 ) printf("    \033[=7F%-11s\033[=15F%s %s\r\n", "커널", v[0].c_str(), string_truncate(v[2], 58, "").c_str());

	std::string cpu;
	int cores = 0;
	std::vector<std::string> ci = split_string(read_file("/proc/cpuinfo"), '\n');
	for ( unsigned int i = 0; i < ci.size(); i++ ) {
		std::string::size_type p = ci[i].find(':');
		if ( p == std::string::npos ) continue;
		std::string k = trim(ci[i].substr(0, p));
		if ( k == "processor" ) cores++;
		if ( k == "model name" && cpu.empty() ) cpu = trim(ci[i].substr(p + 1));
	}
	// 겹친 빈칸을 하나로
	std::string c2;
	for ( unsigned int i = 0; i < cpu.size(); i++ ) if ( !(cpu[i] == ' ' && !c2.empty() && c2[c2.size() - 1] == ' ') ) c2 += cpu[i];
	char cb[128];
	snprintf(cb, sizeof(cb), " x %d", cores);
	printf("    \033[=7F%-11s\033[=15F%s%s\r\n", "CPU", string_truncate(c2, 56, "").c_str(), cores > 1 ? cb : "");

	long sec = atol(first_line_of("/proc/uptime").c_str());
	std::vector<std::string> la = split_string(first_line_of("/proc/loadavg"), ' ');
	printf("    \033[=7F%-11s\033[=15F%ld일 %02ld:%02ld:%02ld", "가동 시간", sec / 86400, sec % 86400 / 3600, sec % 3600 / 60, sec % 60);
	if ( la.size() >= 3 ) printf("      \033[=7F부하 (1/5/15 분)\033[=15F  %s  %s  %s", la[0].c_str(), la[1].c_str(), la[2].c_str());
	printf("\r\n");

	// ---- 메모리, 디스크
	printf("  \033[=14F◆ 메모리 / 디스크\033[=15F\r\n");
	std::map<std::string, double> mem;
	std::vector<std::string> mi = split_string(read_file("/proc/meminfo"), '\n');
	for ( unsigned int i = 0; i < mi.size(); i++ ) {
		std::string::size_type p = mi[i].find(':');
		if ( p != std::string::npos ) mem[trim(mi[i].substr(0, p))] = atof(mi[i].c_str() + p + 1) * 1024;
	}
	double total = mem["MemTotal"];
	// MemAvailable 이 없는 옛 커널 (CentOS 6) 은 남은 것 + 버퍼 + 캐시
	double avail = mem.count("MemAvailable") ? mem["MemAvailable"] : mem["MemFree"] + mem["Buffers"] + mem["Cached"];
	usage_bar("메모리", total - avail, total, "");
	if ( mem["SwapTotal"] > 0 ) usage_bar("스왑", mem["SwapTotal"] - mem["SwapFree"], mem["SwapTotal"], "");

	struct statvfs fs;
	if ( statvfs(getenv("HANULSO"), &fs) == 0 ) {
		double dt = (double)fs.f_blocks * fs.f_frsize;
		double du = (double)(fs.f_blocks - fs.f_bfree) * fs.f_frsize;
		double da = (double)fs.f_bavail * fs.f_frsize;
		usage_bar("디스크", du, dt, "  남음 " + size_text(da));
	}

	// ---- BBS
	printf("  \033[=14F◆ BBS\033[=15F\r\n");
	printf("    \033[=7F%-11s\033[=15F%s", "사이트", replace_bbcode("[host_name]").c_str());
	std::string since = trim(replace_bbcode("[since_days]"));
	if ( !since.empty() && since != "[since_days]" ) printf("  \033[=7F(연 지 \033[=15F%s\033[=7F 일째)\033[=15F", since.c_str());
	printf("\r\n");
	printf("    \033[=7F%-11s\033[=15F%s 명  \033[=7F(오늘 가입 %s, 다녀감 %s)\033[=15F  지금 접속 \033[=14F%s\033[=15F 명\r\n", "회원",
		trim(replace_bbcode("[num_members]")).c_str(), trim(replace_bbcode("[today_members]")).c_str(),
		trim(replace_bbcode("[today_visitors]")).c_str(), trim(replace_bbcode("[num_conns]")).c_str());
	printf("    \033[=7F%-11s\033[=15F%s 개  \033[=7F(오늘 %s)\033[=15F\r\n", "글",
		trim(replace_bbcode("[num_articles]")).c_str(), trim(replace_bbcode("[today_num_articles]")).c_str());

	bool ok;
	std::string dbsize = database::fetch((char*)"SELECT IFNULL(SUM(DATA_LENGTH + INDEX_LENGTH), 0) FROM information_schema.TABLES "
		"WHERE TABLE_SCHEMA = DATABASE()", &ok);
	std::string tables = database::fetch((char*)"SELECT COUNT(*) FROM information_schema.TABLES WHERE TABLE_SCHEMA = DATABASE()", &ok);
	// "5.1.73" 또는 "11.8.6-MariaDB-0+deb13u1 from Debian"
	std::string ver = mysql_get_server_info(mysql);
	ver = ver.substr(0, ver.find(' '));
	std::string server = "MySQL " + ver;
	if ( ver.find("MariaDB") != std::string::npos ) server = "MariaDB " + ver.substr(0, ver.find('-'));
	printf("    \033[=7F%-11s\033[=15F%s  \033[=7F(테이블 %s 개, %s)\033[=15F\r\n", "DB",
		server.c_str(), tables.c_str(), size_text(atof(dbsize.c_str())).c_str());

	char built[64];
	snprintf(built, sizeof(built), "%s %s", __DATE__, __TIME__);
	printf("    \033[=7F%-11s\033[=15F%s  \033[=7F(g++ %s)\033[=15F\r\n", "BBS 빌드", built, string_truncate(__VERSION__, 20, "").c_str());

	printf("\r\n [Enter] 를 누르세요.");
	press_enter();
}
