#include "main.h"
#include <sys/time.h>
#include <math.h>

// ------------------------------------------------------------------
// 윷놀이 : 컴퓨터와 한판 (말 4 개씩)
//   bin/yut <호스트이름> <아이디> <tty>
//   윷가락 넷을 던져 (배 = 평평한 면, 칼자국 ××) 도 개 걸 윷 모, 표시한 가락만 배면 빽도.
//   윷 / 모 / 잡기면 한 번 더. 내 말끼리 겹치면 업는다. 모, 뒷모, 방에 멈추면 지름길.
//   참먹이에 온 말은 다음에 움직이면 난다. 넷 모두 나면 이긴다.
// ------------------------------------------------------------------

struct termio sys_term;

char tty[10];

static std::string user_id;
static std::string user_nick;

#define Y_W		"\033[=15F"
#define Y_Y		"\033[=14F"
#define Y_R		"\033[=12F"
#define Y_G		"\033[=7F"
#define Y_DG	"\033[=8F"
#define Y_C		"\033[=11F"
#define Y_GR	"\033[=10F"
#define Y_M		"\033[=13F"
#define BG_OFF	"\033[=1G"

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
}

int host_close(void)
{
	printf(BG_OFF Y_W);
	fflush(stdout);
	database::close();
    ioctl(0, TCSETAF, &sys_term);
    exit(1);
}

static void print_header(const char *head_title)
{
	printf(ESC_CLEAR);
    printf("\033[1;1H");
	printf("\033[=9F\033[=1G%s\033[=15F\033[=1G", repeat("─", 40).c_str());
    printf("\033[1;1H");
	printf("\033[1A\033[7m%s\033[0m", host_name);
	int center = (80 - strlen(strip_ansi_codes(head_title))) / 2;
	if ( center < 0 ) center = 0;
    printf("\033[2;1H");
	printf("\r\033[%dC%s", center, head_title);
    printf("\033[3;1H");
	printf("\033[=0F\033[=1G%s\033[=15F\033[=1G", repeat("━", 40).c_str());
    printf("\033[4;1H");
}

static void at(int r, int c) { printf("\033[%d;%dH", r, c); }

static void wait_enter(void)
{
	printf("\r\n " Y_G "[Enter] 를 누르세요." Y_W);
	press_enter();
}

// ------------------------------------------------------------------
// 판
//   바깥 0~19 (0 = 참먹이, 아래 오른쪽 모서리에서 위로 돌아 시계 반대 방향)
//   5 = 모 (위 오른쪽), 10 = 뒷모 (위 왼쪽), 15 = 찌모 (아래 왼쪽)
//   20, 21 : 모 -> 방 대각선,  22 = 방 (가운데),  23, 24 : 뒷모 -> 방
//   25, 26 : 방 -> 참먹이,  27, 28 : 방 -> 찌모 (그리기만)
// ------------------------------------------------------------------
enum { N_NODES = 29, CENTER = 22, WAIT = -1, DONE = 99 };

static const double NODE_X[N_NODES] = {
	5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4,
	4.1667, 3.3333, 2.5, 0.8333, 1.6667, 3.3333, 4.1667, 1.6667, 0.8333 };
static const double NODE_Y[N_NODES] = {
	5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 5, 5, 5, 5,
	0.8333, 1.6667, 2.5, 0.8333, 1.6667, 3.3333, 4.1667, 3.3333, 4.1667 };

static int node_row(int n) { return (int)floor(NODE_Y[n] * 3 + 0.5); }
static int node_col(int n) { return (int)floor(NODE_X[n] * 7 + 0.5); }

// 한 걸음 앞으로. first: 이번 움직임의 첫 걸음 (모 / 뒷모 / 방에 서 있었으면 지름길)
static int step_forward(int n, bool first)
{
	if ( n == WAIT ) return 1;
	if ( first ) {
		if ( n == 5 ) return 20;
		if ( n == 10 ) return 23;
		if ( n == CENTER ) return 25;
	}
	if ( n >= 1 && n <= 18 ) return n + 1;
	switch ( n ) {
	case 19: return 0;		// 참먹이
	case 0: return DONE;		// 참먹이에서 한 걸음 = 남
	case 20: return 21;
	case 21: return CENTER;
	case CENTER: return 25;	// 방을 지나가도 참먹이 쪽으로
	case 23: return 24;
	case 24: return CENTER;
	case 25: return 26;
	case 26: return 0;
	}
	return DONE;
}

static int step_back(int n)
{
	if ( n >= 2 && n <= 19 ) return n - 1;
	switch ( n ) {
	case 1: return 0;
	case 0: return 19;
	case 20: return 5;
	case 21: return 20;
	case CENTER: return 21;
	case 23: return 10;
	case 24: return 23;
	case 25: return CENTER;
	case 26: return 25;
	}
	return n;
}

// steps 만큼 (빽도 = -1). 대기 말은 앞으로만
static int move_to(int n, int steps)
{
	if ( steps < 0 ) return n == WAIT ? WAIT : step_back(n);
	for ( int i = 0; i < steps; i++ ) {
		n = step_forward(n, i == 0);
		if ( n == DONE ) return DONE;
	}
	return n;
}

// 나기까지 남은 걸음 (지름길로)
static int remaining(int n)
{
	if ( n == DONE ) return 0;
	int c = 0;
	while ( n != DONE && c < 40 ) { n = step_forward(n, true); c++; }
	return c;
}

// ------------------------------------------------------------------
// 놀이 상태
// ------------------------------------------------------------------
static int pieces[2][4];			// [0] 나, [1] 컴퓨터. WAIT / 칸 / DONE
static std::vector<int> throws;		// 아직 쓰지 않은 던지기 (도 1 ~ 모 5, 빽도 -1)
static std::vector<std::string> logs;
static int sticks[4];				// 마지막 윷가락 (1 = 배)
static int last_result = 0;
static int turns = 0;

static const char *result_name(int r)
{
	switch ( r ) {
	case -1: return "빽도";
	case 1: return "도";
	case 2: return "개";
	case 3: return "걸";
	case 4: return "윷";
	case 5: return "모";
	}
	return "";
}

static void add_log(const std::string &s)
{
	logs.push_back(s);
	while ( logs.size() > 4 ) logs.erase(logs.begin());
}

static int count_at(int team, int pos)
{
	int c = 0;
	for ( int i = 0; i < 4; i++ ) if ( pieces[team][i] == pos ) c++;
	return c;
}

// 내 말 무리 (판 위의 칸) 에 붙일 글자: 앞선 것부터 A B C D
static std::vector<int> my_groups(void)
{
	std::vector<int> g;
	for ( int i = 0; i < 4; i++ ) {
		int p = pieces[0][i];
		if ( p != WAIT && p != DONE && std::find(g.begin(), g.end(), p) == g.end() ) g.push_back(p);
	}
	for ( unsigned int a = 0; a < g.size(); a++ )
		for ( unsigned int b = a + 1; b < g.size(); b++ )
			if ( remaining(g[b]) < remaining(g[a]) ) std::swap(g[a], g[b]);
	return g;
}

// ------------------------------------------------------------------
// 그리기
// ------------------------------------------------------------------
#define BR 5		// 판 맨 윗줄 (화면 행)
#define BC 4		// 판 왼쪽 (화면 열)
#define BW 37
#define BH 16

static void draw_board(void)
{
	std::string cell[BH][BW];
	std::string color[BH][BW];
	for ( int r = 0; r < BH; r++ ) for ( int c = 0; c < BW; c++ ) { cell[r][c] = " "; color[r][c] = Y_DG; }
	// 바깥 선
	for ( int c = 0; c <= 35; c++ ) { cell[0][c] = "-"; cell[15][c] = "-"; }
	for ( int r = 0; r <= 15; r++ ) { cell[r][0] = "|"; cell[r][35] = "|"; }
	// 대각선: 한 줄에 한 글자씩
	for ( int r = 1; r < 15; r++ ) {
		int c1 = (int)floor(r * 35.0 / 15 + 0.5);
		int c2 = 35 - c1;
		if ( cell[r][c1] == " " ) cell[r][c1] = "\\";
		if ( cell[r][c2] == " " ) cell[r][c2] = "/";
	}
	// 자리
	std::vector<int> g = my_groups();
	for ( int n = 0; n < N_NODES; n++ ) {
		int r = node_row(n), c = node_col(n);
		bool big = (n == 0 || n == 5 || n == 10 || n == 15 || n == CENTER);
		std::string s = big ? "◎" : "○";
		std::string col = big ? Y_W : Y_G;
		int mine = count_at(0, n), com = count_at(1, n);
		if ( mine > 0 ) {
			int k = std::find(g.begin(), g.end(), n) - g.begin();
			char b[4];
			snprintf(b, sizeof(b), "%c%c", 'A' + k, mine > 1 ? '0' + mine : ' ');
			s = b;
			col = Y_Y;
		} else if ( com > 0 ) {
			static const char *circ[5] = { "", "◆", "②", "③", "④" };
			s = circ[com];
			col = Y_R;
		}
		cell[r][c] = s;
		color[r][c] = col;
		if ( c + 1 < BW ) cell[r][c + 1] = "";
	}
	for ( int r = 0; r < BH; r++ ) {
		at(BR + r, BC);
		const char *cur = "";
		for ( int c = 0; c < BW; c++ ) {
			if ( cell[r][c].empty() ) continue;
			if ( color[r][c] != cur ) { printf("%s", color[r][c].c_str()); cur = color[r][c].c_str(); }
			printf("%s", cell[r][c].c_str());
		}
	}
	printf(Y_W);
	// 자리 이름
	at(BR + 16, BC + 33); printf(Y_G "참먹이" Y_W);
	at(BR - 1, BC + 34); printf(Y_G "모" Y_W);
	at(BR - 1, BC - 1); printf(Y_G "뒷모" Y_W);
	at(BR + 16, BC - 1); printf(Y_G "찌모" Y_W);
}

// 윷가락 하나: 폭 4 칸, 높이 7 줄. 배 (평평한 면) 는 밝은 나무색에 칼자국, 등은 어두운 둥근 면
#define SR 5
#define SC 48
static void draw_stick(int i, int flat)
{
	int c = SC + i * 7;
	for ( int r = 0; r < 7; r++ ) {
		at(SR + r, c);
		if ( flat ) {
			printf("\033[=6G\033[=0F");
			// 한 줄 4 칸: 완성형 기호는 2 칸이라 두 개까지
			if ( i == 3 && r == 3 ) printf("\033[=12F◆\033[=0F  ");	// 빽도 표시
			else if ( r == 1 || r == 3 || r == 5 ) printf("××");
			else printf("    ");
		} else {
			// 등 (둥근 면): 어두운 나무결
			printf("\033[=0G\033[=8F");
			printf(r == 0 || r == 6 ? "    " : "▒▒");
		}
	}
	printf(BG_OFF Y_W);
}

static void draw_sticks(void)
{
	for ( int i = 0; i < 4; i++ ) draw_stick(i, sticks[i]);
	at(SR + 7, SC);
	printf("\033[K");
	if ( last_result ) {
		const char *col = last_result >= 4 ? Y_Y : last_result < 0 ? Y_R : Y_C;
		std::string name = result_name(last_result);
		std::string text = name + (last_result >= 4 ? "! 한 번 더" : "!");
		int pad = (25 - (int)text.size()) / 2;
		at(SR + 7, SC + (pad > 0 ? pad : 0));
		printf("%s%s" Y_W, col, text.c_str());
	}
}

static void draw_panel(void)
{
	for ( int t = 0; t < 2; t++ ) {
		int wait = count_at(t, WAIT), done = count_at(t, DONE);
		at(14 + t, 46);
		printf("\033[K%s%-6s" Y_W "대기 ", t == 0 ? Y_Y : Y_R, t == 0 ? "나" : "컴퓨터");
		for ( int i = 0; i < 4; i++ ) printf("%s", i < wait ? (t == 0 ? Y_Y "○" : Y_R "○") : "  ");
		printf(Y_W " 난 말 ");
		for ( int i = 0; i < 4; i++ ) printf("%s", i < done ? (t == 0 ? Y_Y "●" : Y_R "●") : Y_DG "·");
		printf(Y_W);
	}
	at(17, 46);
	printf("\033[K" Y_G "남은 윷:" Y_W);
	for ( unsigned int i = 0; i < throws.size(); i++ ) printf(" " Y_C "[%s]" Y_W, result_name(throws[i]));
	for ( unsigned int i = 0; i < 4; i++ ) {
		at(18 + i, 46);
		printf("\033[K");
		if ( i < logs.size() ) printf("%s", logs[i].c_str());
	}
}

static void draw_all(void)
{
	print_header(Y_C "윷놀이" Y_W);
	draw_board();
	draw_sticks();
	draw_panel();
	at(23, 1);
	printf("\033[K");
	fflush(stdout);
}

// ------------------------------------------------------------------
// 던지기
// ------------------------------------------------------------------
static int throw_once(void)
{
	// 굴러가는 모습: 가락들이 몇 번 뒤집힌다
	for ( int f = 0; f < 7; f++ ) {
		for ( int i = 0; i < 4; i++ ) draw_stick(i, rand() % 2);
		at(SR + 7, SC); printf("\033[K" Y_G "     휘리릭 ..." Y_W);
		at(23, 1);
		fflush(stdout);
		usleep(90000 + f * 25000);
	}
	int flat = 0;
	for ( int i = 0; i < 4; i++ ) {
		sticks[i] = (rand() % 100) < 60 ? 1 : 0;		// 배가 나올 확률 60%
		flat += sticks[i];
	}
	int r = flat == 0 ? 5 : flat;
	if ( flat == 1 && sticks[3] ) r = -1;		// 표시한 가락만 배 = 빽도
	last_result = r;
	draw_sticks();
	at(23, 1);
	fflush(stdout);
	return r;
}

// ------------------------------------------------------------------
// 움직이기
// ------------------------------------------------------------------
struct option { int result; int from; };		// from = 칸 (WAIT = 새 말)

static std::vector<option> options(int team)
{
	std::vector<option> o;
	std::vector<int> seen;
	for ( unsigned int i = 0; i < throws.size(); i++ ) {
		int r = throws[i];
		if ( std::find(seen.begin(), seen.end(), r) != seen.end() ) continue;
		seen.push_back(r);
		std::vector<int> froms;
		for ( int k = 0; k < 4; k++ ) {
			int p = pieces[team][k];
			if ( p == DONE ) continue;
			if ( p == WAIT && r < 0 ) continue;
			if ( std::find(froms.begin(), froms.end(), p) != froms.end() ) continue;
			froms.push_back(p);
			option op; op.result = r; op.from = p;
			o.push_back(op);
		}
	}
	return o;
}

// 움직인다. 잡았으면 true
static bool apply(int team, const option &op, std::string &what)
{
	int to = move_to(op.from, op.result);
	int moved = 0;
	for ( int k = 0; k < 4; k++ ) {
		if ( pieces[team][k] == op.from ) {
			pieces[team][k] = to;
			moved++;
			if ( op.from == WAIT ) break;		// 대기 말은 하나씩 놓는다
		}
	}
	throws.erase(std::find(throws.begin(), throws.end(), op.result));
	bool caught = false;
	if ( to != DONE && to != WAIT ) {
		int other = 1 - team;
		int n = 0;
		for ( int k = 0; k < 4; k++ ) if ( pieces[other][k] == to ) { pieces[other][k] = WAIT; n++; }
		if ( n > 0 ) {
			caught = true;
			what = std::string(team == 0 ? "잡았다!" : "잡혔다!") + (n > 1 ? " (" + TO_STRING(n) + " 개)" : "");
		}
		if ( count_at(team, to) > moved ) what += " 업었다!";
	}
	if ( to == DONE ) what = moved > 1 ? TO_STRING(moved) + " 개가 났다!" : "한 개가 났다!";
	return caught;
}

// 컴퓨터가 고르기: 잡기 > 나기 > 업기 > 앞으로 많이, 상대 바로 앞 (1~5 걸음) 은 피한다
static int danger(int pos)
{
	if ( pos == WAIT || pos == DONE ) return 0;
	int d = 0;
	for ( int k = 0; k < 4; k++ ) {
		int p = pieces[0][k];
		if ( p == DONE ) continue;
		for ( int r = 1; r <= 5; r++ ) if ( move_to(p, r) == pos ) { d += (r >= 2 && r <= 3) ? 3 : 1; break; }
	}
	return d;
}

static int choose_ai(const std::vector<option> &o)
{
	int best = 0, best_score = -100000;
	for ( unsigned int i = 0; i < o.size(); i++ ) {
		int to = move_to(o[i].from, o[i].result);
		int stack = o[i].from == WAIT ? 1 : count_at(1, o[i].from);
		int score = 0;
		if ( to == DONE ) score += 60 * stack;
		else {
			if ( count_at(0, to) > 0 ) score += 120 + 20 * count_at(0, to);
			if ( count_at(1, to) > 0 ) score += 15;
			score += (remaining(o[i].from == WAIT ? 1 : o[i].from) - remaining(to)) * 4 * stack;
			score -= danger(to) * 12 * stack;
			score += danger(o[i].from) * 8 * stack;		// 위험한 자리에서 벗어나기
			if ( to == 5 || to == 10 || to == CENTER ) score += 10;
		}
		if ( o[i].result < 0 ) score -= 5;
		score += rand() % 5;
		if ( score > best_score ) { best_score = score; best = i; }
	}
	return best;
}

static bool winner(int team) { return count_at(team, DONE) == 4; }

// 사람이 고르기. 그만두면 -1
static int choose_human(const std::vector<option> &o)
{
	std::vector<int> g = my_groups();
	// 결과가 여럿이면 먼저 결과를
	std::vector<int> rs;
	for ( unsigned int i = 0; i < o.size(); i++ )
		if ( std::find(rs.begin(), rs.end(), o[i].result) == rs.end() ) rs.push_back(o[i].result);
	int r = rs[0];
	if ( rs.size() > 1 ) {
		while ( 1 ) {
			at(23, 1);
			printf("\033[K" Y_W "어느 윷을 쓸까요?");
			for ( unsigned int i = 0; i < rs.size(); i++ ) printf("  " Y_Y "%d" Y_W ") %s", i + 1, result_name(rs[i]));
			printf("  >> ");
			char b[8];
			line_input(b, 2);
			if ( !strcasecmp(b, "q") ) return -1;
			int k = atoi(b);
			if ( k >= 1 && k <= (int)rs.size() ) { r = rs[k - 1]; break; }
		}
	}
	// 그 결과로 움직일 말
	std::vector<int> idx;
	for ( unsigned int i = 0; i < o.size(); i++ ) if ( o[i].result == r ) idx.push_back(i);
	if ( idx.size() == 1 ) return idx[0];
	while ( 1 ) {
		at(23, 1);
		printf("\033[K" Y_C "[%s]" Y_W " 어느 말을?", result_name(r));
		for ( unsigned int i = 0; i < idx.size(); i++ ) {
			int f = o[idx[i]].from;
			if ( f == WAIT ) printf("  " Y_Y "N" Y_W ") 새 말");
			else printf("  " Y_Y "%c" Y_W ")", 'A' + (int)(std::find(g.begin(), g.end(), f) - g.begin()));
		}
		printf("  >> ");
		char b[8];
		line_input(b, 2);
		if ( !strcasecmp(b, "q") ) return -1;
		char ch = toupper(b[0]);
		for ( unsigned int i = 0; i < idx.size(); i++ ) {
			int f = o[idx[i]].from;
			if ( f == WAIT && ch == 'N' ) return idx[i];
			if ( f != WAIT && ch == 'A' + (int)(std::find(g.begin(), g.end(), f) - g.begin()) ) return idx[i];
		}
	}
}

// 한 사람의 차례. 그만두면 false
static bool play_turn(int team)
{
	const char *who = team == 0 ? Y_Y "나" Y_W : Y_R "컴퓨터" Y_W;
	throws.clear();
	bool again = true;
	while ( again ) {
		again = false;
		draw_all();
		if ( team == 0 ) {
			at(23, 1);
			printf("\033[K" Y_Y "[Enter]" Y_W " 윷을 던집니다   " Y_G "(Q: 그만두기)" Y_W " >> ");
			char b[8];
			line_input(b, 2);
			if ( !strcasecmp(b, "q") ) return false;
		} else {
			at(23, 1);
			printf("\033[K" Y_G "컴퓨터가 윷을 던집니다..." Y_W);
			fflush(stdout);
			sleep(1);
		}
		int r = throw_once();
		throws.push_back(r);
		add_log(std::string(who) + ": " + result_name(r) + (r >= 4 ? " (한 번 더)" : ""));
		if ( r >= 4 ) again = true;

		// 던진 것을 다 쓰거나 쓸 수 없을 때까지 (잡으면 한 번 더 던진다)
		while ( !again && !throws.empty() ) {
			std::vector<option> o = options(team);
			if ( o.empty() ) {
				add_log(std::string(who) + ": [" + result_name(throws[0]) + "] 버림 (말 없음)");
				throws.erase(throws.begin());
				continue;
			}
			draw_all();
			int k;
			if ( team == 0 ) {
				k = choose_human(o);
				if ( k < 0 ) return false;
			} else {
				usleep(700000);
				k = choose_ai(o);
			}
			std::string what;
			bool caught = apply(team, o[k], what);
			if ( !what.empty() ) add_log(std::string(team == 0 ? Y_Y : Y_R) + what + Y_W);
			if ( winner(team) ) return true;
			if ( caught ) { again = true; add_log(std::string(who) + ": 잡아서 한 번 더"); }
		}
	}
	draw_all();
	if ( team == 1 ) usleep(600000);
	return true;
}

// ------------------------------------------------------------------
// 기록
// ------------------------------------------------------------------
static void create_tables(void)
{
	mysql_query(mysql, "CREATE TABLE IF NOT EXISTS game_yut ( "
			"USER_ID VARCHAR(50) NOT NULL PRIMARY KEY, "
			"NAME VARCHAR(50) NOT NULL, "
			"WINS INT NOT NULL, "
			"LOSSES INT NOT NULL, "
			"BEST_TURNS INT NOT NULL, "		/* 가장 적은 차례로 이긴 판 (0 = 아직) */
			"DATE_TIME DATETIME NOT NULL )");
}

static void save(bool won)
{
	char q[1024];
	std::string id = database::escape(user_id.c_str());
	std::string name = database::escape(user_nick.c_str());
	int bt = won ? turns : 0;
	snprintf(q, sizeof(q), "INSERT INTO game_yut (USER_ID, NAME, WINS, LOSSES, BEST_TURNS, DATE_TIME) VALUES "
			"('%s', '%s', %d, %d, %d, NOW()) ON DUPLICATE KEY UPDATE NAME='%s', WINS=WINS+%d, LOSSES=LOSSES+%d, "
			"BEST_TURNS=IF(%d > 0 AND (BEST_TURNS = 0 OR %d < BEST_TURNS), %d, BEST_TURNS), DATE_TIME=NOW()",
			id.c_str(), name.c_str(), won ? 1 : 0, won ? 0 : 1, bt, name.c_str(), won ? 1 : 0, won ? 0 : 1, bt, bt, bt);
	mysql_query(mysql, q);
}

static void show_rank(void)
{
	print_header(Y_C "윷놀이 명예의 전당" Y_W);
	std::vector<std::map<std::string, std::string> > rows = database::fetch_rows((char*)
			"SELECT * FROM game_yut ORDER BY WINS DESC, BEST_TURNS = 0, BEST_TURNS, LOSSES LIMIT 15");
	printf("\r\n  " Y_G "%4s  %-16s %6s %6s %8s  %s" Y_W "\r\n", "순위", "이름", "이김", "짐", "최소 차례", "마지막");
	if ( rows.empty() ) printf("  " Y_G "    아직 기록이 없습니다." Y_W "\r\n");
	for ( unsigned int i = 0; i < rows.size(); i++ ) {
		bool me = rows[i]["USER_ID"] == user_id;
		std::string bt = rows[i]["BEST_TURNS"] == "0" ? "-" : rows[i]["BEST_TURNS"];
		printf("  %s%4d  %-16s %6s %6s %8s  %s" Y_W "\r\n", me ? Y_Y : Y_W, i + 1,
				string_truncate(display_text(rows[i]["NAME"]), 16, "").c_str(), rows[i]["WINS"].c_str(),
				rows[i]["LOSSES"].c_str(), bt.c_str(), rows[i]["DATE_TIME"].substr(0, 10).c_str());
	}
	wait_enter();
}

static void game(void)
{
	for ( int t = 0; t < 2; t++ ) for ( int k = 0; k < 4; k++ ) pieces[t][k] = WAIT;
	logs.clear();
	throws.clear();
	for ( int i = 0; i < 4; i++ ) sticks[i] = rand() % 2;
	last_result = 0;
	turns = 0;
	int team = rand() % 2;
	add_log(team == 0 ? Y_Y "내가 먼저 던집니다." Y_W : Y_R "컴퓨터가 먼저 던집니다." Y_W);
	while ( 1 ) {
		if ( team == 0 ) turns++;
		if ( !play_turn(team) ) return;		// 그만둠 (기록하지 않음)
		if ( winner(team) ) break;
		team = 1 - team;
	}
	bool won = winner(0);
	save(won);
	draw_all();
	at(23, 1);
	printf("\033[K");
	if ( won ) printf(Y_Y "★ 이겼습니다! " Y_W "%d 번째 차례에 말 넷이 모두 났습니다.", turns);
	else printf(Y_R "졌습니다. " Y_W "컴퓨터의 말 넷이 먼저 났습니다.");
	printf("  " Y_G "[Enter]" Y_W);
	press_enter();
}

static void title(void)
{
	while ( 1 ) {
		print_header(Y_C "윷놀이" Y_W);
		for ( int i = 0; i < 4; i++ ) sticks[i] = (i % 2 == 0);
		last_result = 0;
		draw_sticks();
		at(6, 3); printf(Y_Y "⊙" Y_W " 윷가락 넷을 던져 말 넷을 먼저");
		at(7, 6); printf("나게 하면 이깁니다. (컴퓨터와 한판)");
		at(9, 6); printf(Y_G "배 (칼자국 면) 가 1 개 도, 2 개 개," Y_W);
		at(10, 6); printf(Y_G "3 개 걸, 4 개 윷, 하나도 없으면 모." Y_W);
		at(11, 6); printf(Y_G "◆ 가락만 배면 빽도 (한 칸 뒤로)." Y_W);
		at(12, 6); printf(Y_G "윷, 모, 잡기는 한 번 더 던집니다." Y_W);
		at(13, 6); printf(Y_G "내 말끼리 겹치면 업어서 함께 갑니다." Y_W);
		at(14, 6); printf(Y_G "모, 뒷모, 방에 멈추면 지름길로." Y_W);
		at(17, 9); printf("1. 컴퓨터와 한판");
		at(18, 9); printf("2. 명예의 전당");
		at(21, 3);
		printf(ESC_ENG);
		printf("선택 (끝내기: Q) >> ");
		char cmd[8];
		line_input(cmd, 2);
		std::string c = trim(cmd);
		if ( !strcasecmp(c.c_str(), "q") || !strcasecmp(c.c_str(), "p") || !strcasecmp(c.c_str(), "x") ) return;
		if ( c == "1" ) game();
		else if ( c == "2" ) show_rank();
	}
}

int main(int argc, char **argv)
{
	if ( argc < 3 ) {
		printf("usage: %s <host_name> <user_id> [tty]\n", argv[0]);
		return 1;
	}
	snprintf(host_name, sizeof(host_name), "%s", argv[1]);
	user_id = argv[2];
	snprintf(tty, sizeof(tty), "%s", argc > 3 ? argv[3] : "");

    signal(SIGQUIT, SIG_IGN);
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
    signal(SIGHUP, (__sighandler_t)host_close);
    signal(SIGSEGV, (__sighandler_t)host_close);
    signal(SIGBUS, (__sighandler_t)host_close);

	read_settings("hanulso.cfg");

    ioctl(0, TCGETA, &sys_term);
	raw_mode();

	struct timeval tv;
	gettimeofday(&tv, NULL);
	srand(tv.tv_usec ^ getpid());

	if ( database::open() == false )
		exit(1);
	create_tables();

	bool exist;
	std::map<std::string, std::string> user = database::user_info((char*)user_id.c_str(), &exist);
	user_nick = display_text(user["NICK_NAME"]);
	if ( user_nick.empty() ) user_nick = user_id;

	title();

	host_close();
	return 0;
}
