#include "main.h"

bool file_upload(int protocol, char **tmp_filename, char **filename, int *size)
{
	char tmpdir[9072];
	char dir[9072];
	char buf[9072];
		
	*size = 0;

	// ---------------------------------------------------
	// 우선 임시 폴더를 생성하여 업로드를 한다.
	sprintf(tmpdir, "%s/tmp", getenv("HANULSO"));
	sprintf(tmpdir, "%s", tempnam(tmpdir, "file"));

	if ( !mkdir2(tmpdir) ) {
		return false;
	}

	// 폴더 변경
	chdir(tmpdir);
	
	printf("\r\n전송 프로토콜을 실행하세요.\r\n");
	fflush(stdout);
	
	// zmodem 프로토콜 실행
	ioctl(0, TCSETAF, &sys_term);
	int a;
    if ( protocol == 1 ) {
        a = system("rz --xmodem -e");
    } else if (protocol == 2) {
        a = system("rz --ymodem -e");
    } else if (protocol == 3) {
        a = system("rz --zmodem -e");
    }
	ioctl(0, TCSETAF, &curr_term);

	// 본래의 폴더로 복귀
	chdir(getenv("HANULSO"));

	if ( WEXITSTATUS(a) != 0 ) {
		return false;
	}

	// 전송된 파일 검색
	sprintf(buf, "%s/*", tmpdir);
	std::vector<std::string> files = find_files_time_sorted(buf);
	if ( files.size() == 0 ) {
		printf("\r\n전송된 파일이 없습니다.");
		return false;
	}
	
	// ---------------------------------------------------
	// 업로드된 첨부 파일을 랜덤 이름으로 변경하여 file 폴더에 옮긴다.
	// 첨부 파일 이름 생성 (file랜덤이름)
	sprintf(dir, "%s/file", getenv("HANULSO"));
	char *new_path = tempnam(dir, "file");

	sprintf(buf, "mv \"%s\" \"%s\"", files[0].c_str(), new_path);
	a = system(buf);

	if ( WEXITSTATUS(a) != 0 ) {
		return false;
	}

	// 업로드된 임시 폴더를 삭제한다.
	sprintf(buf, "rm -rf \"%s\"", tmpdir);
	a = system(buf);

	if ( WEXITSTATUS(a) != 0 ) {
		return false;
	}

	// 업로드된 파일 이름 (겹치지 않는 임시 파일 이름)
	*tmp_filename = strdup(split_file_name(new_path).c_str());
	// 본래의 업로드된 파일 이름
	*filename = strdup(split_file_name(files[0]).c_str());
	// 파일 사이즈
	*size = file_size(new_path);

	return true;
}
