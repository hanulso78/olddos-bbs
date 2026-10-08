#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <malloc.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <termio.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <utmp.h>
#include <sys/mman.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <errno.h>

#include <mysql/mysql.h>
#include <crypt.h>
#include <iconv.h>

#include <list>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glob.h>
#include <map>
#include <set>
#include <string>
#include <algorithm>
#include <iomanip>

#include "base64.h"
#include "ansi.h"
// https://github.com/kokke/tiny-regex-c
#include "re.h"

#include "pugixml.hpp"

#define YES	1
#define NO	0

extern struct termio sys_term, curr_term;

extern char login_user_id[50];
extern int login_user_level;
extern bool login_user_is_admin;

extern char db_host[256];
extern char db_user[256];
extern char db_passwd[256];
extern char db_name[256];

extern char mailserver_host[256];
extern char mailserver_port[256];
extern char mailserver_user[256];
extern char mailserver_passwd[256];

extern std::vector<std::string> sysop_users;

extern std::vector<int> level_nums;
extern std::vector<std::string> level_names;

extern char tty[10];


extern MYSQL *mysql;

struct FileTimeComparator {
    bool operator() (const std::string& f1, const std::string& f2){
        struct stat attr1;
        struct stat attr2;  
        stat( f1.c_str(), &attr1);
        stat( f2.c_str(), &attr2); 
        return attr1.st_mtime < attr2.st_mtime;
    }
};

#define TO_STRING( x ) dynamic_cast< std::ostringstream & >( \
            ( std::ostringstream() << std::dec << x ) ).str()

namespace database {
	bool open();
	bool close();
	std::string escape(const char *str);
	std::string hash_password(const char *passwd);
	bool verify_password(const char *passwd, const std::string &stored, bool *legacy);
	void ensure_indexes(const std::vector<std::string> &boards);
	bool create_attachment(void);
	bool create_board(char *table_name);
	int add_article(char *table_name, char *user_id, char *date, char *time, char *title, char *content);
	bool attach_article(char *table_name, int no, int parent_no);
	bool delete_article(char *table_name, int no);
	std::string fetch(char *query, bool *ok);
	std::vector<std::map<std::string, std::string> > fetch_rows(char *query);
	int article_count(char *table_name);
	int article_max_number(char *table_name);
	int article_count_by_title_content(char *table_name, char *text);
	int article_count_by_user_id(char *table_name, char *user_id);
	int article_count_by_nick_name(char *table_name, char *nick_name);
	int attachment_count(char *table_name, int no);
	bool increase_hit(char *table_name, int no);
	bool increase_recommend(char *table_name, int no);
	bool increase_download(int no);
	bool add_attachment(char *family_table_name, int family_no, char *user_id, 
			char *date, char *time, char *tmp_filename, char *filename);
	bool delete_attachment(char *table_name, int no, int attach_no);
	bool edit_attachment(char *table_name, int no, std::string attachment);
	bool edit_article_content(char *table_name, int no, std::string content);
	bool edit_article_title(char *table_name, int no, std::string title);
	bool check_exist(char *table_name, int no);
	bool check_same_author(char *table_name, int no, char *user_id);

	bool create_member(void);
	bool exist_user_id(char *id);
	bool exist_nick_name(char *name);
	bool exist_email_address(char *email);
	bool check_same_password(char *user_id, char *passwd);
	//bool check_admin(char *user_id);
	bool set_lastlogin_datetime(char *user_id);
	std::map<std::string, std::string> user_info(char *user_id, bool *exist);
	std::map<std::string, std::string> user_info_by_nick_name(char *nick_name, bool *exist);
	bool set_user_level(char *user_id, int level);
	bool set_user_name(char *user_id, char *name);
	bool set_user_nick_name(char *user_id, char *nick_name);
	bool set_user_password(char *user_id, char *password);
	bool set_user_birthday(char *user_id, char *birthday);
	bool set_user_sex(char *user_id, char *sex);
	bool set_user_email(char *user_id, char *email);
	bool delete_user(char *user_id);
	//bool set_admin(char *user_id);
	//bool unset_admin(char *user_id);
	
	bool create_memo(void);

	bool create_chatt_room(char *table_name);
	bool add_chatt_room(char *table_name, int room_no, int port_number,
		   	char *user_id, char *password, int max_user, char *title);
	bool delete_chatt_room(char *table_name, int room_no);
	int chatt_room_port_number(char *table_name, int room_no);
	int chatt_room_max_user(char *table_name, int room_no);
	bool exist_chatt_room(char *table_name, int room_no);
	bool check_chatt_password(char *table_name, int room_no, char *passwd);
	bool check_public_chatt_room(char *table_name, int room_no);
}

// ----------------------------------------------------------
// 호스트 정보 읽음
extern bool ks_conversion;
extern unsigned int show_max_line;
extern char host_name[256];

void read_settings(std::string cfg_path);

// ----------------------------------------------------------
// 메뉴 파일 정보 읽음
extern std::vector<std::map<std::string, pugi::xml_node> > go_menus;
extern pugi::xml_node current_menu;
extern std::vector<std::string> table_names;

void read_go_menus(pugi::xml_node node, std::vector<std::map<std::string, pugi::xml_node> > &list);
void read_table_names(pugi::xml_node node, std::vector<std::string> &list);
pugi::xml_node find_node_by_id(pugi::xml_node node, std::string id);

// ----------------------------------------------------------
void raw_mode(void);
int host_close();
bool host_close_yesno(void);
int host_close(void);
void prompt(char *cmd, bool enable_write, bool enable_del);

// 성격 검사
void run_mbti_gs();
bool run_menu_program(pugi::xml_node node, const std::string &type);

// 출석 체크 / 생일 축하 (attendance.cpp)
void attendance_login_info(const char *user_id);
bool is_birthday_today(const std::string &birthday);
void birthday_celebrate(const char *nick);
void show_attendance(const char *user_id);

// 전보 (telegram.cpp)
void telegram_init(void);
void telegram_show_pending(void);
void telegram_live_begin(const char *prompt_text);
void telegram_live_end(void);
void telegram_command(const std::string &cmd);

// 꼬리말, 새 글 모아보기, 전체 게시판 검색 (board_extra.cpp)
extern pugi::xml_node menu_root;
void comments_init(void);
int comment_count(const char *table, int no);
std::vector<std::string> comment_lines(const char *table, int no);
bool add_comment(const char *table, int no, std::string text);
void delete_comment(const char *table, int no, int index);
void delete_comments_of(const char *table, int no);
void show_new_articles(void);
void search_all_boards(std::string word);
void show_best(std::string arg);
void show_my(std::string arg);
void scrap_init(void);
void scrap_toggle(const char *table, int no);
extern long login_log_no;
void login_log_upgrade(void);
std::string remote_host_of_tty(void);
void show_login_log(void);
// 게시판 구독: 들어가 있는 게시판 (board_scope 가 들어갈 때 정하고 나올 때 되돌린다)
extern std::string current_board;
struct board_scope {
	std::string old;
	board_scope(const char *id);
	~board_scope();
};
void subscribe_init(void);
void subscribe_command(void);
void notify_subscribers(const char *table, int no);
// 투표
void poll_init(void);
void show_polls(void);

// 한줄 낙서장 (graffiti.cpp)
void graffiti_login_info(void);
void show_graffiti(const char *user_id, bool is_admin);

// 오늘의 통계 (stats.cpp)
void stats_record_online(void);
void show_stats(void);
void print_news_title(const char *title);

void print_board_header(char *table_name, char *title, int page_count, int page_no);
void print_biorhythm(char *user_id, bool full = false);
void print_luck(char *user_id);
void print_user_info(char *user_id);

bool edit_profile(char *user_id);
void profile_upgrade(void);

std::string get_level_name(int user_level);
bool check_exist_level(int user_level);

void show_menu(pugi::xml_node node);
void show_board(pugi::xml_node node);
void show_article(char *table_name, int board_page_count, int board_page_no, char *title, int no, bool attachment, bool reply, bool *is_dir);
int write_article(char *table_name);
void article_reply_family(char *table_name, int no, int *family, int *orderby, int *step);
bool edit_article(char *table_name, int no);
bool delete_article(char *table_name, int no);
bool delete_attachment(char *table_name, int no, int attach_no);
bool delete_attachment_all(char *table_name, int no);

bool jump_go(std::string go);
void show_chatt_rooms(pugi::xml_node node, bool *goto_top);

void show_weather_board(pugi::xml_node node, bool *goto_top);
void show_newspaper_board(pugi::xml_node node, bool *goto_top);

std::string random_string(const int len);
std::string centered(const std::string s, const int w);
std::string repeat(const std::string s, int n);

bool line_editor(std::vector<std::string> &lines, bool edit);
void line_editor_layout(int indent, int width);
bool file_editor(char **lines, int *length, int protocol);
int ask_upload_protocol(void);
void strip_xmodem_pad(const char *path);
// Kermit 프로그램: $HANULSO 아래 G-Kermit (-i -r = 바이너리 받기, -i -s 파일 = 바이너리 보내기)
#define KERMIT_PROG "%s/bin/gkermit"

bool file_upload(int protocol, const char *xname, char **tmp_filename, char **filename, int *size);
bool file_download(int protocol, char *tmp_filename, char *filename, std::string *err = NULL);

// ----------------------------------------------------------------
std::string replace_bbcode(std::string text);
std::string bbcode_host(std::string text);
std::string bbcode_ansi(std::string text);

// ----------------------------------------------------------------
std::string query_luck(int yy, int mm, int dd, int sex, int birth_yy, int birth_mm, int birth_dd, int lunar, int yun);

// ----------------------------------------------------------------
std::string html2text(std::string html);
std::string make_private_tmpfile(const char *prefix);
std::string safe_terminal_text(const std::string &s);
bool board_sysop_only(const char *table);
void print_file(const char *filename);
void print_file_page(const char *filename);
std::string read_file(const char *path);
std::vector<std::string> find_files(const std::string& path);
std::string replace_all(std::string str, const std::string& from, const std::string& to);
bool is_binary(const void *data, size_t len);
std::string string_convert(std::vector<std::string> lines);
std::string convert_to_string(double num);
void date_now(int *year, int *month, int *day, std::string &week);
void time_now(int *hour, int *min, int *sec);
std::string date_now_string(bool week);
std::string time_now_string(void);
std::string datetime_now_string(bool week);
std::vector<std::string> split_string(std::string str, char delimiter);
std::vector<std::string> split_string_with_width(std::string str, char delimiter, int width);
bool is_number(std::string s);
long long ms_time_now (void);
long file_size(std::string filename);
long file_mtime(std::string filename);
long file_ms_mtime(std::string filename);
std::string ms_time_to_string(long ms_time);
std::string human_file_size(long size);
std::vector<std::string> split(const std::string& str, const std::string& delim);
std::string split_file_path (const std::string& str);
std::string split_file_name (const std::string& str);
std::string filename_only(std::string file);
std::vector<std::string> exec_command(char *command, bool *success);
bool is_han(char c);
bool dir_exists(char *dir);
bool file_exists(char *file);
bool is_date_valid(int d, int m, int y);
bool is_email_valid(const char *address);
std::vector<std::string> find_files_time_sorted(char *dir);
bool mkdir2 (char *dir);
std::string trim(std::string s);
void bbcode_printf(const char *fmt,...);
extern int screen_rows;
int terminal_rows(void);
extern std::string (*bbcode_tag_hook)(const std::string &name, bool *found);
std::string bbtag_value(const std::string &name, bool *found);
std::string utf8_substr2(const std::string &str,int start, int length);
std::string string_truncate_center (std::string str, int max, std::string sep );
std::string string_truncate (std::string str, int length, std::string suffix );
std::string string_chop(std::string str, unsigned int length, std::string chop);
bool check_used_port(int port);
std::string add_slashes(std::string str);
std::string shell_quote(const std::string &str);
bool download_url(const std::string &url, const std::string &path);
std::string fix_hangul(const std::string &s);
std::string display_text(const std::string &s);
bool read_tty_file(const std::string &path, std::string &user_id);
bool is_bbs_process(int pid);
int notify_online(const std::string &user_id, const std::string &line, bool signal_bbs);
int check_password_strongness(std::string str);

char *utf8_to_cp949(char * input);
char *cp949_to_utf8(char * input);

void add_user_tmpfile(char *path);
bool make_editor_tmpfile(char *dir, char *file, size_t size);
std::vector<std::string> wrap_words(const std::string &line, int width);
std::string screen_editor_command(const char *dir, const char *file);
std::vector<std::string> user_tmpfiles(void);
void del_user_tmpfiles(void);
void remove_tmp_dir(const char *dir);
void sweep_stale_tmp(void);
std::string attachment_bucket(void);
int count_dir_files(const std::string &dir, int limit);
bool download_text(const std::string &url, std::string &out);
std::string utf8_to_cp949(const std::string &in);

void press_enter(void);
void line_input(char *str,int len);
bool line_input_wrap(char *str, const char *init_str, int len, std::string &carry);
void line_input_edit(char *str, char *init_str, int len);
void line_input2(char *mess, char *str, int len);
void line_input_echo(char *str, int len);
extern void (*line_input_wait_hook)(const char *typed);
int yesno(int defaultkey);
void goto_screen(int row, int col);
void clear_screen(void);
//std::string strip_ansi_codes(const std::string &text);
//
char *strip_ansi_codes(const char *line);
std::string replace_bbcode(std::string text);

// 완성형/조합형 판단
int check_hangul_code( const char *buf, int len, int *ks_buf, int *ksm_buf, int *eng_buf );
