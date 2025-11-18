#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <signal.h>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#define MAX_LINE 	1024
#define MAX_CLIENT 	1000

char greeting[1024];
//char greeting[] = "\r\n## \033[7m대화방에 오신걸 환영합니다.\033[0m\r\n\r\n";

int max_user;
int server_fd;
int server_port;

struct client {
	bool author;
	char userid[256];
	char nickname[256];
	char ip[40];
	int socket;
	int port;
};

std::vector<client> clients;

void send_msg(int socket, char *msg);
int recv_msg(int socket, char *msg);
void write_user_count(void);
void write_room_info(void);

std::vector<std::string> split_string(std::string str, char delimiter)
{
	std::stringstream test(str);
	std::string segment;
	std::vector<std::string> result;

	while(std::getline(test, segment, delimiter))
	   result.push_back(segment);
	
	return result;
}

//앞에 있는 개행 문자 제거
std::string ltrim(std::string s) 
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), 
				std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

//뒤에 있는 개행 문자 제거
std::string rtrim(std::string s) 
{
	s.erase(std::find_if(s.rbegin(), s.rend(), 
				std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

//양쪽 끝의 개행 문자 제거
std::string trim(std::string s) 
{
	return ltrim(rtrim(s));
}

client push_client(int socket, char *userid, char* nickname, char* ip, int port)
{
	client c;
	// 첫번째 접속자가 방 개설자이다.
	if (clients.size() == 0)
		c.author = true;
	else
		c.author = false;
	strcpy(c.userid, userid);
	strcpy(c.nickname, nickname);
	strcpy(c.ip, ip);
	c.port = port;
	c.socket = socket;
	clients.push_back(c);
	return c;
}

int pop_client(client c)
{
	std::vector<client> res;

	for(unsigned int i=0; i<clients.size(); i++) {
		client c2 = clients[i];
		if ( c.socket != c2.socket ) {
			res.push_back(c2);
		}
	}

	clients = res;
	close(c.socket);

	return 0;
}

void constr_func(client c2, client c)
{
	char buf1[MAX_LINE];
	
	memset(buf1, 0, sizeof(buf1));
	sprintf(buf1, "\r\n\033[7m%s(%s) 님이 입장 하셨습니다.\033[0m\r\n", c.nickname, c.userid);

	send_msg(c2.socket, buf1);
}

void quit_func(client c)
{
	char buf1[MAX_LINE];

	memset(buf1, 0, sizeof(buf1));
	printf("%s is leaved at %s\r\n", c.userid, c.ip);

	sprintf(buf1, "\r\n\033[7m%s(%s) 님이 퇴장 하셨습니다.\033[0m\r\n", c.nickname, c.userid);

	if ( c.author == true ) {
		unsigned int cnt = 0;
		for(unsigned int i=0; i<clients.size(); i++) {
			client c2 = clients[i];
			if (c.socket != c2.socket) {
				// 방장이 퇴장하면 다음 사용자에게 위임한다.
				clients[i].author = true;
				sprintf(buf1, "%s\033[7m%s(%s) 님이 방장을 위임받았습니다.\033[0m\r\n", 
					buf1, c2.nickname, c2.userid);
				break;
			}
		}
	}

	unsigned int cnt = 0;
	for(unsigned int i=0; i<clients.size(); i++) {
		client c2 = clients[i];
		if (c.socket != c2.socket) {
			send_msg(c2.socket, buf1);
			cnt++;
		}
	}
}

void list_func(client c)
{
	char buf1[MAX_LINE];

	memset(buf1, 0, sizeof(buf1));
	sprintf(buf1, "\r\n\033[7m대화방 접속인원은 %d명 입니다.\033[0m\r\n", (int)clients.size());
	send_msg(c.socket, buf1);

	for(unsigned int i=0; i<clients.size(); i++) {
		client c2 = clients[i];
		if ( c2.author == true ) {
			sprintf(buf1, "\r\n[%s(%s) from %s:%d] : 방장\r\n", c2.nickname, c2.userid, c2.ip, c2.port);
		} else {
			sprintf(buf1, "\r\n[%s(%s) from %s:%d]\r\n", c2.nickname, c2.userid, c2.ip, c2.port);
		}
		send_msg(c.socket, buf1);
	}
}

int say_func(client from, client to, char *msg)
{
	char buf[9072];
	sprintf(buf,"\r\n\033[7m!%s(%s)\033[0m : %s\r\n", from.nickname, from.userid, msg);

	// 귓속말 받을 회원에게 메세지 전달
	send_msg(to.socket, buf);

	// 귓속말 보낸 사람에게도 보냄
	send_msg(from.socket, buf);
	return 0;
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

int server_close (void)  
{
	for (unsigned int i=0; i<clients.size(); i++) {
		client c = clients[i];
		
		send_msg(c.socket, (char*)"/quit");
		usleep(1000*500);
	}

	close(server_fd);
	
	char buf[1024];
	sprintf(buf, "%s/chatt/%d.room", getenv("HANULSO"), server_port);
	unlink(buf);

	sleep(1);

	exit(0);
}

// 대화방 접속 인원수 파일 업데이트
void write_user_count(void)
{
	char buf[1024];
	sprintf(buf, "%s/chatt/%d.room", getenv("HANULSO"), server_port);
	FILE *fp = fopen(buf, "w");
	fprintf(fp, "%d", (int)clients.size());
	fclose(fp);
}

// 대화방 정보 파일 업데이트
void write_room_info()
{
	char buf[1024];
	sprintf(buf, "%s/chatt/%d.room", getenv("HANULSO"), server_port);

	std::string author;
	for (unsigned int i=0; i<clients.size(); i++) {
		client c = clients[i];
		if ( c.author == true ) {
			author = c.userid;
		}
	}

	FILE *fp = fopen(buf, "w");
	// 방장,접속인원수
	fprintf(fp, "%s,%d", author.c_str(), (int)clients.size());
	fclose(fp);
}
	
int main(int argc,char *argv[])
{
	int client_fd;
	struct sockaddr_in server_addr, client_addr;
	socklen_t client_len;

	int max_fd = 0;
	fd_set read_fds;

	if (argc < 4)
	{
		printf("## HANULSO BBS ##\n");
		printf("Chatting server program\n");
		printf("usage: %s port max_user greeting \n",argv[0]);
		exit(-1);
	}
	
	server_port = atoi(argv[1]);
	max_user = atoi(argv[2]);
	sprintf(greeting, "\r\n## \033[7m%s\033[0m\r\n\r\n", argv[3]);
		
    signal(SIGQUIT, (__sighandler_t)server_close);
    signal(SIGINT, (__sighandler_t)server_close);
    signal(SIGTERM, (__sighandler_t)server_close);
    signal(SIGHUP, (__sighandler_t)server_close);
    signal(SIGSEGV, (__sighandler_t)server_close);
    signal(SIGBUS, (__sighandler_t)server_close);
	
	server_fd=socket(AF_INET,SOCK_STREAM,0);
	memset(&server_addr,0,sizeof(server_addr));
	server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(server_port);

	// prevent bind error
	int sockopt = 1;
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof(sockopt));

	if(bind(server_fd,(struct sockaddr *) &server_addr,sizeof(server_addr))==-1){
		printf("Can not Bind\n");
		return -1;
	}

	if(listen(server_fd, MAX_CLIENT)==-1){
		printf("listen Fail\n");
		return -1;
	}
	
	char buf1[MAX_LINE];

	memset(buf1, 0, sizeof(buf1));
	inet_ntop(AF_INET, &server_addr.sin_addr, buf1, sizeof(buf1));
	printf("[server address is %s : %d]\r\n", buf1, ntohs(server_addr.sin_port));

	for (;;)
	{
		FD_ZERO(&read_fds);
		FD_SET(server_fd, &read_fds);
		
		max_fd = server_fd;

		for (unsigned int i=0; i<clients.size(); i++) {
			client c = clients[i];

            //if valid socket descriptor then add to read list
            if (c.socket > 0)
                FD_SET(c.socket, &read_fds);
             
            //highest file descriptor number, need it for the select function
            if(c.socket > max_fd)
                max_fd = c.socket;
		}

		if (select(max_fd+1, &read_fds, NULL, NULL, NULL) < 0)
		{
			printf("Select error\n");
			exit(1);
		}

		// 클라이언트 접속 감지
		if (FD_ISSET(server_fd, &read_fds))
		{
			client_len = sizeof(client_addr);

			if ((client_fd = accept(server_fd,(struct sockaddr *)&client_addr, &client_len)) > 0) 
			{
				char userid[256];
				char nickname[256];
				char ip[256];
				int port;

				memset(userid, 0, sizeof(userid));
				memset(nickname, 0, sizeof(nickname));

				// 접속 사용자ID 받음
				recv_msg(client_fd, userid);
				// 접속 사용자 닉네임 받음
				recv_msg(client_fd, nickname);
				// 접속 사용자 IP 주소
				inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));
				// 접속 사용자 포트 번호
				port = ntohs(client_addr.sin_port);

				// 클라이언트 추가
				client c = push_client(client_fd, userid, nickname, ip, port);
				printf("%s is connected from %s\r\n", c.userid, c.ip);

				// 대화방 접속 인원수 파일 업데이트
				write_room_info();
				
				// 접속자에게 환영 메세지 보내기
				send_msg(client_fd, greeting);

				// 다른 접속자들에게 접속 사실을 알림
				for (unsigned int i=0; i<clients.size(); i++) {
					client c2 = clients[i];
					if (c2.socket != c.socket) {
						constr_func(c2, c);
					}
				}
			}
		}

		for (unsigned int i=0; i<clients.size(); i++) {
			client c = clients[i];

			if (FD_ISSET(c.socket, &read_fds)) {
				char line[MAX_LINE];
				memset(line, 0, sizeof(line));

				//int n = read(c.socket, msg, sizeof(msg));
				int n = recv_msg(c.socket, line);
				if (n > 0) {
					std::vector<std::string> args = split_string(trim(line), ' ');

					// 접속자 로그아웃
					if (!strcasecmp(args[0].c_str(), "/bye")) {
						quit_func(c);
						pop_client(c);

						// 대화방 접속 인원수 파일 업데이트
						write_room_info();

						// 대화방에 아무도 없으면 방 종료
						if ( clients.size() == 0 ) {
							server_close();
						}

						continue;
					}
					
					// 방장으로부터 방폭파 메세지가 왔을때
					if (!strcasecmp(args[0].c_str(), "/quit")) {
						printf("destroy requested from %s\n", c.userid);

						// 방장인지 아닌지 검사
						if ( c.author == true ) {
							// 모든 접속자에게 메세지 전달
							for(unsigned int j=0; j<clients.size(); j++) {
								client c2 = clients[j];
								char msg[MAX_LINE];
								sprintf(msg, "\r\n\033[7m%s(%s) 님으로부터 대화방이 종료 되었습니다.\033[0m\r\n",
										c.nickname, c.userid);
								send_msg(c2.socket, msg);
							}

							server_close();

						} else {
							char msg[MAX_LINE];
							sprintf(msg, "\r\n\033[7m방장만 대화방을 종료 가능합니다.\033[0m\r\n");
							send_msg(c.socket, msg);
						}

						continue;
					}

					// 요청 접속자에게 접속자 목록 전달
					if (!strcasecmp(args[0].c_str(), "/list")) {
						list_func(c);
						continue;
					}

					// 특정 접속자에게 개인 메세지 전달
					if (!strcasecmp(args[0].c_str(), "/say")) {
						/* 
						 /say olddos 안녕 모두~
						*/
						char say[1024];
						memset(say, 0, sizeof(say));

						// '안녕 모두~' 를 찾는다.
						int space_count=0;
						for(unsigned int j=0; j<strlen(line); j++) {
							if ( line[j] == ' ' ) space_count++;
							if ( space_count == 2 ) {
								sprintf(say, "%s", trim(line+j).c_str());
								break;
							}
						}

						for(unsigned int j=0; j<clients.size(); j++) {
							client c2 = clients[j];
							if ( !strcmp(c2.userid, args[1].c_str()) ) {
								say_func(c, c2, say);
							}
						}

						continue;
					}

					// 모든 접속자에게 메세지 전달
					for(unsigned int j=0; j<clients.size(); j++) {
						client c2 = clients[j];
						send_msg(c2.socket, line);
					}
				}
			}
		}

		// 0.1sec
		usleep(1000*100);
	}

	return 0;
}

