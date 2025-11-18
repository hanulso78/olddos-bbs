#include "main.h"

bool create_chatt_room(char *table_name);
void clean_chatt_room(char *table_name);

bool open_chatt_room(int port, int max_user, char *greeting);
bool connect_chatt_room(int port);

void state_chatt_room(int port, std::string &author, int &user_count);

void show_chatt_rooms(pugi::xml_node node, bool *goto_top)
{
	char *table_name = (char*)(node.attribute("id").value());
	char *title = (char*)(node.child("name").child_value());

	*goto_top = false;
	
	int access_level = atoi((char*)(node.attribute("access_level").value()));

	if ( login_user_level < access_level ) {
		printf("\r\n%s 이상 진입 가능합니다.", get_level_name(access_level).c_str());
		printf("\r\n[Enter] 를 누르세요.");
		press_enter();
		return;
	}

	if ( database::create_chatt_room(table_name) == false )
		return;
	
	int offset = 0;

	// 사용되지 않는 포트의 대화방은 삭제
	clean_chatt_room(table_name);

	while (1) {
		printf(ESC_CLEAR);
		
		int total = database::article_count(table_name);

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

		char buf[1024];
		sprintf(buf, "%4d/%4d (총 %4d건) ", 1, 1, total);
		printf("\r\033[57C%s", buf);
		
        printf("\033[3;1H");
		printf("\033[=0F\033[=1G%s\033[=15F\033[=1G", repeat("━", 40).c_str());
	
        printf("\033[4;1H");
		printf("%5s %s %s %s %s", 
				"번호",
				centered("방장", 12).c_str(), 
				centered("인원", 6).c_str(), 
				centered("공개", 8).c_str(), 
				"주제");
        printf("\033[5;1H");
		printf("%s", repeat("─", 40).c_str());
		
		// ----------------- 대화방 목록 출력 -------------------
        printf("\033[6;1H");

		if ( total < offset ) {
			offset -= show_max_line;
		}
		
		char sql[1024];
		// 대화방 개설 순서의 내림차순으로 보여줌
		sprintf(sql, "SELECT * FROM %s ORDER BY ROOM_NO DESC LIMIT %d, %d", 
				table_name, offset, show_max_line);
		
		std::vector<std::map<std::string, std::string> > rows = database::fetch_rows(sql);

		if ( rows.size() == 0 ) {
			printf("%s\r\n", centered("개설된 대화방이 없습니다.", 80).c_str());

		} else {
			for(unsigned int i=0; i<rows.size(); i++) {
				std::map<std::string, std::string> row = rows.at(i);

				int port_no = atoi(row[std::string("PORT_NO")].c_str());
				const char *room_no = row[std::string("ROOM_NO")].c_str();
				int max_user = atoi(row[std::string("MAX_USER")].c_str());
				char *pub = (char*)"공개";
				if ( row[std::string("PASSWORD")].length() > 0 ) {
					pub = (char*)"비공개";
				}
				
				std::ostringstream title;
				title << string_truncate((char*)row[std::string("TITLE")].c_str(), 33, "...");

				int user_count;
				std::string author;
				state_chatt_room(port_no, author, user_count);

				char buf[1024];
				sprintf(buf, "%2d/%2d", user_count, max_user);

				bool exist;
				std::map<std::string, std::string> user = 
					database::user_info((char*)author.c_str(), &exist);
				const char *nick_name = user["NICK_NAME"].c_str();

				printf("%5s %s %s %s %s",
					room_no,
					centered(string_truncate(nick_name, 12, ""), 12).c_str(), 
					centered(buf, 6).c_str(), 
					centered(pub, 8).c_str(), 
					title.str().c_str());
				printf("\r\n");
			}
		}

		printf("%s\r\n", repeat("━", 40).c_str());
		
		char *footer = (char*)(node.child("footer").child_value());
		print_file(footer);

		// ----------------- 프롬프트 처리 -------------------
		printf(ESC_ENG);
		printf("참여(번호) 개설(O) 이전메뉴(P) 초기화면(T)\r\n");
		printf("선택 >> ");
		char cmd[1024];
		line_input(cmd, 30);

		std::vector<std::string> args = split_string(std::string(cmd), ' ');

		// 엔터만 입력이 되었을경우 다음 페이지를 보여준다.
		if ( args.size() == 0 ) {
			offset += show_max_line;

		} else {
			/* 입력이 숫자 */
			if ( is_number(args[0].c_str()) ) {
				/* 입력이 대화방 번호 */
				int room_no = atoi(args[0].c_str());

				if ( database::exist_chatt_room(table_name, room_no) ) {
					int port = database::chatt_room_port_number(table_name, room_no);
					int max_user = database::chatt_room_max_user(table_name, room_no);

					int user_count;
					std::string author;
					state_chatt_room(port, author, user_count);

					if ( user_count+1 > max_user ) {
						printf("\r\n대화방 허용 인원이 꽉 찼습니다.");
						printf("\r\n[Enter] 를 누르세요.");
						press_enter();

					} else {
						bool connect = true;

						if ( !database::check_public_chatt_room(table_name, room_no) ) {
							// 비밀번호 입력
							char passwd[50];
							printf("\r\n비밀번호 : ");
							line_input_echo(passwd, 50);

							if ( strlen(passwd) == 0 ) {
								printf("\r\n비밀번호를 입력해주세요.");
								printf("\r\n[Enter] 를 누르세요.");
								press_enter();
								connect = false;

							} else {
								if ( database::check_chatt_password(table_name, room_no, passwd) == false ) {
									printf("\r\n비밀번호가 틀렸습니다.");
									printf("\r\n[Enter] 를 누르세요.");
									press_enter();
									connect = false;
								}
							}
						}

						if ( connect == true ) {
							// 대화방 접속
							if ( connect_chatt_room(port) == false ) {
								printf("\r\n대화방 접속에 실패 하였습니다.");
								printf("\r\n[Enter] 를 누르세요.");
								press_enter();
							}

							// 사용되지 않는 채팅방 DB에서 삭제
							clean_chatt_room(table_name);
						}
					}

				} else {
					printf("\r\n입력한 대화방이 개설되어 있지 않습니다.");
					printf("\r\n[Enter] 를 누르세요.");
					press_enter();
				}

			} else {
				// 상위 명령
				if ( !strcasecmp(args[0].c_str(), "p") ) {
					break;
				}
				if ( !strcasecmp(args[0].c_str(), "t") ) {
					*goto_top = true;
					break;
				}
				if ( !strcasecmp(args[0].c_str(), "o") ) {
					create_chatt_room(table_name);
				}
			}
		}
	}
}

void clean_chatt_room(char *table_name)
{
	char sql[1024];
	sprintf(sql, "SELECT * FROM %s", table_name);
	
	std::vector<std::map<std::string, std::string> > rows = database::fetch_rows(sql);

	for(unsigned int i=0; i<rows.size(); i++) {
		std::map<std::string, std::string> row = rows.at(i);

		int room_no = atoi(row[std::string("ROOM_NO")].c_str());
		int port = atoi(row[std::string("PORT_NO")].c_str());

		// 포트 접속이 되지 않는다면 대화방이 폐쇠된것이므로 DB에서 삭제
		if ( !check_used_port(port) ) {
			database::delete_chatt_room(table_name, room_no);
		}
	}
}

// 대화방의 상태
void state_chatt_room(int port, std::string &author, int &user_count)
{
	char buf[1024];
	sprintf(buf, "%s/chatt/%d.room", getenv("HANULSO"), port);

	std::string txt = read_file(buf);
	if ( txt.length() > 0 ) {
		std::vector<std::string> info = split_string(txt, ',');
		author = info[0];
		user_count = atoi(info[1].c_str());
	}
}

bool create_chatt_room(char *table_name)
{
	char title[256];
	char greeting[256];
	char pub[2];
	char password[256];
	char max_user[256];

	memset(title, 0, sizeof(title));
	memset(greeting, 0, sizeof(greeting));
	memset(pub, 0, sizeof(pub));
	memset(password, 0, sizeof(password));
	memset(max_user, 0, sizeof(max_user));

	printf(ESC_CLEAR);

	// 게시글 검색어 입력
	printf(ESC_HAN);
	printf("\r\n대화방 주제를 입력하세요.");
	while (1) {
		printf("\r\n주제 >> ");
		line_input(title, 60);
		if ( strlen(title) > 0 ) break;
	}

	printf("\r\n대화방 환영 메세지를 입력하세요.");
	while (1) {
		printf("\r\n메세지 >> ");
		line_input(greeting, 60);
		if ( strlen(greeting) > 0 ) break;
	}

	printf("\r\n대화방 종류를 선택하세요.");
	while (1) {
		printf("\r\n1.공개  2.비공개 >> ");
		line_input(pub, 1);
		if ( strlen(pub) == 1 && ( !strcmp(pub, "1") || !strcmp(pub, "2") ) ) {
			break;
		}
	}

	if ( !strcmp(pub, "2") ) {
		printf("\r\n대화방 비밀번호를 입력하세요. (4글자이상)");
		while (1) {
			printf("\r\n비밀번호 >> ");
			line_input(password, 20);
			if ( strlen(password) > 4 ) break;
		}
	}
	
	printf("\r\n대화방 허용 인원수를 입력하세요. (2명이상)");
	while (1) {
		printf("\r\n인원수 >> ");
		line_input(max_user, 3);
		if ( strlen(max_user) > 0 && is_number(max_user) && atoi(max_user) > 1 ) {
			break;
		}
	}
	
	// 사용 가능한 포트 번호 검색
	int port = 6000;
	while(1) { 
		if ( !check_used_port(port) ) break;
		port++;
	}

	// 사용 가능한 대화방 번호 검색
	int room_no = 1;
	while(1) {
		if ( !database::exist_chatt_room(table_name, room_no) ) break;
		room_no++;
	}

	bool ret = true;
	// 데이타 베이스에 대화방 추가
	if ( database::add_chatt_room(table_name, room_no, port,
		   	login_user_id, password, atoi(max_user), title) ) {
		// 대화방 생성
		ret = open_chatt_room(port, atoi(max_user), greeting);
		if ( ret ) {
			// 대화방 접속
			ret = connect_chatt_room(port);
			if ( ret == false ) {
				printf("\r\n대화방 접속에 실패 하였습니다.");
				printf("\r\n[Enter] 를 누르세요.");
				press_enter();
			}

			// 사용되지 않는 채팅방 DB에서 삭제
			clean_chatt_room(table_name);

		} else {
			printf("\r\n대화방 개설에 실패 하였습니다.");
			printf("\r\n[Enter] 를 누르세요.");
			press_enter();
		}

	} else {
		ret = false;
	}

	return ret;
}

bool open_chatt_room(int port, int max_user, char *greeting)
{
	// 대화방 서버 생성
	if ( !check_used_port(port) ) {
		std::string greeting2 = add_slashes(greeting);

		char buf[1024];
		sprintf(buf, "%s/bin/startchattserver \"%d\" \"%d\" \"%s\" > /dev/null", 
				getenv("HANULSO"), port, max_user, greeting2.c_str());
		
		int a = system(buf);

		if ( WEXITSTATUS(a) != 0 ) {
			return false;
		}
	}

	return true;
}

bool connect_chatt_room(int port)
{
	// 대화방 서버에 접속
	bool exist;
	std::map<std::string, std::string> user = database::user_info((char*)login_user_id, &exist);
	const char *nick_name = user["NICK_NAME"].c_str();

	char cmd[1024];
	sprintf(cmd, "%s/bin/chattclient \"127.0.0.1\" \"%d\" \"%s\" \"%s\"", 
			getenv("HANULSO"), port, login_user_id, nick_name);
	int a = system(cmd);

	if ( WEXITSTATUS(a) != 0 ) {
		return false;
	}

	return true;
}

