<div align="center">

# 도스박물관 BBS

**1990년대 PC통신 시절의 BBS를 텔넷으로 되살린 프로그램**

![CentOS 6](https://img.shields.io/badge/CentOS-6-262577?logo=centos)
![C++98](https://img.shields.io/badge/C%2B%2B-98-00599C?logo=cplusplus)
![EUC-KR](https://img.shields.io/badge/%ED%95%9C%EA%B8%80-EUC--KR-0000AA)
![Telnet](https://img.shields.io/badge/%EC%A0%91%EC%86%8D-Telnet-555555)
![License LGPL](https://img.shields.io/badge/License-LGPL-blue)

</div>

<p align="center"><img width="850" alt="도스박물관 대문 화면" src="docs/screenshots/front_page.png" /></p>

소개
-
파란 화면, 번호로 고르는 메뉴, `GO` 명령, 한 줄씩 쓰는 줄 편집기, Zmodem 으로 주고받던 자료실.
하이텔, 천리안, 나우누리 시절 PC통신의 모습과 쓰임새를 그대로 재현한 텔넷 BBS 입니다.

- **옛 통신 에뮬레이터로 접속**: 이야기 5.4 같은 옛 통신 프로그램에 맞춘 80 칸 화면, EUC-KR 한글, ANSI 색
- **그때 그 기능**: 게시판과 자료실, 대화방, 쪽지, 회원 등급, 도어 게임과 머드 게임
- **요즘 정보도 그 시절 화면으로**: 날씨와 미세먼지, 뉴스, 환율, 음력 달력, 운세
- **직접 운영 중**: CentOS 6.9 에서 개발하고 운영하고 있습니다
  - 웹: https://bbsweb.oscc.kr/
  - 카페: http://cafe.naver.com/olddos

주요 기능
-
**접속과 화면**
- 텔넷 접속 (xinetd + in.telnetd), EUC-KR 완성형 한글, ANSI 색상
- 이야기 같은 옛 통신 에뮬레이터에 맞춘 화면 (80 칸, 이야기 색상 코드)
- 한글 입력 편집 (한글 한 글자 단위 백스페이스)
- 10 분 동안 입력이 없으면 자동 접속 종료
- 대문 화면에 회원 수, 접속자 수, 전체 글 수 표시
- 메뉴, 게시판, 화면 구성을 `hanulso.mnu` (XML) 와 `txt/` 화면 파일로 설정
  (화면 파일에 `[num_members]`, `[date]`, `[nick]` 같은 태그를 쓸 수 있음: [docs/txt_tags.md](docs/txt_tags.md))
- `GO {메뉴명}` 으로 바로 이동, `T` 초기 화면, `P` 이전 메뉴, `H` 도움말
- 로그인하면 가입일, 닉네임, 등급, 최근 접속, 노드, 쪽지, 오늘의 바이오리듬을 보여 줌

<img width="850" alt="접속 화면" src="docs/screenshots/login.png" />

**회원**
- 가입, 로그인, 비밀번호 찾기 (새 비밀번호를 이메일로 발송)
- 회원 정보 보기/변경 (`PF`, `PE`), 접속 중인 회원 목록 (`US`)
- 쪽지 (`MEMO`): 받은/보낸 쪽지함, 쓰기, 답장, 지우기. 로그인할 때 읽지 않은 쪽지 수 알림
- 회원 등급 (`hanulso.cfg` 의 `level`), 메뉴/게시판별 등급 제한
- 비밀번호는 SHA-512 crypt 로 저장 (예전 MySQL `PASSWORD()` 해시는 로그인할 때 자동 전환)

<img width="850" alt="쪽지 읽기 화면" src="docs/screenshots/memo.png" />

**게시판 / 자료실**
- 글쓰기 (`W`), 답글 (`RE`), 고치기 (`ED`), 지우기 (`DD`, 범위 지정 가능), 추천 (`OK`)
- 목록 쪽 넘기기 (`Enter`/`N`/`B`), 연속 읽기 (`PR`)
- 찾기: 제목/본문 (`LT`), 아이디 (`LI`), 닉네임 (`LN`)
- 글 작성: 줄 편집기, 화면 편집기 (pico), 텍스트 파일 올리기 (조합형 한글은 완성형으로 바꿈)
- 첨부 파일 올리기/받기 (`UP`, `DN`): Zmodem, Ymodem, Xmodem, Kermit
- 프로그램/게임/특별/음원 자료실

<img width="850" alt="프로그램 자료실 화면" src="docs/screenshots/pds_prog.png" />
<img width="850" alt="게시판 목록 화면" src="docs/screenshots/board_list.png" />

**대화방**
- 대화방 개설, 비밀방, 인원 제한
- 귓속말 (`/SAY`), 접속자 조회 (`/LIST`), 퇴장 (`/BYE`), 대화방 닫기 (`/QUIT`)

<img width="850" alt="대화방 화면" src="docs/screenshots/chat.png" />

**생활정보** (`go life`)
- 날씨와 미세먼지: 전국 시/군/구 231 곳, 3 시간 간격 예보 (Open-Meteo)
- 뉴스: 71 개 언론사 342 개 RSS
- 환율: 22 개 통화, 원화 환산 (ExchangeRate-API)
- 만세력/음력: 양력/음력 변환, 간지/띠, 음력이 함께 나오는 달력, 다가오는 공휴일 (한국천문연구원 기준)
- 바이오리듬 (신체/감성/지성/지각, 앞뒤 2 주 그래프, 위험일, 오늘의 풀이 / `BIO`), 성격 유형 검사 (MBTI), 오늘의 운세 (`LUCK`)
- 로또 당첨번호, 코인 시세, 해와 달, 세계 시각, 오늘의 영어 한 문장
- 사랑의 별점 (이름 획수 궁합, 별자리 + 띠 궁합, 회원과 궁합), 토정비결 (144 괘, 올해 / 내년)
- AI 와 이야기 (`go ai`): 구글 Gemini 와 한 줄씩 묻고 답하기. 무료 키를 `hanulso.cfg` 에
  `<ai><key>AIza...</key></ai>` 로 넣는다 (https://aistudio.google.com 의 Get API key).
- 우편번호 찾기 (`go zip`): 도로명/지번 주소나 건물 이름으로 5 자리 우편번호를 찾는다 (행정안전부 도로명주소 검색 API).
  무료 승인키를 https://business.juso.go.kr 에서 받아 `hanulso.cfg` 에 `<juso><key>승인키</key></juso>` 로 넣는다.
  `<model>`, `<url>` (OpenAI 방식이면 다른 곳도), 한 사람의 하루 질문 수 `<daily>` (기본 50) 도 바꿀 수 있다

<img width="850" alt="생활정보 화면" src="docs/screenshots/life_info.png" />
<img width="850" alt="날씨 정보 화면" src="docs/screenshots/weather.png" />
<img width="850" alt="뉴스 언론사 목록 화면" src="docs/screenshots/news.png" />
<img width="850" alt="뉴스 기사 목록 화면" src="docs/screenshots/news_list.png" />
<img width="850" alt="환율 정보 화면" src="docs/screenshots/exchange.png" />

**게임 마당** (`go game`)
- 용사의 전설: 옛 BBS 도어 게임 방식의 1 인용 텍스트 RPG
- 쥬라기공원 2: PC통신 시절 머드의 HanLP 복원판 (아래 머드 게임 항목 참고)
- 무한대전: Mordor 기반의 1990 년대 한글 머드, 64 비트로 이식 (아래 머드 게임 항목 참고)
- 조크 I: Infocom 의 1980 년 문 게임을 한글로. 원작 게임 파일을 그대로 돌리고 화면 글만 번역 (아래 항목 참고)
- 산성비 (타자), 오목 (회원끼리 / 컴퓨터와), 숫자야구 (매일 바뀌는 오늘의 문제 순위),
  지뢰찾기 (초급 / 중급 / 고급), 블랙잭 (칩 모으기). 게임마다 명예의 전당
- 타자 연습: 속담, 시 (윤동주, 김소월, 한용운), 애국가, 영문 글을 따라 치기. 타수와 정확도,
  틀린 글자 표시, 오늘의 도전 (모두 같은 글) 순위, 명예의 전당
- 윷놀이: 컴퓨터와 한판. 윷가락 넷이 굴러 배 / 등이 보이고 (빽도 표시 가락), 업기, 잡기, 모 / 뒷모 / 방 지름길,
  명예의 전당
- 대화방 놀이: 주사위 (`/주사위`), 끝말잇기 (`/끝말잇기`), 퀴즈 (`/퀴즈`), 사다리 타기 (`/사다리`)

**운영자**
- 운영자 메뉴 (`SYSOP`)
  - 접속자: 지금 접속자와 강제 종료, 전체 공지 방송, 로그인 공지 (`txt/login_notice.txt`)
  - 회원: 찾기 / 등급 / 비밀번호, 아이디 바꾸기 (닉네임으로 찾기, 글/쪽지/꼬리말/게임 기록도 함께), 이용 정지 (기간, 사유), 날짜별 로그인 / 새 회원 통계, 삭제
  - 게시판: 게시물 옮기기 (답글, 꼬리말, 첨부도), 지우기, 공지 고정 (첫 쪽 맨 위 [공지]), 꼬리말 지우기, 투표 관리
  - 네이버 카페 글 가져오기: 글 번호 범위를 넣으면 제목/작성자/본문/그림/첨부를 카페에서 받아 두고, 확인한 뒤 메뉴별 게시판에 올림. 작성자는 닉네임으로 BBS 회원과 자동 연결 (`bin/cafeimport`)
  - 서버: 디스크, 임시 파일 정리, 주인 없는 첨부 파일 정리, AI 와 이야기 사용량
  - DB 백업: `backup/bbs-날짜-시각.sql.gz` (mysqldump, latin1 그대로), 목록과 지우기. 되살리기는 서버에서 `gunzip < 파일 | mysql`
- 첨부 파일은 `file/000`, `file/001` ... 번호 폴더에 1000 개씩 나눠 둔다 (DB 에는 `001/file...` 처럼 폴더까지).
  예전에 `file/` 바로 아래 올라간 파일도 그대로 받을 수 있고, 운영자 메뉴의 정리와 점검 → M 으로 번호 폴더로 옮길 수 있다
- 글쓴이 등급 변경 (`LV`), 서버 정보 (`SYS`)

지원 프로토콜
-
**접속**

| 항목 | 내용 |
|---|---|
| 접속 | Telnet (xinetd + in.telnetd) |
| 터미널 | ANSI (커서 이동, 반전, 화면 지우기) + 이야기 색상 코드 (`ESC[=nF` 글자색, `ESC[=nG` 배경색), 80 x 24 |
| 한글 | KS X 1001 완성형 (EUC-KR). 텍스트 파일로 글을 올릴 때는 조합형도 받아 완성형으로 바꿈 |
| 통신 프로그램 | 이야기 5.4 기준으로 화면을 맞춤 |

**파일 전송** (첨부 파일 올리기 `UP`, 받기 `DN`)

| 번호 | 프로토콜 | 서버 쪽 프로그램 | 비고 |
|---|---|---|---|
| 1 | Xmodem | lrzsz (`rz`/`sz --xmodem`) | 파일 이름이 전송되지 않아 올릴 때 이름을 먼저 입력. 끝의 채움 글자(0x1A)는 자동으로 뗌 |
| 2 | Ymodem | lrzsz (`rz`/`sz --ymodem`) | 파일 이름과 크기를 함께 전송 |
| 3 | Zmodem | lrzsz (`rz`/`sz --zmodem`) | 기본값. 파일 이름과 크기를 함께 전송하고 가장 빠름 |
| 4 | Kermit | gkermit (`bin/gkermit`) | 바이너리 모드 (`-i`) |

- lrzsz 는 `-e` (제어 문자 이스케이프) 로 실행해 텔넷에서도 전송이 끊기지 않게 합니다.
- 글쓰기에서 `[3]zmodem` 을 고르면 텍스트 파일을 Zmodem 으로 올려 글 본문으로 씁니다.

라이선스
-
이 프로그램은 LGPL 라이선스를 따릅니다.

`src/jurassic/driver` 의 머드 엔진(MudOS)과 쥬라기공원 2 복원판 머드 라이브러리(`src/jurassic/jp2_v15.tgz`)는 LGPL 대상이 아닙니다. 아래 머드 게임 항목을 참고하세요.

설치
-
설치 방법은 INSTALL.TXT 파일을 참고하세요.

### 미리 설치할 패키지 (CentOS 6 기준)

| 용도 | 패키지 |
|---|---|
| 텔넷 접속 | `xinetd` `telnet-server` |
| 빌드 | `gcc-c++` `make` `zlib-devel` `openssl-devel` |
| 데이터베이스 | `MariaDB-server` `MariaDB-client` (MariaDB 저장소), `mysql-devel` |
| 파일 올리기/받기 | `lrzsz` (Zmodem/Ymodem/Xmodem), `gkermit` (소스로 빌드) |
| 생활정보 (날씨/뉴스/환율 등) | `curl` `wget` `lynx` |
| 글자 변환, 시스템 정보 | `iconv` (glibc-common, 기본 설치), `dos2unix` `unix2dos`, `redhat-lsb-core` |
| 화면 편집기 (pico) | `ncurses-devel` |
| 머드 게임 (쥬라기공원 2) | `gcc` `bison` |

```bash
yum install xinetd telnet-server gcc-c++ gcc make bison zlib-devel openssl-devel mysql-devel lrzsz curl wget lynx dos2unix unix2dos redhat-lsb-core ncurses-devel
```

- 한글 로캘 `ko_KR.eucKR` 이 필요합니다 (`LANG=ko_KR.eucKR`).
- `bin/mailsend` (비밀번호 찾기 메일) 는 `src/mailsend/` 의 소스를 빌드해 넣습니다 (`BUILD.TXT` 참고).
- 글쓰기 화면 편집기는 `bin/pico` 를 실행합니다. `src/pico/pico` 의 pico (한글 고침, 한글 메뉴) 를 `cd src && make pico` 로 빌드하면 `bin/pico` 가 만들어집니다. `make all` (update.sh) 에는 들어 있지 않으니 pico 를 고쳤을 때만 따로 빌드하세요.

업데이트
-
olddos 계정에서 실행합니다.

| 바뀐 것 | 실행할 것 |
|---|---|
| BBS 소스, 화면 파일(`txt/`), 메뉴(`*.mnu`) | `/home/olddos/olddos-bbs/update.sh` |
| 쥬라기공원 머드 (엔진, 게임 내용 `libpatch`) | `/home/olddos/olddos-bbs/src/jurassic/update.sh` |
| 무한대전 | `/home/olddos/olddos-bbs/src/muhan/update.sh` |
| 둘 다 | BBS 먼저, 그다음 머드 (머드 쪽은 `NOPULL=1`) |

### BBS — `update.sh`

```bash
/home/olddos/olddos-bbs/update.sh
```

| 순서 | 하는 일 |
|---|---|
| 1 | `git pull` 로 소스 받기. 화면 파일(`txt/`)과 메뉴(`*.mnu`)는 이것으로 바로 반영 |
| 2 | 임시 디렉터리(`/tmp/olddos-bbs-build`)에 `src` 를 복사해 **처음부터** `make all` (makefile 에 헤더 의존성이 없어 예전 `.o` 를 쓰지 않음) |
| 3 | 모두 성공했을 때만 바뀐 실행 파일을 `bin/` 과 BBS 홈(`ctime`)에 반영하고 바뀐 목록을 보여 줌 |

- **빌드가 하나라도 실패하면 아무것도 바꾸지 않습니다.** 오류 줄을 보여 주고, 전체 기록은 `/tmp/olddos-bbs-build.log` 에 남습니다.
- **재시작할 것이 없습니다.** 실행 파일을 임시 이름으로 복사한 뒤 `mv` 로 바꾸므로 접속 중인 사용자는 기존 프로그램을 계속 쓰고, 다음 접속부터 새 버전이 실행됩니다.
- 데이터베이스 테이블 변경(새 칸 추가 등)은 프로그램이 처음 실행될 때 자동으로 합니다.
- 머드 연결 프로그램 `bin/mudlink` 도 여기서 함께 빌드됩니다.

| 옵션 | 뜻 |
|---|---|
| `NOPULL=1 ./update.sh` | `git pull` 건너뜀 (서버에서 직접 고친 소스를 빌드할 때) |
| `./update.sh [빌드 디렉터리]` | 기본값 `/tmp/olddos-bbs-build` |

### 쥬라기공원 머드 — `src/jurassic/update.sh`

```bash
/home/olddos/olddos-bbs/src/jurassic/update.sh
```

| 순서 | 하는 일 |
|---|---|
| 1 | `git pull` 로 소스 받기 |
| 2 | `build.sh` 로 엔진 빌드. 머드를 끄기 전에 하므로 **빌드가 실패하면 머드는 켜진 그대로** 둡니다 |
| 3 | 머드 끄기. pid 파일이 틀리거나 `startmud` 가 여러 개 떠 있어도 이 설치본의 것을 모두 찾아 끕니다 |
| 4 | `deploy.sh` 로 설치 (사용자 자료 `lib/data` 와 로그는 그대로) |
| 5 | `startmud` 로 시작 |
| 6 | `127.0.0.1:4444` 에 접속되는지 확인 |

| 옵션 | 뜻 |
|---|---|
| `NOPULL=1 ./update.sh` | `git pull` 건너뜀 |
| `NOBUILD=1 ./update.sh` | 빌드 건너뜀 (게임 내용 `libpatch` 만 바뀌었을 때. 빌드 디렉터리가 남아 있어야 함) |
| `./update.sh [설치 디렉터리] [빌드 디렉터리]` | 기본값 `/home/olddos/jurassic`, `/tmp/jurassic-build` |

- 빌드 기록은 `/tmp/jurassic-build.log` 에 남습니다.
- 머드가 끊기는 시간은 끄기부터 다시 뜰 때까지 몇 초입니다. 접속자 자료는 저장됩니다.

### 머드 끄기 / 켜기 — `bin/killmud`

설치 디렉터리(`/home/olddos/jurassic`)의 `bin/` 에서 씁니다.

| 명령 | 하는 일 |
|---|---|
| `bin/killmud` | `startmud` 와 드라이버를 모두 끔 |
| `bin/killmud -r` | 드라이버만 끔. `startmud` 가 10 초 뒤 다시 띄움 (재시작) |
| `cd bin && nohup ./startmud > /dev/null 2>&1 &` | 꺼 둔 머드 켜기 (이미 떠 있으면 시작하지 않음) |
| `pgrep -fl 'startmud\|driver config.jurassic'` | 떠 있는지 확인 (CentOS 6 의 pgrep 은 `-a` 가 없음) |

드라이버는 kill(SIGTERM) 을 받으면 접속자 자료를 저장하고 끝납니다. 게임 안에서는 운영자(하늘소)로 `0 다운` 해도 됩니다.

무한대전도 같은 방식입니다: `src/muhan/update.sh`, `/home/olddos/muhan/bin/killmud` (`-r` 다시 띄우기). 처음 실행하면 데이터를 풀어 새로 설치하고, 그다음부터는 실행 파일만 바꿉니다 (플레이어와 게임 안에서 고친 방은 그대로).

머드 게임
-
대문 메뉴의 **7. 게임 마당** (`go game`) 에서 세 가지 텍스트 게임을 즐길 수 있습니다.

### 1. 용사의 전설 — `go hero`

옛 BBS 도어 게임 방식의 1인용 텍스트 RPG 로, 이 BBS 를 위해 새로 만든 게임입니다. 여럿이 같은 세계에서 만나는 머드와 달리 혼자 진행하고, 다른 용사와는 순위와 마을 소식으로 이어집니다. (`src/hero.cpp`, `bin/hero`)

- 마을: 숲 사냥, 사부님과의 결투(레벨업), 대장간/갑옷 가게, 잡화점, 약방, 전장(돈 맡기기)
- 숲 사냥은 하루 15 번. 쓰러지면 지니고 있던 돈을 잃고 다음 날 깨어납니다
- 레벨 1~12, 몬스터 66 종, 무기 20 종, 갑옷 20 종, 장신구 10 종, 소모품 4 종
- 12 레벨에 붉은 용을 쓰러뜨리면 영웅 칭호를 얻고 처음부터 다시 시작합니다
- 장면마다 아스키 그림, 체력 막대가 있는 2 단 전투 화면, 함께 보는 용사 순위와 마을 소식 (MySQL `game_hero`, `game_news` 테이블)

<img width="850" alt="용사의 전설 타이틀 화면" src="docs/screenshots/hero_title.png" />

### 2. 쥬라기공원 2 — `go jurassic`

쥬라기공원은 1994 년 천리안에서 서비스를 시작한 국내 초기 상용 머드입니다.
이 게임은 **쥬라기공원 2 의 HanLP 복원판** (JuDessic Park 1.5, 2001, MaGuN) 을 개조된 HanLP 드라이버 대신 원본 **MudOS v22.2b14** 에서 돌아가도록 옮긴 것입니다.

- `src/jurassic/driver` — [maldorne/mudos](https://github.com/maldorne/mudos) 의 MudOS v22.2b14 + 한글 수정
  - `packages/hangul.c` — HanLP 라이브러리가 쓰는 한글 조사 함수 (`han_iga` 이/가, `han_obj` 을/를, `han_tool` 으로/로 등). 완성형 2,350 자의 받침 표 사용
  - `add_action.c` — 한국어 어순: 마지막 단어를 명령으로 (`칼 가져`)
  - `backend.c` — glibc 의 `ualarm()` 이 1 초 이상 값을 거부해 heart_beat 가 돌지 않던 문제를 `setitimer()` 로 해결
  - `local_options` — HanLP 라이브러리가 기대하는 옵션 (`PACKAGE_HANGUL`, `INTERACTIVE_CATCH_TELL` 등)
- `src/jurassic/jp2_v15.tgz` — 원본 머드 라이브러리 (EUC-KR)
- `src/jurassic/libpatch` — 설치할 때 원본 위에 덮어쓰는 파일: 원래 HanLP 드라이버에 있던 함수(`ktime`, 전투 메시지 등)와 운영자 설정
- `build.sh` 로 엔진을 빌드하고 (기본 64 비트, `BITS=32` 로 32 비트), `deploy.sh` 로 라이브러리와 함께 설치하며, `bin/startmud` 가 머드를 계속 띄워 둡니다
- 머드는 `127.0.0.1:4444` 에서 접속을 받습니다. BBS 는 `bin/mudlink` (`src/mudlink.cpp`) 로 사용자를 연결합니다. 이 프로그램은 로컬 머드 포트에만 접속하고, 줄 입력과 한글 백스페이스, 비밀번호 숨김을 처리하며, `끝` 이나 `/x` 로 BBS 에 돌아옵니다.

업데이트와 끄기/켜기는 위의 **업데이트** 항목을 보세요. 처음 설치와 운영자 설정은 INSTALL.TXT 의 **머드 게임 연결** 을 참고하세요.

<img width="850" alt="쥬라기공원 2 접속 화면" src="docs/screenshots/jurassic.png" />

출처와 라이선스:
- MudOS 의 저작권은 Lars Pensjö, Erik Kay, Adam Beeman, Stephan Iannce, John Garnett, Tim Hollebeek 에게 있으며 **금전적 이익을 위해 사용할 수 없습니다** (`src/jurassic/driver/Copyright`). 비상업 용도로만 운영하세요.
- 쥬라기공원 2 복원판은 MaGuN (HanLP) 이 만들었고, 크루젼(이상신)님과 꼬마기사(김진태)님이 나우누리 머드동호회에 공개한 구공원 라이브러리의 지역 데이터를 사용했습니다. 원작 쥬라기공원은 송재경, 김성배 님이 만들었습니다. 원작의 권리 관계는 확인되지 않았습니다.

### 3. 무한대전 — `go muhan`

무한대전은 Mordor 2.5 (Brett J. Vickers, 1992) 를 금오공대 네트워크 동아리가 한글화하고 고친 1990 년대 한글 머드입니다. 명령어도 한글입니다 (`봐`, `북`, `정보`, `끝` ...). 원본은 [nicecapj/mudmuhan](https://github.com/nicecapj/mudmuhan) 입니다.

- `src/muhan/game/src` — 엔진 소스 (EUC-KR). 원본은 리눅스 커널 2.0 (32 비트) 용입니다
- `src/muhan/muhan_data.tgz` — 방 3,218 개와 몬스터, 물건, 도움말, 게시판 (원본 그대로)
- 64 비트로 옮기며 고친 것
  - 데이터 파일에 C 구조체를 그대로 저장하는 방식이라 64 비트에서는 크기가 달라집니다. `disk32.c` 가 읽고 쓸 때 32 비트 배치로 바꿔 원본 데이터를 그대로 씁니다. 변환 코드는 `tools/gen_disk32.py` 가 `mstruct.h` 에서 만들고, 103 개 필드의 위치가 32 비트와 같은지 `tools/layout_check.c` 로 검사했습니다
  - 가변 인자를 `int` 매개변수로 흉내 낸 `print`/`logf` 등에서 포인터가 잘리지 않게, 선언 없이 쓴 표준 함수(`malloc` 등) 정리
  - 같은 플레이를 32 비트 빌드와 64 비트 빌드에 돌려 출력이 같은지 확인했습니다
- `127.0.0.1:4100` 에서만 접속을 받고 BBS 는 `bin/mudlink` 로 연결합니다. 운영자는 `하늘소` 입니다

출처와 라이선스: Mordor 는 Brett J. Vickers 의 저작물로 비상업 용도로만 쓸 수 있습니다. 무한대전 한글판은 금오공대 네트워크 동아리가 만들었고, 배포 권리는 확인되지 않았습니다.

### 4. 조크 I — `go zork`

Infocom 의 Zork I: The Great Underground Empire (1980) 를 한글로 옮긴 1 인용 문 게임입니다. 하얀 집 서쪽에서 시작해 지하 제국의 보물 19 개를 트로피 진열장에 모으면 끝납니다 (350 점).

- 원작 게임 파일 `src/zork1/zork1.z3` (Release 119) 을 그대로 돌리는 작은 Z-machine 실행기 `src/zork1/zork1.cpp` 를 만들었습니다. 퍼즐, 도둑, 트롤, 점수는 원작과 똑같이 움직입니다
- 매 턴 아래에 고르기 메뉴가 나옵니다: 갈 곳(출구), 보이는 것, 가진 것, 둘러보기/소지품/점수. 물건 번호를 고르면 그 물건에 할 수 있는 동작(보기, 집기, 열기, 읽기, 켜기, 넣기, 공격 ...)을 고르고, 실행기가 영어 명령을 만들어 넣습니다. `/m` 으로 끄고 켭니다
- 매 턴 화면을 지우고 한 화면으로 다시 그립니다: 맨 위 상태 줄(위치, 점수, 횟수), 방금 넣은 명령과 게임의 대답, 아래에 지도와 메뉴. 글이 길면 [계속] 으로 나눕니다 (`/s` 로 예전처럼 흘러가게)
- 방을 옮길 때마다 둘레 지도가 나옵니다: 지금 방을 가운데에, 8 방향 이웃 방과 위/아래/안/밖. 가 본 방은 이름, 안 가 본 방은 `?` (`/g` 끄기)
- 모든 방(110 곳, 이름으로 78 가지)에 안시 그림이 있어 방에 들어갈 때 나옵니다 (`src/zork1/art/*.ans`). `tools/make_art.py` 의 도트 그림과 `tools/art_small.txt` 로 만듭니다. 메뉴의 "그림" 으로 다시 봅니다
- 명령을 원작 그대로 영어로 넣어도 됩니다 (`n`, `look`, `take lamp`, `open mailbox`, `inventory` ...). 화면에 나오는 글만 번역표 `src/zork1/ko.txt` (문장 1,519 개) 로 바꿉니다
  - 게임은 "The " + 물건 이름 + " is now open." 처럼 조각으로 찍으므로, 이름 뒤의 조사는 `{이}` `{을}` `{은}` ... 로 적어 두고 앞 글자의 받침에 맞춰 고릅니다
  - 한 줄의 조각들을 모아 `There is nothing behind the %o.` 처럼 문장 틀째로 번역표에 넣으면 한글 어순으로 다시 짭니다 (`%o` 물건, `%n` 숫자)
  - 번역표에 없는 문장은 `data/zork1/missing.txt`, 조각으로 번역된 줄은 `data/zork1/lines.txt` 에 모이니 보고 번역표에 더하면 됩니다
  - 문장은 `src/zork1/tools/zstrings.py` 로 게임 파일에서 뽑았습니다
- `/x` 로 나가거나 접속이 끊기면 그 자리를 `data/zork1/<아이디>.sav` 에 저장하고, 다음에 들어오면 이어서 할지 묻습니다. 원작의 `save` / `restore` 도 같은 파일을 씁니다. `/?` 도움말

출처와 라이선스: Zork I 원작 소스와 게임 파일은 [historicalsource/zork1](https://github.com/historicalsource/zork1) 에 있으며 MIT License (Copyright (c) 2025 Microsoft) 입니다 (`src/zork1/LICENSE`). ZORK 는 상표이므로 메뉴에는 한글 이름 "조크" 를 씁니다.

