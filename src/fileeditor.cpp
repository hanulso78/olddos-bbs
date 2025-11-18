#include "main.h"

bool file_editor(char **lines, int *length)
{
	char tmpdir[9072];
	char buf[9072];
		
	*length = 0;

	printf("\r\n전송 프로토콜을 실행하세요.\r\n");
	
	// ---------------------------------------------------
	// 우선 임시 폴더를 생성하여 업로드를 한다.
	sprintf(tmpdir, "%s/tmp", getenv("HANULSO"));
	sprintf(tmpdir, "%s", tempnam(tmpdir, "file"));

	if ( !mkdir2(tmpdir) ) {
		printf("\r\n[Enter] 를 누르세요.");
		press_enter();
		return false;
	}

	// 폴더 변경
	chdir(tmpdir);

	// zmodem 프로토콜 실행
	ioctl(0, TCSETAF, &sys_term);
	int a = system("rz -e");
	ioctl(0, TCSETAF, &curr_term);

	// 본래의 폴더로 복귀
	chdir(getenv("HANULSO"));

	if ( WEXITSTATUS(a) != 0 ) {
		printf("\r\n파일 전송을 실패하였습니다.\r\n");
		printf("\r\n[Enter] 를 누르세요.");
		press_enter();
		return false;
	}

	// 전송된 텍스트 파일 검색
	sprintf(buf, "%s/*", tmpdir);
	std::vector<std::string> files = find_files_time_sorted(buf);
	if ( files.size() == 0 ) {
		printf("\r\n전송된 파일이 없습니다.");
		printf("\r\n[Enter] 를 누르세요.");
		press_enter();
		return false;
	}
	
	// 전송된 텍스트 파일을 unix2dos 를 수행한다.
	//sprintf(buf, "unix2dos --quiet %s", files[0].c_str());
	sprintf(buf, "%s/bin/unix2dos --quiet \"%s\"", getenv("HANULSO"), files[0].c_str());
	a = system(buf);
	if ( WEXITSTATUS(a) != 0 ) {
		printf("\r\nunix2dos 실행을 실패하였습니다.\r\n");
		printf("\r\n[Enter] 를 누르세요.");
		press_enter();
		return false;
	}

	//sleep(1);

	// 전송된 파일의 텍스트 반환
	std::string text = read_file(files[0].c_str());

	// 조합형일 경우 완성형으로 변환
	if ( ks_conversion ) {
		int ks_count, kssm_count, eng_count;
		check_hangul_code((char*)text.c_str(), text.length(), &ks_count, &kssm_count, &eng_count);

		if ( ks_count != 0 || kssm_count != 0 ) {
			if (kssm_count > ks_count) {
				char out_file[1024];
				sprintf(out_file, "%s.out", files[0].c_str());

				sprintf(buf, "iconv -c -f JOHAB -t CP949//IGNORE//TRANSLIT '%s' > '%s'", files[0].c_str(), out_file);
				a = system(buf);
				if ( WEXITSTATUS(a) != 0 ) {
					printf("\r\niconv 실행을 실패하였습니다.\r\n");
					printf("\r\n[Enter] 를 누르세요.");
					press_enter();
					return false;
				}

				text = read_file(out_file);
			}
		}
	}

	// 임시 폴더 삭제
	sprintf(buf, "rm -rf %s", tmpdir);
	a = system(buf);

	if ( WEXITSTATUS(a) != 0 ) {
		printf("\r\n파일 전송을 실패하였습니다.");
		printf("\r\n[Enter] 를 누르세요.");
		press_enter();
	}

#if 0
	*lines = (char*)malloc(text.length()+1);
	memcpy(*lines, text.c_str(), text.length());
	lines[text.length()] = '\0';
#else
	*lines = strdup(text.c_str());
#endif

	*length = text.length();

	printf("\r\n파일 전송을 성공하였습니다.\r\n");
	printf("\r\n[Enter] 를 누르세요.");
	fflush(stdout);
	press_enter();

	return true;
}
