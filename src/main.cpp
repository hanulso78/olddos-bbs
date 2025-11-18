#include "main.h"

struct termio sys_term, curr_term;

// Á¢¼Ó pts ³ëµå ¹øÈ£
char tty[10];
	
// ·Î±×ÀÎ Á¤º¸
char login_user_id[50];
int login_user_level;
bool login_user_is_admin;

int main(int argc, char **argv)
{
	sprintf(tty, "%s", argv[1]);

    signal(SIGQUIT, SIG_IGN);
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
    signal(SIGHUP, (__sighandler_t)host_close);
    signal(SIGSEGV, (__sighandler_t)host_close);
    signal(SIGBUS, (__sighandler_t)host_close);

    ioctl(0,TCGETA, &sys_term);
	raw_mode();
    ioctl(0, TCGETA, &curr_term);
	
    umask(0111);

	printf(ESC_CLEAR);

	// --------------------------------------
	read_settings("hanulso.cfg");
	// --------------------------------------
	// DB open ...
	if ( database::open() == false )
		exit(1);

	// È¸¿ø Å×ÀÌºí »ý¼º
	database::create_member();

	// Ã·ºÎ ÆÄÀÏ Å×ÀÌºí »ý¼º
	database::create_attachment();

	// ·£´ý ´ë¹® Ãâ·Â
	std::vector<std::string> door_files = find_files("txt/door/*");
	if ( door_files.size() > 0 ) {
		srand(time(NULL));
		int i = rand() % door_files.size();
		print_file(door_files[i].c_str());
	}
	//print_file("txt/login.txt");

	printf(ESC_ENG);
        
	printf("%s", repeat("¦¬", 40).c_str());
	bbcode_printf("\r\n ## [=15G[=0FÀÌ¿ëÀÚID°¡ ¾ø°Å³ª ½Å±Ô/¹«·á°¡ÀÔÀ» ÇÏ½Ã·Á¸é guestÀ» ÀÔ·ÂÇÏ½Ê½Ã¿À.[=15F[=1G");
	
	int retry = 0;
	while (1) {
		if ( retry == 3 ) {
			host_close();
		}

		printf("\r\n ÀÌ¿ëÀÚID : ");
		line_input(login_user_id, 20);

		// È¸¿ø °¡ÀÔ Ã³¸®
		if ( !strcasecmp(login_user_id, "guest") ) {
			char buf[1024];
			sprintf(buf, "%s/bin/guest", getenv("HANULSO"));

			ioctl(0, TCSETAF, &sys_term);
			execl(buf, "guest", tty, (char*)0);
			host_close();

		} else if ( !strcasecmp(login_user_id, "pass") ) {
			char buf[1024];
			sprintf(buf, "%s/bin/pass", getenv("HANULSO"));

			ioctl(0, TCSETAF, &sys_term);
			execl(buf, "pass", tty, (char*)0);
			host_close();

		// È¸¿ø °Ë»ç
		} else {
			if ( strlen(login_user_id) == 0 ) {
				printf("\r\n ¾ÆÀÌµð¸¦ ÀÔ·ÂÇØÁÖ¼¼¿ä.");
			} else {
				if ( database::exist_user_id(login_user_id) == false ) {
					printf("\r\n µî·ÏµÈ ¾ÆÀÌµð°¡ ¾ø½À´Ï´Ù.");
				} else {
					break;
				}
			}
		}

		retry+=1;
	}

	retry = 0;
	while (1) {
		if ( retry == 3 ) {
			host_close();
		}

		// ºñ¹Ð¹øÈ£ ÀÔ·Â
		char passwd[50];
		printf("\r\n ºñ¹Ð¹øÈ£ : ");
		line_input_echo(passwd, 20);

		if ( strlen(passwd) == 0 ) {
			printf("\r\n ºñ¹Ð¹øÈ£¸¦ ÀÔ·ÂÇØÁÖ¼¼¿ä.");

		} else {
			if ( database::check_same_password(login_user_id, passwd) == false ) {
				printf("\r\n ºñ¹Ð¹øÈ£°¡ Æ²·È½À´Ï´Ù.");
			} else {
				break;
			}
		}

		retry+=1;
	}

	// --------------------------------------
	// Á¢¼ÓID ±â·Ï
	char buf[1024];
	sprintf(buf, "%s/tmp/%s.tty", getenv("HANULSO"), tty);

	FILE *fp = fopen(buf, "w");
	fprintf(fp, "%s", login_user_id);
	fclose(fp);

	// ------------------------------------
	bool exist;
	std::map<std::string, std::string> user = database::user_info(login_user_id, &exist);

	login_user_level = atoi(user["LEVEL"].c_str());
	std::string login_user_level_name = get_level_name(login_user_level);

	// °ü¸®ÀÚÀÎÁö È®ÀÎ
	if ( std::find(sysop_users.begin(), sysop_users.end(), login_user_id) != sysop_users.end() )
		login_user_is_admin = true;
	else
		login_user_is_admin = false;

	printf("\r\n");
	printf("\r\n [°¡ ÀÔ ÀÏ] : %s", user["REGISTRATION_DATETIME"].c_str());
	printf("\r\n [´Ð ³× ÀÓ] : %s", user["NICK_NAME"].c_str());
	if ( std::find(sysop_users.begin(), sysop_users.end(), login_user_id) != sysop_users.end() ) {
		printf("\r\n [·¹    º§] : ½Ã¼ó(¿î¿µÀÚ)");
	} else {
		printf("\r\n [·¹    º§] : %s", login_user_level_name.c_str());
	}
	char *lastlogin_datatime = (char*)user["LASTLOGIN_DATETIME"].c_str();
	if ( strlen(lastlogin_datatime) == 0 ) {
		printf("\r\n [ÃÖ±ÙÁ¢¼Ó] : ¾øÀ½");
	} else {
		printf("\r\n [ÃÖ±ÙÁ¢¼Ó] : %s", lastlogin_datatime);
	}
	printf("\r\n [³ë    µå] : pts/%s", tty);

	
	// ¹ÙÀÌ¿À¸®µë Ãâ·Â
	printf("\r\n\r\n");
	print_biorhythm(login_user_id);
	printf("\r\n [Enter] ¸¦ ´©¸£¼¼¿ä.");
	press_enter();
	
	// -----------------------------
	pugi::xml_document doc;

	pugi::xml_parse_result res = doc.load_file("hanulso.mnu", 
			pugi::parse_default|pugi::parse_declaration);
    if (!res) {
		std::ostringstream msg;
		msg << " Parse error: " << res.description() << ", character pos= " << res.offset;
		printf("\r\n\r\n%s", msg.str().c_str());
		printf("\r\n [Enter] ¸¦ ´©¸£¼¼¿ä.");
		press_enter();

		host_close();
	}
	

	// -----------------------------------------------------------
	pugi::xml_node root_node = doc.first_element_by_path("/hanulso/item");

	read_go_menus(doc, go_menus);
	read_table_names(doc, table_names);

	// Å×ÀÌºíÀÌ ¾øÀ»°æ¿ì »ý¼ºÇÑ´Ù.
	for(unsigned int i=0; i<table_names.size(); i++) {
		if ( database::create_board((char*)table_names[i].c_str()) == false ) {
			host_close();
		}
	}

#if 0
	// °øÁö º¸¿©ÁÖ±â
	pugi::xml_node notice = find_node_by_id(root_node, "notice");
	if ( !notice.empty() ) {
		show_board(notice);
	}
#endif

	//printf("\r\n%d\r\n", go_menus.size());
	//press_enter();

	show_menu(root_node);

	host_close();

	return 0;
}

void show_menu(pugi::xml_node node)
{
	while (1) {
		char *header = (char*)(node.child("header").child_value());
		print_file(header);

		std::string text = node.attribute("text").value();
		print_file(text.c_str());

		char *footer = (char*)(node.child("footer").child_value());
		print_file(footer);

		char cmd[1024];
		prompt(cmd, true, true);

		/* ÀÔ·ÂÀÌ ¼ýÀÚ */
		if( is_number(cmd) ) {
			pugi::xml_node node2 = node.first_child();

			bool proceed = false;
			while (1) {
				if ( !strcasecmp(node2.name(), "item") ) {
					std::string door = node2.attribute("door").value();

					if ( !strcasecmp( cmd, door.c_str() ) ) {
						std::string type = node2.attribute("type").value();

						if ( !strcasecmp( type.c_str(), "board" ) ) {
							show_board(node2);
							proceed = true;
							break;

						} else if ( !strcasecmp( type.c_str(), "menu" ) ) {
							show_menu(node2);
							proceed = true;
							break;

						} else if ( !strcasecmp( type.c_str(), "chatt" ) ) {
							bool goto_top;
							show_chatt_rooms(node2, &goto_top);
							if ( goto_top ) {
								jump_go("top");
							}
							proceed = true;
							break;

						} else if ( !strcasecmp( type.c_str(), "weather" ) ) {
#if 0
							bool goto_top;
							show_weather_board(node2, &goto_top);
							if ( goto_top ) {
								jump_go("top");
							}
#else
                            char buf[1024];
                            sprintf(buf, "%s/bin/weather \"%s\"", getenv("HANULSO"), host_name);
                            system(buf);
#endif
							proceed = true;
							break;

						} else if ( !strcasecmp( type.c_str(), "news" ) ) {
							bool goto_top;
							show_newspaper_board(node2, &goto_top);
							if ( goto_top ) {
								jump_go("top");
							}
							proceed = true;
							break;
						}
					}
				}
				node2 = node2.next_sibling();
				if ( node2.empty() ) break;
			}

			if ( !proceed ) {
				printf("\r\nÇØ´ç ¸Þ´º°¡ Á¤ÀÇ µÇ¾î ÀÖÁö ¾Ê½À´Ï´Ù.");
				printf("\r\n[Enter] ¸¦ ´©¸£¼¼¿ä.");
				press_enter();
			}

		} else {
			// »óÀ§ ¸í·É
			if ( !strcasecmp(cmd, "p") ) {
				if ( strcasecmp(node.parent().name(), "hanulso") ) {
					break;
				} else {
					/*
					printf("\r\n\r\nÃÖ»óÀ§ ¸Þ´º ÀÔ´Ï´Ù.");
					printf("\r\n[Enter] ¸¦ ´©¸£¼¼¿ä.");
					press_enter();
					*/
				}
			}
		}
	}
}

void show_board(pugi::xml_node node)
{
	char cmd[1024];
	char search_lt[1024];
	char search_li[1024];
	char search_ln[1024];

	char *table_name = (char*)(node.attribute("id").value());
	char *title = (char*)(node.child("name").child_value());
	
	int access_level = atoi((char*)(node.attribute("access_level").value()));

	if ( !login_user_is_admin && login_user_level < access_level ) {
		printf("\r\n%s ÀÌ»ó ÁøÀÔ °¡´ÉÇÕ´Ï´Ù.", get_level_name(access_level).c_str());
		printf("\r\n[Enter] ¸¦ ´©¸£¼¼¿ä.");
		press_enter();
		return;
	}

	// Ã·ºÎ ÆÄÀÏ ¾÷·Îµå¸¦ Áö¿øÇÏ´ÂÁöÀÇ ¿©ºÎ
	char *value = (char*)(node.child("attachment").child_value());
	bool attachment = true;
	if ( !strcasecmp(value, "no") ) {
		attachment = false;
	}

	// ´ä±ÛÀ» ´Þ¼ö ÀÖ´Â °Ô½ÃÆÇÀÎÁöÀÇ ¿©ºÎ
	value = (char*)(node.child("reply").child_value());
	bool reply = false;
	if ( !strcasecmp(value, "yes") ) {
		reply = true;
	}

	// ¿î¿µÀÚ¸¸ ²ø ¾²±â °¡´ÉÇÑ °Ô½ÃÆÇÀÎÁöÀÇ ¿©ºÎ
	value = (char*)(node.child("write_sysop_only").child_value());
	bool write_sysop_only = false;
	if ( !strcasecmp(value, "yes") ) {
		write_sysop_only = true;
	}

	if ( database::create_board(table_name) == false )
		return;

	int offset = 0;

	// ÇöÀç ÆäÀÌÁö
	int page_no = 1;

	// °Ë»ö¾î ÃÊ±âÈ­
	sprintf(search_lt, "%s", "");
	sprintf(search_li, "%s", "");
	sprintf(search_ln, "%s", "");

	while (1) {
		int total_count;
		if ( strlen(search_lt) > 0 ) {
			total_count = database::article_count_by_title_content(table_name, search_lt);
		} else if ( strlen(search_li) > 0 ) {
			total_count = database::article_count_by_user_id(table_name, search_li);
		} else if ( strlen(search_ln) > 0 ) {
            bool exist;
            std::map<std::string, std::string> user = database::user_info_by_nick_name((char*)search_ln, &exist);
            if ( exist ) {
                char *user_id = (char*)(user["USER_ID"].c_str());
                total_count = database::article_count_by_user_id(table_name, user_id);
            } else {
                total_count = 0;
            }
		} else {
			total_count = database::article_count(table_name);
		}

		// ÀüÃ¼ ÆäÀÌÁö ¼ö
		int page_count = total_count / show_max_line;
		if ( total_count % show_max_line >= 1 ) {
			page_count += 1;
		}

#if 1
		if ( total_count <= offset ) {
			offset -= show_max_line;
			if ( offset < 0 ) offset = 0;
			page_no -= 1;
		}

		char sql[1024];

#if 0
		// ³¯Â¥·Î ¼ÒÆ®
		// select * from bbs_notice order by date_time desc;
#endif

		// Á¦¸ñ/³»¿ëÀ¸·Î °Ë»ö
		if ( strlen(search_lt) > 0 ) {
			sprintf(sql, "SELECT * FROM %s "
					"WHERE TITLE LIKE '%%%s%%' "
					"OR CONTENT LIKE '%%%s%%' "
					"ORDER BY FAMILY DESC, ORDERBY ASC LIMIT %d, %d", 
					table_name, search_lt, search_lt, offset, show_max_line);

		// ¾ÆÀÌµð·Î °Ë»ö
		} else if ( strlen(search_li) > 0 ) {
			sprintf(sql, "SELECT * FROM %s "
					"WHERE USER_ID LIKE '%%%s%%' "
					"ORDER BY FAMILY DESC, ORDERBY ASC LIMIT %d, %d", 
					table_name, search_li, offset, show_max_line);

		// ´Ð³×ÀÓÀ¸·Î °Ë»ö
		} else if ( strlen(search_ln) > 0 ) {
            bool exist;
            std::map<std::string, std::string> user = database::user_info_by_nick_name((char*)search_ln, &exist);
            const char *user_id;
            if ( exist ) {
                user_id = user["USER_ID"].c_str();
                sprintf(sql, "SELECT * FROM %s "
                        "WHERE USER_ID LIKE '%%%s%%' "
                        "ORDER BY FAMILY DESC, ORDERBY ASC LIMIT %d, %d", 
                        table_name, user_id, offset, show_max_line);
            }

		} else {
			sprintf(sql, "SELECT * FROM %s ORDER BY FAMILY DESC, ORDERBY ASC LIMIT %d, %d", 
					table_name, offset, show_max_line);
		}

		printf(ESC_CLEAR);

		char *header = (char*)(node.child("header").child_value());
		print_file(header);

		print_board_header(table_name, title, page_count, page_no);
		
        printf("\033[4;1H");
		printf("%5s %s %s %s %s %s", 
				"¹øÈ£",
				centered("ÀÛ¼ºÀÚ", 12).c_str(), 
				//centered("ID", 10).c_str(), 
				centered("³¯Â¥", 5).c_str(), 
				centered("Á¶È¸", 5).c_str(), 
				centered("ÃßÃµ", 5).c_str(), 
				"Á¦¸ñ");
        printf("\033[5;1H");
		printf("%s\r\n", repeat("¦¡", 40).c_str());

		std::vector<std::map<std::string, std::string> > rows = database::fetch_rows(sql);

		if ( rows.size() == 0 ) {
			if ( strlen(search_lt) > 0 || strlen(search_li) > 0 || strlen(search_ln) ) {
				printf("%s\r\n", centered("°Ë»öµÈ ±ÛÀÌ ¾ø½À´Ï´Ù.", 80).c_str());
			} else {
				printf("%s\r\n", centered("µî·ÏµÈ ±ÛÀÌ ¾ø½À´Ï´Ù.", 80).c_str());
			}

		} else {
            printf("\033[6;1H");
			for(unsigned int i=0; i<rows.size(); i++) {
				std::map<std::string, std::string> row = rows.at(i);

				const char *no = row[std::string("NO")].c_str();
				const char *user_id = row[std::string("USER_ID")].c_str();
				bool exist;
				std::map<std::string, std::string> user = database::user_info((char*)user_id, &exist);
				const char *nick_name;
				if ( exist )
					nick_name = user["NICK_NAME"].c_str();
				else
					nick_name = "Å»ÅðÈ¸¿ø";
				//const char *date = row[std::string("DATE")].c_str();
				const char *date_time = row[std::string("DATE_TIME")].c_str();
				const char *hit = row[std::string("HIT")].c_str();
				const char *recommend = row[std::string("RECOMMEND")].c_str();

				int step = atoi(row[std::string("STEP")].c_str());

				std::ostringstream title;
#if 0
			   	for (unsigned int s=0; s < step; s++) {
					if ( s == step-1 ) {
						title << "[RE] ";
					} else {
						title << "  ";
					}
				}
#else

#if 0
			   	for (int s=0; s < step; s++) {
					if ( s == step-1 ) {
						title << "[RE] ";
					} else {
						title << "[RE]";
					}
				}
#endif

			   	for (int s=0; s < step; s++) {
					if ( s == step-1 ) {
						title << "¦± ";
					} else {
						title << " ";
					}
				}

#endif
				title << row[std::string("TITLE")];

				std::string title2 = string_truncate(title.str(), 40, "");
				
				printf("%5s ", no);
                if (exist == false) printf("[=1G[=7F");
				printf("%s ", centered(string_truncate(nick_name, 10, ""), 12).c_str());
				printf("[=1G[=15F");
				printf("%s %s %s ", 
					centered(date_time, 5).substr(5, 5).c_str(), 
					centered(hit, 5).c_str(), 
					centered(recommend, 5).c_str());

                bool color_changed = false;
				// ´ä±ÛÀ» °æ¿ì È¸»öÀ¸·Î Ç¥½Ã
				if(step > 0) {
                    printf("[=1G[=7F");
                    color_changed = true;
                } else {
                    // Æ¯Á¤ Á¶È¸¼ö°¡ ³Ñ¾î°¡¸é..
                    if(atoi(hit) > 60) {
                        // ºÓÀº»ö
                        printf("[=1G[=12F");
                        color_changed = true;
                    } else if(atoi(hit) > 40) {
                        // ÃÊ·Ï»ö
                        printf("[=1G[=10F");
                        color_changed = true;
                    } else if(atoi(hit) > 20) {
                        // ³ë¶õ»ö
                        printf("[=1G[=14F");
                        color_changed = true;
                    }
                }

				printf("%s", title2.c_str());
				// ´ä±ÛÀÌ³ª Á¶È¸¼ö°¡ 20ÀÌ ³ÑÀ»°æ¿ì Èò»öÀ¸·Î º¹±¸
				if(color_changed) printf("[=1G[=15F");
				printf("\r\n");
			}
		}
#endif
		printf("%s\r\n", repeat("¦¬", 40).c_str());

		char *footer = (char*)(node.child("footer").child_value());
		print_file(footer);

		prompt(cmd, true, true);
		
		std::vector<std::string> args = split_string(std::string(cmd), ' ');

		// ¿£ÅÍ¸¸ ÀÔ·ÂÀÌ µÇ¾úÀ»°æ¿ì ´ÙÀ½ ÆäÀÌÁö¸¦ º¸¿©ÁØ´Ù.
		if ( args.size() == 0 ) {
			offset += show_max_line;
			page_no += 1;

		} else {
			/* ÀÔ·ÂÀÌ ¼ýÀÚ */
			if ( is_number(args[0].c_str()) ) {
				/* ÀÔ·ÂÀÌ °Ô½Ã¹° ¹øÈ£ */
				bool is_dir;
				show_article(table_name, page_count, page_no, 
					title, atoi(args[0].c_str()), attachment, reply, &is_dir);

				if ( is_dir ) {
					offset = 0;
				}

			} else {
				// »óÀ§ ¸í·É
				if ( !strcasecmp(args[0].c_str(), "p") ) {
					break;
				}
				// °Ô½Ã¹°À» Ã³À½ºÎÅÍ Ãâ·Â
				if ( !strcasecmp(args[0].c_str(), "dir") ) {
					offset = 0;
				}
				
				// °Ô½Ã±ÛÀÇ ´ÙÀ½ ÆäÀÌÁö¸¦ º¸¿©ÁØ´Ù.
				if ( !strcasecmp(args[0].c_str(), "n") ) {
					offset += show_max_line;
					page_no += 1;

				}

				// °Ô½Ã±ÛÀÇ ÀÌÀü ÆäÀÌÁö¸¦ º¸¿©ÁØ´Ù.
				if ( !strcasecmp(args[0].c_str(), "b") ) {
					offset -= show_max_line;
					page_no -= 1;

					if ( page_no <= 1 ) {
						offset = 0;
						page_no = 1;
					}
				}

				// °Ô½Ã±Û °Ë»ö (Á¦¸ñ+³»¿ëÀ¸·Î)
				if ( !strcasecmp(args[0].c_str(), "lt") ) {
					if ( args.size() > 1 ) {
						sprintf(search_lt, "%s", args[1].c_str());

						sprintf(search_li, "%s", "");
						sprintf(search_ln, "%s", "");

					} else {
						// °Ô½Ã±Û °Ë»ö¾î ÀÔ·Â
						printf(ESC_HAN);
						printf("\r\n°Ë»ö¾î ÀÔ·Â : ");
						line_input(search_lt, 50);

						sprintf(search_li, "%s", "");
						sprintf(search_ln, "%s", "");
					}

					offset = 0;
				}

				// °Ô½Ã±Û °Ë»ö (¾ÆÀÌµð·Î)
				if ( !strcasecmp(args[0].c_str(), "li") ) {
					if ( args.size() > 1 ) {
						sprintf(search_li, "%s", args[1].c_str());

						sprintf(search_lt, "%s", "");
						sprintf(search_ln, "%s", "");

					} else {
						// °Ô½Ã±Û °Ë»ö¾î ÀÔ·Â
						printf(ESC_ENG);
						printf("\r\n¾ÆÀÌµð ÀÔ·Â : ");
						line_input(search_li, 50);

						sprintf(search_lt, "%s", "");
						sprintf(search_ln, "%s", "");
					}

					offset = 0;
				}
				
                // °Ô½Ã±Û °Ë»ö (´Ð³×ÀÓÀ¸·Î)
				if ( !strcasecmp(args[0].c_str(), "ln") ) {
					if ( args.size() > 1 ) {
						sprintf(search_ln, "%s", args[1].c_str());

						sprintf(search_li, "%s", "");
						sprintf(search_lt, "%s", "");

					} else {
						// °Ô½Ã±Û °Ë»ö¾î ÀÔ·Â
						printf(ESC_HAN);
						printf("\r\n´Ð³×ÀÓ ÀÔ·Â : ");
						line_input(search_ln, 50);

						sprintf(search_li, "%s", "");
						sprintf(search_lt, "%s", "");
					}

					offset = 0;
				}

				// °Ô½Ã±Û Áö¿ì±â
				if ( !strcasecmp(args[0].c_str(), "dd") ) {
					char input[256];

					int no1 = -1;
					int no2 = -1;

					if ( args.size() > 1 ) {
						sprintf(input, "%s", args[1].c_str());
						sscanf(input, "%d-%d", &no1, &no2);

					} else {
						// °Ô½Ã±Û NO ÀÔ·Â
						printf(ESC_ENG);
						printf("\r\n°Ô½Ã±Û ¹øÈ£ (¿¹: 1, 1-10): ");
						line_input(input, 30);
						sscanf(input, "%d-%d", &no1, &no2);
					}

					if ( no1 == -1 ) {
						printf("\r\nÃë¼Ò µÇ¾ú½À´Ï´Ù.");
						printf("\r\n[Enter] ¸¦ ´©¸£¼¼¿ä.");
						press_enter();

					} else {
						if ( no2 == -1 ) no2 = no1;

						if ( no2 < no1 ) {
							printf("\r\n°Ô½Ã¹° ¹øÈ£ ÁöÁ¤ÀÌ Àß¸ø µÇ¾ú½À´Ï´Ù.");
							printf("\r\n[Enter] ¸¦ ´©¸£¼¼¿ä.");
							press_enter();

						} else {
							bool ok = true;
							int i;
							for(i=no1; i<=no2; i++) {
								if ( !database::check_exist(table_name, i) ) {
									printf("\r\n%d¹ø °Ô½Ã¹°ÀÌ ¾ø½À´Ï´Ù.", i);
									printf("\r\n[Enter] ¸¦ ´©¸£¼¼¿ä.");
									press_enter();
									ok = false;
									break;
								}

								if ( !login_user_is_admin && !database::check_same_author(table_name, i, login_user_id) ) {
									printf("\r\n%d¹ø °Ô½Ã¹°Àº ÀÛ¼ºÀÚ¸¸ »èÁ¦ ÇÒ ¼ö ÀÖ½À´Ï´Ù.", i);
									printf("\r\n[Enter] ¸¦ ´©¸£¼¼¿ä.");
									press_enter();
									ok = false;
									break;
								}
							}

							if ( ok ) {
								printf(ESC_ENG);
								printf("\r\n%d°³ÀÇ °Ô½Ã¹°À» »èÁ¦ ÇÒ±î¿ä? (y/N) ", no2-no1+1);
								if ( yesno(NO) == YES ) {
									for(i=no1; i<=no2; i++) {
										delete_article(table_name, i);
									}
								}
							}
						}
					}
				}

				if ( !strcasecmp(args[0].c_str(), "w") ) {
					bool ok = true;
					if ( write_sysop_only == true )  {
						if ( !login_user_is_admin ) {
							printf("\r\n");
							printf("¿î¿µÀÚ¸¸ °Ô½Ã±ÛÀ» ÀÛ¼ºÇÒ ¼ö ÀÖ½À´Ï´Ù.\r\n");
							printf("[Enter]¸¦ ´©¸£¼¼¿ä.");
							press_enter();
							ok = false;
						}
					}

					if ( ok ) {
						int new_no = write_article(table_name);
						if ( new_no != -1 ) {
							database::attach_article(table_name, new_no, new_no);
						}
					}
				}

				if ( !strcasecmp(args[0].c_str(), "ed") ) {
					char buf[256];

					if ( args.size() > 1 ) {
						sprintf(buf, "%s", args[1].c_str());

					} else {
						printf(ESC_ENG);
						printf("\r\n°Ô½Ã±Û ¹øÈ£: ");
						line_input(buf, 10);

						if ( strlen(buf) == 0 ) {
							printf("\r\nÃë¼Ò µÇ¾ú½À´Ï´Ù.");
							printf("\r\n[Enter] ¸¦ ´©¸£¼¼¿ä.");
							press_enter();
						}
					}

					if ( is_number(buf) ) {
						if ( !database::check_exist(table_name, atoi(buf)) ) {
							printf("\r\nÇØ´ç ±ÛÀÌ ¾ø½À´Ï´Ù.");
							printf("\r\n[Enter] ¸¦ ´©¸£¼¼¿ä.");
							press_enter();

						} else {
							edit_article(table_name, atoi(buf));
						}
					}
				}
			}
		}
	}
}

void show_article(char *table_name, int board_page_count, int board_page_no, 
	char *title, int no, bool attachment, bool reply, bool *is_dir)
{
	char cmd[1024];
	char sql[1024];

	*is_dir = false;

	int offset = 0;
	int offset_attach = 0;

	// °Ô½Ã±Û¿¡ ÇØ´çÇÏ´Â Á¤º¸¸¦ ¾ò¾î¿È
	sprintf(sql, "SELECT * FROM %s WHERE NO=%d", table_name, no);
	std::vector<std::map<std::string, std::string> > rows = database::fetch_rows(sql);
	if ( rows.size() == 0 ) {
		printf("\r\nÇØ´ç °Ô½Ã±ÛÀÌ ¾ø½À´Ï´Ù.");
		printf("\r\n[Enter] ¸¦ ´©¸£¼¼¿ä.");
		press_enter();
		return;
	}

	// °Ô½Ã±Û ÀÛ¼ºÀÚ¿Í ´Ù¸¦°æ¿ì¸¸ Á¶È¸¼ö Áõ°¡
	if ( !database::check_same_author(table_name, no, login_user_id) ) {
		// °Ô½Ã±ÛÀÇ Á¶È¸¼ö Áõ°¡ ½ÃÅ´
		database::increase_hit(table_name, no);
	}

	// ÇöÀç ÆäÀÌÁö
	int page_no = 1;
	int show_max_attach = 2;

	while (1) {
		printf(ESC_CLEAR);

		//print_file("txt/article_header.txt");

		print_board_header(table_name, title, board_page_count, board_page_no);

        // °Ô½Ã±Û Á¤º¸¸¦ ¾ò¾î¿È
        sprintf(sql, "SELECT * FROM %s WHERE NO=%d", table_name, no);
        rows = database::fetch_rows(sql);
        std::map<std::string, std::string> row = rows.at(0);

        // °Ô½Ã±Û ³»¿ëÀ» ÅØ½ºÆ®·Î º¯È¯
        std::string txt = std::string(row[std::string("CONTENT")].c_str());
        txt = trim(txt);
        std::vector<std::string> lines = split_string_with_width(txt, '\n', 80);


		// ÀüÃ¼ ÆäÀÌÁö ¼ö
		int page_count = lines.size() / show_max_line;
		if ( lines.size() % show_max_line >= 1 ) {
			page_count += 1;
		}

		// °Ô½Ã±Û »ç¿ëÀÚ Á¤º¸ ¾ò¾î¿È
		bool exist;
		std::map<std::string, std::string> user = database::user_info((char*)row["USER_ID"].c_str(), &exist);

		std::string title = row[std::string("TITLE")];
		title = string_truncate(title, 60, "");
        printf("\033[4;1H");
		printf(" Á¦  ¸ñ: %-60s\r\n", title.c_str());

		if ( lines.size() <= offset ) {
			offset -= show_max_line;
			page_no -= 1;
		}

		printf("\033[1A\033[70C(%3d/%3d)\r\n", page_no, page_count);

		char author[1024];
		char date_time[1024];
		char hit[1024];
		char recommend[1024];

        const char *user_id = row[std::string("USER_ID")].c_str();
        const char *nick_name;
        if ( exist ) {
            nick_name = user["NICK_NAME"].c_str();
        } else {
            nick_name = "Å»ÅðÈ¸¿ø";
            user_id = "*****";
        }

		sprintf(author, " ÀÛ¼ºÀÚ: %s (%s)", nick_name, user_id);
		//sprintf(date, "%s %s", row[std::string("DATE")].c_str(), row[std::string("TIME")].c_str());
		sprintf(date_time, "%s", row[std::string("DATE_TIME")].c_str());
		sprintf(hit, "Á¶È¸:%3s", row[std::string("HIT")].c_str());
		sprintf(recommend, "ÃßÃµ:%3s", row[std::string("RECOMMEND")].c_str());
		printf("%-40s%-19s%10s%10s", author, date_time, hit, recommend);

		// Ã·ºÎ ÆÄÀÏÀÌ ÀÖÀ¸¸é Ãâ·Â
		sprintf(sql, "SELECT * FROM attachment WHERE FAMILY_TABLE='%s' AND FAMILY_ID='%d';", table_name, no);
		std::vector<std::map<std::string, std::string> > attach_rows = database::fetch_rows(sql);
		if ( attach_rows.size() > 0 ) {
			printf("\r\n");
			printf(" Ã·  ºÎ: ");
		
			if ( attach_rows.size() <= offset_attach ) {
				offset_attach -= show_max_attach;
			}

			if ( offset_attach < 0 ) offset_attach = 0;

			unsigned int count=0;
			for(unsigned int y=offset_attach; y<attach_rows.size(); y++) {
				std::map<std::string, std::string> attach_row = attach_rows.at(y);
				char tmp_filename[1024];
				sprintf(tmp_filename, "%s/file/%s", getenv("HANULSO"), attach_row["FILENAME"].c_str());

				printf("\r\033[9C");
				printf("[%d] %s (%s) (´Ù¿î: %d)", y+1, 
						string_truncate_center(attach_row["ORIGINAL_FILENAME"], 43, "...").c_str(),
						human_file_size(file_size(tmp_filename)).c_str(), 
						atoi(attach_row["DOWNLOAD"].c_str()));

				if (count == show_max_attach-1) break;
				if ( attach_rows.size()-1 != y ) printf("\r\n");

				count++;
			}

			if ( show_max_attach < attach_rows.size() ) {
				printf("\r\n\033[9C");
				char msg[128];
				sprintf(msg, "ÃÑ %d°³ÀÇ Ã·ºÎ ÆÄÀÏÀÌ ÀÖ½À´Ï´Ù.", attach_rows.size());
				if ( offset_attach == 0 ) {
					printf("%-50s%20s", msg, "´ÙÀ½(NA)");
				} else if ( offset_attach + count == attach_rows.size() ) {
					printf("%-50s%20s", msg, "ÀÌÀü(PA)");
				} else {
					printf("%-50s%20s", msg, "´ÙÀ½(NA) ÀÌÀü(PA)");
				}
			}
		}

		printf("\r\n%s\r\n", repeat("¦¡", 40).c_str());

		// °Ô½Ã±Û Ãâ·Â
		unsigned int l=0;
		for(unsigned int i=offset; i<lines.size(); i++) {
			if (l < show_max_line )
				printf("%s\r\n", lines[i].c_str());
			l++;
		}

		printf("%s\r\n", repeat("¦¬", 40).c_str());

		print_file("txt/article_footer.txt");

		prompt(cmd, true, true);

		std::vector<std::string> args = split_string(std::string(cmd), ' ');

		// ¿£ÅÍ¸¸ ÀÔ·ÂÀÌ µÇ¾úÀ»°æ¿ì ±ÛÀÇ ´ÙÀ½ ÆäÀÌÁö¸¦ º¸¿©ÁØ´Ù.
		if ( args.size() == 0 ) {
			offset += show_max_line;
			page_no += 1;

		} else {
			// »óÀ§ ¸í·É
			if ( !strcasecmp(args[0].c_str(), "p") ) {
				return;
			}

            // Ã·ºÎ ÆÄÀÏ ¸ñ·Ï
			if ( !strcasecmp(args[0].c_str(), "la") ) {
                if ( attach_rows.size() > 0 ) {
                    printf("\r\n");
                    printf("Ã·ºÎ ÆÄÀÏ ¸ñ·Ï:\r\n");
		
                    for(unsigned int y=0; y<attach_rows.size(); y++) {
                        std::map<std::string, std::string> attach_row = attach_rows.at(y);
                        char tmp_filename[1024];
                        sprintf(tmp_filename, "%s/file/%s", getenv("HANULSO"), attach_row["FILENAME"].c_str());

                        printf("[%2d] %s (%s) (´Ù¿î: %d)", y+1, 
                                string_truncate(attach_row["ORIGINAL_FILENAME"], 43, "...").c_str(),
                                human_file_size(file_size(tmp_filename)).c_str(), 
                                atoi(attach_row["DOWNLOAD"].c_str()));

                        if ( attach_rows.size()-1 != y ) printf("\r\n");
                    }
                } else {
                    printf("\r\nÃ·ºÎ ÆÄÀÏÀÌ ¾ø½À´Ï´Ù.");
                }

                printf("\r\n[Enter] ¸¦ ´©¸£¼¼¿ä.");
                press_enter();
            }

			// ÇØ´ç °Ô½Ã±ÛÀÇ ÀÛ¼ºÀÚ ·¹º§ º¯°æ
			if ( !strcasecmp(args[0].c_str(), "lv") ) {
				// ¿î¿µÀÚÀÏ °æ¿ì¸¸ °¡´É
				if ( login_user_is_admin ) {
					int author_level = atoi(user["LEVEL"].c_str());

					printf("\r\n'%s(%s)' ´ÔÀÇ µî±ÞÀº '%s' ÀÔ´Ï´Ù.", 
						user["NICK_NAME"].c_str(), row[std::string("USER_ID")].c_str(),
						get_level_name(author_level).c_str());
					printf("\r\n»õ·Î¿î µî±ÞÀ» ¼±ÅÃÇÏ¼¼¿ä.");
					printf("\r\n");
					for(unsigned int i=0; i<level_nums.size(); i++) {
						printf("[%d]%s ", level_nums[i], level_names[i].c_str());
					}

					printf("\r\n¼±ÅÃ : ");
					
					char buf[20];
					line_input(buf, 2);

					if ( strlen(buf) == 0 ) {
						printf("\r\nÃë¼Ò µÇ¾ú½À´Ï´Ù.");
						printf("\r\n[Enter] ¸¦ ´©¸£¼¼¿ä.");
						press_enter();
					} else {
						if ( is_number(buf) ) {
							int new_level = atoi(buf);
							if (check_exist_level(new_level)) {
								std::string user_id = row[std::string("USER_ID")];
								if ( database::set_user_level((char*)user_id.c_str(), new_level) ) {
									printf("\r\nµî±ÞÀÌ º¯°æµÇ¾ú½À´Ï´Ù.");
									printf("\r\n[Enter] ¸¦ ´©¸£¼¼¿ä.");
									press_enter();
								}
							} else {
								printf("\r\nµî±ÞÀÌ Á¸ÀçÇÏÁö ¾Ê½À´Ï´Ù.");
								printf("\r\n[Enter] ¸¦ ´©¸£¼¼¿ä.");
								press_enter();
							}
						} else { 
							printf("\r\nÃë¼Ò µÇ¾ú½À´Ï´Ù.");
							printf("\r\n[Enter] ¸¦ ´©¸£¼¼¿ä.");
							press_enter();
						}
					}

				} else {
					printf("\r\n¿î¿µÀÚ¸¸ Á¢±Ù °¡´ÉÇÕ´Ï´Ù.");
					printf("\r\n[Enter] ¸¦ ´©¸£¼¼¿ä.");
					press_enter();
				}
			}

			// Ã·ºÎ ÆÄÀÏ Ãß°¡
			if ( !strcasecmp(args[0].c_str(), "up") ) {
				if ( attachment == false ) {
					printf("\r\nÇØ´ç °Ô½ÃÆÇÀº Ã·ºÎ ÆÄÀÏ ¾÷·Îµå¸¦ Çã¿ëÇÏÁö ¾Ê½À´Ï´Ù.");
					printf("\r\n[Enter] ¸¦ ´©¸£¼¼¿ä.");
					press_enter();

				} else {
					if ( database::check_same_author(table_name, no, login_user_id) ) {
						printf(ESC_ENG);
						printf("\r\nÆÄÀÏÀ» Ã·ºÎ ÇÒ±î¿ä? (Y/n) ");
						if ( yesno(YES) == YES ) {
                            char buf2 [10];
                            int protocol = 3;
                            bool cancel = false;

                            printf(ESC_ENG);
                            printf("\r\n¼Û½Å ÇÁ·ÎÅäÄÝ(1:Xmodem, 2:Ymodem, 3:Zmodem): ");
                            line_input(buf2, 4);
                            if ( strlen(buf2) == 0 ) {
                                cancel = true;
                            } else {
                                unsigned int i = atoi(buf2);
                                if ( i < 1 && i > 3) {
                                    cancel = true;
                                } else {
                                    protocol = i;
                                }
                            }

                            if(cancel == false) {
                                char *tmp_filename;
                                char *filename;
                                int size;

                                ioctl(0, TCSETAF, &sys_term);

                                if ( file_upload(protocol, &tmp_filename, &filename, &size) ) {
                                    database::add_attachment(table_name, no, 
                                        login_user_id,
                                        (char*)(date_now_string(false).c_str()), 
                                        (char*)(time_now_string().c_str()),
                                        tmp_filename, filename);

                                    printf("\r\nÆÄÀÏ Àü¼ÛÀ» ¼º°øÇÏ¿´½À´Ï´Ù.");
                                    printf("\r\n[Enter] ¸¦ ´©¸£¼¼¿ä.");
                                    press_enter();
                                }

                                ioctl(0, TCSETAF, &curr_term);

                                if ( size > 0 ) {
                                    free(tmp_filename);
                                    free(filename);
                                }
                            }
						}
					} else {
						printf("\r\n°Ô½Ã±Û ÀÛ¼ºÀÚ°¡ ´Ù¸¨´Ï´Ù.");
						printf("\r\n[Enter] ¸¦ ´©¸£¼¼¿ä.");
						press_enter();
					}
				}	
			}

			// Ã·ºÎ ÆÄÀÏ ´Ù¿î·Îµå
			if ( !strcasecmp(args[0].c_str(), "dn") ) {
				char buf[10];
                char buf2 [10];
                int protocol = 3;

				if ( attach_rows.size() == 0 ) {
					printf("\r\nÃ·ºÎ ÆÄÀÏÀÌ ¾ø½À´Ï´Ù.");
					printf("\r\n[Enter] ¸¦ ´©¸£¼¼¿ä.");
					press_enter();

				} else {
                    bool cancel = false;

					if ( args.size() > 1 ) {
						sprintf(buf, "%s", args[1].c_str());

					} else {
						printf(ESC_ENG);
						printf("\r\nÃ·ºÎ ¹øÈ£: ");
						line_input(buf, 4);
                        if ( strlen(buf) == 0 ) {
                            cancel = true;
                        }

						printf(ESC_ENG);
						printf("\r\n¼ö½Å ÇÁ·ÎÅäÄÝ(1:Xmodem, 2:Ymodem, 3:Zmodem): ");
						line_input(buf2, 4);
                        if ( strlen(buf2) == 0 ) {
                            cancel = true;
                        } else {
                            unsigned int i = atoi(buf2);
                            if ( i < 1 && i > 3) {
                                cancel = true;
                            } else {
                                protocol = i;
                            }
                        }
					}

                    if(cancel == true) {
						printf("\r\nÃë¼Ò µÇ¾ú½À´Ï´Ù.");
						printf("\r\n[Enter] ¸¦ ´©¸£¼¼¿ä.");
						press_enter();

					} else {
                        if ( is_number(buf) ) {
                            unsigned int i = (atoi(buf)-1);

                            if ( i < 0 || i > attach_rows.size()-1 ) {
                                printf("\r\nÀß¸øµÈ Ã·ºÎ ÆÄÀÏ ¹øÈ£ ÀÔ´Ï´Ù.");
                                printf("\r\n[Enter] ¸¦ ´©¸£¼¼¿ä.");
                                press_enter();

                            } else {
                                std::map<std::string, std::string> attach_row = attach_rows.at(i);

                                char *tmp_filename = (char*)attach_row["FILENAME"].c_str();
                                char *filename = (char*)attach_row["ORIGINAL_FILENAME"].c_str();

                                char path[1024];
                                sprintf(path, "%s/file/%s", getenv("HANULSO"), tmp_filename);
                                if ( file_exists(path) ) {
                                    ioctl(0, TCSETAF, &sys_term);

                                    if ( file_download(protocol, tmp_filename, filename) ) {
                                        // ´Ù¿î·Îµå ¼ö Áõ°¡ ½ÃÅ´
                                        int no = atoi(attach_row["NO"].c_str());
                                        database::increase_download(no);

                                        printf("\r\nÆÄÀÏ ¼ö½ÅÀ» ¼º°øÇÏ¿´½À´Ï´Ù.");
                                        printf("\r\n[Enter] ¸¦ ´©¸£¼¼¿ä.");
                                        press_enter();

                                    } else {
                                        printf("\r\nÆÄÀÏ ¼ö½ÅÀ» ½ÇÆÐÇÏ¿´½À´Ï´Ù.");
                                        printf("\r\n[Enter] ¸¦ ´©¸£¼¼¿ä.");
                                        press_enter();
                                    }

                                    ioctl(0, TCSETAF, &curr_term);

                                } else {
                                    printf("\r\nÃ·ºÎ ÆÄÀÏÀÌ Á¸ÀçÇÏÁö ¾Ê½À´Ï´Ù.");
                                    printf("\r\n[Enter] ¸¦ ´©¸£¼¼¿ä.");
                                    press_enter();
                                }
                            }
                        }
                    }
				}
			}

			// °Ô½Ã±Û ÆíÁý
			if ( !strcasecmp(args[0].c_str(), "ed") ) {
				edit_article(table_name, no);
			}

			// °Ô½Ã±Û »èÁ¦
			if ( !strcasecmp(args[0].c_str(), "dd") ) {
				if ( !database::check_exist(table_name, no) ) {
					printf("\r\nÇØ´ç ±ÛÀÌ ¾ø½À´Ï´Ù.");
					printf("\r\n[Enter] ¸¦ ´©¸£¼¼¿ä.");
					press_enter();

				} else {
					if ( login_user_is_admin || database::check_same_author(table_name, no, login_user_id) ) {
						char buf[20];

						printf("\r\n[1]Ã·ºÎ»èÁ¦ [2]°Ô½Ã±Û»èÁ¦ : ");
						line_input(buf, 1);

						if ( !strcasecmp(buf, "1") ) {
							printf("\r\nÃ·ºÎ ¹øÈ£: ");
							line_input(buf, 10);

							if ( is_number(buf) ) {
								int attach_no = atoi(buf);
								int count = database::attachment_count(table_name, no);
								if ( attach_no > count || attach_no <= 0 ) {
									printf("\r\nÇØ´ç Ã·ºÎ ÆÄÀÏÀÌ ¾ø½À´Ï´Ù.");
									printf("\r\n[Enter] ¸¦ ´©¸£¼¼¿ä.");
									press_enter();
								} else {
									delete_attachment(table_name, no, attach_no-1);
								}
							}

						} else if ( !strcasecmp(buf, "2") ) {
							delete_article(table_name, no);
							break;
						} 

					} else {
						printf("\r\nÀÛ¼ºÀÚ¸¸ »èÁ¦ ÇÒ ¼ö ÀÖ½À´Ï´Ù.");
						printf("\r\n[Enter] ¸¦ ´©¸£¼¼¿ä.");
						press_enter();
					}
				}
			}
			
			// ´ÙÀ½ Ã·ºÎ ÆÄÀÏ ¸ñ·Ï º¸±â
			if ( !strcasecmp(args[0].c_str(), "na") ) {
				offset_attach += show_max_attach;
			}

			// ÀÌÀü Ã·ºÎ ÆÄÀÏ ¸ñ·Ï º¸±â
			if ( !strcasecmp(args[0].c_str(), "pa") ) {
				offset_attach -= show_max_attach;
			}

			
			// º»¹®ÀÇ ´ÙÀ½ ÆäÀÌÁö¸¦ º¸¿©ÁØ´Ù.
			if ( !strcasecmp(args[0].c_str(), "n") ) {
				offset += show_max_line;
				page_no += 1;
			}

			// º»¹®ÀÇ ÀÌÀü ÆäÀÌÁö¸¦ º¸¿©ÁØ´Ù.
			if ( !strcasecmp(args[0].c_str(), "b") ) {
				offset -= show_max_line;
				page_no -= 1;

				if ( page_no <= 1 ) {
					offset = 0;
					page_no = 1;
				}
			}
				
			// °Ô½Ã¹°À» ´Ù½Ã Ã³À½ºÎÅÍ Ãâ·Â
			if ( !strcasecmp(args[0].c_str(), "dir") ||
				!strcasecmp(args[0].c_str(), "ls") ) {
				*is_dir = true;
				return;
			}

			// ÃßÃµ ÇÏ±â
			if ( !strcasecmp(args[0].c_str(), "ok") ) {
				if ( database::check_same_author(table_name, no, login_user_id) ) {
					printf("\r\nÀÛ¼ºÀÚ´Â °Ô½Ã±ÛÀ» ÃßÃµÇÒ ¼ö ¾ø½À´Ï´Ù.");
					printf("\r\n[Enter] ¸¦ ´©¸£¼¼¿ä.");
					press_enter();

				} else {
					// °Ô½Ã±ÛÀÇ ÃßÃµ¼ö Áõ°¡ ½ÃÅ´
					if ( database::increase_recommend(table_name, no) ) {
						printf("\r\nÇØ´ç °Ô½Ã±ÛÀ» ÃßÃµ ÇÏ¿´½À´Ï´Ù.");
						printf("\r\n[Enter] ¸¦ ´©¸£¼¼¿ä.");
						press_enter();
					}
				}
			}

			// ´ä±Û ´Þ±â
			if ( !strcasecmp(args[0].c_str(), "re") ) {
				bool ok = true;
				if ( reply == false )  {
					printf("\r\n");
					printf("´ä±Û ¾²±â°¡ ±ÝÁöµÇ¾îÀÖ´Â °Ô½ÃÆÇÀÔ´Ï´Ù.\r\n");
					printf("[Enter]¸¦ ´©¸£¼¼¿ä.");
					press_enter();
					ok = false;
				}

				if ( ok ) {
					int parent_no = no;
					int new_no = write_article(table_name);
					if ( new_no != -1 ) {
						database::attach_article(table_name, new_no, parent_no);
					}
				}
				break;
			}

			// °Ô½Ã±Û ³»¿ë Ãâ·Â
			if ( !strcasecmp(args[0].c_str(), "pr") ) {
				printf("\r\n°¥¹«¸®¸¦ ÁØºñÇÏ½Ã°í [Enter]¸¦ ´©¸£½Ê½Ã¿À.");
				press_enter();

				printf("\r\n\r\n");
				printf(" Á¦  ¸ñ: %-70s\r\n", title.c_str());

				char author[1024];
				char date_time[1024];
				sprintf(author, " ÀÛ¼ºÀÚ: %s (%s)", row[std::string("USER_ID")].c_str(), user["NICK_NAME"].c_str());
				sprintf(date_time, "%s", row[std::string("DATE_TIME")].c_str());
				printf("%-54s%-24s", author, date_time);
				printf("\r\n%s\r\n", repeat("¦¡", 40).c_str());

				for(unsigned int i=0; i<lines.size(); i++) {
					printf("%s\r\n", lines[i].c_str());
				}

				printf("\r\n");
				printf("°Ô½Ã±ÛÀÇ ¸¶Áö¸· ÀÔ´Ï´Ù. [Enter]¸¦ ´©¸£¼¼¿ä.");
				press_enter();
			}
		}
	}
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

/* ÇÁ·Î±×·¥ Á¾·á ·çÆ¾ */
int host_close (void)  
{
	database::set_lastlogin_datetime(login_user_id);

	database::close();

    ioctl(0, TCSETAF, &sys_term);

	// ÇöÀç Á¢¼ÓÀÚ Á¤º¸ ÆÄÀÏ »èÁ¦
	char buf[1024];
    sprintf(buf,"tmp/%s.tty", tty);
	unlink(buf);

	// ÇöÀç Á¢¼ÓÀÚ¿¡ ÀÇÇØ ÀÓ½Ã·Î »ý¼º(º¹»ç)µÈ ÆÄÀÏÀÌ ÀÖ´Ù¸é »èÁ¦.. 
	// ÆÄÀÏ ´Ù¿î·Îµå½Ã ÀÓ½Ã ÆÄÀÏ º¹»çÁß Åë½Å ²÷°Ü ¾²·¹±â·Î ³²¾Æ ÀÖ´Â ÆÄÀÏµé..
	del_user_tmpfiles();

	exit(1);
}

bool jump_go(std::string go)
{
	bool proceed = false;
	for(unsigned int i=0; i<go_menus.size(); i++) {
		std::map<std::string, pugi::xml_node> row = go_menus.at(i);
		if ( row.count(go) ) {
			pugi::xml_node node = row[go];

			std::string type = node.attribute("type").value();
			if ( !strcasecmp( type.c_str(), "menu" ) ) {
				show_menu(node);
				proceed = true;

			} else if ( !strcasecmp( type.c_str(), "board" ) ) {
				show_board(node);
				proceed = true;

			} else if ( !strcasecmp( type.c_str(), "chatt" ) ) {
				bool goto_top;
				show_chatt_rooms(node, &goto_top);
				if ( goto_top ) {
					jump_go("top");
				}
				proceed = true;

			} else if ( !strcasecmp( type.c_str(), "weather" ) ) {
#if 0
				bool goto_top;
				show_weather_board(node, &goto_top);
				if ( goto_top ) {
					jump_go("top");
				}
#else
				char buf[1024];
                sprintf(buf, "%s/bin/weather \"%s\"", getenv("HANULSO"), host_name);
				system(buf);
#endif
				proceed = true;

			} else if ( !strcasecmp( type.c_str(), "news" ) ) {
				bool goto_top;
				show_newspaper_board(node, &goto_top);
				if ( goto_top ) {
					jump_go("top");
				}
				proceed = true;
			}
		}
	}

	if ( !proceed ) {
		printf("\r\nÇØ´ç ¸Þ´º°¡ Á¤ÀÇ µÇ¾î ÀÖÁö ¾Ê½À´Ï´Ù.");
		printf("\r\n[Enter] ¸¦ ´©¸£¼¼¿ä.");
		press_enter();

		return false;
	}

	return true;
}


void prompt(char *cmd, bool enable_write, bool enable_del)
{
	printf(ESC_ENG);
	//printf("ÁÖ¿ä¸í·É(W,P,DD) ÀÌµ¿(GO,¹øÈ£) ÃÊ±âÈ­¸é(T) Á¾·á(X)\r\n");
	printf("ÁÖ¿ä¸í·É(");
	if ( enable_write ) {
		printf("W,");
	}
	printf("P");
	if ( enable_del ) {
		printf(",DD");
	}
	printf(") ÀÌµ¿(GO,¹øÈ£) ÃÊ±âÈ­¸é(T) Á¾·á(X)\r\n");
	printf("¼±ÅÃ(µµ¿ò¸»[H]) >> ");
	line_input(cmd, 30);
		
	std::vector<std::string> args = split_string(std::string(cmd), ' ');
	if ( args.size() == 0 ) return;

	/* ÀÔ·ÂÀÌ ¸í·É ÄÚµå */
	if( !is_number(cmd) ) {
		// Á¾·á ¸í·É
		if ( !strcasecmp(args[0].c_str(), "x") ||
			!strcasecmp(args[0].c_str(), "bye") ) {
			printf("\033&6@\r\nÁ¢¼ÓÀ» ²÷°Ú½À´Ï±î? (y/N) ");
			if ( yesno(NO) == YES ) {
				printf("\r\n");
				host_close();
			}

		// µµ¿ò¸» ¸í·É
		} else if ( !strcasecmp(args[0].c_str(), "h") ) {
			print_file("txt/help.txt");

			printf("\r\n [Enter] ¸¦ ´©¸£¼¼¿ä.");
			press_enter();

		// ¸Þ´º °¡±â (go), TOP À¸·Î ÀÌµ¿(t)
		} else if ( !strcasecmp(args[0].c_str(), "go") || !strcasecmp(args[0].c_str(), "t") ) {
			std::string go;

			if ( !strcasecmp(args[0].c_str(), "go") ) {
				if (args.size() == 1) {
					printf("\r\n¸Þ´º: ");
					// ¸Þ´º ¸ñ·Ï Ãâ·Â
					for(unsigned int i=0; i<go_menus.size(); i++) {
						std::map<std::string, pugi::xml_node> row = go_menus.at(i);
						for(std::map<std::string, pugi::xml_node>::iterator iter = row.begin(); 
								iter != row.end(); ++iter)
							printf("%s ", iter->first.c_str());
							//iter->second; // pugi::xml_node
					}
					printf("\r\n[Enter] ¸¦ ´©¸£¼¼¿ä.");
					press_enter();

				} else {
					if (args.size() == 2) {
						go = args[1];
					}
				}
			} else if ( !strcasecmp(args[0].c_str(), "t") ) {
				go = "top";
			}

			if ( go.length() > 0 ) {
				jump_go(go);
			}

		// °ü¸®ÀÚ ÇÁ·Î±×·¥
		} else if ( !strcasecmp(args[0].c_str(), "sysop") ) {
			if ( login_user_is_admin ) {
				char buf[1024];
				sprintf(buf, "%s/bin/sysop \"%s\"", getenv("HANULSO"), tty);
				system(buf);

			} else {
				printf("\r\n¿î¿µÀÚ¸¸ Á¢±Ù °¡´ÉÇÕ´Ï´Ù.");
				printf("\r\n[Enter] ¸¦ ´©¸£¼¼¿ä.");
				press_enter();
			}

		// °ü¸®ÀÚ ÇÁ·Î±×·¥
		} else if ( !strcasecmp(args[0].c_str(), "xx") ) {
			char buf[1024];
			sprintf(buf, "%s/bin/memo \"%s\"", getenv("HANULSO"), tty);
			system(buf);

		// Á¢¼Ó È¸¿ø ¸ñ·Ï
		} else if ( !strcasecmp(args[0].c_str(), "us") ) {
			printf("\r\n");

			char buf[1024];
			sprintf(buf, "%s/tmp/*.tty", getenv("HANULSO"), tty);
			std::vector<std::string> files = find_files_time_sorted(buf);

			printf("Á¢¼ÓÁßÀÎ È¸¿ø ¸ñ·Ï ÀÔ´Ï´Ù.");

			for(int i=0; i<files.size(); i++) {
				std::string user_id = trim(read_file(files[i].c_str()));
				if (user_id.length() > 0) {
					bool exist;
					std::map<std::string, std::string> user = database::user_info((char*)user_id.c_str(), &exist);

					char buf[1024];
					sprintf(buf, "%s(%s)", user["NICK_NAME"].c_str(), user["USER_ID"].c_str());
				
					std::string node = split_string(split_file_name(files[i]), '.')[0];

					char buf2[1024];
					sprintf(buf2, "Á¢¼Ó³ëµå: pts/%s", node.c_str());

					long login_time = file_ms_mtime(files[i]);
					long now_time = ms_time_now();

					printf("\r\n%-25s %-20s »ç¿ë½Ã°£: %s", 
						buf, buf2, ms_time_to_string(now_time-login_time).c_str());
				}
			}

			printf("\r\n[Enter] ¸¦ ´©¸£¼¼¿ä.");
			press_enter();

		// ½Ã½ºÅÛ Á¤º¸
		} else if ( !strcasecmp(args[0].c_str(), "sys") ) {
			printf("\r\n");
#if 0
			ioctl(0, TCSETAF, &sys_term);
			system("lsb_release -d");
			printf("Database:\t"); fflush(stdout);
			system("mysql --version|awk '{ print $5 }'|awk -F\\, '{ print $1 }'");
			printf("Freespace:\t"); fflush(stdout);
			system("df -h --total | grep total | tr -s ' ' | cut -f 4 -d ' '");
			//printf("Freemem:\t"); fflush(stdout);
			//system("free -m | tr -s ' ' | cut -f 4 -d ' ' | sed -n 2p");
			system("sed '1q;d' < /proc/meminfo");
			system("sed '2q;d' < /proc/meminfo");
			ioctl(0, TCSETAF, &curr_term);
#else
			printf("½Ã½ºÅÛ Á¤º¸¸¦ Ç¥½ÃÇÕ´Ï´Ù.");

			bool success;
			std::vector<std::string> lines = exec_command("awk '{print $1}' /proc/uptime", &success);
			long sec = atol(lines[0].c_str());

			printf("\r\n%-15s: %dÀÏ %02d:%02d:%02d °æ°ú", "°¡µ¿½Ã°£", sec/86400, sec%86400/3600, (sec%3600)/60, sec%60);

			std::string dist_name = trim(read_file("/etc/system-release"));
			printf("\r\n%-15s: %s", "¹èÆ÷ÆÇ", dist_name.c_str());

			std::string txt = trim(read_file("/proc/version"));
			std::vector<std::string> tokens = split_string(txt, ' ');
			printf("\r\n%-15s: %s %s %s", "Ä¿³Î", tokens[0].c_str(), tokens[1].c_str(), tokens[2].c_str());

			lines = exec_command("mysql --version", &success);
			tokens = split_string(lines[0], ',');
			printf("\r\n%-15s: %s", "µ¥ÀÌÅ¸º£ÀÌ½º", tokens[0].c_str());
			
			lines = exec_command("df -h --total | tail -n 1", &success);
			tokens = split_string(lines[0], ' ');
			char tmp[10];
			char tmp2[10];
			char tmp3[10];
			char tmp4[10];
			char tmp5[10];
			sscanf(lines[0].c_str(), "%s %s %s %s %s", tmp, tmp2, tmp3, tmp4, tmp5);
			printf("\r\n%-15s: %s", "ÀüÃ¼ÇÏµå¿ë·®", tmp2);
			printf("\r\n%-15s: %s", "³²ÀºÇÏµå¿ë·®", tmp4);
			//txt = trim(read_file("/sys/block/sda/size"));
			//printf("\r\n%-15s: %d GB", "ÇÏµå¿ë·®", atoi(txt.c_str())/1024/1024);
			
			txt = trim(read_file("/proc/meminfo"));
			lines = split_string(txt, '\n');
			for(int i=0; i<lines.size(); i++) {
				tokens = split_string(lines[i], ':');
				if (trim(tokens[0]) == "MemTotal") {
					printf("\r\n%-15s: %dM (%dG)", 
						"ÀüÃ¼¸Þ¸ð¸®¿ë·®",
						atoi(tokens[1].c_str())/1024,
						atoi(tokens[1].c_str())/1024/1024);
				}
				if (trim(tokens[0]) == "MemFree") {
					printf("\r\n%-15s: %dM", "³²Àº¸Þ¸ð¸®¿ë·®", atoi(tokens[1].c_str())/1024);
				}
			}

#endif
			printf("\r\n[Enter] ¸¦ ´©¸£¼¼¿ä.");
			press_enter();

		// È¸¿øÁ¤º¸ º¸±â
		} else if ( !strcasecmp(args[0].c_str(), "pf") ) {
			char user_id[50];

			if ( args.size() > 1 ) {
				sprintf(user_id, "%s", args[1].c_str());

			} else {
				sprintf(user_id, "%s", login_user_id);
			}

			if ( strlen(user_id) > 0 ) {
				if ( database::exist_user_id(user_id) ) {
					print_user_info(user_id);
				} else {
					printf("\r\nÈ¸¿ø Á¤º¸°¡ ¾ø½À´Ï´Ù.");
				}
				printf("\r\n[Enter] ¸¦ ´©¸£¼¼¿ä.");
				press_enter();
			}

		} else if ( !strcasecmp(args[0].c_str(), "pe") ) {
			edit_profile(login_user_id);

		// ¹ÙÀÌ¿À ¸®µë
		} else if ( !strcasecmp(args[0].c_str(), "bio") ) {
			printf("\r\n\r\n");
			print_biorhythm(login_user_id);
			printf("\r\n [Enter] ¸¦ ´©¸£¼¼¿ä.");
			press_enter();

		// ¿À´ÃÀÇ ¿î¼¼
		} else if ( !strcasecmp(args[0].c_str(), "luck") ) {
			printf("\r\n");

			printf("Àá½Ã¸¸ ±â´Ù¸®¼¼¿ä. ¿î¼¼ Á¤º¸¸¦ °¡Á®¿À´Â ÁßÀÔ´Ï´Ù...");
			fflush(stdout);

			print_luck(login_user_id);

			printf("\r\n [Enter] ¸¦ ´©¸£¼¼¿ä.");
			press_enter();

		// ¼º°Ý °Ë»ç ÇÁ·Î±×·¥
		} else if ( !strcasecmp(args[0].c_str(), "mbti") ) {
			run_mbti_gs();
			printf("\r\n[Enter] ¸¦ ´©¸£¼¼¿ä.");
			press_enter();
		}
	}
}

bool delete_article(char *table_name, int no)
{
	// Ã·ºÎ ÆÄÀÏ »èÁ¦
	delete_attachment_all(table_name, no);

	// ±Û »èÁ¦
	database::delete_article(table_name, no);

	return true;
}

bool delete_attachment_all(char *table_name, int no)
{
	return delete_attachment(table_name, no, -1);
}

bool delete_attachment(char *table_name, int no, int attach_no)
{
	char buf[9072];

	// Ã·ºÎ ÆÄÀÏÀÌ ÀÖ´ÂÁö º¸°í ÀÖÀ¸¸é »èÁ¦
	sprintf(buf, "SELECT * FROM attachment WHERE FAMILY_TABLE='%s' AND FAMILY_ID='%d';", table_name, no);
	std::vector<std::map<std::string, std::string> > attach_rows = database::fetch_rows(buf);

	if ( attach_rows.size() > 0 ) {
		// ÀüÃ¼ Ã·ºÎ »èÁ¦
		if ( attach_no == -1 ) {
			for(unsigned int x=0; x < attach_rows.size(); x++) {
				std::map<std::string, std::string> attach_row = attach_rows.at(x);

				char tmp_filename[1024];
				sprintf(tmp_filename, "%s/file/%s", getenv("HANULSO"), attach_row["FILENAME"].c_str());
				unlink(tmp_filename);
			}

		// ÁöÁ¤ Ã·ºÎ ¹øÈ£¸¸ »èÁ¦
		} else {
			std::map<std::string, std::string> attach_row = attach_rows.at(attach_no);

			char tmp_filename[1024];
			sprintf(tmp_filename, "%s/file/%s", getenv("HANULSO"), attach_row["FILENAME"].c_str());
			unlink(tmp_filename);
		}
	}

	return database::delete_attachment(table_name, no, attach_no);
}

void print_board_header(char *table_name, char *title, int page_count, int page_no)
{
	#if 0
	printf("\033[=9F\033[=1G%s\033[=15F\033[=1G", repeat("¦¡", 40).c_str());
	printf("\033[1A\033[7m%s\033[0m\r\n", host_name);
	#else
	int len = strlen(host_name);
	int border_len = 40-(len/2);
	printf("\033[1;1H");
	printf("\033[1A\033[7m%s\033[0m", host_name);
	printf("\033[=9F\033[=1G%s\033[=15F\033[=1G\r\n", repeat("¦¡", border_len).c_str());
	#endif
	// Å¸ÀÌÆ² Ãâ·Â
	int center = (80-strlen(strip_ansi_codes(title)))/2;
	printf("\033[2;1H");
	printf("\r\033[%dC%s", center, title);

	// ÃÑ ±Û °¹¼ö Ãâ·Â
	int total_count = database::article_count(table_name);
	char buf[1024];
	sprintf(buf, "%4d/%4d (ÃÑ %4d°Ç) ", page_no, page_count, total_count);
	printf("\r\033[57C%s\r\n", buf);
	
	printf("\033[3;1H");
	printf("\033[=0F\033[=1G%s\033[=15F\033[=1G\r\n", repeat("¦¬", 40).c_str());

}

// ¼º°Ý °Ë»ç ÇÁ·Î±×·¥
void run_mbti_gs() 
{
	char buf[1024];
	sprintf(buf, "%s/bin/mbti_gs", getenv("HANULSO"));
	system(buf);
}

void print_biorhythm(char *user_id) 
{
	bool exist;
	std::map<std::string, std::string> user = database::user_info(user_id, &exist);

	ioctl(0, TCSETAF, &sys_term);
	int year = NULL, month = NULL, day = NULL;
	sscanf(user["BIRTHDAY"].c_str(), "%d-%d-%d", &year, &month, &day);

	char buf[1024];
	sprintf(buf, "%s/bin/biorhythm %d %d %d", getenv("HANULSO"), year, month, day);
	system(buf);
	ioctl(0, TCSETAF, &curr_term);
}

void print_luck(char *user_id) 
{
	bool exist;
	std::map<std::string, std::string> user = database::user_info(user_id, &exist);

	int birth_yy = NULL, birth_mm = NULL, birth_dd = NULL;
	sscanf(user["BIRTHDAY"].c_str(), "%d-%d-%d", &birth_yy, &birth_mm, &birth_dd);
	
	int sex = atoi(user["SEX"].c_str());
	if (sex == 1)  { // ³²ÀÚ
		sex = 1;
	} else {		// ¿©ÀÚ
		sex = 2;
	}

	time_t current = time(NULL);

	struct tm *d;
	d = localtime(&current);
	int yy = 1900 + (d->tm_year);
	int mm = (d->tm_mon) + 1;
	int dd = (d->tm_mday);
	
	std::string luck = query_luck(yy, mm, dd, sex, birth_yy, birth_mm, birth_dd, 1, 1);
	std::vector<std::string> lines = split_string(luck, '\n');

	bool show_header = true;
	int c = 0;
	int line_no = 0;
	for(int i=0; i<lines.size(); i++) {
		std::string line = trim(lines[i]);

		if (show_header) {
			printf("[2J[H\r\n");
			printf(" [=15G[=0F                            ¡Ø  ¿À ´Ã ÀÇ ¿î ¼¼ ¡Ø                            [=1G[=15F  \r\n");
			printf(" [=15G[=0F                %4d³â %02d¿ù %02dÀÏ »ýÀÎ ´ç½ÅÀÇ ¿À´Ã ¿î¼¼ÀÔ´Ï´Ù.                [=1G[=15F  \r\n", 
			birth_yy, birth_mm, birth_dd);
			printf("\r\n");
			show_header = false;
		}

		if ( line == "[INS: :INS]" )  {
			c++;
		} else {
			if ( c == 3 && line.length() > 0 ) {
				if ( line[0] == '*' )
					printf("\r\n %s ....¡Ù¡Ú\r\n", line.c_str());
				else
					printf(" %s\r\n", line.c_str());
				line_no++;
			}
		}

		if(line_no>0 && line_no%show_max_line == 0) {
			printf("\r\n [Enter] ¸¦ ´©¸£¼¼¿ä.");
			press_enter();
			show_header = true;
		}
	}

}


void print_user_info(char *user_id)
{
	bool exist;
	std::map<std::string, std::string> user = database::user_info(user_id, &exist);
	printf("\r\n'%s'´ÔÀÇ È¸¿ø Á¤º¸ÀÔ´Ï´Ù.", user["NICK_NAME"].c_str());
	printf("\r\nÀÌ¿ëÀÚID: %s", user["USER_ID"].c_str());
	printf("\r\n´Ð ³× ÀÓ: %s", user["NICK_NAME"].c_str());

	if ( std::find(sysop_users.begin(), sysop_users.end(), user_id) != sysop_users.end() ) {
		printf("\r\n·¹    º§: ½Ã¼ó(¿î¿µÀÚ)");
	} else {
		int user_level = atoi(user["LEVEL"].c_str());
		std::string user_level_name = get_level_name(user_level);

		printf("\r\n·¹    º§: %s", user_level_name.c_str());
	}
	//printf("\r\n»ý    ÀÏ: %s", user["BIRTHDAY"].c_str());
	printf("\r\nÀÌ ¸Þ ÀÏ: %s", user["EMAIL"].c_str());
	printf("\r\n°¡ ÀÔ ÀÏ: %s", user["REGISTRATION_DATETIME"].c_str());
	if ( user["LASTLOGIN_DATETIME"].length() > 0 ) {
		printf("\r\nÃÖ±ÙÁ¢¼Ó: %s", user["LASTLOGIN_DATETIME"].c_str());
	} else {
		printf("\r\nÃÖ±ÙÁ¢¼Ó: ¾øÀ½");
	}
	printf("\r\n¼º    º°: ");
	int sex = atoi(user["SEX"].c_str());
	if ( sex == 1) {
		printf("³²ÀÚ");
	} else {
		printf("¿©ÀÚ");
	}
	//printf("\r\n");
}

