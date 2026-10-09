#include "main.h"

typedef struct _category {
	std::string door;
	std::string name;
	std::string rss;
} category;

typedef struct _newspaper {
	std::string door;
	std::string name;
	std::vector<category> categories;
} newspaper;

typedef struct _news_data {
	int no;
	std::string author;
	std::string title;
	std::string link;
	std::string description;
	std::string date;
} news_data;

std::vector<newspaper> newspapers;

bool show_category(newspaper np, bool *goto_top);
bool show_news_menu(category c, std::string paper_name, bool *goto_top);
void show_news(std::string title, int no, std::vector<news_data> list, bool *is_dir);

// 신문사 메뉴 읽음
void read_newspaper_menu(std::vector<newspaper> &list)
{
	char tmp[1024];
	sprintf(tmp, "%s/news.mnu", getenv("HANULSO"));

	pugi::xml_document doc;
	pugi::xml_parse_result res = doc.load_file(tmp, pugi::parse_default|pugi::parse_declaration);
    if (!res) {
		std::ostringstream msg;
		msg << " Parse error: " << res.description() << ", character pos= " << res.offset;
		printf("\r\n\r\n%s", msg.str().c_str());
		printf("\r\n [Enter] 를 누르세요.");
		press_enter();
		return;
	}

	// -----------------------------------------------------------
	pugi::xml_node node = doc.first_element_by_path("/news");

	for (pugi::xml_node child = node.child("newspaper"); child; child = child.next_sibling("newspaper")) {
		newspaper np;
		np.name = child.attribute("name").value();
		np.door = child.attribute("door").value();

		for (pugi::xml_node child2 = child.child("category"); child2; child2 = child2.next_sibling("category")) {
			category c;
			c.door = (char*)(child2.child("door").child_value());
			c.name = (char*)(child2.child("name").child_value());
			c.rss = trim((char*)(child2.child("rss").child_value()));
			np.categories.push_back(c);
		}

		list.push_back(np);
	}
}

// 화면 상단 헤더 출력
void print_news_title(const char *title)
{
    printf("\033[1;1H");
	printf("\033[=9F\033[=1G%s\033[=15F\033[=1G", repeat("─", 40).c_str());
    printf("\033[1;1H");
	printf("\033[1A\033[7m%s\033[0m", host_name);
	// 타이틀 출력
	int center = (80-strlen(strip_ansi_codes(title)))/2;
	if ( center < 0 ) center = 0;
    printf("\033[2;1H");
	printf("\r\033[%dC%s", center, title);

    printf("\033[3;1H");
	printf("\033[=0F\033[=1G%s\033[=15F\033[=1G", repeat("━", 40).c_str());
}

// 번호/이름 목록을 여러 단으로 출력 (항목이 많으면 2~3 단)
void print_door_list(const std::vector<std::string> &doors, const std::vector<std::string> &names)
{
	int n = names.size();
	int col = 1;
	int width = 30;
	if ( n > 45 ) {
		col = 4;
		width = 14;
	} else if ( n > 30 ) {
		col = 3;
		width = 21;
	} else if ( n > 15 ) {
		col = 2;
		width = 30;
	}
	int rows = (n + col - 1) / col;

	// 위에서 아래로 먼저 채운다
	for (int r=0; r<rows; r++) {
		for (int c=0; c<col; c++) {
			int i = c * rows + r;
			if ( i >= n ) break;
			printf("%5s %s", doors[i].c_str(),
					string_truncate(names[i], width-1, "").c_str());
			int len = strlen(string_truncate(names[i], width-1, "").c_str());
			for (int k=len; k<width-1; k++) putchar(' ');
		}
		printf("\r\n");
	}
}

void show_newspaper_board(pugi::xml_node node, bool *goto_top)
{
	newspapers.clear();
	read_newspaper_menu(newspapers);
	where_scope where(node_where(node));

	char *title = (char*)(node.child("name").child_value());

	*goto_top = false;

	int access_level = atoi((char*)(node.attribute("access_level").value()));

	if ( login_user_level < access_level ) {
		printf("\r\n%s 이상 진입 가능합니다.", get_level_name(access_level).c_str());
		printf("\r\n[Enter] 를 누르세요.");
		press_enter();
		return;
	}

	while (1) {
		printf(ESC_CLEAR);

		char *header = (char*)(node.child("header").child_value());
		print_file(header);

		print_news_title(title);

        printf("\033[4;1H");

		std::vector<std::string> doors, names;
		for(unsigned int i=0; i<newspapers.size(); i++) {
			doors.push_back(newspapers[i].door);
			names.push_back(newspapers[i].name);
		}
		print_door_list(doors, names);

		printf("%s\r\n", repeat("━", 40).c_str());

		char *footer = (char*)(node.child("footer").child_value());
		print_file(footer);

		// ----------------- 프롬프트 처리 -------------------
		printf(ESC_ENG);
		printf("보기(번호) 이전메뉴(P) 초기화면(T)\r\n");
		printf("선택 >> ");
		char cmd[1024];
		line_input(cmd, 30);

		std::vector<std::string> args = split_string(std::string(cmd), ' ');
		// 입력이 되었을경우
		if ( args.size() > 0 ) {
			// 상위 명령
			if ( !strcasecmp(args[0].c_str(), "p") ) {
				break;
			} else if ( !strcasecmp(args[0].c_str(), "t") ) {
				*goto_top = true;
				break;
			} else {
				for(unsigned int i=0; i<newspapers.size(); i++) {
					newspaper np = newspapers[i];
					if (!strcmp(np.door.c_str(), args[0].c_str())) {
						bool top;
						show_category(np, &top);
						if ( top ) {
							*goto_top = true;
							return;
						}
					}
				}
			}
		}
	}
}

bool show_category(newspaper np, bool *goto_top)
{
	char *title = (char*)np.name.c_str();

	*goto_top = false;

	while (1) {
		printf(ESC_CLEAR);

		print_news_title(title);

        printf("\033[4;1H");

		std::vector<std::string> doors, names;
		for(unsigned int i=0; i<np.categories.size(); i++) {
			doors.push_back(np.categories[i].door);
			names.push_back(np.categories[i].name);
		}
		print_door_list(doors, names);

		printf("%s\r\n", repeat("━", 40).c_str());

		// ----------------- 프롬프트 처리 -------------------
		printf(ESC_ENG);
		printf("보기(번호) 이전메뉴(P) 초기화면(T)\r\n");
		printf("선택 >> ");
		char cmd[1024];
		line_input(cmd, 30);

		std::vector<std::string> args = split_string(std::string(cmd), ' ');
		// 입력이 되었을경우
		if ( args.size() > 0 ) {
			// 상위 명령
			if ( !strcasecmp(args[0].c_str(), "p") ) {
				break;
			} else if ( !strcasecmp(args[0].c_str(), "t") ) {
				*goto_top = true;
				break;
			} else {
				for(unsigned int i=0; i<np.categories.size(); i++) {
					category c = np.categories[i];
					if (!strcmp(c.door.c_str(), args[0].c_str())) {
						bool top;
						show_news_menu(c, np.name, &top);
						if ( top ) {
							*goto_top = true;
							return true;
						}
					}
				}
			}
		}
	}

	return true;
}

// XML 선언의 인코딩이 EUC-KR 계열인지
bool is_euckr_xml(const std::string &xml)
{
	std::string head = xml.substr(0, 200);
	for (unsigned int i=0; i<head.size(); i++) {
		head[i] = tolower((unsigned char)head[i]);
	}
	std::size_t p = head.find("encoding=");
	if ( p == std::string::npos ) return false;
	std::string enc = head.substr(p + 10, 15);
	return enc.compare(0, 6, "euc-kr") == 0 || enc.compare(0, 5, "euckr") == 0 ||
		enc.compare(0, 5, "cp949") == 0 || enc.compare(0, 9, "ks_c_5601") == 0;
}

// 제목 등에 남아 있는 HTML 엔티티와 줄바꿈 정리
std::string clean_text(std::string s)
{
	s = replace_all(s, "&amp;", "&");
	s = replace_all(s, "&quot;", "\"");
	s = replace_all(s, "&#34;", "\"");
	s = replace_all(s, "&#39;", "'");
	s = replace_all(s, "&#039;", "'");
	s = replace_all(s, "&apos;", "'");
	s = replace_all(s, "&lt;", "<");
	s = replace_all(s, "&gt;", ">");
	s = replace_all(s, "&nbsp;", " ");
	s = replace_all(s, "&middot;", "·");
	s = replace_all(s, "&hellip;", "…");
	s = replace_all(s, "<br>", "");
	s = replace_all(s, "<BR>", "");
	s = replace_all(s, "<b>", "");
	s = replace_all(s, "</b>", "");
	for (unsigned int i=0; i<s.size(); i++) {
		if ( s[i] == '\r' || s[i] == '\n' || s[i] == '\t' ) s[i] = ' ';
	}
	return trim(s);
}

// 날짜를 YY-MM-DD 로 (RFC 822 "Sat, 04 Oct 2026 ..." 또는 "2026-10-04...")
std::string short_date(const std::string &date)
{
	static const char *months[] = { "jan", "feb", "mar", "apr", "may", "jun",
		"jul", "aug", "sep", "oct", "nov", "dec" };
	int y = 0, m = 0, d = 0;
	char buf[16];

	if ( sscanf(date.c_str(), "%d-%d-%d", &y, &m, &d) == 3 ) {
		snprintf(buf, sizeof(buf), "%02d-%02d-%02d", y % 100, m, d);
		return buf;
	}

	const char *p = strchr(date.c_str(), ',');
	p = p ? p + 1 : date.c_str();
	char mon[8];
	if ( sscanf(p, "%d %3s %d", &d, mon, &y) == 3 ) {
		for (int i=0; i<12; i++) {
			if ( !strncasecmp(mon, months[i], 3) ) {
				snprintf(buf, sizeof(buf), "%02d-%02d-%02d", y % 100, i+1, d);
				return buf;
			}
		}
	}

	return "";
}

// 자식 노드 중 첫 번째로 값이 있는 것
std::string child_text(pugi::xml_node node, const char *a, const char *b = NULL, const char *c = NULL)
{
	const char *names[3] = { a, b, c };
	for (int i=0; i<3; i++) {
		if ( names[i] == NULL ) continue;
		pugi::xml_node n = node.child(names[i]);
		if ( n ) {
			std::string v = n.child_value();
			// CDATA 등 자식 노드에 값이 있는 경우
			if ( v.empty() && n.first_child() ) v = n.first_child().value();
			if ( !v.empty() ) return v;
		}
	}
	return "";
}

// 기사 읽음
// 일본어 기사인지 (EUC-KR 의 히라가나 0xAA, 가타카나 0xAB 줄 글자가 있으면)
// 조선일보 등의 RSS 에 일본어판 기사가 섞여 들어온다.
bool has_kana(const std::string &s)
{
	for (unsigned int i=0; i<s.size(); i++) {
		unsigned char c = s[i];
		if ( c < 0x80 ) continue;
		if ( (c == 0xAA || c == 0xAB) && i + 1 < s.size() && (unsigned char)s[i+1] >= 0xA1 )
			return true;
		i++;	// 2 바이트 글자
	}
	return false;
}

bool read_news(category c, std::vector<news_data> &list)
{
	// 임시 파일 없이 받는다 (받는 중에 끊겨도 tmp 에 남지 않게)
	std::string string;
	if ( !download_text(c.rss, string) ) {
		printf("\r\n다운로드를 실패하였습니다.\r\n");
		printf("\r\n[Enter] 를 누르세요.");
		press_enter();
		return false;
	}

	// UTF-8 이면 완성형으로 변환
	if ( !is_euckr_xml(string) ) {
		string = utf8_to_cp949(string);
	}

	// 버퍼 파싱
	pugi::xml_document doc;
	pugi::xml_parse_result res = doc.load_string(string.c_str());
    if (!res) {
		std::ostringstream msg;
		msg << " Parse error: " << res.description() << ", character pos= " << res.offset;
		printf("\r\n\r\n%s", msg.str().c_str());
		printf("\r\n[Enter] 를 누르세요.");
		press_enter();
		return false;
	}

	// -----------------------------------------------------------
	// RSS 2.0 (rss/channel/item), RSS 1.0 (rdf:RDF/item), Atom (feed/entry)
	pugi::xml_node root = doc.document_element();
	std::string root_name = root.name();

	pugi::xml_node parent;
	const char *item_name = "item";
	if ( root_name == "rss" ) {
		parent = root.child("channel");
	} else if ( root_name == "feed" ) {
		parent = root;
		item_name = "entry";
	} else {
		parent = root;
	}

	int no = 1;
	for (pugi::xml_node child = parent.child(item_name); child; child = child.next_sibling(item_name)) {
		news_data data;
		data.no = no;
		data.title = clean_text(child_text(child, "title"));
		if ( has_kana(data.title) ) continue;	// 일본어 기사는 뺀다

		data.author = child_text(child, "author", "dc:creator");
		// Atom: <author><name>..</name></author>
		if ( child.child("author").child("name") ) {
			data.author = child.child("author").child("name").child_value();
		}
		data.author = clean_text(data.author);

		data.link = trim(child_text(child, "link"));
		// Atom: <link href=".." />
		if ( data.link.empty() ) {
			data.link = child.child("link").attribute("href").value();
		}

		data.description = child_text(child, "description", "content:encoded", "summary");
		if ( data.description.empty() ) {
			data.description = child_text(child, "content");
		}

		data.date = trim(child_text(child, "pubDate", "dc:date", "published"));
		if ( data.date.empty() ) {
			data.date = trim(child_text(child, "updated"));
		}

		list.push_back(data);
		no++;
	}

	return true;
}

void print_news_board_header(int total, int page_no, int page_count, char *title)
{
	print_news_title(title);

	// 총 글 갯수 출력
	char buf[1024];
	if ( page_count > 0 ) {
		sprintf(buf, "%4d/%4d (총 %4d건) ", page_no, page_count, total);
	} else {
		sprintf(buf, "(총 %4d건) ", total);
	}
    printf("\033[2;1H");
	printf("\r\033[%dC%s", (int)(79 - strlen(buf)), buf);

    printf("\033[3;1H\r\n");
}

bool show_news_menu(category c, std::string paper_name, bool *goto_top)
{
	*goto_top = false;

	printf(ESC_CLEAR);
	printf("뉴스를 다운로드 중입니다...\r\n");
	fflush(stdout);

	std::vector<news_data> list;
	if ( !read_news(c, list) ) {
		return false;
	}

	std::string title = paper_name + " - " + c.name;

	int offset = 0;

	while (1) {
		printf(ESC_CLEAR);

		// 마지막 페이지를 넘어가면 이전 페이지로 (빈 목록이면 0 유지)
		if ( offset > 0 && list.size() <= (unsigned int)offset ) {
			offset -= show_max_line;
			if ( offset < 0 ) offset = 0;
		}

		int page_count = (list.size() + show_max_line - 1) / show_max_line;
		if ( page_count < 1 ) page_count = 1;

		print_news_board_header(list.size(), offset / show_max_line + 1, page_count, (char*)title.c_str());

        printf("\033[4;1H");
		printf("%5s %s %s %s",
				"번호",
				centered("작성자", 12).c_str(),
				centered("날짜", 8).c_str(),
				"제목");
		printf("\r\n%s\r\n", repeat("─", 40).c_str());

		if ( list.size() == 0 ) {
			printf("%s\r\n", centered("기사가 없습니다.", 80).c_str());
		}

		for(unsigned int i=offset; i<offset+show_max_line; i++) {
			if ( i >= list.size() ) break;

			news_data data = list[i];

			// 날짜를 YY-MM-DD 로 늘린 만큼 제목을 줄임 (한 줄 79 자)
			std::string news_title = string_truncate(data.title, 51, "...");
			std::string author = data.author.empty() ? paper_name : data.author;

			printf("%5d %s %8s ",
				data.no,
				centered(string_truncate(author, 10, ""), 12).c_str(),
				short_date(data.date).c_str());
			printf("%s", news_title.c_str());
			printf("\r\n");
		}

		printf("%s\r\n", repeat("━", 40).c_str());

		char cmd[1024];
		prompt(cmd, false, false);

		std::vector<std::string> args = split_string(std::string(cmd), ' ');

		// 엔터만 입력이 되었을경우 다음 페이지를 보여준다.
		if ( args.size() == 0 ) {
			offset += show_max_line;

		} else {
			/* 입력이 숫자 */
			if ( is_number(args[0].c_str()) ) {
				/* 입력이 게시물 번호 */
				bool is_dir;
				show_news(title, atoi(args[0].c_str()), list, &is_dir);

				if ( is_dir ) {
					offset = 0;
				}

			} else {
				// 상위 명령
				if ( !strcasecmp(args[0].c_str(), "p") ) {
					break;
				}
				// 초기화면
				if ( !strcasecmp(args[0].c_str(), "t") ) {
					*goto_top = true;
					break;
				}
				// 게시물을 처음부터 출력
				if ( !strcasecmp(args[0].c_str(), "dir") ) {
					offset = 0;
				}

				// 게시글의 다음 페이지를 보여준다.
				if ( !strcasecmp(args[0].c_str(), "n") ) {
					offset += show_max_line;
				}

				// 게시글의 이전 페이지를 보여준다.
				if ( !strcasecmp(args[0].c_str(), "b") ) {
					offset -= show_max_line;
					if ( offset <= 0 ) offset = 0;
				}
			}
		}
	}

	return true;
}

void show_news(std::string title, int no, std::vector<news_data> list, bool *is_dir)
{
	char cmd[1024];

	*is_dir = false;

	// 기사 번호 범위 검사
	if ( no < 1 || (unsigned int)no > list.size() ) {
		printf("\r\n해당 번호의 기사가 없습니다.");
		printf("\r\n[Enter] 를 누르세요.");
		press_enter();
		return;
	}

	const news_data &data = list[no-1];

	// 본문은 한 번만 변환
	std::string text = html2text(data.description);
	std::vector<std::string> lines = split_string(text, '\n');

	// 앞뒤 빈 줄 제거
	while ( lines.size() > 0 && trim(lines[0]).empty() ) lines.erase(lines.begin());
	while ( lines.size() > 0 && trim(lines[lines.size()-1]).empty() ) lines.pop_back();
	if ( lines.size() == 0 ) {
		lines.push_back("  (본문 요약이 없습니다. 링크에서 기사를 확인하세요.)");
	}

	// 전체 페이지 수
	int page_count = lines.size() / show_max_line;
	if ( lines.size() % show_max_line >= 1 ) {
		page_count += 1;
	}

	unsigned int offset = 0;

	// 현재 페이지
	int page_no = 1;

	while (1) {
		printf(ESC_CLEAR);

		if ( offset > 0 && lines.size() <= offset ) {
			offset -= show_max_line;
			page_no -= 1;
		}

		print_news_board_header(list.size(), page_no, page_count, (char*)title.c_str());

		std::string news_title = string_truncate(data.title, 70, "...");
		printf(" 제  목: %s\r\n", news_title.c_str());

		char author[1024];
		snprintf(author, sizeof(author), " 작성자: %s", data.author.c_str());
		printf("%-40s%39s", author, string_truncate(data.date, 39, "").c_str());
		printf("\r\n");
		printf(" 링  크: %s", string_truncate(data.link, 70, "").c_str());

		printf("\r\n%s\r\n", repeat("─", 40).c_str());

		// 게시글 출력
		for(unsigned int i=offset; i<lines.size() && i<offset+show_max_line; i++) {
			printf("%s\r\n", lines[i].c_str());
		}

		printf("%s\r\n", repeat("━", 40).c_str());

		print_file("txt/article_footer.txt");

		prompt(cmd, false, false);

		std::vector<std::string> args = split_string(std::string(cmd), ' ');

		// 엔터만 입력이 되었을경우 글의 다음 페이지를 보여준다.
		if ( args.size() == 0 ) {
			// 마지막 페이지에서 엔터면 목록으로
			if ( page_no >= page_count ) {
				return;
			}
			offset += show_max_line;
			page_no += 1;

		} else {
			// 상위 명령
			if ( !strcasecmp(args[0].c_str(), "p") ) {
				return;
			}

			// 본문의 다음 페이지를 보여준다.
			if ( !strcasecmp(args[0].c_str(), "n") ) {
				if ( page_no < page_count ) {
					offset += show_max_line;
					page_no += 1;
				}
			}

			// 본문의 이전 페이지를 보여준다.
			if ( !strcasecmp(args[0].c_str(), "b") ) {
				if ( page_no > 1 ) {
					offset -= show_max_line;
					page_no -= 1;
				}
			}

			// 게시물을 다시 처음부터 출력
			if ( !strcasecmp(args[0].c_str(), "dir") ||
				!strcasecmp(args[0].c_str(), "ls") ) {
				*is_dir = true;
				return;
			}
		}
	}
}
