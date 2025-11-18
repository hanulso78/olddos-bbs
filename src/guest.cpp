#include "main.h"

struct termio sys_term;

char user_id[50];
char nick_name[50];
char birthday[50];
char email_address[50];
char sex[50];
char user_passwd[50];

std::ostringstream query;

char tty[10];

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

	return;
}

/* 프로그램 종료 루틴 */
int host_close (void)  
{
	database::close();

    ioctl(0, TCSETAF, &sys_term);
    exit(1);
}


void return_login () 
{
	ioctl(0, TCSETAF, &sys_term);
	execl("bin/main","main", tty, (char *)0);
}


int main(int argc, char **argv)
{
	sprintf(tty, "%s", argv[1]);

    signal(SIGQUIT, SIG_IGN);
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
    signal(SIGHUP, (__sighandler_t)host_close);
    signal(SIGSEGV, (__sighandler_t)host_close);
    signal(SIGBUS, (__sighandler_t)host_close);

	read_settings("hanulso.cfg");

    ioctl(0,TCGETA, &sys_term);
	raw_mode();
	
    umask(0111);

	// DB open ...
	if ( database::open() == false )
		exit(1);

	if ( file_exists("txt/register.txt") ) {
		print_file("txt/register.txt");

		printf("\r\n [Enter] 를 누르세요.");
		press_enter();
	}

    printf(ESC_CLEAR);
	
	// member 테이블 생성
	database::create_member();

	printf("\r\n※ 입력란에 /x 를 입력하면 가입을 중단합니다.\r\n");

	char check[6];
	for(int i=0; i<6; i++) {
		check[i] = 0;
	}

	while (1) {
		if ( check[0] == 0 ) {
			printf(ESC_ENG);
			printf("\r\n 1. 아이디를 입력해주세요. (최소 영문 5자~40자 가능)");
			while (1) {
				printf("\r\n >> ");
				line_input(user_id, 40);
				if ( !strcasecmp(user_id, "/x") ) return_login();
				if ( strlen(user_id) >= 5 ) {
					if ( database::exist_user_id(user_id) == true ) {
						printf("\r\n 이미 가입된 아이디 입니다.");
					} else {
						int error=0;
						int i;
						for(i=0; i<strlen(user_id); i++) {
							if (is_han(user_id[i])) {
								printf("\r\n 영문만 지원합니다.");
								error=1;
								break;
							}
						}
						if ( error == 0 ) {
							check[0] = 1;
							break;
						}
					}
				} else {
					printf("\r\n 5자 이상 입력해주세요.");
				}
			}
		}

		if ( check[1] == 0 ) {
			printf(ESC_ENG);
			printf("\r\n 2. 비밀번호를 입력해주세요. (특수문자/숫자를 조합해주세요.)");
			while (1) {
				printf("\r\n >> ");
				line_input(user_passwd, 40);
				if ( !strcasecmp(user_passwd, "/x") ) return_login();
                if(strlen(user_passwd) <= 7) {
					printf("\r\n 비밀번호는 8자리이상 지정하세요.");
                } else {
                    int strong = check_password_strongness(user_passwd);
                    if ( strong == 0 ) {
                        printf("\r\n 비밀번호가 약합니다.");
                    } else {
                        check[1] = 1;
                        break;
                    }
                }
			}
		}

		if ( check[2] == 0 ) {
			printf(ESC_HAN);
			printf("\r\n 3. 닉네임을 입력하세요. (영문 40자, 한글 20자 가능)");
			while (1) {
				printf("\r\n >> ");
				line_input(nick_name, 40);
				if ( !strcasecmp(nick_name, "/x") ) return_login();
				if ( strlen(nick_name) >= 4 ) {
					if ( database::exist_nick_name(nick_name) == true ) {
						printf("\r\n 이미 등록된 닉네임 입니다.");
					} else {
						check[2] = 1;
						break;
					}
				}
			}
		}

		if ( check[3] == 0 ) {
			printf(ESC_ENG);
			printf("\r\n 4. 생년월일을 입력하세요. (년-월-일) (ex) 1970-1-11");
			while (1) {
				printf("\r\n >> ");
				line_input(birthday, 20);
				if ( !strcasecmp(birthday, "/x") ) return_login();

				int year = NULL, month = NULL, day = NULL;
				sscanf(birthday, "%d-%d-%d", &year, &month, &day);

				if ( !is_date_valid(day, month, year) ) {
					printf("\r\n 잘못 입력 되었습니다.");
				} else {
					check[3] = 1;
					break;
				}
			}
		}

		if ( check[4] == 0 ) {
			printf(ESC_ENG);
			printf("\r\n 5. 이메일 주소를 입력하세요.");
			while (1) {
				printf("\r\n >> ");
				line_input(email_address, 40);
				if ( !strcasecmp(email_address, "/x") ) return_login();
				if ( !is_email_valid(email_address) ) {
					printf("\r\n 잘못 입력 되었습니다.");
				} else {
					if ( database::exist_email_address(email_address) == true ) {
						printf("\r\n 이미 등록된 이메일 주소입니다.");
					} else {
						check[4] = 1;
						break;
					}
				}
			}
		}

		if ( check[5] == 0 ) {
			printf(ESC_ENG);
			printf("\r\n 6. [1]남자 / [2]여자 를 선택해주세요.");
			while (1) {
				printf("\r\n >> ");
				line_input(sex, 5);
				if ( !strcasecmp(sex, "/x") ) return_login();
				if ( !strcasecmp(sex, "1") || !strcasecmp(sex, "2") ) {
					check[5] = 1;
					break;
				}
			}
		}

        printf("\r\n\n 수정 항목이 있습니까? (번호/n) ");
		
		char buf[2];
		line_input(buf, 1);

		if ( !strcasecmp(buf, "n") ) break;

		if ( atoi(buf) <= 6 && atoi(buf) >= 1 ) {
			check[atoi(buf)-1] = 0;
		}
	}

	int level = 1;

	query << "INSERT INTO member ( ";
	query << "		USER_ID, NICK_NAME, BIRTHDAY, PASSWORD, EMAIL, SEX, LEVEL, IS_OPEN,";
	query << "		REGISTRATION_DATETIME, LASTLOGIN_DATETIME ";
	query << ") VALUES (";
	query << "'" << user_id << "', ";
	query << "'" << nick_name << "', ";
	query << "'" << birthday << "', ";
	// 단방향 패스워드 알고리즘 사용
	query << "PASSWORD('" << user_passwd << "'), ";
	query << "'" << email_address << "', ";
	query << "'" << sex << "', ";
	query << "'" << level << "', ";
	query << "'" << 0 << "', ";
	query << "'" << datetime_now_string(false) << "', ";
	query << "'" << datetime_now_string(false) << "'";
	query << ")";
	if ( mysql_query(mysql, query.str().c_str()) != 0 ) {
		printf("\r\n(%d) [%s] \"%s\"\r\n", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
		press_enter();
		host_close();
	}

#if 0
	query.str("");
	query << "SELECT * FROM member";
	std::vector<std::map<std::string, std::string> > rows = database::fetch_rows((char*)query.str().c_str());
	std::map<std::string, std::string> row = rows.at(0);

	printf("\r\n------%s\r\n", row[std::string("REGISTRATION_DATETIME")].c_str());
	printf("\r\n------%s\r\n", row[std::string("PASSWORD")].c_str());
	
#endif
	
	printf("\r\n\r\n 정상적으로 사용자 등록이 되었습니다. 로그인해주세요.");
	printf("\r\n [Enter] 를 누르세요.");
	press_enter();

	return_login();
}

