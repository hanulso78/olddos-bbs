#include "main.h"

// 서버 셋팅 변수
char host_name[256];
unsigned int show_max_line = 15;
bool ks_conversion = false;

// 서버 DB 변수
char db_host[256];
char db_user[256];
char db_passwd[256];
char db_name[256];

// mail server 변수
char mailserver_host[256];
char mailserver_port[256];
char mailserver_user[256];
char mailserver_passwd[256];

// 관리자 권한 유저 목록
std::vector<std::string> sysop_users;

// 레벨 목록
std::vector<int> level_nums;
std::vector<std::string> level_names;

void read_settings(std::string cfg_path)
{
	pugi::xml_document doc;
	//pugi::xml_parse_result res = doc.load_file("hanulso.cfg", 
	pugi::xml_parse_result res = doc.load_file(cfg_path.c_str(),
			pugi::parse_default|pugi::parse_declaration);
    if (!res) {
		std::ostringstream msg;
		msg << " Parse error: " << res.description() << ", character pos= " << res.offset;
		printf("\r\n\r\n%s", msg.str().c_str());
		printf("\r\n [Enter] 를 누르세요.");
		press_enter();
		host_close();
	}

	pugi::xml_node node = doc.first_element_by_path("/hanulso");

	// 호스트 이름
	sprintf(host_name, "%s", node.child("name").child_value());
	
	// db 정보
	sprintf(db_name, "%s", node.child("database").child("name").child_value());
	sprintf(db_host, "%s", node.child("database").child("host").child_value());
	sprintf(db_user, "%s", node.child("database").child("user").child_value());
	sprintf(db_passwd, "%s", node.child("database").child("password").child_value());

    // mail server 정보
    sprintf(mailserver_host, "%s", node.child("mailserver").child("host").child_value());
    sprintf(mailserver_port, "%s", node.child("mailserver").child("port").child_value());
    sprintf(mailserver_user, "%s", node.child("mailserver").child("user").child_value());
    sprintf(mailserver_passwd, "%s", node.child("mailserver").child("password").child_value());
	
	// 관리자권한 유저 목록
	pugi::xml_node node2 = node.child("sysop").first_child();
	while (1) {
		std::string user_id = node2.child_value();
		sysop_users.push_back(std::string(user_id));
		node2 = node2.next_sibling();
		if ( node2.empty() ) break;
	}

	// show max line
	show_max_line = atoi(node.child("article").child("show_max_line").child_value());
	if ( !strcasecmp( node.child("article").child("ks_conversion").child_value(), "true" ) ) {
		ks_conversion = true;
	}

	// 레벨 목록
	node2 = node.child("level").first_child();
	while (1) {
		std::string alias = node2.child_value();
		std::vector<std::string> tokens = split_string(alias, ',');
		level_nums.push_back(atoi(tokens[0].c_str()));
		level_names.push_back(tokens[1]);
		node2 = node2.next_sibling();
		if ( node2.empty() ) break;
	}
}

std::string get_level_name(int user_level)
{
	std::string name = "";
	for(unsigned int i=0; i<level_nums.size(); i++) {
		int level = level_nums[i];
		if (level == user_level) {
			name = level_names[i];
		}
	}

	return name;
}

bool check_exist_level(int user_level)
{
	for(unsigned int i=0; i<level_nums.size(); i++) {
		int level = level_nums[i];
		if (level == user_level) {
			return true;
		}
	}

	return false;
}

