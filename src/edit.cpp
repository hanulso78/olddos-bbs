#include "main.h"

bool edit_article(char *table_name, int no)
{
	char cmd[20];
	char buf[9072];
	
	bool ok = false;

	//bool is_admin = database::check_admin(login_user_id);
	if ( !login_user_is_admin && !database::check_same_author(table_name, no, login_user_id) ) {
		printf("\r\n작성자만 편집 할 수 있습니다.");
		printf("\r\n[Enter] 를 누르세요.");
		press_enter();
		return false;
	}

	printf("\r\n[1]제목 [2]본문 : ");
	line_input(cmd, 1);

	// 제목 편집
	if ( !strcasecmp(cmd, "1") ) {
		char new_title[1024];

		sprintf(buf, "SELECT * FROM %s WHERE NO=%d", table_name, no);
		std::vector<std::map<std::string, std::string> > rows = database::fetch_rows(buf);
		std::map<std::string, std::string> row = rows.at(0);

		// 게시글 제목
		std::string title = std::string(row[std::string("TITLE")].c_str());

		// 타이틀 입력 (기본 한글 입력으로 전환)
		printf(ESC_HAN);

		printf("\r\n제목 : ");
		line_input_edit(new_title, (char*)title.c_str(), 60);
		if ( strlen(new_title) <= 0 ) {
			printf("\r\n취소 되었습니다.");
			printf("\r\n[Enter] 를 누르세요.");
			press_enter();

		} else {
			// DB 업데이트
			database::edit_article_title(table_name, no, new_title);
			ok = true;
		}

	// 본문 편집
	} else if ( !strcasecmp(cmd, "2") ) {
		while (1) {
			printf("\r\n\r\n작성하실 편집기나 전송 프로토콜을 선택해주세요.");
			//printf("\r\n[1]라인편집기 [2]스크린편집기(vi) [3]zmodem [0]취소");
			printf("\r\n[1]라인편집기 [2]스크린편집기(pico) [3]zmodem [0]취소");
			printf("\r\n선택 >> ");
			line_input(cmd, 1);

			if ( !strcasecmp(cmd, "0") || strlen(cmd) == 0 ) {
				printf("\r\n취소 되었습니다.");
				printf("\r\n[Enter] 를 누르세요.");
				press_enter();
				break;
			}

			sprintf(buf, "SELECT * FROM %s WHERE NO=%d", table_name, no);
			std::vector<std::map<std::string, std::string> > rows = database::fetch_rows(buf);
			std::map<std::string, std::string> row = rows.at(0);

			// 게시글 내용을 텍스트로 변환
			std::string base64_string = std::string(row[std::string("CONTENT")].c_str());
			//std::string plain_string = base64_decode(base64_string);
			std::string plain_string = base64_string;
			std::vector<std::string> lines = split_string(plain_string, '\n');

			// 라인 편집기 진입
			if ( !strcasecmp(cmd, "1") ) {
				if(line_editor(lines, true)) {
					std::string str = string_convert(lines);
					if ( str.length() > 0 ) {
						// 게시글 내용 base64 인코딩
						//std::string base64_string = base64_encode((const unsigned char*)str.c_str(), str.length());
						std::string base64_string = str;

						// DB 업데이트
						database::edit_article_content(table_name, no, base64_string);

						ok = true;
						break;

					} else {
						printf("\r\n입력된 글이 없습니다.");
						printf("\r\n[Enter] 를 누르세요.");
						press_enter();
					}
				}

			// 스크린 편집기 진입
			} else if ( !strcasecmp(cmd, "2") ) {
				char tmpfile[9072];
				char rcfile[9072];

				// ---------------------------------------------------
				// 우선 게시글의 텍스트를 임시 파일로 저장한다.
				sprintf(tmpfile, "%s/tmp", getenv("HANULSO"));
				sprintf(tmpfile, "%s", tempnam(tmpfile, "file"));

				FILE *fp = fopen(tmpfile, "w");
				for(int i=0; i<lines.size(); i++) {
					fprintf(fp, "%s\n", trim(lines[i]).c_str());
				}
				fclose(fp);

				std::string txt1 = read_file(tmpfile);

				add_user_tmpfile(tmpfile);
				// -----------------------------------------------------

				//sprintf(buf, "stty rows 80 cols 80; vi \"%s\"", tmpfile);
				// -Z: Start vim in restricted mode. Do not allow shell commands or suspension of the editor. {vim}
#if 0
				sprintf(rcfile, "%s/vi.rc", getenv("HANULSO"));
				sprintf(buf, "vi -Z -u \"%s\" \"%s\"", rcfile, tmpfile);
#else
				//sprintf(buf, "nano \"%s\"", tmpfile);
				sprintf(buf, "%s/bin/pico \"%s\"", getenv("HANULSO"), tmpfile);
#endif

				ioctl(0, TCSETAF, &sys_term);
				int a = system(buf);
				ioctl(0, TCSETAF, &curr_term);

				printf(ESC_RESET);

#if 0
				if ( WEXITSTATUS(a) != 0 ) {
					printf("\r\n문서 편집이 취소 되었습니다.\r\n");
					printf("\r\n[Enter] 를 누르세요.");
					press_enter();
					break;
				}
#endif

				std::string str = read_file(tmpfile);
				if ( txt1 != str ) {
					// 게시글 내용 base64 인코딩
					//base64_string = base64_encode((const unsigned char*)str.c_str(), str.length());
					base64_string = str;

					// DB 업데이트
					database::edit_article_content(table_name, no, base64_string);

					ok = true;
				} else {
					printf("\r\n취소 되었습니다.");
					printf("\r\n[Enter] 를 누르세요.");
					press_enter();

					ok = false;
				}

				// 파일 삭제
				unlink(tmpfile);
				break;

			// zmodem 업로드 
			} else if ( !strcasecmp(cmd, "3") ) {
				char *lines;
				int length;

				if ( file_editor(&lines, &length) ) {
					if ( length > 0 ) {
						if ( is_binary(lines, length) ) {
							printf("\r\n텍스트 파일만 지원합니다.");
							printf("\r\n[Enter] 를 누르세요.");
							press_enter();
							break;
						}

						// 게시글 내용 base64 인코딩
						//std::string base64_string = base64_encode((const unsigned char*)lines, length);
						std::string base64_string = lines;

						// DB 업데이트
						database::edit_article_content(table_name, no, base64_string);

						ok = true;
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

	return ok;	
}

