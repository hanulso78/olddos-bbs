#include "main.h"

MYSQL *mysql;

namespace database {
	bool open()
	{
		mysql= mysql_init(NULL);
		if (!mysql_real_connect(mysql, db_host, db_user, db_passwd, db_name, 0, NULL, 0)) {
			printf("\r\n(%d) [%s] \"%s\"\r\n", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
			press_enter();
			return false;
		}
		//mysql_query(mysql, "set names latin1;");

		return true;
	}

	bool close()
	{
		mysql_close(mysql);
		return true;
	}

	// 첨부 파일 테이블 생성
	bool create_attachment(void)
	{
		char query[9072];

		sprintf(query, "CREATE TABLE IF NOT EXISTS attachment ( "
				"NO INTEGER NOT NULL AUTO_INCREMENT PRIMARY KEY, "
				"FAMILY_TABLE TEXT NOT NULL, "
				"FAMILY_ID INT NOT NULL, " 
				"USER_ID TEXT NOT NULL, "
				"DATE_TIME DATETIME NOT NULL, "
				"DOWNLOAD INT UNSIGNED NOT NULL DEFAULT 0, "
				"FILENAME TEXT NOT NULL, "
				"ORIGINAL_FILENAME TEXT NOT NULL"
			");");
		if ( mysql_query(mysql, query) != 0 ) {
			printf("\r\n(%d) [%s] \"%s\"\r\n", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
			press_enter();
			return false;
		}

		return true;
	}

	bool create_board(char *table_name) 
	{
		char query[9072];

		// CONTENT 컬럼의 경우 대용량 텍스트를 위해 MEDIUMTEXT로 설정 
		// TEXT: 65535 bytes
		// MEDIUMTEXT: 16777215 bytes
		// LONGTEXT: 4294967295 bytes

		sprintf(query, "CREATE TABLE IF NOT EXISTS %s ( "
				"NO INTEGER NOT NULL AUTO_INCREMENT PRIMARY KEY, "
				/* 부모글과 부모글로부터 파생된 모든 자식글은 같은 번호를 갖는다. */
				"FAMILY INT, " 
				/* 같은 그룹내 게시물의 순서 */
				"ORDERBY INT, " 
				/* 같은 그룹내 계층 */
				"STEP INT, " 
				"USER_ID TEXT NOT NULL, "
				"DATE_TIME DATETIME NOT NULL, "
				"HIT INT UNSIGNED NOT NULL DEFAULT 0, "
				"RECOMMEND INT UNSIGNED NOT NULL DEFAULT 0, "
				"TITLE TEXT NOT NULL, "
				"CONTENT MEDIUMTEXT NOT NULL"
			");", table_name);
		if ( mysql_query(mysql, query) != 0 ) {
			printf("\r\n(%d) [%s] \"%s\"\r\n", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
			press_enter();
			return false;
		}

		return true;
	}

	int add_article(char *table_name, char *user_id, 
			char *date, char *time, char *title, char *content)
	{
		std::ostringstream query;

		char *title2 = (char*)malloc(strlen(title)*2+1);
		mysql_real_escape_string(mysql, title2, title, strlen(title));
		
		char *content2 = (char*)malloc(strlen(content)*2+1);
		mysql_real_escape_string(mysql, content2, content, strlen(content));

		char date_time[1024];
		sprintf(date_time, "%s %s", date, time);

		query.str("");
		query << "INSERT INTO ";
		query << table_name << " (USER_ID, DATE_TIME, TITLE, CONTENT) VALUES ( ";
		query << "'" << user_id << "', ";
		query << "'" << date_time << "', ";
		query << "'" << title2 << "', ";
		query << "'" << content2 << "'";
		query << ");";

		free(title2);
		free(content2);

		if ( mysql_query(mysql, query.str().c_str()) != 0 ) {
			printf("\r\n(%d) [%s] \"%s\"\r\n", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
			press_enter();
			return -1;
		}

		// 새로 등록된 글 번호
		int no = database::article_max_number(table_name);
		return no;
	}

	/*
		참고: 
			http://pram.tistory.com/entry/%EA%B3%84%EC%B8%B5%ED%98%95%EA%B2%8C%EC%8B%9C%ED%8C%90-%EB%8B%B5%EB%B3%80%ED%98%95

		글 작성후 답글로 등록하거나 독립된 글로 셋팅

		family: 글 등록시 부모글과 동일한 번호를 갖는다.
		orderby: 같은 그룹내 게시물의 순서로 0이다.
		family: 같은 그룹내 계층으로 0이다.
	*/
	bool attach_article(char *table_name, int no, int parent_no)
	{
		std::ostringstream query;

		if ( no != parent_no ) {
			// -------------------------------------------------------------
			// 부모글의 값을 가져옴
			query << "SELECT * FROM " << table_name << " WHERE NO=" << parent_no; 
			std::vector<std::map<std::string, std::string> > rows = database::fetch_rows((char*)query.str().c_str());

			std::map<std::string, std::string> row = rows.at(0);

			int family2 = atoi(row[std::string("FAMILY")].c_str());
			int orderby2 = atoi(row[std::string("ORDERBY")].c_str());
			int step2 = atoi(row[std::string("STEP")].c_str());
			
			query.str("");
			query << "UPDATE " << table_name << 
				" SET ORDERBY=ORDERBY+1 WHERE FAMILY=" << family2 << " AND ORDERBY>" << orderby2;
			bool ok;
			database::fetch ((char*)query.str().c_str(), &ok);

			// select (no, family, orderby, step) from knowhow_sound;
			int family = atoi(row[std::string("FAMILY")].c_str());
			int orderby = atoi(row[std::string("ORDERBY")].c_str()) + 1;
			int step = atoi(row[std::string("STEP")].c_str()) + 1;

			// -------------------------------------------------------------
			query.str("");
			query << "UPDATE " << table_name << " SET FAMILY=" << family;
			query << ", ORDERBY=" << orderby << ", STEP=" << step;
			query << " WHERE NO=" << no;

		} else {
			int family = no;
			int orderby = 0;
			int step = 0;

			query.str("");
			query << "UPDATE " << table_name << " SET FAMILY=" << family;
			query << ", ORDERBY=" << orderby << ", STEP=" << step;
			query << " WHERE NO=" << no;
		}

		if ( mysql_query(mysql, query.str().c_str()) != 0 ) {
			printf("\r\n(%d) [%s] \"%s\"\r\n", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
			press_enter();
			return false;
		}

		return true;
	}

	bool delete_article(char *table_name, int no) 
	{
		std::ostringstream query;
		query << "DELETE FROM " << table_name << " WHERE NO=" << no << ";";

		if ( mysql_query(mysql, query.str().c_str()) != 0 ) {
			printf("\r\n(%d) [%s] \"%s\"\r\n", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
			press_enter();
			return false;
		}

		return true;
	}

	int article_max_number(char *table_name)
	{
		std::ostringstream query;
		query << "SELECT MAX(NO) FROM " << table_name << ";";

		bool ok;
		int max_no = atoi(fetch((char *)(query.str().c_str()), &ok).c_str());

		return max_no;
	}

	// 게시글에 첨부 파일 추가
	bool add_attachment(char *family_table_name, int family_no, char *user_id, 
			char *date, char *time, char *tmp_filename, char *filename) 
	{
		std::ostringstream query;

		char *filename2 = (char*)malloc(strlen(filename)*2+1);
		mysql_real_escape_string(mysql, filename2, filename, strlen(filename));

		char date_time[1024];
		sprintf(date_time, "%s %s", date, time);

		query.str("");
		query << "INSERT INTO attachment";
		query << " (FAMILY_TABLE, FAMILY_ID, USER_ID, DATE_TIME, FILENAME, ORIGINAL_FILENAME) VALUES ( ";
		query << "'" << family_table_name << "', ";
		query << "'" << family_no << "', ";
		query << "'" << user_id << "', ";
		query << "'" << date_time << "', ";
		query << "'" << tmp_filename << "', ";
		query << "'" << filename2 << "'";
		query << ");";

		free(filename2);

		if ( mysql_query(mysql, query.str().c_str()) != 0 ) {
			printf("\r\n(%d) [%s] \"%s\"\r\n", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
			press_enter();
			return false;
		}

		return true;
	}

	// 게시글의 첨부 파일 목록 업데이트
	bool delete_attachment(char *table_name, int no, int attach_no)
	{
		char buf[9072];

		// 전체 삭제
		if ( attach_no == -1 ) {
			sprintf(buf, "DELETE FROM attachment WHERE FAMILY_TABLE='%s' AND FAMILY_ID=%d;", table_name, no);

		// 일부만 삭제
		} else {
			char tmp[9072];

			sprintf(tmp, "SELECT * FROM attachment WHERE FAMILY_TABLE='%s' AND FAMILY_ID='%d';", table_name, no);
			std::vector<std::map<std::string, std::string> > attach_rows = fetch_rows(tmp);

			std::map<std::string, std::string> attachment = attach_rows.at(attach_no);
			int id = atoi(attachment["NO"].c_str());
			
			sprintf(buf, "DELETE FROM attachment WHERE NO='%d';", id);
		}

		if ( mysql_query(mysql, buf) != 0 ) {
			printf("\r\n(%d) [%s] \"%s\"\r\n", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
			press_enter();
			return false;
		}

		return true;
	}

	// 게시글의 본문 변경
	bool edit_article_content(char *table_name, int no, std::string content)
	{
		char *content2 = (char*)malloc(strlen(content.c_str())*2+1);
		mysql_real_escape_string(mysql, content2, content.c_str(), strlen(content.c_str()));

		std::ostringstream query;
		query << "UPDATE " << table_name << " SET CONTENT = '" 
			<< content2 << "' WHERE NO=" << no;

		free(content2);

		if ( mysql_query(mysql, query.str().c_str()) != 0 ) {
			printf("\r\n(%d) [%s] \"%s\"\r\n", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
			press_enter();
			return false;
		}

		return true;
	}

	// 게시글의 제목 변경
	bool edit_article_title(char *table_name, int no, std::string title)
	{
		char *title2 = (char*)malloc(strlen(title.c_str())*2+1);
		mysql_real_escape_string(mysql, title2, title.c_str(), strlen(title.c_str()));

		std::ostringstream query;
		query << "UPDATE " << table_name << " SET TITLE = '" 
			<< title2 << "' WHERE NO=" << no;

		free(title2);

		if ( mysql_query(mysql, query.str().c_str()) != 0 ) {
			printf("\r\n(%d) [%s] \"%s\"\r\n", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
			press_enter();
			return false;
		}

		return true;
	}

	// 다운로드수 증가 시킴
	bool increase_download(int no)
	{
		char query[1024];
		sprintf(query, "UPDATE attachment SET DOWNLOAD = DOWNLOAD + 1 WHERE NO=%d", no);

		if ( mysql_query(mysql, query) != 0 ) {
			printf("\r\n(%d) [%s] \"%s\"\r\n", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
			press_enter();
			return false;
		}

		return true;
	}

	// 게시글의 조회수 증가 시킴
	bool increase_hit(char *table_name, int no)
	{
		char query[1024];
		sprintf(query, "UPDATE %s SET HIT = HIT + 1 WHERE NO=%d", table_name, no);

		if ( mysql_query(mysql, query) != 0 ) {
			printf("\r\n(%d) [%s] \"%s\"\r\n", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
			press_enter();
			return false;
		}

		return true;
	}

	// 게시글의 추천수 증가 시킴
	bool increase_recommend(char *table_name, int no)
	{
		char query[1024];
		sprintf(query, "UPDATE %s SET RECOMMEND = RECOMMEND + 1 WHERE NO=%d", table_name, no);

		if ( mysql_query(mysql, query) != 0 ) {
			printf("\r\n(%d) [%s] \"%s\"\r\n", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
			press_enter();
			return false;
		}

		return true;
	}

	bool check_exist(char *table_name, int no)
	{
		char buf[1024];

		sprintf(buf, "SELECT * FROM %s WHERE NO=%d", table_name, no);
		std::vector<std::map<std::string, std::string> > rows = fetch_rows(buf);
		if ( rows.size() != 1 ) {
			return false;
		}

		return true;
	}

	bool check_same_author(char *table_name, int no, char *user_id)
	{
		char buf[1024];
		sprintf(buf, "SELECT * FROM %s WHERE NO=%d", table_name, no);

		std::vector<std::map<std::string, std::string> > rows = fetch_rows(buf);
		std::map<std::string, std::string> row = rows.at(0);

		std::string author = row[std::string("USER_ID")];
		if ( !strcmp(author.c_str(), user_id) ) {
			return true;
		}

		return false;
	}

	int article_count(char *table_name)
	{
		char query[9072];

		sprintf(query, "SELECT COUNT(*) FROM %s ;", table_name);
		if ( mysql_query(mysql, query) != 0 ) {
			printf("\r\n(%d) [%s] \"%s\"\r\n", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
			press_enter();
			return false;
		}

		MYSQL_RES *res;
		MYSQL_ROW row;

		int count = 0;
		res = mysql_store_result(mysql);
		if( (row = mysql_fetch_row(res)) != NULL ) {
			count = atoi(row[0]);
		}

		mysql_free_result(res);
		return count;
	}

	int article_count_by_title_content(char *table_name, char *text)
	{
		char query[9072];

		sprintf(query, "SELECT COUNT(*) FROM %s "
				"WHERE TITLE LIKE '%%%s%%' "
				"OR CONTENT LIKE '%%%s%%';",
				table_name, text, text);
		if ( mysql_query(mysql, query) != 0 ) {
			printf("\r\n(%d) [%s] \"%s\"\r\n", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
			press_enter();
			return false;
		}

		MYSQL_RES *res;
		MYSQL_ROW row;

		int count = 0;
		res = mysql_store_result(mysql);
		if( (row = mysql_fetch_row(res)) != NULL ) {
			count = atoi(row[0]);
		}

		mysql_free_result(res);
		return count;
	}

	int article_count_by_user_id(char *table_name, char *user_id)
	{
		char query[9072];

		sprintf(query, "SELECT COUNT(*) FROM %s "
				"WHERE USER_ID LIKE '%%%s%%';", table_name, user_id);
		if ( mysql_query(mysql, query) != 0 ) {
			printf("\r\n(%d) [%s] \"%s\"\r\n", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
			press_enter();
			return false;
		}

		MYSQL_RES *res;
		MYSQL_ROW row;

		int count = 0;
		res = mysql_store_result(mysql);
		if( (row = mysql_fetch_row(res)) != NULL ) {
			count = atoi(row[0]);
		}

		mysql_free_result(res);
		return count;
	}

	int attachment_count(char *table_name, int no)
	{
		char query[9072];

		sprintf(query, "SELECT * FROM attachment WHERE FAMILY_TABLE='%s' AND FAMILY_ID='%d';", table_name, no);
		std::vector<std::map<std::string, std::string> > attach_rows = database::fetch_rows(query);

		return attach_rows.size();
	}

	std::string fetch(char *query, bool *ok)
	{
		if ( mysql_query(mysql, query) != 0 ) {
			//printf("\r\n(%d) [%s] \"%s\"\r\n", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
			//press_enter();
			//return false;
			*ok = false;
			return "";
		}

		MYSQL_RES *sql_res;
		MYSQL_ROW sql_row;

		std::string res = "";
		sql_res = mysql_store_result(mysql);

		if ( sql_res != NULL ) {
			if ( mysql_num_rows(sql_res) > 0 ) {
				if( (sql_row = mysql_fetch_row(sql_res)) != NULL ) {
					if ( sql_row[0] == NULL ) {
						res = "";
					} else {
						res = sql_row[0];
					}
				}
			}
		}

		mysql_free_result(sql_res);

		*ok = true;
		return res;
	}

	std::vector<std::map<std::string, std::string> > fetch_rows(char *query)
	{
		std::vector<std::map<std::string, std::string> > rows;
		std::map<std::string, std::string> row;
		
		if ( mysql_query(mysql, query) != 0 ) {
			printf("\r\n(%d) [%s] \"%s\"\r\n", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
			press_enter();
			return rows;
		}

		MYSQL_RES *sql_res;
		MYSQL_ROW sql_row;
		MYSQL_FIELD *sql_field;

		sql_res = mysql_store_result(mysql);
		int num_fields = mysql_num_fields(sql_res);

		// 컬럼 이름 얻어옴
		char *headers[num_fields];
		for(unsigned int i = 0; (sql_field = mysql_fetch_field(sql_res)); i++) {
			headers[i] = sql_field->name;
		}

		while ( (sql_row = mysql_fetch_row(sql_res)) ) {
			row.clear();

			for (int cnt = 0 ; cnt < num_fields; cnt++) {
				std::string name = headers[cnt];
				std::string value;
				if ( sql_row[cnt] == NULL ) {
					value = "";
				} else {
					value = sql_row[cnt];
				}

				row.insert(std::pair<std::string, std::string>(name, value));
			}

			rows.push_back(row);
		}
			
		mysql_free_result(sql_res);

		return rows;
	}

	bool create_member(void)
	{
		// 멤버 테이블 생성
		char buf[9072];
		sprintf(buf, "CREATE TABLE IF NOT EXISTS member ( "
				"USER_ID TEXT NOT NULL, "
				"PASSWORD TEXT NOT NULL, "
				"NICK_NAME TEXT NOT NULL, "
				"BIRTHDAY DATE NOT NULL, "
				"EMAIL TEXT, "
				"SEX INT NOT NULL, "
				"LEVEL INT UNSIGNED NOT NULL DEFAULT 1, "
				"IS_OPEN INT UNSIGNED NOT NULL DEFAULT 0, "
				"REGISTRATION_DATETIME DATETIME NOT NULL, "
				"LASTLOGIN_DATETIME DATETIME NOT NULL);");
		if ( mysql_query(mysql, buf) != 0 ) {
			printf("\r\n(%d) [%s] \"%s\"\r\n", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
			press_enter();
			return false;
		}

		return true;
	}


	bool exist_user_id(char *id) 
	{
		char *buf2 = (char*)malloc(strlen(id)*2+1);
		mysql_real_escape_string(mysql, buf2, id, strlen(id));

		char buf[1024];
		sprintf(buf, "SELECT * FROM member WHERE USER_ID='%s';", buf2);
		free(buf2);

		std::vector<std::map<std::string, std::string> > rows = fetch_rows(buf);
		if ( rows.size() > 0 ) {
			return true;
		}
		return false;
	}

	bool exist_nick_name(char *name) 
	{
		char buf[1024];
		sprintf(buf, "SELECT * FROM member WHERE NICK_NAME='%s';", name);
		std::vector<std::map<std::string, std::string> > rows = fetch_rows(buf);
		if ( rows.size() > 0 ) {
			return true;
		}
		return false;
	}

	bool exist_email_address(char *email) 
	{
		char buf[1024];
		sprintf(buf, "SELECT * FROM member WHERE EMAIL='%s';", email);
		std::vector<std::map<std::string, std::string> > rows = fetch_rows(buf);
		if ( rows.size() > 0 ) {
			return true;
		}
		return false;
	}

	bool check_same_password(char *user_id, char *passwd) 
	{
		char buf[1024];
		sprintf(buf, "SELECT PASSWORD FROM member WHERE USER_ID='%s';", user_id);
		bool ok;
		std::string password = fetch(buf, &ok);

		
		sprintf(buf, "SELECT PASSWORD('%s')", passwd);
		if ( !strcmp(fetch((char*)buf, &ok).c_str(), password.c_str()) ) {
			return true;
		}

		return false;
	}
	
#if 0
	bool check_admin(char *user_id)
	{
		char buf[1024];
		sprintf(buf, "SELECT IS_ADMIN FROM member WHERE USER_ID='%s';", user_id);
		bool ok;

		if ( atoi(fetch(buf, &ok).c_str()) == 0 ) {
			return false;
		}
		return true;
	}
#endif

	bool set_lastlogin_datetime(char *user_id)
	{
		char buf[1024];
		sprintf(buf, "UPDATE member SET LASTLOGIN_DATETIME='%s' WHERE USER_ID='%s';", 
			datetime_now_string(false).c_str(), user_id);
		if ( mysql_query(mysql, buf) != 0 ) {
			printf("\r\n(%d) [%s] \"%s\"\r\n", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
			press_enter();
			return false;
		}

		return true;
	}
	
	bool set_user_level(char *user_id, int level)
	{
		char buf[1024];
		sprintf(buf, "UPDATE member SET LEVEL='%d' WHERE USER_ID='%s';", level, user_id);
		if ( mysql_query(mysql, buf) != 0 ) {
			printf("\r\n(%d) [%s] \"%s\"\r\n", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
			press_enter();
			return false;
		}

		return true;
	}

	bool set_user_nick_name(char *user_id, char *nick_name)
	{
		char buf[1024];
		sprintf(buf, "UPDATE member SET NICK_NAME='%s' WHERE USER_ID='%s';", nick_name, user_id);
		if ( mysql_query(mysql, buf) != 0 ) {
			printf("\r\n(%d) [%s] \"%s\"\r\n", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
			press_enter();
			return false;
		}

		return true;
	}

	bool set_user_password(char *user_id, char *password)
	{
		char buf[1024];
		sprintf(buf, "UPDATE member SET PASSWORD=PASSWORD('%s') WHERE USER_ID='%s';", password, user_id);
		if ( mysql_query(mysql, buf) != 0 ) {
			printf("\r\n(%d) [%s] \"%s\"\r\n", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
			press_enter();
			return false;
		}

		return true;
	}

	bool set_user_birthday(char *user_id, char *birthday)
	{
		char buf[1024];
		sprintf(buf, "UPDATE member SET BIRTHDAY='%s' WHERE USER_ID='%s';", birthday, user_id);
		if ( mysql_query(mysql, buf) != 0 ) {
			printf("\r\n(%d) [%s] \"%s\"\r\n", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
			press_enter();
			return false;
		}

		return true;
	}

	bool set_user_sex(char *user_id, char *sex)
	{
		char buf[1024];
		sprintf(buf, "UPDATE member SET SEX='%s' WHERE USER_ID='%s';", sex, user_id);
		if ( mysql_query(mysql, buf) != 0 ) {
			printf("\r\n(%d) [%s] \"%s\"\r\n", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
			press_enter();
			return false;
		}

		return true;
	}

	bool set_user_email(char *user_id, char *email)
	{
		char buf[1024];
		sprintf(buf, "UPDATE member SET EMAIL='%s' WHERE USER_ID='%s';", email, user_id);
		if ( mysql_query(mysql, buf) != 0 ) {
			printf("\r\n(%d) [%s] \"%s\"\r\n", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
			press_enter();
			return false;
		}

		return true;
	}

	bool delete_user(char *user_id)
	{
		char buf[1024];
		sprintf(buf, "DELETE FROM member WHERE USER_ID='%s';", user_id);

		if ( mysql_query(mysql, buf) != 0 ) {
			printf("\r\n(%d) [%s] \"%s\"\r\n", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
			press_enter();
			return false;
		}

		return true;
	}

#if 0
	bool set_admin(char *user_id)
	{
		char buf[1024];
		sprintf(buf, "UPDATE member SET IS_ADMIN=1 WHERE USER_ID='%s';", user_id);
		if ( mysql_query(mysql, buf) != 0 ) {
			printf("\r\n(%d) [%s] \"%s\"\r\n", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
			press_enter();
			return false;
		}

		return true;
	}

	bool unset_admin(char *user_id)
	{
		char buf[1024];
		sprintf(buf, "UPDATE member SET IS_ADMIN=0 WHERE USER_ID='%s';", user_id);
		if ( mysql_query(mysql, buf) != 0 ) {
			printf("\r\n(%d) [%s] \"%s\"\r\n", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
			press_enter();
			return false;
		}

		return true;
	}
#endif

	std::map<std::string, std::string> user_info(char *user_id, bool *exist)
	{
		char buf[1024];
		sprintf(buf, "SELECT * FROM member WHERE USER_ID='%s';", user_id);
		std::vector<std::map<std::string, std::string> > rows = fetch_rows(buf);
		if ( rows.size() == 1 )  {
			*exist = true;
			return fetch_rows(buf).at(0);
		}
		else {
			*exist = false;
			std::map<std::string, std::string> empty;
			return empty;
		}
	}

	std::map<std::string, std::string> user_info_by_nick_name(char *nick_name, bool *exist)
	{
		char buf[1024];
		sprintf(buf, "SELECT * FROM member WHERE NICK_NAME='%s';", nick_name);
		std::vector<std::map<std::string, std::string> > rows = fetch_rows(buf);
		if ( rows.size() >= 1 )  {
			*exist = true;
			return fetch_rows(buf).at(rows.size()-1);
		}
		else {
			*exist = false;
			std::map<std::string, std::string> empty;
			return empty;
		}
	}
	

	// 쪽지 테이블 생성
	bool create_memo(void)
	{
		char buf[9072];
		sprintf(buf, "CREATE TABLE IF NOT EXISTS memo ( "
				"SENDER_USER_ID TEXT NOT NULL, "
				"RECIPIENT_USER_ID TEXT NOT NULL, "
				"CREATION_DATETIME DATETIME NOT NULL, "
				"CONFIRMATION_DATETIME DATETIME, "
				"TITLE TEXT NOT NULL, "
				"CONTENT MEDIUMTEXT NOT NULL);");
		if ( mysql_query(mysql, buf) != 0 ) {
			printf("\r\n(%d) [%s] \"%s\"\r\n", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
			press_enter();
			return false;
		}

		return true;
	}

	// 대화방 테이블 생성
	bool create_chatt_room(char *table_name)
	{
		char query[9072];

		sprintf(query, "CREATE TABLE IF NOT EXISTS %s ( "
				"ROOM_NO INT NOT NULL, "	// 방번호
				"PORT_NO INT NOT NULL, "  // 포트번호
				"PASSWORD TEXT NOT NULL, "	// 패스워드
				"MAX_USER INT NOT NULL, "	// 최대허용인원
				"TITLE TEXT NOT NULL"		// 주제
			");", table_name);
		if ( mysql_query(mysql, query) != 0 ) {
			printf("\r\n(%d) [%s] \"%s\"\r\n", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
			press_enter();
			return false;
		}

		return true;
	}

	bool add_chatt_room(char *table_name, int room_no, int port_number,
		   	char *user_id, char *password, int max_user, char *title)
	{
		std::ostringstream query;

		char *title2 = (char*)malloc(strlen(title)*2+1);
		mysql_real_escape_string(mysql, title2, title, strlen(title));
		
		query.str("");
		query << "INSERT INTO ";
		query << table_name << " (ROOM_NO, PORT_NO, PASSWORD, MAX_USER, TITLE) VALUES ( ";
		query << "'" << room_no << "', ";
		query << "'" << port_number << "', ";
		if ( strlen(password) > 0 ) {
			// 단방향 패스워드 알고리즘 사용
			query << "PASSWORD('" << password << "'), ";
		} else {
			query << "'" << password << "', ";
		}
		query << "'" << max_user << "', ";
		query << "'" << title2 << "'";
		query << ");";

		free(title2);

		if ( mysql_query(mysql, query.str().c_str()) != 0 ) {
			printf("\r\n(%d) [%s] \"%s\"\r\n", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
			press_enter();
			return false;
		}

		return true;
	}

	bool delete_chatt_room(char *table_name, int room_no)
	{
		std::ostringstream query;
		query << "DELETE FROM " << table_name << " WHERE ROOM_NO=" << room_no << ";";

		if ( mysql_query(mysql, query.str().c_str()) != 0 ) {
			printf("\r\n(%d) [%s] \"%s\"\r\n", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
			press_enter();
			return false;
		}

		return true;
	}
	
	bool check_public_chatt_room(char *table_name, int room_no)
	{
		char sql[1024];
		sprintf(sql, "SELECT * FROM %s WHERE ROOM_NO=%d", table_name, room_no);

		std::vector<std::map<std::string, std::string> > rows = fetch_rows(sql);
		if ( rows.size() != 1 ) {
			return -1;
		}
		std::map<std::string, std::string> row = rows.at(0);

		if (strlen(row[std::string("PASSWORD")].c_str()) > 0) {
			return false;
		}

		return true;
	}

	int chatt_room_port_number(char *table_name, int room_no)
	{
		char sql[1024];
		sprintf(sql, "SELECT * FROM %s WHERE ROOM_NO=%d", table_name, room_no);
		
		std::vector<std::map<std::string, std::string> > rows = fetch_rows(sql);
		if ( rows.size() != 1 ) {
			return -1;
		}
		std::map<std::string, std::string> row = rows.at(0);

		int port = atoi(row[std::string("PORT_NO")].c_str());
		return port;
	}
	
	int chatt_room_max_user(char *table_name, int room_no)
	{
		char sql[1024];
		sprintf(sql, "SELECT * FROM %s WHERE ROOM_NO=%d", table_name, room_no);
		
		std::vector<std::map<std::string, std::string> > rows = fetch_rows(sql);
		if ( rows.size() != 1 ) {
			return -1;
		}
		std::map<std::string, std::string> row = rows.at(0);

		int max_user = atoi(row[std::string("MAX_USER")].c_str());
		return max_user;
	}

	bool exist_chatt_room(char *table_name, int room_no)
	{
		char sql[1024];
		sprintf(sql, "SELECT * FROM %s WHERE ROOM_NO=%d", table_name, room_no);
		
		std::vector<std::map<std::string, std::string> > rows = fetch_rows(sql);
		if ( rows.size() == 1 ) {
			return true;
		}

		return false;
	}

	bool check_chatt_password(char *table_name, int room_no, char *passwd) 
	{
		char buf[1024];
		sprintf(buf, "SELECT PASSWORD FROM %s WHERE ROOM_NO='%d';", table_name, room_no);
		bool ok;
		std::string password = fetch(buf, &ok);

		
		sprintf(buf, "SELECT PASSWORD('%s')", passwd);
		if ( !strcmp(fetch((char*)buf, &ok).c_str(), password.c_str()) ) {
			return true;
		}

		return false;
	}
}
