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
bool show_news_menu(category c, bool *goto_top);
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
	}

	// -----------------------------------------------------------
	pugi::xml_node node = doc.first_element_by_path("/news");
	pugi::xml_node child = node.first_child();

	while (1) {
		if ( child.type() == pugi::node_element ) {
			newspaper np;
			np.name = child.attribute("name").value();
			np.door = child.attribute("door").value();

			//printf(" %s\r\n", np.name.c_str());
	
			pugi::xml_node child2 = child.first_child();
			while (1) {
				if ( child2.type() == pugi::node_element ) {
					category c;
					c.door = (char*)(child2.child("door").child_value());
					c.name = (char*)(child2.child("name").child_value());
					c.rss = (char*)(child2.child("rss").child_value());
					np.categories.push_back(c);

/*
					printf("--> %s\r\n", c.door.c_str());
					printf("--> %s\r\n", c.name.c_str());
					printf("--> %s\r\n", c.rss.c_str());
*/
				}

				child2 = child2.next_sibling();
				if ( child2.empty() ) break;
			}

			list.push_back(np);
		}

		child = child.next_sibling();
		if ( child.empty() ) break;
	}
}

void show_newspaper_board(pugi::xml_node node, bool *goto_top)
{
	newspapers.clear();
	read_newspaper_menu(newspapers);

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

		// ----------------- 헤더 출력 -------------------
        printf("\033[1;1H");
		printf("\033[=9F\033[=1G%s\033[=15F\033[=1G", repeat("─", 40).c_str());
        printf("\033[1;1H");
		printf("\033[1A\033[7m%s\033[0m", host_name);
		// 타이틀 출력
		int center = (80-strlen(strip_ansi_codes(title)))/2;
        printf("\033[2;1H");
		printf("\r\033[%dC%s", center, title);

        printf("\033[3;1H");
		printf("\033[=0F\033[=1G%s\033[=15F\033[=1G", repeat("━", 40).c_str());

        printf("\033[4;1H");
		printf("%5s%s", 
				"번호",
				centered("신문사", 30).c_str());
		printf("\r\n%s\r\n", repeat("─", 40).c_str());

		for(unsigned int i=0; i<newspapers.size(); i++) {
			newspaper np = newspapers[i];
			printf("%5s%s%s",
				np.door.c_str(),
				centered(np.name, 30).c_str(), 
				"");
			printf("\r\n");
		}

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
						bool goto_top;
						show_category(np, &goto_top);
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
			
		//char *header = (char*)(node.child("header").child_value());
		//print_file(header);

		// ----------------- 헤더 출력 -------------------
        printf("\033[1;1H");
		printf("\033[=9F\033[=1G%s\033[=15F\033[=1G", repeat("─", 40).c_str());
        printf("\033[1;1H");
		printf("\033[1A\033[7m%s\033[0m", host_name);
		// 타이틀 출력
		int center = (80-strlen(strip_ansi_codes(title)))/2;
        printf("\033[2;1H");
		printf("\r\033[%dC%s", center, title);

        printf("\033[3;1H");
		printf("\033[=0F\033[=1G%s\033[=15F\033[=1G", repeat("━", 40).c_str());

        printf("\033[4;1H");
		printf("%5s%s", 
				"번호",
				centered("카테고리", 30).c_str());
		printf("\r\n%s\r\n", repeat("─", 40).c_str());

		for(unsigned int i=0; i<np.categories.size(); i++) {
			category c = np.categories[i];
			printf("%5s%s%s",
				c.door.c_str(),
				centered(c.name, 30).c_str(), 
				"");
			printf("\r\n");
		}

		printf("%s\r\n", repeat("━", 40).c_str());

		//char *footer = (char*)(node.child("footer").child_value());
		//print_file(footer);

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
						bool goto_top;
						show_news_menu(c, &goto_top);
					}
				}
			}
		}
	}
}

// 기사 읽음
bool read_news(category c, std::vector<news_data> &list)
{
	char buf[1024];
	sprintf(buf, "wget -O %s/tmp/news_%s.utf8.xml -o %s/tmp/news_%s.log %s", 
			getenv("HANULSO"), login_user_id, 
			getenv("HANULSO"), login_user_id, 
			c.rss.c_str());
	int a = system(buf);
	if ( WEXITSTATUS(a) != 0 ) {
		printf("\r\n다운로드를 실패하였습니다.\r\n");
		printf("\r\n[Enter] 를 누르세요.");
		press_enter();
		return false;
	}

	char out_file[1024];
	sprintf(out_file, "%s/tmp/news_%s.cp949.xml", getenv("HANULSO"), login_user_id);

	sprintf(buf, "iconv -c -f UTF8 -t CP949//IGNORE//TRANSLIT "
			"'%s/tmp/news_%s.utf8.xml' > ''%s''", 
			getenv("HANULSO"), login_user_id, out_file);

	a = system(buf);
	if ( WEXITSTATUS(a) != 0 ) {
		printf("\r\niconv 실행을 실패하였습니다.\r\n");
		printf("\r\n[Enter] 를 누르세요.");
		press_enter();
		return false;
	}

	// 날씨 xml 파일 버퍼로 읽어들임
	std::string string = read_file(out_file);

	// 임시 파일들 삭제
	sprintf(buf, "rm -f \"%s/tmp/news_%s.utf8.xml\"", getenv("HANULSO"), login_user_id);
	a = system(buf);
	if ( WEXITSTATUS(a) != 0 ) {
		return false;
	}

	sprintf(buf, "rm -f \"%s/tmp/news_%s.cp949.xml\"", getenv("HANULSO"), login_user_id);
	a = system(buf);
	if ( WEXITSTATUS(a) != 0 ) {
		return false;
	}

	sprintf(buf, "rm -f \"%s/tmp/news_%s.log\"", getenv("HANULSO"), login_user_id);
	a = system(buf);
	if ( WEXITSTATUS(a) != 0 ) {
		return false;
	}

	// 버퍼 파싱
	pugi::xml_document doc;
	//pugi::xml_parse_result res = doc.load_file(out_file, pugi::parse_default|pugi::parse_declaration);
	pugi::xml_parse_result res = doc.load_string(string.c_str());
    if (!res) {
		std::ostringstream msg;
		msg << " Parse error: " << res.description() << ", character pos= " << res.offset;
		printf("\r\n\r\n%s", msg.str().c_str());
		printf("\r\n[Enter] 를 누르세요.");
		press_enter();
	}

	// -----------------------------------------------------------
	pugi::xml_node node = doc.first_element_by_path("/rss/channel");
	pugi::xml_node child = node.first_child();

	int no = 1;
	while (1) {
		if ( child.type() == pugi::node_element ) {
			std::string name = child.name();
			if ( name == "item" ) {
				news_data data;
				data.no = no;
				data.author = (char*)(child.child("author").child_value());
				std::string title = child.child("title").child_value();
				title = replace_all(title, "&lt;", "<");
				title = replace_all(title, "&gt;", ">");
				title = replace_all(title, "<br>", "");
				title = replace_all(title, "<BR>", "");
				data.title = title;
				data.link = (char*)(child.child("link").child_value());
				data.description = (char*)(child.child("description").child_value());
				data.date = (char*)(child.child("pubDate").child_value());
				list.push_back(data);
				no++;
			}
		}

		child = child.next_sibling();
		if ( child.empty() ) break;
	}
}

void print_news_board_header(int total, char *title)
{
    printf("\033[1;1H");
	printf("\033[=9F\033[=1G%s\033[=15F\033[=1G", repeat("─", 40).c_str());
    printf("\033[1;1H");
	printf("\033[1A\033[7m%s\033[0m", host_name);
	// 타이틀 출력
	int center = (80-strlen(strip_ansi_codes(title)))/2;
    printf("\033[2;1H");
	printf("\r\033[%dC%s", center, title);

	// 총 글 갯수 출력
	char buf[1024];
	sprintf(buf, "%4d/%4d (총 %4d건) ", 1, 1, total);
	printf("\r\033[57C%s", buf);
	
    printf("\033[3;1H");
	printf("\033[=0F\033[=1G%s\033[=15F\033[=1G\r\n", repeat("━", 40).c_str());
}

bool show_news_menu(category c, bool *goto_top)
{
	printf(ESC_CLEAR);
	printf("뉴스를 다운로드 중입니다...\r\n");
	//printf("%s\r\n", c.rss.c_str());
	usleep(1000);

	std::vector<news_data> list;
	read_news(c, list);
	
	int offset = 0;
		
	while (1) {
		printf(ESC_CLEAR);

		//char *header = (char*)(node.child("header").child_value());
		//print_file(header);

		print_news_board_header(list.size(), (char*)c.name.c_str());
		
        printf("\033[4;1H");
		printf("%5s %s %s %s", 
				"번호",
				centered("작성자", 12).c_str(), 
				centered("날짜", 5).c_str(), 
				"제목");
		printf("\r\n%s\r\n", repeat("─", 40).c_str());

#if 1
		if ( list.size() <= offset ) {
			offset -= show_max_line;
		}

		for(unsigned int i=offset; i<offset+show_max_line; i++) {
			if ( i >= list.size()-1 ) break;

			news_data data = list[i];

			std::string title = string_truncate(data.title, 49, "...");

			printf("%5d %s %s ", 
				data.no,
				centered(string_truncate(data.author, 10, ""), 12).c_str(), 
				centered(data.date, 5).substr(5, 5).c_str());
			// 답글을 경우 회색으로 표시
			printf("%s", title.c_str());
			// 답글을 표시했을경우 흰색으로 복구
			printf("\r\n");
		}
#endif
		printf("%s\r\n", repeat("━", 40).c_str());

		//char *footer = (char*)(node.child("footer").child_value());
		//print_file(footer);

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
				show_news(c.name, atoi(args[0].c_str()), list, &is_dir);

				if ( is_dir ) {
					offset = 0;
				}

			} else {
				// 상위 명령
				if ( !strcasecmp(args[0].c_str(), "p") ) {
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
}

void show_news(std::string title, int no, std::vector<news_data> list, bool *is_dir)
{
	char cmd[1024];
	char sql[1024];

	*is_dir = false;

	unsigned int offset = 0;

	// 현재 페이지
	int page_no = 1;

	while (1) {
		printf(ESC_CLEAR);

		print_news_board_header(list.size(), (char*)title.c_str());
		
#if 1
		std::string text = html2text(list[no-1].description);
		std::vector<std::string> lines = split_string(text, '\n');

#else
		std::vector<std::string> lines = split_string(list[no-1].description, '\n');
#endif
		
		// 전체 페이지 수
		int page_count = lines.size() / show_max_line;
		if ( lines.size() % show_max_line >= 1 ) {
			page_count += 1;
		}

		std::string title = list[no-1].title;
		title = string_truncate(title, 60, "...");
		printf(" 제  목: %-60s\r\n", title.c_str());

		if ( lines.size() <= offset ) {
			offset -= show_max_line;
			page_no -= 1;
		}

		printf("\033[1A\033[70C(%3d/%3d)\r\n", page_no, page_count);

		char author[1024];
		char date_time[1024];
		sprintf(author, " 작성자: %s", list[no-1].author.c_str());
		//sprintf(date, "%s %s", row[std::string("DATE")].c_str(), row[std::string("TIME")].c_str());
		sprintf(date_time, "%s", list[no-1].date.c_str());
		printf("%-40s%39s", author, date_time);
		printf("\r\n");
		printf(" 링  크: %s", list[no-1].link.c_str());

		printf("\r\n%s\r\n", repeat("─", 40).c_str());

		// 게시글 출력
		unsigned int l=0;
		for(unsigned int i=offset; i<lines.size(); i++) {
			if (l < show_max_line )
				printf("%s\r\n", lines[i].c_str());
			l++;
		}

		printf("%s\r\n", repeat("━", 40).c_str());

		print_file("txt/article_footer.txt");

		prompt(cmd, false, false);

		std::vector<std::string> args = split_string(std::string(cmd), ' ');

		// 엔터만 입력이 되었을경우 글의 다음 페이지를 보여준다.
		if ( args.size() == 0 ) {
			offset += show_max_line;
			page_no += 1;

		} else {
			// 상위 명령
			if ( !strcasecmp(args[0].c_str(), "p") ) {
				return;
			}

			// 본문의 다음 페이지를 보여준다.
			if ( !strcasecmp(args[0].c_str(), "n") ) {
				offset += show_max_line;
				page_no += 1;
			}

			// 본문의 이전 페이지를 보여준다.
			if ( !strcasecmp(args[0].c_str(), "b") ) {
				offset -= show_max_line;
				page_no -= 1;

				if ( page_no <= 1 ) {
					offset = 0;
					page_no = 1;
				}
			}
				
			// 게시물을 다시 처음부터 출력
			if ( !strcasecmp(args[0].c_str(), "dir") ||
				!strcasecmp(args[0].c_str(), "ls") ) {
				*is_dir = true;
				return;
			}

#if 0
			// 게시글 내용 출력
			if ( !strcasecmp(args[0].c_str(), "pr") ) {
				printf("\r\n갈무리를 준비하시고 [Enter]를 누르십시오.");
				press_enter();

				printf("\r\n\r\n");
				printf(" 제  목: %-70s\r\n", title.c_str());

				char author[1024];
				char date_time[1024];
				sprintf(author, " 작성자: %s (%s)", row[std::string("USER_ID")].c_str(), user["NICK_NAME"].c_str());
				sprintf(date_time, "%s %s", row[std::string("DATE_TIME")].c_str());
				printf("%-54s%-24s", author, date_time);
				printf("\r\n%s\r\n", repeat("─", 40).c_str());

				for(unsigned int i=offset; i<lines.size(); i++) {
					printf("%s\r\n", lines[i].c_str());
				}

				printf("\r\n");
				printf("게시글의 마지막 입니다. [Enter]를 누르세요.");
				press_enter();
			}
#endif
		}
	}
}
