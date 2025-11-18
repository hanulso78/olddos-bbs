#include "main.h"

struct termio sys_term;

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
	printf("읽기(번호) 쓰기(W) 상위메뉴(P) 종료(X)\r\n");
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

void print_header(char *title, int total_count, int page_count, int page_no)
{
	printf("\033[=9F\033[=1G%s\033[=15F\033[=1G", repeat("─", 40).c_str());
	printf("\033[1A\033[7m%s\033[0m\r\n", host_name);
	// 타이틀 출력
	int center = (80-strlen(strip_ansi_codes(title)))/2;
	printf("\r\033[%dC%s", center, title);

	// 총 글 갯수 출력
	char buf[1024];
	sprintf(buf, "%4d/%4d (총 %4d건) ", page_no, page_count, total_count);
	printf("\r\033[57C%s\r\n", buf);
	
	printf("\033[=0F\033[=1G%s\033[=15F\033[=1G\r\n", repeat("━", 40).c_str());
}

void show_memos(void)
{
	while (1) {
		printf(ESC_CLEAR);

		char sql[1024];

		sprintf(sql, "SELECT * FROM memo WHERE RECIPIENT_USER_ID='%s'", "h2h1995");
		std::vector<std::map<std::string, std::string> > rows = database::fetch_rows(sql);

		int total_count = rows.size();

		int offset = 0;

		// 현재 페이지
		int page_no = 1;

		printf(ESC_CLEAR);

		// 전체 쪽지수
		int page_count = total_count / show_max_line;
		if ( total_count % show_max_line >= 1 ) {
			page_count += 1;
		}

		print_header("받은 쪽지함", total_count, page_count, page_no);
		
		printf("%5s %s %s %s %s", 
				"번호",
				centered("작성자", 12).c_str(), 
				centered("작성일", 6).c_str(), 
				centered("읽음", 6).c_str(), 
				"제목");
		printf("\r\n%s\r\n", repeat("─", 40).c_str());




		char cmd[1024];
		prompt(cmd);

		/* 입력이 숫자 */
		if( is_number(cmd) ) {
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

	// 쪽지 테이블 생성
	database::create_memo();

	show_memos();

	host_close();
}

