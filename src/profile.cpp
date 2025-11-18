#include "main.h"

bool edit_profile(char *user_id)
{
	// 닉네임, 생일, 이메일, 성별, 비밀번호

	bool exist;
	std::map<std::string, std::string> user = database::user_info(user_id, &exist);
	if ( !exist ) {
		printf("\r\n회원 정보가 없습니다.");
		printf("\r\n[Enter] 를 누르세요.");
		press_enter();
		return false;
	}

	printf("\r\n'%s'님의 회원 정보를 변경합니다.\r\n", user["NICK_NAME"].c_str());
	while(1) {
		char cmd[20];
		printf("\r\n[1]비밀번호 [2]닉네임 [3]생일 [4]이메일 [5]성별 [0]그만");
		printf("\r\n선택: ");
		line_input(cmd, 1);

		if ( cmd[0] == '0' ) break;

		if ( cmd[0] == '1' ) {
			printf(ESC_ENG);
			printf("\r\n비밀번호를 입력해주세요. (특수문자/숫자를 조합해주세요.)");
			while (1) {
				char passwd[50];

				printf("\r\n: ");
				line_input(passwd, 20);

				if(strlen(passwd) == 0) {
					printf("\r\n취소되었습니다.");
					break;
				}

				int strong = check_password_strongness(passwd);
				if ( strong == 0 ) {
					printf("\r\n비밀번호가 약합니다.");
				} else {
					database::set_user_password(user_id, passwd);
					break;
				}
			}
		}

		if ( cmd[0] == '2' ) {
			printf(ESC_HAN);
			printf("\r\n닉네임을 입력하세요. (영문 10자, 한글 5자 가능)");
			while (1) {
				char nick_name[50];

				printf("\r\n: ");
				line_input(nick_name, 10);

				if(strlen(nick_name) == 0) {
					printf("\r\n취소되었습니다.");
					break;
				}

				if ( strlen(nick_name) >= 4 ) {
					if ( database::exist_nick_name(nick_name) == true ) {
						printf("\r\n이미 등록된 닉네임 입니다.");
					} else {
						database::set_user_nick_name(user_id, nick_name);
						printf("\r\n수정되었습니다.");
						break;
					}
				}
			}
		}

		if ( cmd[0] == '3' ) {
			printf(ESC_ENG);
			printf("\r\n생년월일을 입력하세요. (년-월-일) (ex) 1978-1-8");
			while (1) {
				char birthday[50];

				printf("\r\n: ");
				line_input(birthday, 20);

				if(strlen(birthday) == 0) {
					printf("\r\n취소되었습니다.");
					break;
				}

				int year = NULL, month = NULL, day = NULL;
				sscanf(birthday, "%d-%d-%d", &year, &month, &day);

				if ( !is_date_valid(day, month, year) ) {
					printf("\r\n잘못 입력 되었습니다.");
				} else {
					database::set_user_birthday(user_id, birthday);
					printf("\r\n수정되었습니다.");
					break;
				}
			}
		}

		if ( cmd[0] == '4' ) {
			printf(ESC_ENG);
			printf("\r\n이메일 주소를 입력하세요.");
			while (1) {
				char email[50];

				printf("\r\n: ");
				line_input(email, 20);

				if(strlen(email) == 0) {
					printf("\r\n취소되었습니다.");
					break;
				}

				if ( !is_email_valid(email) ) {
					printf("\r\n잘못 입력 되었습니다.");
				} else {
					if ( database::exist_email_address(email) == true ) {
						printf("\r\n이미 등록된 이메일 주소입니다.");
					} else {
						database::set_user_email(user_id, email);
						printf("\r\n수정되었습니다.");
						break;
					}
				}
			}
		}

		if ( cmd[0] == '5' ) {
			printf(ESC_ENG);
			printf("\r\n[1]남자 / [2]여자 를 선택해주세요.");
			while (1) {
				char sex[5];

				printf("\r\n: ");
				line_input(sex, 5);

				if(strlen(sex) == 0) {
					printf("\r\n취소되었습니다.");
					break;
				}

				if ( !strcasecmp(sex, "1") || !strcasecmp(sex, "2") ) {
					database::set_user_sex(user_id, sex);
					printf("\r\n수정되었습니다.");
					break;
				}
			}
		}
	}
}
