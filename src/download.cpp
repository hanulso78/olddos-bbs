#include "main.h"

bool file_download(int protocol, char *tmp_filename, char *filename)
{
	char buf[9072];

	char tmpdir[1024];
	char path[1024];
	char path2[1024];

	// ---------------------------------------------------
	// 우선 임시 폴더를 생성하여 본래의 업로드 파일 이름으로 복사한다.
	sprintf(tmpdir, "%s/tmp", getenv("HANULSO"));
	sprintf(tmpdir, "%s", tempnam(tmpdir, "file"));
	if ( !mkdir2(tmpdir) ) {
		return false;
	}

	// 임시 파일 이름으로 업로드된 패스
	sprintf(path, "%s/file/%s", getenv("HANULSO"), tmp_filename);

	// 본래의 파일 이름으로 복사될 위치
	sprintf(path2, "%s/%s", tmpdir, filename);

	printf("\r\n파일 수신 준비 중입니다."); fflush(stdout);
	
	// 임시 파일 패스를 적어둔다. 
	// 파일 복사중 통신을 종료하면 쓰레기 파일로 남기 때문에 나중에 자동 종료시 삭제한다.
	add_user_tmpfile(path2);

	// 임시 폴더로 본래의 이름으로 복사
	sprintf(buf, "cp \"%s\" \"%s\"", path, path2);
	int a = system(buf);

	if ( WEXITSTATUS(a) != 0 ) {
		return false;
	}
		
	// 임시 폴더로 이동
	chdir(tmpdir);
	
	printf("\r\n전송 프로토콜을 실행하세요.\r\n");
	fflush(stdout);

	// zmodem 프로토콜 실행
    if (protocol == 1) {
        sprintf(buf, "sz --xmodem -e \"%s\"", filename);
    } else if(protocol == 2) {
        sprintf(buf, "sz --ymodem -e \"%s\"", filename);
    } else if(protocol == 3) {
        sprintf(buf, "sz --zmodem -e \"%s\"", filename);
    }
	//sprintf(buf, "%s/bin/sexyz sz \"%s\"", getenv("HANULSO"), filename);
	ioctl(0, TCSETAF, &sys_term);
	a = system(buf);
	ioctl(0, TCSETAF, &curr_term);

	chdir(getenv("HANULSO"));

	if ( WEXITSTATUS(a) != 0 ) {
		return false;
	}

	// 업로드된 임시 폴더를 삭제한다.
	sprintf(buf, "rm -rf \"%s\"", tmpdir);
	a = system(buf);

	if ( WEXITSTATUS(a) != 0 ) {
		return false;
	}

	return true;
}

