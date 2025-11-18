#include "../main.h"

// https://github.com/dlecocq/apathy
#include "path.hpp"

using namespace apathy;

// ------------------------------------
// fake functions for compile
int host_close() {}
char tty[10];
std::vector<std::string> table_names;
// ------------------------------------

int main(int argc, char **argv)
{
	// 테이블 매핑 파일 분석
	std::map<int, std::string> table_mapper;

	std::string txt = read_file("table.txt");
	std::vector<std::string> lines = split_string(txt, '\n');
	for(int i=0; i<lines.size(); i++) 
	{
		std::string line = trim(lines[i]);

		if (line.length() == 0) continue;
		unsigned found = line.find_last_of(";");
		if (found) line = line.substr(0, found);
		if ( line[0] == ';') continue;

		unsigned menu_id;
		char table_name[256];

		sscanf(line.c_str(), "%d %s", &menu_id, table_name);
		table_names.push_back(table_name);

		table_mapper.insert(std::pair<int, std::string>(menu_id, table_name));
	}

	// 복구된 게시물 번호 분석
	std::list<int> restored_nums;

	txt = read_file("restore.log");
	lines = split_string(txt, '\n');
	for(int i=0; i<lines.size(); i++) 
	{
		std::string line = trim(lines[i]);

		if (line.length() == 0) continue;
		unsigned found = line.find_last_of(";");
		if (found) line = line.substr(0, found);
		if ( line[0] == ';') continue;

		unsigned num;

		sscanf(line.c_str(), "%d", &num);
		restored_nums.push_back(num);
	}

	// -----------------------------------------------------------
	read_settings("../../hanulso.cfg");
	database::open();

#if 0
	for(int i=0; i<table_names.size(); i++) 
	{
		char *table_name = (char*)table_names[i].c_str();

		bool ok;
		char query[9072];
		sprintf(query, "DROP TABLE %s;", table_name);
		database::fetch(query, &ok);
	}
	exit(-1);
#endif

	// --------------------------
	std::vector<std::string> files = find_files("data/*/restore/content.xml");
	if ( files.size() == 0 ) {
		printf("파일이 없습니다.");
		exit(-1);
	}

	std::vector<int> nums;
	for(int i=0; i<files.size(); i++) 
	{
		std::vector<std::string> parts = split_string(files[i], '/');
		nums.push_back(atoi(parts[1].c_str()));
	}
	sort(nums.begin(), nums.end());


	std::string datetime = datetime_now_string(true);

	FILE *log = fopen("restore.log", "a");
	fprintf(log, ";%s\n", datetime.c_str());
	fclose(log);

	pugi::xml_document doc;
	for(int i=0; i<nums.size(); i++) 
	{
		int num = nums[i];

		// 이미 복구된 게시물이면 SKIP
		bool found = (std::find(restored_nums.begin(), 
			restored_nums.end(), num) != restored_nums.end());
		if ( found ) continue;

		char path[1024];
		sprintf(path, "data/%d/restore/content.xml", num);

		printf("# %s\n", path);

		doc.reset();
		pugi::xml_parse_result res = doc.load_file(path,
				pugi::parse_default|pugi::parse_declaration);
		if (!res) {
			std::ostringstream msg;
			msg << " Parse error: " << res.description() << ", character pos= " << res.offset;
			printf("%s", msg.str().c_str());
			exit(-1);
		}

		// -----------------------------------------------------------
		pugi::xml_node root = doc.first_element_by_path("/root");

		// 타이틀
		char *title = utf8_to_cp949((char*)(root.child("title").child_value()));
		// 닉네임
		char *nick_name = utf8_to_cp949((char*)(root.child("nick_name").child_value()));
		// 유저 ID
		char *user_id = utf8_to_cp949((char*)(root.child("user_id").child_value()));
		char *date_time = (char*)(root.child("date").child_value());
		int year, month, day;
		int hour, min;
		sscanf(date_time, "%d.%d.%d. %d:%d", &year, &month, &day, &hour, &min);
		// 날짜
		char date[256];
		sprintf(date, "%04d-%02d-%02d", year, month, day);
		// 시간
		char time[256];
		sprintf(time, "%02d:%02d:%02d", hour, min, 1);
		// 메뉴 ID
		int menu_id = atoi((char*)(root.child("menu_id").child_value()));
		// 내용
		char *html = utf8_to_cp949((char*)(root.child("html").child_value()));
		//char *html = (char*)(root.child("html").child_value());

		// 테이블 매핑 파일에 등록된 menu_id 가 아니면 글을 등록하지 않음
		if ( table_mapper.count(menu_id) == 0 ) {
			printf("\t%d --> SKIP\n", menu_id);
			continue;
		}

		std::string table_name = table_mapper[menu_id];
		printf("\t%d --> %s\n", menu_id, table_name.c_str());

		FILE *fp = fopen("/tmp/a.html", "wb");
		fprintf(fp, "%s", html);
		fclose(fp);

		system("lynx -dump -nomargins -width 8000 --display_charset=euc-kr /tmp/a.html > /tmp/b.txt");

		std::string content = read_file("/tmp/b.txt");

		std::ostringstream query;
		
		// ----------------------------------------------------------
		// 사용자가 없으면 등록
		query.str("");
		if ( !database::exist_user_id(user_id) ) {
			int level=1;
			int sex=1;
			const char *email="";
			query << "INSERT INTO member ( ";
			query << "		USER_ID, NICK_NAME, BIRTHDAY, PASSWORD, EMAIL, SEX, LEVEL, ";
			query << "		REGISTRATION_DATETIME, LASTLOGIN_DATETIME ";
			query << ") VALUES (";
			query << "'" << user_id << "', ";
			query << "'" << nick_name << "', ";
			query << "'" << date_now_string(false) << "', ";
			// 단방향 패스워드 알고리즘 사용
			//query << "PASSWORD('" << user_id << "'), ";
			query << "PASSWORD('12345678'), ";
			query << "'" << email << "', ";
			query << "'" << sex << "', ";
			query << "'" << level << "', ";
			query << "'" << datetime_now_string(false) << "', ";
			query << "''";
			query << ")";
			if ( mysql_query(mysql, query.str().c_str()) != 0 ) {
				printf("(%d) [%s] \"%s\"", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
				exit(1);
			}
		}

		// 테이블에 글 등록
		int no = database::add_article((char*)table_name.c_str(), user_id, date, time, 
			(char*)trim(title).c_str(), (char*)content.c_str());
		if (no != -1) {
			database::attach_article((char*)table_name.c_str(), no, no);
		}
	
		// 첨부 파일 등록
		Path a(path);
		Path b = a.parent().parent();
		Path c;

		c = b;
		c.append("attachment");
		if ( c.exists() ) {
			Path d = c.string().c_str();
			std::vector<std::string> attachments = find_files(c.string() + "/*");
			if ( attachments.size() > 0 ) {
				for(int j=0; j<attachments.size(); j++) {
					char *new_path = tempnam("../../file", "file");

					// 파일 복사
					char buf[9072];
					sprintf(buf, "cp \"%s\" \"%s\"", attachments[j].c_str(), new_path);
					int ret = system(buf);

					if ( WEXITSTATUS(ret) != 0 ) {
						exit(-1);
					}

					int size = file_size(new_path);
					std::string tmp_filename = filename_only(new_path);
					std::string filename = filename_only(attachments[j]);
                    char *filename2 = utf8_to_cp949((char*)filename.c_str());
					
					database::add_attachment((char*)table_name.c_str(), no, user_id, 
						date, time, (char*)tmp_filename.c_str(), (char*)filename2);
				}
			}
		}

		// 이미지 파일 첨부로 등록
		c = b;
		c.append("image");
		if ( c.exists() ) {
			Path d = c.string().c_str();
			std::vector<std::string> images = find_files(c.string() + "/*");
			if ( images.size() > 0 ) {
				for(int j=0; j<images.size(); j++) {
					char *new_path = tempnam("../../file", "file");

					// 파일 복사
					char buf[9072];
					sprintf(buf, "cp \"%s\" \"%s\"", images[j].c_str(), new_path);
					int ret = system(buf);

					if ( WEXITSTATUS(ret) != 0 ) {
						exit(-1);
					}

					int size = file_size(new_path);
					std::string tmp_filename = filename_only(new_path);
					std::string filename = filename_only(images[j]);
                    char *filename2 = utf8_to_cp949((char*)filename.c_str());
					
					database::add_attachment((char*)table_name.c_str(), no, user_id, 
						date, time, (char*)tmp_filename.c_str(), (char*)filename2);
				}
			}
		}

		FILE *log = fopen("restore.log", "a");
		fprintf(log, "%d\n", num);
		fclose(log);
	}

	database::close();

	return 0;
}

