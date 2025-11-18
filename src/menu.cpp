#include "main.h"

std::vector<std::map<std::string, pugi::xml_node> > go_menus;
std::vector<std::string> table_names;

// go 메뉴가 정의되어 있는 xml 노드를 파싱한다.
void read_go_menus(pugi::xml_node node, 
		std::vector<std::map<std::string, pugi::xml_node> > &list)
{
	pugi::xml_node child = node.first_child();

	while (1) {
		if ( child.type() == pugi::node_element ) {
			pugi::xml_attribute attr = child.attribute("go");
			if ( !attr.empty() ) {
				std::map<std::string, pugi::xml_node> row;
				row[attr.value()] = child;
				list.push_back(row);
			}

			pugi::xml_node node2 = child.first_child();
			if ( !node2.empty() ) {
				read_go_menus (child, list);
			}
		}

		child = child.next_sibling();
		if ( child.empty() ) break;
	}
}

// 지정 id가 있는 노드를 리턴한다.
pugi::xml_node find_node_by_id(pugi::xml_node node, std::string id)
{
	pugi::xml_node child = node.first_child();

	while (1) {
		if ( child.type() == pugi::node_element ) {
			pugi::xml_attribute attr = child.attribute("id");
			if ( !attr.empty() ) {
				if ( attr.value() == id ) {
					return child;
				}
			}

			pugi::xml_node node2 = child.first_child();
			if ( !node2.empty() ) {
				find_node_by_id (child, id);
			}
		}

		child = child.next_sibling();
		if ( child.empty() ) break;
	}

	return pugi::xml_node();
}

// board id 목록 파싱한다.
void read_table_names(pugi::xml_node node, std::vector<std::string> &list)
{
	pugi::xml_node child = node.first_child();

	while (1) {
		if ( child.type() == pugi::node_element ) {
			pugi::xml_attribute type = child.attribute("type");
			pugi::xml_attribute id = child.attribute("id");
			if ( !id.empty() ) {
				if ( !strcmp(type.value(), "board") ) {
					list.push_back(id.value());
				}
			}

			pugi::xml_node node2 = child.first_child();
			if ( !node2.empty() ) {
				read_table_names (child, list);
			}
		}

		child = child.next_sibling();
		if ( child.empty() ) break;
	}
}

