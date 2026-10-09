#include "main.h"

int write_article(char *table_name)
{	
	where_scope where("글쓰기: " + current_where);
	char title[1024];
	char buf[1024];
	int ret = -1;

	// 타이틀 입력 (기본 한글 입력으로 전환)
	printf(ESC_HAN);

	printf("\r\n제목 : ");
	line_input(title, 60);
	if ( strlen(title) <= 0 ) {
		printf("\r\n취소 되었습니다.");
		printf("\r\n[Enter] 를 누르세요.");
		press_enter();

		ret = -1;

	} else {
		while (1) {
			// 작성 방법 선택
			printf("\r\n\r\n작성하실 편집기나 전송 프로토콜을 선택해주세요.");
			printf("\r\n\033[=14F[1]\033[=15F줄 편집기 \033[=14F[2]\033[=15F화면 편집기(pico) \033[=14F[3]\033[=15F파일 올리기 "
					"\033[=14F[0]\033[=15F취소");
			printf("\r\n\033[=7F(Enter: 줄 편집기)\033[=15F >> ");
			printf(ESC_ENG);
			line_input(buf, 1);
			if ( strlen(buf) == 0 ) strcpy(buf, "1");

			if ( !strcasecmp(buf, "0") ) {
				printf("\r\n취소 되었습니다.");
				printf("\r\n[Enter] 를 누르세요.");
				press_enter();
				ret = -1;
				break;

			} else if ( !strcasecmp(buf, "1") ) {
				std::vector<std::string> lines;

				if(line_editor(lines, false)) {
					std::string str = string_convert(lines);
					if ( str.length() > 0 ) {
						// 게시글 내용 base64 인코딩
						//std::string base64_string = base64_encode((const unsigned char*)str.c_str(), str.length());
						std::string base64_string = str;

						// .......
						// 게시글 DB에 저장
						int no = database::add_article(table_name, login_user_id,
								(char*)(date_now_string(false).c_str()), 
								(char*)(time_now_string().c_str()),
								title, (char*)base64_string.c_str());
						ret = no;
						break;

					} else {
						printf("\r\n입력된 글이 없습니다.");
						printf("\r\n[Enter] 를 누르세요.");
						press_enter();
						ret = -1;
					}
				}

			} else if ( !strcasecmp(buf, "2") ) {
				char tmpfile[9072];
				char rcfile[9072];

				// ---------------------------------------------------
				// 우선 게시글의 텍스트를 임시 파일로 저장한다.
				char edit_dir[9072];
				if ( !make_editor_tmpfile(edit_dir, tmpfile, sizeof(tmpfile)) ) {
					printf("\r\n임시 파일을 만들지 못했습니다.\r\n");
					press_enter();
					continue;
				}

				std::string txt1 = read_file(tmpfile);
				// -----------------------------------------------------

				//sprintf(buf, "stty rows 80 cols 80; vi \"%s\"", tmpfile);
				snprintf(buf, sizeof(buf), "%s", screen_editor_command(edit_dir, tmpfile).c_str());

				ioctl(0, TCSETAF, &sys_term);
				system(buf);
				ioctl(0, TCSETAF, &curr_term);

				printf(ESC_RESET);

				std::string txt2 = read_file(tmpfile);

				// 파일이 수정되었으면..
				if ( txt1 != txt2 ) {
					std::string str = read_file(tmpfile);

					// 작성일
					int year, month, day;
					std::string week;
					date_now(&year, &month, &day, week);

					std::ostringstream date;
					date << year << "-" << month << "-" << day;

					// 게시글 내용 base64 인코딩
					//std::string base64_string = base64_encode((const unsigned char*)str.c_str(), str.length());
					std::string base64_string = str;

					// 게시글 DB에 저장
					int no = database::add_article(table_name, login_user_id,
							(char*)(date_now_string(false).c_str()), 
							(char*)(time_now_string().c_str()),
							title, (char*)base64_string.c_str());
					ret = no;

				} else {
					printf("\r\n취소 되었습니다.");
					printf("\r\n[Enter] 를 누르세요.");
					press_enter();
					ret = -1;
				}

				// 편집 폴더째 삭제 (파일만 지우면 빈 폴더가 tmp 에 쌓인다)
				remove_tmp_dir(edit_dir);
				break;

			} else if ( !strcasecmp(buf, "3") ) {
				char *lines = NULL;
				int length = 0;

				int protocol = ask_upload_protocol();
				if ( protocol == 0 ) continue;

				if ( file_editor(&lines, &length, protocol) ) {
					if ( length > 0 ) {
						if ( is_binary(lines, length) ) {
							printf("\r\n텍스트 파일만 지원합니다.");
							printf("\r\n[Enter] 를 누르세요.");
							press_enter();
							ret = -1;

						} else {
							// 작성일
							int year, month, day;
							std::string week;
							date_now(&year, &month, &day, week);

							std::ostringstream date;
							date << year << "-" << month << "-" << day;

							// 게시글 내용 base64 인코딩
							//std::string base64_string = base64_encode((const unsigned char*)lines, length);
							std::string base64_string = lines;

							// 게시글 DB에 저장
							int no = database::add_article(table_name, login_user_id,
									(char*)(date_now_string(false).c_str()), 
									(char*)(time_now_string().c_str()),
									title, (char*)base64_string.c_str());
							ret = no;
						}
					}
				}

				if ( length > 0 ) {
					free(lines);
				}
				break;

			} else {
				printf("\r\n현재 지원하지 않습니다.");
				printf("\r\n[Enter] 를 누르세요.");
				press_enter();
			}
		}
	}

	// 이 게시판을 구독한 회원에게 알림
	if ( ret != -1 ) notify_subscribers(table_name, ret);

	return ret;
}

