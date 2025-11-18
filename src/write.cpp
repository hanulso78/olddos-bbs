#include "main.h"

int write_article(char *table_name)
{	
	char title[1024];
	char buf[1024];
	int ret;

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
			printf("\r\n[1]라인편집기 [2]스크린편집기(pico) [3]zmodem [0] 취소");
			printf("\r\n선택 >> ");
			line_input(buf, 1);

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
				sprintf(tmpfile, "%s/tmp", getenv("HANULSO"));
				sprintf(tmpfile, "%s", tempnam(tmpfile, "file"));

				std::string txt1 = read_file(tmpfile);

				add_user_tmpfile(tmpfile);
				// -----------------------------------------------------

				//sprintf(buf, "stty rows 80 cols 80; vi \"%s\"", tmpfile);
				sprintf(buf, "%s/bin/pico \"%s\"", getenv("HANULSO"), tmpfile);

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

				// 파일 삭제
				unlink(tmpfile);
				break;

			} else if ( !strcasecmp(buf, "3") ) {
				char *lines;
				int length;

				if ( file_editor(&lines, &length) ) {
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

	return ret;
}

