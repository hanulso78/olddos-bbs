#include "main.h"

struct termio sys_term;

char user_id[50];
char birthday[50];

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

	if ( file_exists("txt/pass.txt") ) {
		print_file("txt/pass.txt");
	}

	printf("\r\n※ 입력란에 /x 를 입력하면 중단합니다.\r\n");

	printf(ESC_ENG);
	printf("\r\n 1. 아이디를 입력해주세요.");
	while (1) {
		printf("\r\n >> ");
		line_input(user_id, 40);
		if ( !strcasecmp(user_id, "/x") ) return_login();
		if ( strlen(user_id) == 0 ) {
			printf("\r\n 잘못 입력 되었습니다.");
		} else {
			if ( database::exist_user_id(user_id) == false ) {
				printf("\r\n 가입되지 않은 아이디 입니다.");
			} else {
				break;
			}
		}
	}

	bool exist;
	std::map<std::string, std::string> user = database::user_info(user_id, &exist);
			
    if ( user["EMAIL"].length() <= 0 ) {
        printf("\r\n\r\n 등록시 이메일 주소를 기입하지 않으셨습니다.");
        printf("\r\n 도스박물관 [하늘소] 에게 쪽지나 댓글을 주시면 변경해드립니다.");
        printf("\r\n [Enter] 를 누르세요.");
        press_enter();
        return_login();
    }

	printf(ESC_ENG);
	printf("\r\n\n 2. 등록된 이메일 주소는 다음과 같습니다.");
    printf("\r\n    이메일 주소 뒷 부분이 짤렸다면 짤린부분을 입력해주세요.");
    printf("\r\n    아니라면 [Enter]를 누르세요.");
    char email_address[1024];
    char tmp[512];
	while (1) {
		printf("\r\n >> %s", user["EMAIL"].c_str());
		line_input(tmp, 40);
		if ( !strcasecmp(tmp, "/x") ) return_login();

        sprintf(email_address, "%s%s", user["EMAIL"].c_str(), tmp);

        if ( !is_email_valid(email_address) ) {
            printf("\r\n 잘못 입력 되었습니다.");
		} else {
            break;
		}
	}

    /*
	printf("\r\n 2. 생년월일을 입력하세요. (년-월-일) (ex) 1970-9-12");
	while (1) {
		printf("\r\n >> ");
		line_input(birthday, 20);
		if ( !strcasecmp(birthday, "/x") ) return_login();

		int year = NULL, month = NULL, day = NULL;
		sscanf(birthday, "%d-%d-%d", &year, &month, &day);

		if ( !is_date_valid(day, month, year) ) {
			printf("\r\n 잘못 입력 되었습니다.");
		} else {
			char buf[1024];
			sprintf(buf, "%d-%02d-%02d", year, month, day);

			if ( user["BIRTHDAY"] != buf ) {
				printf("\r\n 생년월일이 다릅니다.");
			} else {
				break;
			}
		}
	}
    */

	printf("\r\n\n 아래의 이메일 주소로 변경된 비밀번호가 발송됩니다.");
	printf("\r\n %s", email_address);
	printf("\r\n 비밀번호를 초기화 하시겠습니까? (y/n) ");
		
	char buf[2];
	line_input(buf, 1);

	if ( !strcasecmp(buf, "y") ) {
        char path[9072];
        sprintf(path, "%s/tmp", getenv("HANULSO"));
        sprintf(path, "%s", tempnam(path, "pass"));

        std::string new_pass = random_string(10);

        FILE *fp = fopen(path, "w");
        fprintf(fp, "'%s'님 안녕하세요.\r\n", user["NICK_NAME"].c_str());
        fprintf(fp, "변경된 도스박물관 BBS의 비밀번호는 %s 입니다.\r\n", new_pass.c_str());
        fprintf(fp, "반드시 접속후 'PE' 커맨드로 비밀번호를 변경해주세요.\r\n");
        fprintf(fp, "감사합니다.\r\n");
        fclose(fp);
        
        char cmd[9072];
        bool success;

        char path2[9072];
        sprintf(path2, "%s", tempnam(path, "pass-utf8"));

        sprintf(cmd, "iconv -f EUC-KR -t UTF8 %s > %s", path, path2);
        exec_command(cmd, &success);

        sprintf(cmd, "%s/bin/mailsend -to \"%s\" -from \"%s\""
            " -sub \"[%s] 비밀번호 변경 알림\""
            " -starttls -port %s -auth -smtp %s -user %s"
            " -pass \"%s\" -M \"`cat %s`\" -log \"/tmp/mailsend.log\"",
            getenv("HANULSO"), email_address, 
            mailserver_user, host_name, 
            mailserver_port, mailserver_host, mailserver_user, mailserver_passwd, path2);

        printf("\r\n\r\n 이메일을 발송중입니다..."); fflush(stdout);

        std::vector<std::string> lines = exec_command(cmd, &success);

        if ( success ) {
            database::set_user_password(user_id, (char*)new_pass.c_str());

            printf("\r\n 이메일 '%s'로 변경된 비밀번호를 발송하였습니다.", user["EMAIL"].c_str());

        } else {
            printf("\r\n 이메일 발송을 실패하였습니다.\r\n");
        }

        sprintf(buf, "rm -f %s", path);
        system(buf);
        sprintf(buf, "rm -f %s", path2);
        system(buf);

        printf("\r\n [Enter] 를 누르세요.");
        press_enter();
	}

	return_login();
}

