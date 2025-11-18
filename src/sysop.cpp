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

void prompt(char *cmd)
{
	printf(ESC_ENG);
	printf("이동(번호) 상위메뉴(P) 종료(X)\r\n");
	printf("선택 >> ");
	line_input(cmd, 30);
		
	std::vector<std::string> args = split_string(std::string(cmd), ' ');
	if ( args.size() == 0 ) return;

	/* 입력이 명령 코드 */
	if( !is_number(cmd) ) {
		// 종료 명령
		if ( !strcasecmp(args[0].c_str(), "x") ) {
			host_close();
		}
	}
}

void modify_user_info(void)
{
	while (1) {
		printf(ESC_CLEAR);

		printf("***** 회원 정보 변경 *****\r\n\r\n");
		printf("1. 레벨 변경\r\n");
		printf("2. 비밀번호 변경\r\n");
		printf("\r\n");

		char cmd[1024];
		prompt(cmd);
				
		/* 입력이 숫자 */
		if( is_number(cmd) && (atoi(cmd) >= 1 && atoi(cmd) <= 3) ) {
			char id [256];

			printf(ESC_ENG);
			printf("\r\n아이디 입력 : ");
			line_input(id, 50);
			if ( strlen(id) <= 0 ) continue;

			std::map<std::string, std::string> user;
			if ( database::exist_user_id(id) == false ) {
				printf("\r\n등록된 아이디가 없습니다.");
				printf("\r\n[Enter] 를 누르세요.");
				press_enter();

			} else {
				bool exist;
				user = database::user_info(id, &exist);

				if ( !strcmp(cmd, "1") ) {
					int user_level = atoi(user["LEVEL"].c_str());

					printf("\r\n'%s(%s)' 님의 등급은 '%s' 입니다.", 
						user["NICK_NAME"].c_str(), id,
						get_level_name(user_level).c_str());
					printf("\r\n새로운 등급을 선택하세요.");
					printf("\r\n");
					for(unsigned int i=0; i<level_nums.size(); i++) {
						printf("[%d]%s ", level_nums[i], level_names[i].c_str());
					}

					printf("\r\n선택 : ");
					
					char new_level[20];
					line_input(new_level, 2);

					if ( is_number(new_level) ) {
						if ( check_exist_level(atoi(new_level)) ) {
							if ( database::set_user_level(id, atoi(new_level)) ) {
								printf("\r\n정상적으로 변경 되었습니다.");
								printf("\r\n[Enter] 를 누르세요.");
								press_enter();
							}
						} else {
							printf("\r\n등급이 존재하지 않습니다.");
							printf("\r\n[Enter] 를 누르세요.");
							press_enter();
						}
					}
				} else if ( !strcmp(cmd, "2") ) {
					printf(ESC_HAN);
					printf("\r\n비밀번호 입력 : ");
					char password [256];
					line_input(password, 50);

					if ( strlen(password) > 0 ) {
						if ( database::set_user_password(id, password) ) {
							printf("\r\n정상적으로 변경 되었습니다.");
							printf("\r\n[Enter] 를 누르세요.");
							press_enter();
						}
					}

				}
			}
		} else {
			if ( !strcmp(cmd, "p") ) { 
				return;
			}
		}
	}
}

void find_user(void)
{
	while (1) {
		printf(ESC_CLEAR);

		printf("***** 회원 검색 *****\r\n\r\n");
		printf("1. 아이디로 닉네임 검색\r\n");
		printf("2. 닉네임으로 아이디 검색\r\n");
		printf("\r\n");

		char cmd[1024];
		prompt(cmd);
				
		/* 입력이 숫자 */
		if ( is_number(cmd) && atoi(cmd) == 1 ) {
			char id [256];

			printf(ESC_ENG);
			printf("\r\n아이디 입력 : ");
			line_input(id, 50);
			if ( strlen(id) <= 0 ) continue;

			std::map<std::string, std::string> user;
			if ( database::exist_user_id(id) == false ) {
				printf("\r\n등록된 아이디가 없습니다.");
				printf("\r\n[Enter] 를 누르세요.");
				press_enter();

			} else {
				bool exist;
				user = database::user_info(id, &exist);

				printf("\r\n닉네임은 '%s' 입니다.", user["NICK_NAME"].c_str());

				printf("\r\n[Enter] 를 누르세요.");
				press_enter();

			}

		} else if ( is_number(cmd) && atoi(cmd) == 2 ) {
			char nick_name [256];

			printf(ESC_HAN);
			printf("\r\n닉네임 입력 : ");
			line_input(nick_name, 50);
			if ( strlen(nick_name) <= 0 ) continue;

			std::map<std::string, std::string> user;
			bool exist;
			user = database::user_info_by_nick_name(nick_name, &exist);

			printf("\r\n아이디는 '%s' 입니다.", user["USER_ID"].c_str());

			printf("\r\n[Enter] 를 누르세요.");
			press_enter();

		} else {
			if ( !strcmp(cmd, "p") ) { 
				return;
			}
		}
	}
}

void delete_user(void)
{
	printf(ESC_CLEAR);

	printf("***** 회원 정리 *****\r\n\r\n");
	printf("삭제된 회원은 복구가 불가능합니다.\r\n신중하게 입력해주세요.\r\n");

	printf(ESC_ENG);
	printf("\r\n아이디 입력 : ");
	
	char id [256];
	line_input(id, 50);
	if ( strlen(id) <= 0 ) return;

	std::map<std::string, std::string> user;
	if ( database::exist_user_id(id) == false ) {
		printf("\r\n등록된 아이디가 없습니다.");
		printf("\r\n[Enter] 를 누르세요.");
		press_enter();

	} else {
		if ( database::delete_user(id) == true ) {
			printf("\r\n정상적으로 삭제 되었습니다.");
			printf("\r\n[Enter] 를 누르세요.");
			press_enter();
		}
	}
}

void delete_article(void)
{
	printf(ESC_CLEAR);

	printf("***** 게시물 정리 *****\r\n\r\n");
	printf("삭제된 게시물은 복구가 불가능합니다.\r\n신중하게 입력해주세요.\r\n");

	printf("\r\n현재 구현되지 않았습니다.");
	printf("\r\n[Enter] 를 누르세요.");
	press_enter();
}

void main_menu(void)
{
	while (1) {
		printf(ESC_CLEAR);

		printf("***** 운영자 메뉴 *****\r\n\r\n");
		printf("1. 회원 검색\r\n");
		printf("2. 회원 정보 변경\r\n");
		printf("3. 회원 삭제\r\n");
		printf("4. 게시물 삭제\r\n");
		printf("\r\n");

		char cmd[1024];
		prompt(cmd);

		/* 입력이 숫자 */
		if( is_number(cmd) ) {
			if ( !strcmp(cmd, "1") ) {
				find_user();

			} else if ( !strcmp(cmd, "2") ) {
				modify_user_info();

			} else if ( !strcmp(cmd, "3") ) {
				delete_user();

			} else if ( !strcmp(cmd, "3") ) {
				delete_article();
			}
		}
	}
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

	main_menu();

	host_close();
}

