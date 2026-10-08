#include "main.h"
#include <stdio.h>
#include <unistd.h>

// Xmodem 으로 받은 파일에 붙일 이름: 경로 / 따옴표 / 제어 글자는 '_' 로
static std::string safe_name(const char *s)
{
	std::string r;
	for ( ; *s; s++ ) {
		unsigned char c = (unsigned char)*s;
		if ( c < 0x20 || c == 0x7f || c == '/' || c == '\\' || c == '"' || c == '\'' || c == '`' || c == '$' )
			r += '_';
		else
			r += (char)c;
	}
	if ( r.empty() || r == "." || r == ".." )
		r = "xmodem.bin";
	if ( r[0] == '-' || r[0] == '.' )   // sz / gkermit 가 옵션으로 읽지 않게, 숨김 / 내부 파일과 겹치지 않게
		r[0] = '_';
	return r;
}

// Xmodem 은 마지막 128 바이트 덩이를 0x1A (^Z) 로 채워 보낸다: 끝의 채움 글자를 떼어 낸다 (마지막 덩이 안에서만)
void strip_xmodem_pad(const char *path)
{
	FILE *f = fopen(path, "rb");
	if ( !f )
		return;
	fseek(f, 0, SEEK_END);
	long n = ftell(f);
	long cut = n;
	while ( cut > 0 && n - cut < 128 ) {
		fseek(f, cut - 1, SEEK_SET);
		if ( fgetc(f) != 0x1A )
			break;
		cut--;
	}
	fclose(f);
	if ( cut < n && cut > 0 )
		truncate(path, cut);
}

// 함수를 빠져나갈 때 (실패해서 중간에 돌아가도) 받다 만 파일째 임시 폴더를 지운다
struct upload_tmp_dir {
	std::string dir;
	~upload_tmp_dir() { if ( !dir.empty() ) remove_tmp_dir(dir.c_str()); }
};

bool file_upload(int protocol, const char *xname, char **tmp_filename, char **filename, int *size)
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
	upload_tmp_dir guard;
	guard.dir = tmpdir;

	// 전송 중에 통신이 끊기면 접속 종료 처리(host_close)에서 폴더째 지우도록 적어 둔다
	add_user_tmpfile(tmpdir);

	// 폴더 변경
	chdir(tmpdir);

	printf("\r\n전송 프로토콜을 실행하세요.\r\n");
	fflush(stdout);

	// zmodem 프로토콜 실행
	ioctl(0, TCSETAF, &sys_term);
	int a;
    if ( protocol == 1 ) {
        // 이름을 주지 않으면 lrzsz rz 는 Xmodem 이 아니라 Ymodem 묶음으로 받는다 (0 번 덩이를 기다리다 실패)
        a = system("rz --xmodem -e xmodem.bin");
    } else if (protocol == 2) {
        a = system("rz --ymodem -e");
    } else if (protocol == 3) {
        a = system("rz --zmodem -e");
    } else if (protocol == 4) {
        // Kermit: 바이너리로 받기 (이름은 보내는 쪽이 알려 준다)
        sprintf(buf, KERMIT_PROG " -i -r", getenv("HANULSO"));
        a = system(buf);
    }
	ioctl(0, TCSETAF, &curr_term);

	// 본래의 폴더로 복귀
	chdir(getenv("HANULSO"));

	if ( WEXITSTATUS(a) != 0 ) {
		return false;
	}

	// Xmodem: xmodem.bin 으로 받은 것을 채움 글자를 떼고 사용자가 입력한 이름으로 바꾼다
	if ( protocol == 1 ) {
		char from[9072], to[9072];
		snprintf(from, sizeof from, "%s/xmodem.bin", tmpdir);
		strip_xmodem_pad(from);
		if ( xname && *xname ) {
			snprintf(to, sizeof to, "%s/%s", tmpdir, safe_name(xname).c_str());
			if ( strcmp(from, to) != 0 && rename(from, to) != 0 ) {
				return false;
			}
		}
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
	// 1000 개씩 번호 폴더 (file/000, file/001 ...) 에 나눠 둔다. DB 에는 "001/file..." 처럼 폴더까지 적는다
	std::string bucket = attachment_bucket();
	if ( bucket.empty() ) sprintf(dir, "%s/file", getenv("HANULSO"));
	else snprintf(dir, sizeof(dir), "%s/file/%s", getenv("HANULSO"), bucket.c_str());
	// tempnam 은 그 순간 없는 이름만 알려 주고 만들지는 않는다. 둘이 동시에 올려 같은 이름을 받으면
	// 뒤의 mv 가 앞 파일을 덮으므로, O_EXCL 로 빈 파일을 먼저 만들어 이름을 잡아 둔다 (그 자리에 mv)
	std::string new_path;
	for ( int t = 0; t < 20 && new_path.empty(); t++ ) {
		char *p = tempnam(dir, "file");
		if ( p == NULL ) continue;
		int fd = open(p, O_WRONLY | O_CREAT | O_EXCL, 0644);
		if ( fd >= 0 ) {
			close(fd);
			new_path = p;
		}
		free(p);
	}
	if ( new_path.empty() ) {
		return false;
	}

	snprintf(buf, sizeof(buf), "mv %s %s", shell_quote(files[0]).c_str(), shell_quote(new_path).c_str());
	a = system(buf);

	if ( WEXITSTATUS(a) != 0 ) {
		unlink(new_path.c_str());
		return false;
	}

	// 임시 폴더는 guard 가 지운다

	// 업로드된 파일 이름 (겹치지 않는 임시 파일 이름)
	*tmp_filename = strdup(((bucket.empty() ? "" : bucket + "/") + split_file_name(new_path)).c_str());
	// 본래의 업로드된 파일 이름 (셸/경로에 위험한 문자는 _ 로 바꿈)
	*filename = strdup(safe_name(split_file_name(files[0]).c_str()).c_str());
	// 파일 사이즈
	*size = file_size((char*)new_path.c_str());

	return true;
}
