#include "main.h"

struct termio sys_term;

char title[1024] = "날씨 정보";

char tty[10];

char host_name[256];

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

bool get_list(std::string url, std::vector<std::string> &list)
{
    char tmpdir[1024];
    char tmpname[1024];
	sprintf(tmpdir, "%s/tmp", getenv("HANULSO"));
	sprintf(tmpname, "%s", tempnam(tmpdir, "weather"));

    // -----------------------------------------------------------
	char cmd[1024];
	sprintf(cmd, "wget -O %s.json -o %s.log %s", tmpname, tmpname, url.c_str());
	int a = system(cmd);
	if ( WEXITSTATUS(a) != 0 ) {
		printf("\r\n다운로드를 실패하였습니다.\r\n");
		printf("\r\n[Enter] 를 누르세요.");
		press_enter();
		return false;
	}

    // ----------------------------------------------------
    sprintf(cmd, "%s/bin/jq-linux64 -r 'map({code,value}) | "
            "(first | keys_unsorted) as $keys | "
            "map([to_entries[] | .value]) as $rows | "
            "$keys,$rows[] | @csv' %s.json > %s.csv", getenv("HANULSO"), tmpname, tmpname);
	a = system(cmd);
	if ( WEXITSTATUS(a) != 0 ) {
		printf("\r\njson2csv 변환을 실패하였습니다.\r\n");
		printf("\r\n[Enter] 를 누르세요.");
		press_enter();
		return false;
	}

    sprintf(cmd, "rm -f %s.json %s.log", tmpname, tmpname);
    system(cmd);

    // ----------------------------------------------------
	sprintf(cmd, "iconv -c -f UTF8 -t CP949//IGNORE//TRANSLIT "
			"'%s.csv' > ''%s.cp949.csv''", 
			tmpname, tmpname);

	a = system(cmd);
	if ( WEXITSTATUS(a) != 0 ) {
		printf("\r\niconv 실행을 실패하였습니다.\r\n");
		printf("\r\n[Enter] 를 누르세요.");
		press_enter();
		return false;
	}

    sprintf(cmd, "rm -f %s.csv", tmpname);
    system(cmd);

    // ----------------------------------------------------
    std::string path = std::string(tmpname)+".cp949.csv";
    std::string csv = read_file(path.c_str());
    
    sprintf(cmd, "rm -f %s.cp949.csv", tmpname);
    system(cmd);

	std::vector<std::string> lines = split_string(csv, '\n');
	for(int i=1; i<lines.size(); i++) {
        std::vector<std::string> cols = split_string(lines[i], ',');
        std::string code = cols[0].substr(1, cols[0].size()-2);
        std::string value = cols[1].substr(1, cols[1].size()-2);
        list.push_back(code);
        list.push_back(value);
    }

    return true;
}

std::string select_menu(std::string url)
{
    std::string code;

	while (1) {
		printf(ESC_CLEAR);

		// ----------------- 헤더 출력 -------------------
        printf("\033[1;1H");
		printf("\033[=9F\033[=1G%s\033[=15F\033[=1G", repeat("─", 40).c_str());
        printf("\033[1;1H");
		printf("\033[1A\033[7m%s\033[0m", host_name);
		// 타이틀 출력
		int center = (80-strlen(strip_ansi_codes(title)))/2;
        printf("\033[2;1H");
		printf("\r\033[%dC%s", center, title);
        printf("\033[3;1H");
		printf("\033[=0F\033[=1G%s\033[=15F\033[=1G", repeat("━", 40).c_str());

        printf("\033[4;1H");

        std::vector<std::string> list;
        get_list(url, list);
		
        int num=1;
        int row=0;

        int col=2;
        int width=30;
        if(list.size() <= 30) {
            col=1;
            width=30;
        } else if(list.size() > 80) {
            col=3;
            width=20;
        }

        for(unsigned int i=0; i<list.size(); i+=2) {
			printf("%5d%s", num, centered(list[i+1], width).c_str());
            if( num % col == 0 ) {
                if(list.size()-1 >= num) {
                    printf("\r\n");
                    row++;
                }
            }
            num++;
		}

        if((list.size()/2) % col != 0) {
            row+=1;
        }

        printf("\033[%d;1H", row+4);
		printf("%s\r\n", repeat("━", 40).c_str());

		char cmd[1024];
		prompt(cmd);

		/* 입력이 숫자 */
		if( is_number(cmd) ) {
            if(atoi(cmd) > list.size()/2) {
                // pass
            } else {
                int i = (atoi(cmd)-1) * 2;
                code = list[i];
                break;
            }
		} else {
            if (!strcasecmp(cmd, "p")) { 
                code = "p";
                break; 
            }
        }
	}

    return code;
}

void show_info(std::string code) 
{
    std::string url = std::string("http://www.kma.go.kr/wid/queryDFSRSS.jsp?zone=") + code;

    printf(ESC_CLEAR);

    // ----------------- 헤더 출력 -------------------
    printf("\033[1;1H");
    printf("\033[=9F\033[=1G%s\033[=15F\033[=1G", repeat("─", 40).c_str());
    printf("\033[1;1H");
    printf("\033[1A\033[7m%s\033[0m", host_name);
    // 타이틀 출력
    int center = (80-strlen(strip_ansi_codes(title)))/2;
    printf("\033[2;1H");
    printf("\r\033[%dC%s", center, title);
    printf("\033[3;1H");
    printf("\033[=0F\033[=1G%s\033[=15F\033[=1G", repeat("━", 40).c_str());

    // ---------------------
    char tmpdir[1024];
    char tmpname[1024];
	sprintf(tmpdir, "%s/tmp", getenv("HANULSO"));
	sprintf(tmpname, "%s", tempnam(tmpdir, "weather"));

    // -----------------------------------------------------------
	char cmd[1024];
	sprintf(cmd, "wget -O %s.xml -o %s.log %s", tmpname, tmpname, url.c_str());
	int a = system(cmd);
	if ( WEXITSTATUS(a) != 0 ) {
		printf("\r\n다운로드를 실패하였습니다.\r\n");
		printf("\r\n[Enter] 를 누르세요.");
		press_enter();
		return;
	}

    sprintf(cmd, "rm -f %s.json %s.log", tmpname, tmpname);
    system(cmd);

    // ----------------------------------------------------
	sprintf(cmd, "iconv -c -f UTF8 -t CP949//IGNORE//TRANSLIT "
			"'%s.xml' > ''%s.cp949.xml''", 
			tmpname, tmpname);

	a = system(cmd);
	if ( WEXITSTATUS(a) != 0 ) {
		printf("\r\niconv 실행을 실패하였습니다.\r\n");
		printf("\r\n[Enter] 를 누르세요.");
		press_enter();
		return;
	}

    sprintf(cmd, "rm -f %s.xml", tmpname);
    system(cmd);

    // ----------------------------------------------------
    std::string path = std::string(tmpname)+".cp949.xml";
    //std::string xml = read_file(path.c_str());
    
	pugi::xml_document doc;
	pugi::xml_parse_result res = doc.load_file(path.c_str(), pugi::parse_default|pugi::parse_declaration);
    if (!res) {
		std::ostringstream msg;
		msg << " Parse error: " << res.description() << ", character pos= " << res.offset;
		printf("\r\n\r\n%s", msg.str().c_str());
		printf("\r\n [Enter] 를 누르세요.");
		press_enter();
	}

    sprintf(cmd, "rm -f %s.cp949.xml", tmpname);
    system(cmd);

	// -----------------------------------------------------------
    printf("\033[4;1H");
    printf("%5s %8s %13s %15s %10s %10s %10s\r\n", 
            "시간", "날짜", "온도(고/저)", "날씨", "강수확률", "풍향", "풍속(m/s)");
    printf("\033[5;1H");
    printf("%s", repeat("─", 40).c_str());

	pugi::xml_node node = doc.first_element_by_path("/rss/channel/item/description/body");
	pugi::xml_node child = node.first_child();

    printf("\033[6;1H");
	while (1) {
		if ( child.type() == pugi::node_element ) {
            std::string hour = (char*)(child.child("hour").child_value());
            std::string day = (char*)(child.child("day").child_value());
            std::string temp = (char*)(child.child("temp").child_value());
            std::string tmx = (char*)(child.child("tmx").child_value());
            std::string tmn = (char*)(child.child("tmn").child_value());
            std::string pop = (char*)(child.child("pop").child_value());
            std::string wfKor = (char*)(child.child("wfKor").child_value());
            std::string ws = (char*)(child.child("ws").child_value());
            std::string wdKor = (char*)(child.child("wdKor").child_value());

            hour = hour + "시";

            if(day == "0") day = "오늘";
            else if(day == "1") day = "내일";
            else if(day == "2") day = "모래";

            pop = pop + "%";

            char temp2[1024];
            if(atoi(tmx.c_str()) == -999) tmx = "0";
            if(atoi(tmn.c_str()) == -999) tmn = "0";
            sprintf(temp2, "%2d (%2d/%2d)", 
                    atoi(temp.c_str()),
                    atoi(tmx.c_str()),
                    atoi(tmn.c_str()));

            printf("%5s %8s %13s %15s %10s %10s %10d\r\n", 
                    hour.c_str(),
                    day.c_str(),
                    temp2,
                    wfKor.c_str(),
                    pop.c_str(),
                    wdKor.c_str(),
                    atoi(ws.c_str())
                    );
		}

		child = child.next_sibling();
		if ( child.empty() ) break;
	}

    printf("%s", repeat("─", 40).c_str());
    printf("\r\n [Enter] 를 누르세요.");
    press_enter();
}

int main(int argc, char **argv)
{
    sprintf(host_name, "%s", argv[1]);

    signal(SIGQUIT, SIG_IGN);
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
    signal(SIGHUP, (__sighandler_t)host_close);
    signal(SIGSEGV, (__sighandler_t)host_close);
    signal(SIGBUS, (__sighandler_t)host_close);

    ioctl(0,TCGETA, &sys_term);
	raw_mode();
	
    umask(0111);

    std::string url = "http://www.kma.go.kr/DFSROOT/POINT/DATA/top.json.txt";
    while (1) {
        std::string code = select_menu(url);
        if(code == "p") {
            break;
        } else {
            std::string url2 = "http://www.kma.go.kr/DFSROOT/POINT/DATA/mdl." + code + ".json.txt";
            while (1) {
                std::string code2 = select_menu(url2);

                if(code2 == "p") {
                    break;
                } else  {
                    std::string url3 = "http://www.kma.go.kr/DFSROOT/POINT/DATA/leaf." + code2 + ".json.txt";

                    while(1) {
                        std::string code3 = select_menu(url3);

                        if(code3 == "p") {
                            break;
                        } else  {
                            show_info(code3);
                        }
                    }
                }
            }
        }
    }

	host_close();
}

