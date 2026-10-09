#include "main.h"

// ------------------------------------------------------------------
// txt 파일(대문 등)에 쓰는 [태그] 중 BBS(main) 에서만 알 수 있는 값
//   textutil.cpp 의 replace_bbcode 가 모르는 태그를 bbcode_tag_hook 으로 여기에 묻는다.
//   태그 목록은 docs/txt_tags.md
// 여러 접속자가 대문을 열 때마다 DB 를 뒤지지 않도록, 모두에게 같은 값은
// 60 초 동안 tmp/tags.cache 에 캐시해서 함께 쓴다. 로그인한 회원 본인의 값은 그때그때.
// 날씨/환율처럼 바깥 사이트에서 받아 오는 값은 뒤에서 받아 두고(30 분) 있는 값을 쓴다.
// ------------------------------------------------------------------

// 값 하나 (오류가 나거나 없으면 빈 문자열, 화면에 안 보이는 글자는 뺀다)
static std::string q1(const std::string &q)
{
	bool ok;
	std::string v = database::fetch((char*)q.c_str(), &ok);
	return ok ? display_text(v) : "";
}

static std::string num_or_zero(const std::string &v)
{
	return v.empty() ? "0" : v;
}

static std::string int_str(long v)
{
	char buf[32];
	snprintf(buf, sizeof(buf), "%ld", v);
	return buf;
}

// 명령을 실행해 첫 줄 (bin/lunar --today 같은)
static std::string command_line(const std::string &cmd)
{
	bool ok;
	std::vector<std::string> lines = exec_command((char*)cmd.c_str(), &ok);
	return lines.size() > 0 ? trim(lines[0]) : "";
}

struct board_name {
	std::string id, name;
};

// 메뉴에서 누구나 볼 수 있는(access_level 1) 게시판들과 이름, go="notice" 게시판
static void public_boards(pugi::xml_node node, std::vector<board_name> &list, std::string &notice)
{
	for ( pugi::xml_node c = node.first_child(); c; c = c.next_sibling() ) {
		if ( c.type() != pugi::node_element ) continue;
		if ( !strcmp(c.attribute("type").value(), "board") && !c.attribute("id").empty() ) {
			int level = c.attribute("access_level").empty() ? 1 : atoi(c.attribute("access_level").value());
			if ( level <= 1 ) {
				board_name b;
				b.id = c.attribute("id").value();
				b.name = trim(c.child_value("name"));
				// "자유 게시판 (plaza)" -> "자유 게시판"
				std::string::size_type p = b.name.find(" (");
				if ( p != std::string::npos && p > 0 ) b.name = b.name.substr(0, p);
				list.push_back(b);
			}
			if ( notice.empty() && !strcmp(c.attribute("go").value(), "notice") ) notice = c.attribute("id").value();
		}
		public_boards(c, list, notice);
	}
}

// 지금 대화방에 있는 사람 수 (chatt/<포트>.room : "방장,인원")
static int chat_user_count(void)
{
	char pattern[1024];
	snprintf(pattern, sizeof(pattern), "%s/chatt/*.room", getenv("HANULSO"));
	std::vector<std::string> files = find_files(pattern);
	int total = 0;
	for ( unsigned int i = 0; i < files.size(); i++ ) {
		int port = atoi(split_file_name(files[i]).c_str());
		if ( port <= 0 || !check_used_port(port) ) continue;	// 서버가 없는 방 (남은 파일)
		std::vector<std::string> info = split_string(read_file(files[i].c_str()), ',');
		if ( info.size() >= 2 ) total += atoi(info[1].c_str());
	}
	return total;
}

// 지금 접속해 있는 회원 닉네임들 (같은 회원이 둘이면 한 번)
static std::string online_users(void)
{
	char pattern[1024];
	snprintf(pattern, sizeof(pattern), "%s/tmp/*.tty", getenv("HANULSO"));
	std::vector<std::string> files = find_files_time_sorted(pattern);
	std::vector<std::string> ids;
	std::string out;
	for ( unsigned int i = 0; i < files.size(); i++ ) {
		std::string id;
		if ( !read_tty_file(files[i], id) ) continue;
		if ( std::find(ids.begin(), ids.end(), id) != ids.end() ) continue;
		ids.push_back(id);
		std::string nick = q1("SELECT NICK_NAME FROM member WHERE USER_ID='" + database::escape(id.c_str()) + "'");
		if ( nick.empty() ) nick = display_text(id);
		if ( !out.empty() ) out += ", ";
		out += nick;
	}
	return out;
}

// 모두에게 같은 값들을 새로 계산
static void compute_shared(std::map<std::string, std::string> &v)
{
	v["today_visitors"] = num_or_zero(q1("SELECT COUNT(*) FROM attendance WHERE ATT_DAY = CURDATE()"));
	v["max_online"] = num_or_zero(q1("SELECT MAX_ONLINE FROM stat_online WHERE DAY = CURDATE()"));
	v["max_online_ever"] = num_or_zero(q1("SELECT MAX(MAX_ONLINE) FROM stat_online"));
	v["today_members"] = num_or_zero(q1("SELECT COUNT(*) FROM member WHERE REGISTRATION_DATETIME >= CURDATE()"));
	v["newest_member"] = q1("SELECT NICK_NAME FROM member ORDER BY REGISTRATION_DATETIME DESC LIMIT 1");
	v["graffiti"] = q1("SELECT CONCAT(COALESCE(m.NICK_NAME, g.USER_ID), ': ', g.TEXT) FROM graffiti g "
			"LEFT JOIN member m ON m.USER_ID = g.USER_ID ORDER BY g.NO DESC LIMIT 1");
	v["birthdays"] = q1("SELECT GROUP_CONCAT(NICK_NAME ORDER BY NICK_NAME SEPARATOR ', ') FROM member WHERE "
			"(MONTH(BIRTHDAY) = MONTH(CURDATE()) AND DAYOFMONTH(BIRTHDAY) = DAYOFMONTH(CURDATE())) "
			"OR (MONTH(BIRTHDAY) = 2 AND DAYOFMONTH(BIRTHDAY) = 29 AND MONTH(CURDATE()) = 2 "
			"    AND DAYOFMONTH(CURDATE()) = 28 AND DAYOFMONTH(LAST_DAY(CURDATE())) = 28)");
	v["hero_champ"] = q1("SELECT NAME FROM game_hero ORDER BY WINS DESC, LEVEL DESC, EXP DESC LIMIT 1");
	v["rain_champ"] = q1("SELECT CONCAT(NAME, ' (', BEST, ')') FROM game_rain WHERE MODE = 0 ORDER BY BEST DESC, DATE_TIME LIMIT 1");
	v["online_users"] = online_users();
	v["chat_users"] = int_str(chat_user_count());

	// 최근 공지, 이번 주(7 일) 가장 많이 읽힌 글, 가장 최근 글: 누구나 볼 수 있는 게시판만
	std::vector<board_name> boards;
	std::string notice;
	pugi::xml_document doc;
	if ( doc.load_file("hanulso.mnu") ) public_boards(doc.child("hanulso"), boards, notice);
	if ( notice.empty() ) notice = "bbs_notice";
	v["last_notice"] = q1("SELECT TITLE FROM " + notice + " ORDER BY NO DESC LIMIT 1");

	long best = -1;
	std::string latest_time;
	for ( unsigned int i = 0; i < boards.size(); i++ ) {
		bool ok;
		std::string r = database::fetch((char*)("SELECT CONCAT(HIT, ' ', TITLE) FROM " + boards[i].id +
					" WHERE DATE_TIME >= NOW() - INTERVAL 7 DAY ORDER BY HIT DESC LIMIT 1").c_str(), &ok);
		if ( !ok ) continue;		// 아직 테이블이 없는 게시판
		std::string::size_type sp = r.find(' ');
		if ( !r.empty() && sp != std::string::npos && atol(r.c_str()) > best ) {
			best = atol(r.c_str());
			v["hot_article"] = display_text(r.substr(sp + 1));
		}

		// 가장 최근 글 "[게시판] 제목" (날짜/시각 19 자 + 제목)
		r = database::fetch((char*)("SELECT CONCAT(DATE_FORMAT(DATE_TIME, '%Y-%m-%d %H:%i:%s'), TITLE) FROM " +
					boards[i].id + " ORDER BY NO DESC LIMIT 1").c_str(), &ok);
		if ( ok && r.size() > 19 && r.substr(0, 19) > latest_time ) {
			latest_time = r.substr(0, 19);
			v["last_article"] = "[" + display_text(boards[i].name) + "] " + display_text(r.substr(19));
		}
	}

	// 음력 날짜와 오늘의 공휴일/명절 (bin/lunar --today : "음력 8월 25일<TAB>추석")
	std::string l = command_line(std::string(getenv("HANULSO")) + "/bin/lunar --today");
	std::string::size_type tab = l.find('\t');
	v["lunar_date"] = display_text(l.substr(0, tab));
	v["holiday"] = tab != std::string::npos ? display_text(l.substr(tab + 1)) : "";

	// 오늘의 명언 (bin/today --quote)
	v["today_quote"] = display_text(command_line(std::string(getenv("HANULSO")) + "/bin/today --quote"));
}

// 모두에게 같은 값들 (60 초 캐시)
static std::map<std::string, std::string> &shared_values(void)
{
	static std::map<std::string, std::string> v;
	static time_t loaded = 0;
	if ( loaded && time(NULL) - loaded < 10 ) return v;		// 한 화면에 태그가 여러 개여도 파일은 한 번
	loaded = time(NULL);

	char path[1024];
	snprintf(path, sizeof(path), "%s/tmp/tags.cache", getenv("HANULSO"));
	struct stat st;
	v.clear();
	if ( stat(path, &st) == 0 && time(NULL) - st.st_mtime < 60 ) {
		std::vector<std::string> lines = split_string(read_file(path), '\n');
		for ( unsigned int i = 0; i < lines.size(); i++ ) {
			std::string::size_type tab = lines[i].find('\t');
			if ( tab != std::string::npos ) v[lines[i].substr(0, tab)] = lines[i].substr(tab + 1);
		}
		if ( v.size() > 0 ) return v;
	}

	compute_shared(v);

	// 다른 프로세스가 읽는 중에 반쯤 쓴 파일을 보지 않도록 임시 파일에 쓰고 이름을 바꾼다
	char tmp[1100];
	snprintf(tmp, sizeof(tmp), "%s.%d", path, (int)getpid());
	FILE *fp = fopen(tmp, "w");
	if ( fp != NULL ) {
		for ( std::map<std::string, std::string>::iterator it = v.begin(); it != v.end(); ++it ) {
			fprintf(fp, "%s\t%s\n", it->first.c_str(), it->second.c_str());
		}
		fclose(fp);
		rename(tmp, path);
	}
	return v;
}

// 바깥 사이트에서 받아 오는 값 (날씨, 환율).
// 받아 둔 값(tmp/tag_<이름>.cache)을 바로 쓰고, ttl 초가 지났으면 뒤에서 새로 받아 둔다.
// 사이트가 느리거나 안 돼도 대문이 기다리지 않는다. (처음 한 번은 빈칸)
static std::string slow_value(const std::string &name, const std::string &cmd, int ttl)
{
	std::string base = std::string(getenv("HANULSO")) + "/tmp/tag_" + name;
	std::string cache = base + ".cache", lock = base + ".lock";
	std::string value = display_text(trim(read_file(cache.c_str())));

	struct stat st;
	bool fresh = stat(cache.c_str(), &st) == 0 && time(NULL) - st.st_mtime < ttl;
	if ( !fresh ) {
		// 이미 누가 받는 중이면 (5 분 안) 그냥 둔다
		struct stat ls;
		if ( stat(lock.c_str(), &ls) == 0 && time(NULL) - ls.st_mtime > 300 ) unlink(lock.c_str());
		int fd = open(lock.c_str(), O_CREAT | O_EXCL | O_WRONLY, 0644);
		if ( fd >= 0 ) {
			close(fd);
			// setsid: 접속을 끊어도 받기가 계속되도록. 성공했을 때만 캐시를 바꾼다
			std::string part = base + ".part";
			std::string sh = cmd + " > " + shell_quote(part) + " && mv " + shell_quote(part) + " " + shell_quote(cache)
				+ "; rm -f " + shell_quote(part) + " " + shell_quote(lock);
			std::string run = "setsid sh -c " + shell_quote(sh) + " < /dev/null > /dev/null 2>&1 &";
			system(run.c_str());
		}
	}
	return value;
}

// 로그인한 회원 정보 (한 접속 동안 생일/가입일은 안 바뀌므로 한 번만 읽는다)
static std::map<std::string, std::string> &me(void)
{
	static std::map<std::string, std::string> u;
	static std::string loaded_for;
	if ( loaded_for != login_user_id ) {
		bool exist;
		u = database::user_info(login_user_id, &exist);
		loaded_for = login_user_id;
	}
	return u;
}

// 생일까지 남은 날 "D-23", 당일은 "오늘 생일!"
static std::string birthday_dday(const std::string &birthday)
{
	int y, m, d;
	if ( sscanf(birthday.c_str(), "%d-%d-%d", &y, &m, &d) != 3 || m < 1 || m > 12 || d < 1 ) return "";
	time_t now = time(NULL);
	struct tm today = *localtime(&now);
	today.tm_hour = 12; today.tm_min = today.tm_sec = 0;
	time_t t0 = mktime(&today);
	for ( int add = 0; add <= 1; add++ ) {
		int yy = today.tm_year + 1900 + add;
		int dd = d;
		bool leap = (yy % 4 == 0 && yy % 100 != 0) || yy % 400 == 0;
		if ( m == 2 && d == 29 && !leap ) dd = 28;		// 윤년이 아니면 2/28
		struct tm b;
		memset(&b, 0, sizeof(b));
		b.tm_year = yy - 1900; b.tm_mon = m - 1; b.tm_mday = dd; b.tm_hour = 12; b.tm_isdst = -1;
		long days = (long)((mktime(&b) - t0 + 43200) / 86400);
		if ( days == 0 ) return "오늘 생일!";
		if ( days > 0 ) return "D-" + int_str(days);
	}
	return "";
}

// 띠, 별자리, 오늘의 운세 (bin/fortune --line 생년월일, 날짜가 바뀌면 다시)
static std::string fortune_part(int index)
{
	static std::string loaded_key;
	static std::vector<std::string> parts;
	time_t t = time(NULL);
	char today[16];
	strftime(today, sizeof(today), "%Y-%m-%d", localtime(&t));
	std::string birthday = me()["BIRTHDAY"];
	std::string key = std::string(today) + birthday;
	if ( key != loaded_key ) {
		loaded_key = key;
		parts = split_string(command_line(std::string(getenv("HANULSO")) + "/bin/fortune --line " + shell_quote(birthday)), '\t');
	}
	return (int)parts.size() > index ? display_text(parts[index]) : "";
}

// 내가 쓴 글 수 (게시판마다 세야 해서 5 분 동안 기억)
static std::string my_articles(void)
{
	static std::string value, loaded_for;
	static time_t loaded = 0;
	if ( loaded_for == login_user_id && time(NULL) - loaded < 300 ) return value;
	std::string id = database::escape(login_user_id);
	long total = 0;
	for ( unsigned int i = 0; i < table_names.size(); i++ ) {
		total += atol(q1("SELECT COUNT(*) FROM " + table_names[i] + " WHERE USER_ID='" + id + "'").c_str());
	}
	value = int_str(total);
	loaded_for = login_user_id;
	loaded = time(NULL);
	return value;
}

// 로그인한 회원 본인의 값
static bool user_value(const std::string &name, std::string &out)
{
	static const char *names[] = { "nick", "user_id", "level", "new_memo", "streak", "last_login",
		"total_attend", "attend_rank", "my_articles", "birthday_dday", "member_days",
		"ddi", "zodiac", "my_fortune", NULL };
	bool mine = false;
	for ( int i = 0; names[i]; i++ ) if ( name == names[i] ) mine = true;
	if ( !mine ) return false;

	out = "";
	if ( login_user_id[0] == 0 ) return true;		// 로그인 전
	std::string id = database::escape(login_user_id);

	if ( name == "user_id" ) out = display_text(login_user_id);
	else if ( name == "nick" ) out = q1("SELECT NICK_NAME FROM member WHERE USER_ID='" + id + "'");
	else if ( name == "level" ) out = login_user_is_admin ? "시숍" : get_level_name(login_user_level);
	else if ( name == "new_memo" ) out = num_or_zero(q1("SELECT COUNT(*) FROM memo WHERE RECIPIENT_USER_ID='" + id +
				"' AND RECIPIENT_DELETED=0 AND CONFIRMATION_DATETIME IS NULL"));
	else if ( name == "streak" ) out = num_or_zero(q1("SELECT STREAK FROM attendance_stat WHERE USER_ID='" + id +
				"' AND LAST_DATE >= CURDATE() - INTERVAL 1 DAY"));
	else if ( name == "last_login" ) {
		// 접속을 끝낼 때 기록하므로 접속해 있는 동안은 지난번 접속 시각
		out = q1("SELECT DATE_FORMAT(LASTLOGIN_DATETIME, '%Y-%m-%d %H:%i') FROM member WHERE USER_ID='" + id + "'");
	}
	else if ( name == "total_attend" ) out = num_or_zero(q1("SELECT TOTAL FROM attendance_stat WHERE USER_ID='" + id + "'"));
	else if ( name == "attend_rank" ) {
		// 오늘 몇 번째로 왔는지 (오늘 출석이 없으면 빈칸)
		out = q1("SELECT COUNT(*) FROM attendance a, attendance b WHERE a.ATT_DAY = CURDATE() AND b.ATT_DAY = CURDATE() "
				"AND b.USER_ID = '" + id + "' AND a.DATE_TIME <= b.DATE_TIME");
		if ( out == "0" ) out = "";
	}
	else if ( name == "my_articles" ) out = my_articles();
	else if ( name == "birthday_dday" ) out = birthday_dday(me()["BIRTHDAY"]);
	else if ( name == "member_days" ) {
		out = q1("SELECT DATEDIFF(CURDATE(), DATE(REGISTRATION_DATETIME)) + 1 FROM member WHERE USER_ID='" + id + "'");
	}
	else if ( name == "ddi" ) out = fortune_part(0);
	else if ( name == "zodiac" ) out = fortune_part(1);
	else if ( name == "my_fortune" ) out = fortune_part(2);
	return true;
}

// 사이트를 연 날 (hanulso.cfg 의 <since>2016-01-01</since>) 부터 오늘이 며칠째인지
static std::string since_days(void)
{
	pugi::xml_document doc;
	if ( !doc.load_file("hanulso.cfg") ) return "";
	int y, m, d;
	if ( sscanf(doc.child("hanulso").child_value("since"), "%d-%d-%d", &y, &m, &d) != 3 ) return "";
	struct tm b;
	memset(&b, 0, sizeof(b));
	b.tm_year = y - 1900; b.tm_mon = m - 1; b.tm_mday = d; b.tm_hour = 12; b.tm_isdst = -1;
	time_t now = time(NULL);
	struct tm today = *localtime(&now);
	today.tm_hour = 12; today.tm_min = today.tm_sec = 0;
	long days = (long)((mktime(&today) - mktime(&b) + 43200) / 86400) + 1;
	return days > 0 ? int_str(days) : "";
}

// 접속할 때마다 다른 한 줄 도움말 (txt/tips.txt, 한 줄에 하나, # 은 주석)
static std::string random_tip(void)
{
	static std::string tip;
	static bool chosen = false;
	if ( chosen ) return tip;		// 한 접속 동안은 같은 것 (화면을 다시 그려도 바뀌지 않게)
	chosen = true;
	std::vector<std::string> lines = split_string(read_file((std::string(getenv("HANULSO")) + "/txt/tips.txt").c_str()), '\n');
	std::vector<std::string> tips;
	for ( unsigned int i = 0; i < lines.size(); i++ ) {
		std::string l = trim(lines[i]);
		if ( !l.empty() && l[0] != '#' ) tips.push_back(display_text(l));
	}
	if ( tips.size() > 0 ) {
		srand(time(NULL) ^ getpid());
		tip = tips[rand() % tips.size()];
	}
	return tip;
}

// 게시판 글 수 (60 초 동안 기억)
static long board_count(const std::string &table)
{
	static std::map<std::string, std::pair<time_t, long> > cache;
	std::map<std::string, std::pair<time_t, long> >::iterator it = cache.find(table);
	if ( it != cache.end() && time(NULL) - it->second.first < 60 ) return it->second.second;
	bool ok;
	std::string v = database::fetch((char*)("SELECT COUNT(*) FROM " + table).c_str(), &ok);
	long n = ok ? atol(v.c_str()) : 0;
	cache[table] = std::make_pair(time(NULL), n);
	return n;
}

// 메뉴 안의 게시판 글 수를 모두 더한다 (boards: 게시판 수)
static long menu_articles(pugi::xml_node node, int *boards)
{
	long total = 0;
	for ( pugi::xml_node c = node.first_child(); c; c = c.next_sibling() ) {
		if ( strcmp(c.name(), "item") ) continue;
		if ( !strcmp(c.attribute("type").value(), "board") && !c.attribute("id").empty() ) {
			total += board_count(c.attribute("id").value());
			(*boards)++;
		} else if ( !strcmp(c.attribute("type").value(), "menu") ) {
			total += menu_articles(c, boards);
		}
	}
	return total;
}

// 4 칸 안으로 (괄호를 붙여 6 칸): 999 까지는 그대로, 9.9k 까지는 1.3k 처럼 소수 한 자리, 그 위는 12k, 135k ...
static std::string compact_count(long n)
{
	char buf[32];
	if ( n < 1000 ) snprintf(buf, sizeof(buf), "%ld", n);
	else if ( n < 10000 ) snprintf(buf, sizeof(buf), "%ld.%ldk", n / 1000, (n % 1000) / 100);	// 버림: 1,399 -> 1.3k
	else snprintf(buf, sizeof(buf), "%ldk", n / 1000 > 9999 ? 9999 : n / 1000);
	return buf;
}

// [articles_번호] : 지금 메뉴에서 그 번호 게시판의 글 수 (하위 메뉴면 그 안의 게시판을 모두 더함)
// 게시판이 없는 메뉴/프로그램이면 빈칸
static std::string articles_of(const std::string &door)
{
	if ( !current_menu ) return "";
	for ( pugi::xml_node c = current_menu.first_child(); c; c = c.next_sibling() ) {
		if ( strcmp(c.name(), "item") || door != c.attribute("door").value() ) continue;
		int boards = 0;
		long n = 0;
		if ( !strcmp(c.attribute("type").value(), "board") && !c.attribute("id").empty() ) {
			n = board_count(c.attribute("id").value());
			boards = 1;
		} else if ( !strcmp(c.attribute("type").value(), "menu") ) {
			n = menu_articles(c, &boards);
		}
		return boards > 0 ? "(" + compact_count(n) + ")" : "";		// (57), (1.3k), (12k)
	}
	return "";
}

// 게시판에 오늘 올라온 글이 있나 (60 초 동안 기억). 최근 20 개 글만 본다 (NO 가 큰 쪽이 새 글)
static bool board_new_today(const std::string &table)
{
	static std::map<std::string, std::pair<time_t, bool> > cache;
	std::map<std::string, std::pair<time_t, bool> >::iterator it = cache.find(table);
	if ( it != cache.end() && time(NULL) - it->second.first < 60 ) return it->second.second;
	bool ok;
	std::string v = database::fetch((char*)("SELECT COUNT(*) FROM (SELECT DATE_TIME FROM " + table +
				" ORDER BY NO DESC LIMIT 20) X WHERE DATE_TIME >= CURDATE()").c_str(), &ok);
	bool yes = ok && atol(v.c_str()) > 0;
	cache[table] = std::make_pair(time(NULL), yes);
	return yes;
}

static bool menu_new_today(pugi::xml_node node)
{
	for ( pugi::xml_node c = node.first_child(); c; c = c.next_sibling() ) {
		if ( strcmp(c.name(), "item") ) continue;
		if ( !strcmp(c.attribute("type").value(), "board") && !c.attribute("id").empty() ) {
			if ( board_new_today(c.attribute("id").value()) ) return true;
		} else if ( !strcmp(c.attribute("type").value(), "menu") ) {
			if ( menu_new_today(c) ) return true;
		}
	}
	return false;
}

// [new_번호] : 지금 메뉴에서 그 번호 게시판 (하위 메뉴면 그 안의 게시판 중 하나라도) 에 오늘 글이 있으면 "N", 없으면 ""
//   화면 파일에서는 [new_1? (%s)] 처럼 있을 때만 보이게 쓴다
static std::string new_of(const std::string &door)
{
	if ( !current_menu ) return "";
	for ( pugi::xml_node c = current_menu.first_child(); c; c = c.next_sibling() ) {
		if ( strcmp(c.name(), "item") || door != c.attribute("door").value() ) continue;
		bool yes = false;
		if ( !strcmp(c.attribute("type").value(), "board") && !c.attribute("id").empty() ) {
			yes = board_new_today(c.attribute("id").value());
		} else if ( !strcmp(c.attribute("type").value(), "menu") ) {
			yes = menu_new_today(c);
		}
		return yes ? "N" : "";
	}
	return "";
}

// 지금 날씨 [weather] 와 그 지역 [weather_place]
//   PE 에서 정한 내 지역 (member.REGION "시도|시군구"), 없으면 접속한 곳의 IP 로 찾은 지역 (하루 캐시),
//   둘 다 없거나 아직 받는 중이면 서울
static std::string hex_of(const std::string &s)
{
	static const char *h = "0123456789abcdef";
	std::string o;
	for (unsigned int i=0; i<s.size(); i++) { o += h[(unsigned char)s[i] >> 4]; o += h[(unsigned char)s[i] & 15]; }
	return o;
}

static void weather_now(std::string &place, std::string &value)
{
	std::string bin = std::string(getenv("HANULSO")) + "/bin/";
	std::string region;
	if ( login_user_id[0] ) {
		std::vector<std::map<std::string, std::string> > r = database::fetch_rows((char*)("SELECT REGION FROM member WHERE USER_ID='" +
			database::escape(login_user_id) + "'").c_str());
		if ( !r.empty() ) region = r[0]["REGION"];
	}
	if ( region.empty() ) {
		std::string host = remote_host_of_tty();
		bool ok = !host.empty() && host.size() <= 64;
		for (unsigned int i=0; ok && i<host.size(); i++) {
			char c = host[i];
			if ( !isalnum((unsigned char)c) && c != '.' && c != ':' && c != '-' ) ok = false;
		}
		if ( ok ) {
			std::string key = host;
			std::replace(key.begin(), key.end(), ':', '_');
			std::string g = slow_value("geo_" + key, bin + "weather --locate " + shell_quote(host), 86400);
			if ( g != "-" ) region = g;
		}
	}
	std::string line;
	std::string::size_type bar = region.find('|');
	if ( bar != std::string::npos ) {
		line = slow_value("weather_" + hex_of(region), bin + "weather --now " + shell_quote(region.substr(0, bar)) + " " +
			shell_quote(region.substr(bar + 1)), 1800);
	}
	if ( line.empty() ) line = slow_value("weather", bin + "weather --now", 1800);
	bar = line.find('|');
	if ( bar == std::string::npos ) {		// 예전 캐시 (서울)
		place = line.empty() ? "" : "서울";
		value = line;
	} else {
		place = line.substr(0, bar);
		value = line.substr(bar + 1);
	}
}

// replace_bbcode 가 모르는 태그. 모르는 이름이면 found = false (태그를 그대로 둔다)
std::string bbtag_value(const std::string &name, bool *found)
{
	std::string out;
	*found = true;
	if ( user_value(name, out) ) return out;
	if ( name.compare(0, 9, "articles_") == 0 ) return articles_of(name.substr(9));
	if ( name.compare(0, 4, "new_") == 0 ) return new_of(name.substr(4));

	if ( name == "since_days" ) return since_days();
	if ( name == "random_tip" ) return random_tip();
	std::string bin = std::string(getenv("HANULSO")) + "/bin/";
	if ( name == "weather" || name == "weather_place" ) {
		std::string place, value;
		weather_now(place, value);
		return name == "weather" ? value : place;
	}
	if ( name == "usd_rate" ) return slow_value("usd_rate", bin + "exchange --usd", 3600);

	std::map<std::string, std::string> &v = shared_values();
	std::map<std::string, std::string>::iterator it = v.find(name);
	if ( it != v.end() ) return it->second;

	// 캐시를 만들 때 값이 없던 태그 (게임 기록이 아직 없는 등)
	static const char *known[] = { "today_visitors", "max_online", "max_online_ever", "today_members",
		"newest_member", "graffiti", "birthdays", "hero_champ", "rain_champ", "last_notice",
		"hot_article", "last_article", "online_users", "chat_users", "lunar_date", "holiday",
		"today_quote", NULL };
	for ( int i = 0; known[i]; i++ ) if ( name == known[i] ) return "";

	*found = false;
	return "";
}
