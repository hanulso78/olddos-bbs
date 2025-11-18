#include "main.h"

void text_append(std::vector<std::string> &list);
void text_list(std::vector<std::string> &list);
void text_edit(std::vector<std::string> &list);
void text_insert(std::vector<std::string> &list);
void text_delete(std::vector<std::string> &list);

/* 라인 편집기 */
bool line_editor(std::vector<std::string> &lines, bool edit)
{
    char buf[2];

	std::vector<std::string> list;

	// 새 작성 모드이면
	if ( !edit ) {
		text_append(list);

	// 편집 모드이면
	} else {
		list = lines;
		text_list(list);
	}

#if 0
	printf("\r\n---------------------\r\n");
	for(int i=0; i<list.size(); i++) {
		printf("%s\r\n", list[i].c_str());
	}
#endif

    while(1) {
		printf(ESC_ENG);
        printf("\r\n[S]등록 [Q]취소 [L]읽기 [E]수정 [A]추가 [I]삽입 [D]삭제 : ");
        line_input(buf, 2);

        if(buf[0] == 's' || buf[0] == 'S') { break; }
        else if(buf[0] == 'l' || buf[0] == 'L') { text_list(list); }
        else if(buf[0] == 'a' || buf[0] == 'A') { text_append(list); }
        else if(buf[0] == 'e' || buf[0] == 'E') { text_edit(list); }
        else if(buf[0] == 'd' || buf[0] == 'D') { text_delete(list); }
        else if(buf[0] == 'i' || buf[0] == 'I') { text_insert(list); }
        else if(buf[0] == 'q' || buf[0] == 'Q') {
            printf("\r\n취소 할까요? (y/n)? ");
            if ( yesno(NO) == YES ) {
				return false;
            }
        }
    }

	lines = list;

#if 0
	printf("\r\n---------------------\r\n");
	for(int i=0; i<list.size(); i++) {
		printf("%s\r\n", list[i].c_str());
	}
#endif

    return true;
}

void text_append(std::vector<std::string> &list)
{
    printf("\r\n\033$)1\033[?85h\r첫칸에 '.'을 찍으면 끝납니다.\r\n");
	printf("0---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----\r\n");
	
	char buf[81];
    while(1) {
        line_input(buf, 78);

        if((buf[0] == '.' && buf[1] == '\0') || buf[0] == 0x1a) {
			break;
        }

        printf("\r\n");
        list.push_back(std::string(buf));
    }
}

void text_list(std::vector<std::string> &list)
{
    int i = 0;
	unsigned int l = 0;

	if ( list.size() == 0 ) return;

    while (1) {
        if(i < 18) {
			if ( l > list.size()-1 ) break;
            printf("\r\n%d : %s", l+1, list[l].c_str());
			l++;
            i++;
        }
        else {
			char buf[2];
            printf("\r\n계속[엔터] 중지[P] : ");
			line_input(buf, 1);
            if(buf[0] == 'p' || buf[0] == 'P') {
                break;
            }

            i = 0;
        }
    }
}

void text_edit(std::vector<std::string> &list)
{
	char buf[81];

	printf("\r\n줄번호 : ");
	line_input(buf, 4);
	if ( strlen(buf) == 0 ) return;

	unsigned int l = atoi(buf);
	if ( l > list.size() || l <= 0 ) return;

	printf("\r\n\033$)1\033[?85h\r%s\r\n", list[l-1].c_str());
	line_input(buf, 78);

	list[l-1] = std::string(buf);
}

void text_delete(std::vector<std::string> &list)
{
	char buf[81];

	printf("\r\n줄번호 : ");
	line_input(buf, 4);
	if ( strlen(buf) == 0 ) return;

	unsigned int l = atoi(buf);
	if ( l > list.size() || l <= 0 ) return;

	list.erase(list.begin()+(l-1));
}

void text_insert(std::vector<std::string> &list)
{
	char buf[81];

	printf("\r\n줄번호 : ");
	line_input(buf, 4);
	if ( strlen(buf) == 0 ) return;

	unsigned int l = atoi(buf);
	if ( l > list.size() || l <= 0 ) return;

	printf("\r\n입력: ");
	line_input(buf, 78);
	list.insert(list.begin()+(l-1), std::string(buf));
}

