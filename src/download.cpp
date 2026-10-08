#include "main.h"

// 함수를 빠져나갈 때 (실패해서 중간에 돌아가도) 임시 폴더를 지운다
struct download_tmp_dir {
	std::string dir;
	~download_tmp_dir() { if ( !dir.empty() ) remove_tmp_dir(dir.c_str()); }
};

// 첨부 표의 파일 이름: "file..." 또는 번호 폴더까지 "001/file..." (폴더는 숫자만, 한 단계)
static bool safe_stored_name(const char *s)
{
	const char *sl = strchr(s, '/');
	if ( s[0] == 0 || strstr(s, "..") ) return false;
	if ( sl == NULL ) return true;
	if ( strchr(sl + 1, '/') || sl == s || sl[1] == 0 ) return false;
	for ( const char *p = s; p < sl; p++ ) if ( !isdigit((unsigned char)*p) ) return false;
	return true;
}

// err 에 실패한 까닭 (화면에 보일 한 줄)
bool file_download(int protocol, char *tmp_filename, char *filename, std::string *err)
{
	char buf[9072];

	char tmpdir[1024];
	char path[1024];
	char path2[1024];
	std::string dummy;
	if ( err == NULL ) err = &dummy;
	err->clear();

	// 저장된 이름 (번호 폴더/파일) 과 받을 이름에 위험한 것이 있으면 막는다
	if ( !safe_stored_name(tmp_filename) ) {
		*err = std::string("첨부 파일 이름이 올바르지 않습니다 (") + tmp_filename + ")";
		return false;
	}
	if ( strchr(filename, '/') || filename[0] == 0 ) {
		*err = "받을 파일 이름이 올바르지 않습니다";
		return false;
	}

	// ---------------------------------------------------
	// 우선 임시 폴더를 생성하여 본래의 업로드 파일 이름으로 연결(복사)한다.
	sprintf(tmpdir, "%s/tmp", getenv("HANULSO"));
	sprintf(tmpdir, "%s", tempnam(tmpdir, "file"));
	if ( !mkdir2(tmpdir) ) {
		*err = std::string("임시 폴더를 만들지 못했습니다: ") + strerror(errno);
		return false;
	}
	download_tmp_dir guard;
	guard.dir = tmpdir;

	// 전송 중에 통신이 끊기면 접속 종료 처리(host_close)에서 폴더째 지우도록 적어 둔다
	add_user_tmpfile(tmpdir);

	// 임시 파일 이름으로 업로드된 패스
	snprintf(path, sizeof(path), "%s/file/%s", getenv("HANULSO"), tmp_filename);

	// 본래의 파일 이름으로 복사될 위치
	snprintf(path2, sizeof(path2), "%s/%s", tmpdir, filename);

	printf("\r\n파일 수신 준비 중입니다."); fflush(stdout);

	// 본래의 이름으로 심볼릭 링크 (큰 파일도 복사하지 않아 빠르고, 남아도 자리를 차지하지 않는다)
	// 링크를 못 만들면 복사
	int a = 0;
	if ( symlink(path, path2) != 0 ) {
		snprintf(buf, sizeof(buf), "cp %s %s", shell_quote(path).c_str(), shell_quote(path2).c_str());
		a = system(buf);
		if ( WEXITSTATUS(a) != 0 ) {
			*err = "보낼 파일을 준비하지 못했습니다 (링크와 복사가 모두 실패)";
			return false;
		}
	}

	// 임시 폴더로 이동
	chdir(tmpdir);

	printf("\r\n전송 프로토콜을 실행하세요.\r\n");
	fflush(stdout);

	// zmodem 프로토콜 실행
    // "./" 를 붙여 '-' 로 시작하는 이름도 옵션이 아닌 파일로 (sz / gkermit 은 경로를 떼고 이름만 알린다)
    std::string qname = shell_quote(std::string("./") + filename);
    if (protocol == 1) {
        snprintf(buf, sizeof(buf), "sz --xmodem -e %s", qname.c_str());
    } else if(protocol == 2) {
        snprintf(buf, sizeof(buf), "sz --ymodem -e %s", qname.c_str());
    } else if(protocol == 3) {
        snprintf(buf, sizeof(buf), "sz --zmodem -e %s", qname.c_str());
    } else if(protocol == 4) {
        snprintf(buf, sizeof(buf), KERMIT_PROG " -i -s %s", getenv("HANULSO"), qname.c_str());   // Kermit: 바이너리로 보내기
    }
	//sprintf(buf, "%s/bin/sexyz sz \"%s\"", getenv("HANULSO"), filename);
	// 전송 프로그램의 메시지 (stderr) 는 파일로 받아 실패하면 마지막 줄을 보여 준다
	// 링크 폴더 밖에 둔다 (첨부 본래 이름과 겹치면 그 첨부 파일을 비워 버린다)
	std::string errfile = make_private_tmpfile("sz_err");
	struct errfile_guard { std::string p; ~errfile_guard() { if ( !p.empty() ) unlink(p.c_str()); } } eg;
	eg.p = errfile;
	if ( errfile.empty() ) errfile = "/dev/null";
	strncat(buf, (" 2>" + shell_quote(errfile)).c_str(), sizeof(buf) - strlen(buf) - 1);
	ioctl(0, TCSETAF, &sys_term);
	a = system(buf);
	ioctl(0, TCSETAF, &curr_term);

	chdir(getenv("HANULSO"));

	if ( a == -1 ) {
		*err = std::string("전송 프로그램을 실행하지 못했습니다: ") + strerror(errno);
		return false;
	}
	if ( WIFSIGNALED(a) ) {
		*err = "전송 프로그램이 신호 " + TO_STRING(WTERMSIG(a)) + " 로 멈췄습니다";
		return false;
	}
	if ( WEXITSTATUS(a) != 0 ) {
		int code = WEXITSTATUS(a);
		// 마지막 메시지 한 줄 (sz: "Transfer incomplete", "caught signal" ...)
		std::string last;
		std::vector<std::string> lines = split_string(read_file(errfile.c_str()), '\n');
		for ( int i = (int)lines.size() - 1; i >= 0 && last.empty(); i-- ) {
			std::string l = trim(lines[i]);
			std::string::size_type cr = l.rfind('\r');
			if ( cr != std::string::npos ) l = trim(l.substr(cr + 1));
			last = display_text(l);
		}
		if ( code == 127 ) *err = "전송 프로그램이 없습니다 (" + std::string(protocol == 4 ? "gkermit" : "sz, lrzsz") + " 설치 확인)";
		else *err = "전송이 끝나지 못했습니다 (코드 " + TO_STRING(code) + "): 받는 쪽에서 취소했거나 연결이 끊겼을 수 있습니다";
		if ( !last.empty() ) *err += "\r\n    전송 프로그램: " + string_truncate(last, 60, "");
		return false;
	}

	// 임시 폴더는 guard 가 지운다
	return true;
}

