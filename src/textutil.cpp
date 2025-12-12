#include "main.h"

// BB코드를 교체하는 printf 함수
void bbcode_printf(const char *fmt,...)
{
	char buff[9072];

	va_list ap;
	va_start(ap, fmt);
	vsprintf(buff,fmt, ap);
	va_end(ap);

	printf("%s", replace_bbcode(buff).c_str());
}

std::string replace_bbcode(std::string text)
{
	char buf[9072];
	bool ok;
	
	// 호스트 이름
	if ( text.find("[hostname]", 0) != std::string::npos ) {
		text = replace_all(text, "[hostname]", host_name);
	}

	// 전체 회원 수
	// number of members
	if ( text.find("[nummembers]", 0) != std::string::npos ) {
		sprintf(buf, "SELECT COUNT(*) FROM member;");
		std::string member_count = database::fetch(buf, &ok);
		text = replace_all(text, "[nummembers]", member_count);
	}
	
	// 전체 접속자 수
	// Number of connections
	if ( text.find("[numconns]", 0) != std::string::npos ) {
		sprintf(buf, "%s/tmp/*.tty", getenv("HANULSO"));
		std::vector<std::string> files = find_files(buf);
		int count = 0;
		for(int i=0; i<files.size(); i++) {
			std::string user_id = trim(read_file(files[i].c_str()));
			if (user_id.length() > 0) {
				bool exist;
				std::map<std::string, std::string> user = database::user_info((char*)user_id.c_str(), &exist);
				if ( exist ) {
					count += 1;
				}
			}
		}

		std::ostringstream tmp;
		//tmp << files.size(); 
		tmp << count;
		text = replace_all(text, "[numconns]", tmp.str());
	}

	// 전체 게시글 수
	if ( text.find("[numarticles]", 0) != std::string::npos ) {
		// 전체 테이블에서 글 갯수를 읽어서 더함
		int total_count = 0;
		for(unsigned int i=0; i<table_names.size(); i++) {
			sprintf(buf, "SELECT COUNT(*) FROM %s;", table_names[i].c_str());
			std::string count = database::fetch(buf, &ok);
			if ( ok ) {
				total_count += atoi(count.c_str());
			}
		}
		
		std::ostringstream tmp;
		tmp << total_count;
		text = replace_all(text, "[numarticles]", tmp.str());
	}

	// 오늘의 전체 게시글 수
	if ( text.find("[todaynumarticles]", 0) != std::string::npos ) {
		// 전체 테이블에서 오늘 날짜와 동일한 글 갯수를 읽어서 더함
		int total_count = 0;
		for(unsigned int i=0; i<table_names.size(); i++) {
			sprintf(buf, "SELECT COUNT(*) FROM %s;", table_names[i].c_str());
			std::string count = database::fetch(buf, &ok);
			if ( ok ) {
				total_count += atoi(count.c_str());
			}
		}
		
		std::ostringstream tmp;
		tmp << total_count;
		text = replace_all(text, "[todaynumarticles]", tmp.str());
	}

/*
	char *tmp = "[numarticles:bbb:10 ]";
	char a[10];
	sscanf(tmp, "[ numarticles:%s ]", a);
	printf("%s %s\n", a);

	press_enter();
	*/
	
#if 0

	static const char* COLORNAMES[] = { 
		"black",     "darkgrey",
		"blue",      "lightblue",
		"green",     "lightgreen",
		"cyan",      "lightcyan",
		"red",       "lightred",
		"purple",    "magenta",
		"brown",     "yellow",
		"grey",      "white",

		"bblack",    "bdarkgrey",
		"bblue",     "blightblue",
		"bgreen",    "blightgreen",
		"bcyan",     "blightcyan",
		"bred",      "blightred",
		"bpurple",   "bmagenta",
		"bbrown",    "byellow",
		"bgrey",     "bwhite"
	};

	static const char* COLORCODES[] = { 
		"[=0F", "[=8F",
		"[=1F", "[=9F",
		"[=2F", "[=10F",
		"[=3F", "[=11F",
		"[=4F", "[=12F",
		"[=5F", "[=13F",
		"[=6F", "[=14F",
		"[=7F", "[=15F",

		"[=0G", "[=8G",
		"[=1G", "[=9G",
		"[=2G", "[=10G",
		"[=3G", "[=11G",
		"[=4G", "[=12G",
		"[=5G", "[=13G",
		"[=6G", "[=14G",
		"[=7G", "[=15G"
	};
	
	// 색 변경
	for(unsigned int i=0; i<32; i++) {
		char bbcode[1024];
		sprintf(bbcode, "[%s]", COLORNAMES[i]);

		if ( text.find(bbcode, 0) != std::string::npos ) {
			text = replace_all(text, bbcode, COLORCODES[i]);
		}
	}
#endif
	
	return text;
}

void print_file(const char *filename)
{
    char buf[9072];

    sprintf(buf,"%s/%s", getenv("HANULSO"), filename);
	std::string text = read_file(buf);
	text = replace_bbcode(text);

	std::vector<std::string> lines = split_string(text, '\n');

	for(int i=0; i<lines.size(); i++) {
        std::string line = lines[i];
        std::size_t pos = line.find("#PAGE");

        if (pos != std::string::npos) {
            std::string before = line.substr(0, pos);
            const std::size_t keyLen = std::string("#PAGE").length();
            printf("%s", before.c_str());
            press_enter();
            printf("\r\n");
        } else {
            printf("%s\r\n", line.c_str());
        }
	}
}
