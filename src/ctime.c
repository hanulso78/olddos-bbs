/* 키 입력 시간 검사 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <termio.h>

int isatty(int fd)
{
    struct termios term;
    return(ioctl(fd, TCGETA, &term) != -1);
}

int main(int argc, char **argv)
{

	char buf[1024], *tmp, *getty;
	FILE *fp;

	umask(0111);

	int ps;
	for(ps = 0; ps <= 10; ps++) {
		if(isatty(ps)) {
			getty = ttyname(ps);
			break;
		}
	}

	tmp = &getty[9];

	sprintf(buf, "tmp/%s.tty", tmp);
	fp = fopen(buf, "w");
	fclose(fp);

	ps = fork();
	if (ps) {
		execl("bin/main", "main", tmp, (char*)0);
	}
	else {
		while(1) {
			time_t current;
			struct stat statbuf;

			sleep(10);

			fstat(0, &statbuf);
			time(&current);

			// 60*10(10분) 동안 키 입력이 없으면 종료
			if(current - (statbuf.st_atime) >= 600) {
				ps = getppid();
				sleep(1);
				printf("\r\n\007키 입력이 없어서 자동으로 끊습니다.\r\n");
				kill(ps, SIGHUP);
				sleep(1);
				exit(0);
			}
		}
	}

	return 0;
}

