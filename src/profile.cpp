#include "main.h"
#include "regions.h"

// ------------------------------------------------------------------
// ³» Á¤º¸ °íÄ¡±â (PE)
//   Áö±Ý °ªÀ» ´Ã¾î³õ°í ¹øÈ£·Î °íÄ£´Ù: ºñ¹Ð¹øÈ£, ´Ð³×ÀÓ, »ýÀÏ, ÀÌ¸ÞÀÏ, ¼ºº°, Á¤º¸ °ø°³, ÀÚ±â¼Ò°³
//   ºñ¹Ð¹øÈ£´Â Áö±Ý ºñ¹Ð¹øÈ£¸¦ È®ÀÎÇÑ µÚ, °¡¸° Ã¤ µÎ ¹ø ¹Þ´Â´Ù (°¡ÀÔ°ú °°Àº ±ÔÄ¢: 8 ÀÚ ÀÌ»ó, ¿µ¹® + Æ¯¼ö ¹®ÀÚ)
//   Á¤º¸ °ø°³ (member.IS_OPEN): 1 ÀÌ¸é ´Ù¸¥ È¸¿øÀÇ PF ¿¡ ÀÌ¸ÞÀÏÀÌ º¸ÀÎ´Ù (0 ÀÌ¸é º»ÀÎ°ú ¿î¿µÀÚ¸¸)
//   ÀÚ±â¼Ò°³ (member.INTRO): PF ¿¡ ÇÑ ÁÙ
//   ³» Áö¿ª (member.REGION "½Ãµµ|½Ã±º±¸"): ´ë¹®ÀÇ ³¯¾¾. ºñ¿ì¸é Á¢¼ÓÇÑ °÷ÀÇ IP ·Î ÁüÀÛ
// ------------------------------------------------------------------

#define P_W		"\033[=15F"
#define P_Y		"\033[=14F"
#define P_C		"\033[=11F"
#define P_G		"\033[=7F"
#define P_R		"\033[=12F"
#define P_GR	"\033[=10F"

// ¿¹Àü member Ç¥¿¡ INTRO, REGION Ä­À» ´õÇÑ´Ù (Ã³À½ ÇÑ ¹ø)
void profile_upgrade(void)
{
	static const char *cols[][2] = {
		{ "INTRO", "ALTER TABLE member ADD COLUMN INTRO VARCHAR(100) NOT NULL DEFAULT ''" },
		{ "REGION", "ALTER TABLE member ADD COLUMN REGION VARCHAR(60) NOT NULL DEFAULT ''" },
	};
	for (unsigned int i=0; i<sizeof(cols)/sizeof(cols[0]); i++) {
		bool ok;
		std::string q = std::string("SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA=DATABASE() "
				"AND TABLE_NAME='member' AND COLUMN_NAME='") + cols[i][0] + "'";
		std::string n = database::fetch((char*)q.c_str(), &ok);
		if ( ok && atoi(n.c_str()) == 0 ) mysql_query(mysql, cols[i][1]);
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
	std::string now = ask_line("Áö±Ý ºñ¹Ð¹øÈ£ (Enter: Ãë¼Ò) : ", 40, false, 3);
	if ( now.empty() ) return false;
	if ( !database::check_same_password(user_id, (char*)now.c_str()) ) {
		note(P_R, "ºñ¹Ð¹øÈ£°¡ Æ²·È½À´Ï´Ù.");
		return false;
	}
	note(P_G, "»õ ºñ¹Ð¹øÈ£: 8 ÀÚ ÀÌ»ó, ¿µ¹®°ú Æ¯¼ö ¹®ÀÚ¸¦ ¼¯¾î ÁÖ¼¼¿ä.");
	while ( 1 ) {
		std::string p1 = ask_line("»õ ºñ¹Ð¹øÈ£ : ", 40, false, 3);
		if ( p1.empty() ) return false;
		if ( p1.size() < 8 ) { note(P_R, "8 ÀÚ ÀÌ»óÀ¸·Î ÇØ ÁÖ¼¼¿ä."); continue; }
		if ( check_password_strongness(p1) == 0 ) { note(P_R, "³Ê¹« ¾àÇÕ´Ï´Ù. ¿µ¹®°ú Æ¯¼ö ¹®ÀÚ¸¦ ¼¯¾î ÁÖ¼¼¿ä."); continue; }
		if ( p1 == now ) { note(P_R, "Áö±Ý ºñ¹Ð¹øÈ£¿Í °°½À´Ï´Ù."); continue; }
		std::string p2 = ask_line("ÇÑ ¹ø ´õ    : ", 40, false, 3);
		if ( p1 != p2 ) { note(P_R, "µÎ ¹ø ³ÖÀº °ÍÀÌ ´Ù¸¨´Ï´Ù."); continue; }
		if ( database::set_user_password(user_id, (char*)p1.c_str()) ) {
			note(P_GR, "ºñ¹Ð¹øÈ£¸¦ ¹Ù²å½À´Ï´Ù.");
			return true;
		}
		note(P_R, "¹Ù²ÙÁö ¸øÇß½À´Ï´Ù.");
		return false;
	}
}

static bool change_nick(char *user_id, const std::string &cur)
{
	note(P_G, "ÇÑ±Û 2~5 ÀÚ, ¿µ¹® 4~10 ÀÚ.");
	while ( 1 ) {
		std::string n = ask_line("»õ ´Ð³×ÀÓ (Enter: Ãë¼Ò) : ", 10, true);
		if ( n.empty() ) return false;
		if ( n == cur ) { note(P_G, "Áö±Ý ´Ð³×ÀÓ°ú °°½À´Ï´Ù."); return false; }
		if ( display_text(n) != n ) { note(P_R, "È­¸é¿¡ º¸ÀÌÁö ¾Ê´Â ±ÛÀÚ°¡ ÀÖ½À´Ï´Ù."); continue; }
		if ( n.size() < 4 ) { note(P_R, "³Ê¹« Âª½À´Ï´Ù. ÇÑ±Û 2 ÀÚ, ¿µ¹® 4 ÀÚ ÀÌ»ó."); continue; }
		if ( database::exist_nick_name((char*)n.c_str()) ) { note(P_R, "ÀÌ¹Ì ¾²´Â ´Ð³×ÀÓÀÔ´Ï´Ù."); continue; }
		database::set_user_nick_name(user_id, (char*)n.c_str());
		note(P_GR, "´Ð³×ÀÓÀ» ¹Ù²å½À´Ï´Ù.");
		return true;
	}
}

static bool change_birthday(char *user_id)
{
	while ( 1 ) {
		std::string b = ask_line("»ýÀÏ (¿¹: 1978-1-8, Enter: Ãë¼Ò) : ", 12, false);
		if ( b.empty() ) return false;
		int y = 0, m = 0, d = 0;
		if ( sscanf(b.c_str(), "%d-%d-%d", &y, &m, &d) != 3 || y < 1900 || !is_date_valid(d, m, y) ) {
			note(P_R, "³¯Â¥°¡ ¹Ù¸£Áö ¾Ê½À´Ï´Ù. ³â-¿ù-ÀÏ ·Î ³Ö¾î ÁÖ¼¼¿ä.");
			continue;
		}
		char s[16];
		snprintf(s, sizeof(s), "%04d-%02d-%02d", y, m, d);
		database::set_user_birthday(user_id, s);
		note(P_GR, "»ýÀÏÀ» ¹Ù²å½À´Ï´Ù. ¹ÙÀÌ¿À¸®µë, ¿î¼¼, ¶ì°¡ ÀÌ ³¯Â¥·Î ³ª¿É´Ï´Ù.");
		return true;
	}
}

static bool change_email(char *user_id, const std::string &cur)
{
	// ºñ¹Ð¹øÈ£ Ã£±â (PASS) °¡ ÀÌ ÁÖ¼Ò·Î °¡¹Ç·Î, Áö±Ý ºñ¹Ð¹øÈ£¸¦ È®ÀÎÇÑ´Ù (ÀÚ¸®¸¦ ºñ¿î »çÀÌ ³²ÀÌ ¹Ù²ÙÁö ¸øÇÏ°Ô)
	std::string now = ask_line("Áö±Ý ºñ¹Ð¹øÈ£ (Enter: Ãë¼Ò) : ", 40, false, 3);
	if ( now.empty() ) return false;
	if ( !database::check_same_password(user_id, (char*)now.c_str()) ) {
		note(P_R, "ºñ¹Ð¹øÈ£°¡ Æ²·È½À´Ï´Ù.");
		return false;
	}
	note(P_G, "ºñ¹Ð¹øÈ£¸¦ ÀØ¾úÀ» ¶§ (PASS) ÀÌ ÁÖ¼Ò·Î º¸³À´Ï´Ù.");
	while ( 1 ) {
		std::string e = ask_line("»õ ÀÌ¸ÞÀÏ (Enter: Ãë¼Ò) : ", 50, false);
		if ( e.empty() ) return false;
		if ( !strcasecmp(e.c_str(), cur.c_str()) ) { note(P_G, "Áö±Ý ÁÖ¼Ò¿Í °°½À´Ï´Ù."); return false; }
		if ( !is_email_valid((char*)e.c_str()) ) { note(P_R, "ÀÌ¸ÞÀÏ ÁÖ¼Ò°¡ ¹Ù¸£Áö ¾Ê½À´Ï´Ù."); continue; }
		if ( database::exist_email_address((char*)e.c_str()) ) { note(P_R, "´Ù¸¥ È¸¿øÀÌ ¾²´Â ÁÖ¼ÒÀÔ´Ï´Ù."); continue; }
		database::set_user_email(user_id, (char*)e.c_str());
		note(P_GR, "ÀÌ¸ÞÀÏÀ» ¹Ù²å½À´Ï´Ù.");
		return true;
	}
}

// "½Ãµµ|½Ã±º±¸" -> "½Ã±º±¸ (½Ãµµ)"
static std::string region_label(const std::string &r)
{
	std::string::size_type bar = r.find('|');
	if ( bar == std::string::npos ) return r;
	return r.substr(bar + 1) + " " P_G "(" + r.substr(0, bar) + ")" P_W;
}

// Á¢¼ÓÇÑ °÷À¸·Î ÁüÀÛÇÑ Áö¿ª (´ë¹® ³¯¾¾°¡ ¹Þ¾Æ µÐ °Í)
static std::string guessed_region(void)
{
	std::string host = remote_host_of_tty();
	std::replace(host.begin(), host.end(), ':', '_');
	if ( host.empty() || host.find('/') != std::string::npos ) return "";
	std::string g = trim(read_file((std::string(getenv("HANULSO")) + "/tmp/tag_geo_" + host + ".cache").c_str()));
	return g == "-" ? "" : g;
}

// ¹øÈ£ ¸ñ·ÏÀ» ¿©·¯ ÁÙ·Î (ÇÑ Ä­ Æø width)
static void print_choices(const std::vector<std::string> &names, int cols, int width)
{
	for (unsigned int i=0; i<names.size(); i++) {
		if ( i % cols == 0 ) printf("\r\n  ");
		char num[8];
		snprintf(num, sizeof(num), "%2d", i + 1);
		std::string cell = std::string(P_Y) + num + P_W ". " + names[i];
		printf("%s%s", cell.c_str(), std::string(width > (int)names[i].size() + 4 ? width - names[i].size() - 4 : 1, ' ').c_str());
	}
}

static bool change_region(char *user_id, const std::string &cur)
{
	printf("\r\n  " P_G "´ë¹®ÀÇ ³¯¾¾¸¦ ÀÌ Áö¿ªÀ¸·Î º¸¿© ÁÝ´Ï´Ù. ºñ¿ö µÎ¸é Á¢¼ÓÇÑ °÷À¸·Î ÁüÀÛÇÕ´Ï´Ù." P_W);
	std::string key = ask_line("Áö¿ª ÀÌ¸§ (¿¹: ¼º³², °­³²±¸. Enter: ½Ã/µµ¿¡¼­ °í¸£±â, - : ºñ¿ì±â) : ", 20, true);
	if ( key == "-" ) {
		set_column(user_id, "REGION", "");
		note(P_GR, "ºñ¿ü½À´Ï´Ù. Á¢¼ÓÇÑ °÷À¸·Î ÁüÀÛÇÕ´Ï´Ù.");
		return true;
	}
	std::vector<int> found;
	if ( !key.empty() ) {
		for (int i=0; i<region_count; i++) {
			if ( strstr(regions[i].name, key.c_str()) || strstr(regions[i].sido, key.c_str()) ) found.push_back(i);
		}
		if ( found.empty() ) { note(P_R, "±×·± Áö¿ªÀÌ ¾ø½À´Ï´Ù."); return false; }
		if ( found.size() > 40 ) { note(P_R, "³Ê¹« ¸¹½À´Ï´Ù. ´õ ÀÚ¼¼È÷ ÃÄ ÁÖ¼¼¿ä."); return false; }
	} else {
		std::vector<std::string> sidos;
		for (int i=0; i<region_count; i++) {
			if ( std::find(sidos.begin(), sidos.end(), regions[i].sido) == sidos.end() ) sidos.push_back(regions[i].sido);
		}
		printf(ESC_CLEAR);
		print_news_title("³» Áö¿ª °í¸£±â");
		printf("[4;1H");
		print_choices(sidos, 3, 24);
		int s = atoi(ask_line("\r\n  ½Ã/µµ ¹øÈ£ (Enter: Ãë¼Ò) : ", 2, false).c_str());
		if ( s < 1 || s > (int)sidos.size() ) return false;
		for (int i=0; i<region_count; i++) {
			if ( sidos[s - 1] == regions[i].sido ) found.push_back(i);
		}
	}
	int k = found[0];
	if ( found.size() > 1 ) {
		std::vector<std::string> names;
		for (unsigned int i=0; i<found.size(); i++) {
			std::string n = regions[found[i]].name;
			if ( !key.empty() ) n += std::string("(") + std::string(regions[found[i]].sido).substr(0, 4) + ")";
			names.push_back(n);
		}
		printf(ESC_CLEAR);
		print_news_title("³» Áö¿ª °í¸£±â");
		printf("[4;1H");
		print_choices(names, key.empty() ? 5 : 4, key.empty() ? 15 : 19);
		int c = atoi(ask_line("\r\n  ¹øÈ£ (Enter: Ãë¼Ò) : ", 2, false).c_str());
		if ( c < 1 || c > (int)found.size() ) return false;
		k = found[c - 1];
	}
	std::string v = std::string(regions[k].sido) + "|" + regions[k].name;
	set_column(user_id, "REGION", v);
	note(P_GR, ("³» Áö¿ªÀ» " + std::string(regions[k].name) + " ·Î Á¤Çß½À´Ï´Ù.").c_str());
	(void)cur;
	return true;
}

bool edit_profile(char *user_id)
{
	profile_upgrade();
	while ( 1 ) {
		bool exist;
		std::map<std::string, std::string> u = database::user_info(user_id, &exist);
		if ( !exist ) {
			printf("\r\nÈ¸¿ø Á¤º¸°¡ ¾ø½À´Ï´Ù.\r\n[Enter] ¸¦ ´©¸£¼¼¿ä.");
			press_enter();
			return false;
		}
		bool open = atoi(u["IS_OPEN"].c_str()) != 0;
		std::string intro = display_text(u["INTRO"]);
		std::string region = display_text(u["REGION"]);

		printf(ESC_CLEAR);
		print_news_title("³» Á¤º¸ °íÄ¡±â");
		printf("\033[4;1H\r\n");
		printf("  " P_G "¾ÆÀÌµð %s,  °¡ÀÔ %s" P_W "\r\n\r\n", user_id, u["REGISTRATION_DATETIME"].substr(0, 10).c_str());
		printf("    " P_Y "1" P_W ". ºñ¹Ð¹øÈ£   " P_G "********" P_W "\r\n");
		printf("    " P_Y "2" P_W ". ´Ð³×ÀÓ     %s\r\n", display_text(u["NICK_NAME"]).c_str());
		printf("    " P_Y "3" P_W ". »ýÀÏ       %s\r\n", u["BIRTHDAY"].c_str());
		printf("    " P_Y "4" P_W ". ÀÌ¸ÞÀÏ     %s\r\n", display_text(u["EMAIL"]).c_str());
		printf("    " P_Y "5" P_W ". ¼ºº°       %s\r\n", atoi(u["SEX"].c_str()) == 1 ? "³²ÀÚ" : "¿©ÀÚ");
		printf("    " P_Y "6" P_W ". Á¤º¸ °ø°³  %s\r\n", open ? P_GR "°ø°³" P_W "  " P_G "(´Ù¸¥ È¸¿øÀÇ PF ¿¡ ÀÌ¸ÞÀÏÀÌ º¸ÀÓ)" P_W
				: P_G "ºñ°ø°³ (PF ¿¡¼­ ÀÌ¸ÞÀÏÀº ³ª¿Í ¿î¿µÀÚ¸¸)" P_W);
		printf("    " P_Y "7" P_W ". ÀÚ±â¼Ò°³   %s\r\n", intro.empty() ? P_G "(¾øÀ½, PF ¿¡ ÇÑ ÁÙ·Î º¸ÀÔ´Ï´Ù)" P_W : intro.c_str());
		if ( !region.empty() ) {
			printf("    " P_Y "8" P_W ". ³» Áö¿ª    %s\r\n", region_label(region).c_str());
		} else {
			std::string g = guessed_region();
			printf("    " P_Y "8" P_W ". ³» Áö¿ª    " P_G "%s" P_W "\r\n",
				g.empty() ? "(Á¤ÇÏÁö ¾ÊÀ½, ´ë¹® ³¯¾¾´Â ¼­¿ï)" : ("(Á¤ÇÏÁö ¾ÊÀ½, Á¢¼ÓÇÑ °÷À¸·Î ÁüÀÛ: " + display_text(g.substr(g.find('|') + 1)) + ")").c_str());
		}

		std::string c = ask_line("°íÄ¥ ¹øÈ£ (Enter: ±×¸¸) >> ", 1, false);
		if ( c.empty() || c == "0" || !strcasecmp(c.c_str(), "p") || !strcasecmp(c.c_str(), "q") ) return true;
		bool done = false;
		switch ( atoi(c.c_str()) ) {
		case 1: done = change_password(user_id); break;
		case 2: done = change_nick(user_id, u["NICK_NAME"]); break;
		case 3: done = change_birthday(user_id); break;
		case 4: done = change_email(user_id, u["EMAIL"]); break;
		case 5: {
			std::string s = ask_line("¼ºº° [1]³²ÀÚ [2]¿©ÀÚ (Enter: Ãë¼Ò) : ", 1, false);
			if ( s == "1" || s == "2" ) { database::set_user_sex(user_id, (char*)s.c_str()); done = true; note(P_GR, "¹Ù²å½À´Ï´Ù."); }
			break;
		}
		case 6:
			set_column(user_id, "IS_OPEN", open ? "0" : "1");
			note(P_GR, open ? "ºñ°ø°³·Î ¹Ù²å½À´Ï´Ù." : "°ø°³·Î ¹Ù²å½À´Ï´Ù.");
			done = true;
			break;
		case 8: done = change_region(user_id, region); break;
		case 7: {
			printf("\r\n  " P_G "Áö±Ý: %s" P_W, intro.empty() ? "(¾øÀ½)" : intro.c_str());
			std::string t = ask_line("ÀÚ±â¼Ò°³ ÇÑ ÁÙ (Enter: ±×´ë·Î, - : Áö¿ì±â) : ", 60, true);
			if ( t == "-" ) { set_column(user_id, "INTRO", ""); note(P_GR, "Áö¿ü½À´Ï´Ù."); done = true; }
			else if ( !t.empty() ) { set_column(user_id, "INTRO", display_text(t)); note(P_GR, "¹Ù²å½À´Ï´Ù."); done = true; }
			break;
		}
		default:
			continue;
		}
		(void)done;
		printf("\r\n\r\n  [Enter] ¸¦ ´©¸£¼¼¿ä.");
		press_enter();
	}
}
