#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termio.h>
#include <pthread.h>
#include <signal.h>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include "ansi.h"

#define CHATDATA 1024

int cursor_pos = 0;

char userid[256];
char nickname[256];

int sock_fd;
int max_fd;
fd_set read_fds;

int scroll_starty=2;
int scroll_endy=30;

pthread_t threads[5];

int chatt_close (void);

bool is_han(char c)
{
	if((c & 0x80)==0) {
		return false;
	} else {
		return true;
	}
}

void line_input(char *str, int len)
{
    cursor_pos = 0;
    char ch;

    while((ch=getchar()) != '\r' ) {
        if(ch == '\b') {
            if(cursor_pos > 0) {
#if 0
				if (is_han(str[cursor_pos-1])) {
					putchar(ch); putchar(' '); putchar(ch);
					if(cursor_pos > 0) cursor_pos--;
					putchar(ch); putchar(' '); putchar(ch);
					if(cursor_pos > 0) cursor_pos--;
				} else {
					putchar(ch); putchar(' '); putchar(ch);
					if(cursor_pos > 0) cursor_pos--;
				}
#else
				putchar(ch); putchar(' '); putchar(ch);
				if(cursor_pos > 0) cursor_pos--;
#endif
            }
        }
		else if(ch == 27) {
			char ch2 = getchar();
			char ch3 = getchar();
		}
        else if((ch == 0x1b) | (ch == 0x18) | (ch == 0x0f));
        else if(cursor_pos < len) {
			str[cursor_pos++] = ch;
			putchar(ch);
        }
    }

    str[cursor_pos] = 0;
}

void press_enter(void) 
{
	char buff[1];
	line_input(buff, 1);
}

std::vector<std::string> split_string(std::string str, char delimiter)
{
	std::stringstream test(str);
	std::string segment;
	std::vector<std::string> result;

	while(std::getline(test, segment, delimiter))
	   result.push_back(segment);
	
	return result;
}

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

void print_message(char *msg)
{
	printf("[%d;%dr", scroll_starty, scroll_endy);
	fflush(stdout);
	printf("[%d;1H%s", scroll_endy-1, msg);
	fflush(stdout);
	printf("[%d;%dH", scroll_endy+2, 9+cursor_pos);
	fflush(stdout);
}

int recv_msg(int socket, char *msg)
{
	int size;
	read(socket, &size, sizeof(int));
	
#if 0	
	int n = read(socket, msg, size);
	msg[n] = '\0';
#else

	int len=0;
	int remain=size;
	while (1) {
		int n = read(socket, msg+len, remain);
		len += n;
		remain -= n;
		if ( len >= size ) break;
	}
	msg[size] = '\0';
#endif

	return size;
}

void send_msg(int socket, char *msg)
{
	int size = strlen(msg);
	write(socket, &size, sizeof(int));
	write(socket, msg, size);
}

void *chatt_message(void *arg)
{
	char msg[1024];

	while (1) {
		FD_ZERO(&read_fds);
		FD_SET(0, &read_fds);
		FD_SET(sock_fd, &read_fds);
	
		if(select(max_fd, &read_fds, (fd_set *)0, (fd_set *)0, (struct timeval *)0) <0) {
			printf("select error\n");
			exit(1);
		}

		if (FD_ISSET(sock_fd, &read_fds)) {
			recv_msg(sock_fd, msg);

			// ¼­¹ö·ÎºÎÅÍ /quit ¹®ÀÚ¿­À» ¹ÞÀ¸¸é ¹æÀÌ Á¾·á µÈ °ÍÀÓ.
			if ( !strcasecmp(msg, "/quit") ) {
				sleep(2);

				chatt_close();
			}

			print_message(msg);
		}

		// 0.1sec
		usleep(1000*100);
	}
}

int chatt_close (void)  
{
	send_msg(sock_fd, "/bye");
	
	sleep(1);

	close(sock_fd);
	
	// ½ºÅ©·Ñ ¿µ¿ª ÃÊ±âÈ­
	printf("[%d;%dr", 0, 0);

	exit(0);
}

int main(int argc,char *argv[])
{
	struct sockaddr_in servaddr;

	printf(ESC_CLEAR);

	if (argc < 5) {
		printf("usage:%s ip, port, userid, nickname\n", argv[0]);
		exit(-1);
	}

    signal(SIGQUIT, SIG_IGN);
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
    signal(SIGHUP, (__sighandler_t)chatt_close);
    signal(SIGSEGV, (__sighandler_t)chatt_close);
    signal(SIGBUS, (__sighandler_t)chatt_close);
    
	raw_mode();
	umask(0111);

	printf(ESC_CLEAR);

	sock_fd=socket(AF_INET, SOCK_STREAM,0);
	
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_addr.s_addr=inet_addr(argv[1]);
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(atoi(argv[2]));

	if(connect(sock_fd, (struct sockaddr *)&servaddr, sizeof(servaddr))==-1){
		printf("Can not connect\n");
		return -1;
	}

	sprintf(userid, "%s", argv[3]);
	sprintf(nickname, "%s", argv[4]);

	// Á¢¼Ó »ç¿ëÀÚID º¸³¿
	send_msg(sock_fd, userid);
	// Á¢¼Ó »ç¿ëÀÚ ´Ð³×ÀÓ º¸³¿
	send_msg(sock_fd, nickname);

	max_fd = sock_fd +1;
	
	int i=0;
	pthread_create(&threads[0], NULL, &chatt_message, (void *)i);
	
	while (1) {
		char input[1024];
		printf("[%d;1H±Ó¼Ó¸»(/SAY) Á¢¼ÓÀÚÁ¶È¸(/LIST) ÅðÀå(/BYE) ´ëÈ­¹æÁ¾·á(/QUIT)[K", scroll_endy+1);
		printf("[%d;1H´ëÈ­ >> [K",scroll_endy+2);

		char user[9072];
		sprintf(user,"%s(%s)", nickname, userid);

		line_input(input, 75-strlen(user));
		if (strlen(input) <= 0 ) continue;

		// ¸¶Áö¸· 1¹ÙÀÌÆ®°¡°¡ ÇÑ±ÛÀÌ°í, ¸¶Áö¸· ÀÌÀü ¹ÙÀÌÆ®°¡ ¿µ¹®ÀÌ¸é.. 
		// ¸¶Áö¸· ÇÑ±Û ±úÁü¹æÁö¸¦ À§ÇØ ¸¶Áö¸·À» Áö¿ò
		if (is_han(input[strlen(input)-1]) && !is_han(input[strlen(input)-2])) {
			input[strlen(input)-1] = '\0';
		}

		char msg[9072];
		sprintf(msg,"\r\n\033[7m%s\033[0m %s\r\n", user, input);
		
		std::vector<std::string> args = split_string(input, ' ');
		
		if (!strcasecmp(args[0].c_str(), "/quit")) {
			send_msg(sock_fd, (char*)args[0].c_str());
			continue;
		}

		if (!strcasecmp(args[0].c_str(), "/bye")) {
			send_msg(sock_fd, (char*)args[0].c_str());
			break;
		}

		if (!strcasecmp(args[0].c_str(), "/list")) {
			send_msg(sock_fd, (char*)args[0].c_str());
			continue;
		}

		if (!strcasecmp(args[0].c_str(), "/say")) {
			if ( args.size() >= 3 ) {
				send_msg(sock_fd, input);
			}
			continue;
		}

		send_msg(sock_fd, msg);
	}

	//rc = pthread_join(threads[0], (void **)&status);

	close(sock_fd);
	
	// ½ºÅ©·Ñ ¿µ¿ª ÃÊ±âÈ­
	printf("[%d;%dr", 0, 0);

    exit(0);
}

