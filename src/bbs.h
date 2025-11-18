/* BBS.H */
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

struct config{              /* 메뉴화일 구성 */
    char form;              /* 메뉴 형태 */
    int level;              /* 레벨 증가분 */
    int del;                /* 삭제 권한 플래그 */
    char ccode[9];          /* 가기 코드 */
    int w;                  /* 쓰기,업 레벨 */
    int r;                  /* 읽기,다운 레벨 */
    char list[30];          /* 메뉴 제목 */
    int deep;               /* 게시물 깊이 */
    struct config *downp;   /* 하위메뉴 위치 */
    struct config *upp;     /* 상위메뉴 위치 */
    struct config *aeqp;    /* 이전메뉴 위치 */
    struct config *neqp;    /* 다음메뉴 위치 */
    char file[25];          /* 화일위치 디렉토리 */
};

struct idst {               /* 회원 정보 */
    int num;                /* 회원 일련 번호 */
    char id[9];             /* 아이디 */
    char passwd[9];         /* 비밀번호 */
    char name[9];           /* 성명 */
    int flag;               /* 회원정보 공개 플래그 */
    char sex;               /* 성별 (0x01) 회원구분 */
    char birthday[12];      /* 생년월일 (0x02) */
    time_t inday;           /* 가입일자 (0x04) */
    time_t logout;          /* 마지막 접속일 (0x08) */
    char post[8];           /* 우편번호 (0x10) */
    char home_addr[80];     /* 집주소 (0x20) */
    char home_tel[15];      /* 집전화번호 (0x40) */
    char office_name[80];   /* 직장명 (0x80) */
    char office_tel[15];    /* 직장전화번호 (0x100) */
    char id_no[15];         /* 주민등록번호 (0x200) */
    int circle;             /* 동아리 가입 여부 */
    int level;              /* 레벨 (0x400) */
    char expflag;           /* 확장 플래그 */
    int logcount;           /* 접속횟수 */
    time_t totaltime;       /* 총사용시간기록 */
    time_t lasttime;        /* 유효 사용일 */
};

struct dis_list {           /* 게시물 정보 */
    int num;                /* 일련 번호 */
    char id[9];             /* 작성자 아이디 */
    char name[9];           /* 작성자 이름 */
    time_t date;            /* 작성 일자 */
    int line;               /* 내용 길이(줄수) */
    int look;               /* 조횟수 */
    char title[47];         /* 제목 */
    int position;           /* 내용 포인터 */
    int size;               /* 내용 크기 */
};

struct dis_file {           /* 화일 정보 */
    int num;                /* 일련 번호 */
    char id[9];             /* 업로드 아이디 */
    char name[9];           /* 업로드 이름 */
    time_t date;            /* 업로드 날짜 */
    int line;               /* 설명 길이(줄수) */
    char filename[13];      /* 화일명 */
    long int file_size;     /* 화일의 길이 */
    int down;               /* 다운로드 횟수 */
    char title[40];         /* 제목 */
    int position;           /* 설명 포인터 */
    int size;               /* 설명 크기 */
};

struct mail {               /* 편지 */
    time_t tf;              /* 기록 시간 */
    int num;                /* 일련 번호 */
    char T;                 /* 읽기 확인 */
    char id[9];             /* 보낸사람 아이디 */
    int id_num;             /* 보낸사람 일련번호 */
    char name[9];           /* 보낸사람 이름 */
    char title[40];         /* 제목 */
    int line;               /* 편지 줄수 */
    long int point;         /* 내용 포인터 */
    long int size;          /* 편지 크기 */
};

struct mail_dis {           /* 편지 확인 */
    time_t tf;              /* 기록 시간 */
    char id[9];             /* 받는 사람 아이디 */
    char name[9];           /* 받는 사람 이름 */
    time_t date;            /* 읽은 날짜 */
    char title[40];         /* 보낸 제목 */
};

struct nowwork {            /* 접속자 명단 */
    char chose;             /* 초대상태 */
    char id[9];             /* 아이디 */
    char name[9];           /* 이름 */
    char ccode[10];         /* 있는곳 */
    char tty[15];           /* 접속 노드 */
};

struct lestfile {           /* 접속 기록 화일 */
    char id[9];             /* 아이디 */
    char name[9];           /* 이름 */
    time_t intime;          /* 접속 시간 */
    time_t outtime;         /* 종료 시간 */
    char tty[15];           /* 접속 노드 */
    char flag;              /* 확장플래그 값 */
};

struct fcfg {               /* 초기값 */
/*  char guide[9];             공지사항 */
    int del;                /* 삭제레벨 */
    int sh;                 /* 쉘사용레벨 */
    int chat;               /* 비공개 대화레벨 */
    int seroom;             /* 비공개대화실갯수 */
    int inc;                /* 접속레벨증가분 */
    char secret[10];        /* 운영자 비밀번호 */
    int flag;               /* 셋팅 플래그(종료, 유효일) */
    int room;               /* 대화실 인원 */
};

