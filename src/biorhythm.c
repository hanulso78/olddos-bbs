// 바이오리듬
//   biorhythm YYYY MM DD          짧게 (로그인 화면, bio 명령): 오늘의 막대 4 줄과 한 줄 풀이
//   biorhythm YYYY MM DD full     자세히 (생활정보 메뉴): 오늘 + 앞뒤 2 주 그래프 + 위험일
// 신체 23 일, 감성 28 일, 지성 33 일, 지각 38 일 주기. 태어난 날을 0 일째로 센다.
// 처음 판: 바이오리듬 계산 프로그램 V1.0 by TeIn (2015)

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#define PI 3.1415926535897932384626433832795

#define C_WHITE  "\033[=15F"
#define C_GRAY   "\033[=7F"
#define C_DARK   "\033[=8F"
#define C_YELLOW "\033[=14F"
#define C_RED    "\033[=12F"
// 배경색은 쓰지 않는다 (ESC[0m 은 이야기에서 BBS 바탕까지 바꿔 버리고, 밝은 배경은 깜빡일 수 있다)

struct rhythm {
	const char *name;
	int days;
	const char *color;
	int bg;				// (예전 배경 칠하기 색, 지금은 쓰지 않음)
	const char *good, *bad;		// 좋은 날, 나쁜 날 풀이
};

static const struct rhythm R[4] = {
	{ "신체", 23, "\033[=12F", 4,"몸이 가벼운 날. 운동이나 바깥 일 하기 좋아요.", "몸이 무거운 날. 무리하지 말고 푹 쉬세요." },
	{ "감성", 28, "\033[=10F", 2,"기분이 좋은 날. 사람 만나고 이야기하기 좋아요.", "예민해지기 쉬운 날. 말 한마디 조심하세요." },
	{ "지성", 33, "\033[=11F", 3,"머리가 맑은 날. 공부나 프로그래밍하기 좋아요.", "집중이 잘 안 되는 날. 큰 결정은 미뤄 두세요." },
	{ "지각", 38, "\033[=13F", 5,"눈치와 직감이 빠른 날. 감을 믿어 보세요.", "직감이 흐린 날. 한 번 더 확인하세요." },
};

// 서기 1 년 1 월 1 일부터 센 날 수 (그레고리력)
static long day_number(int y, int m, int d)
{
	static const int before[12] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
	long yy = y - 1;
	long n = yy * 365 + yy / 4 - yy / 100 + yy / 400 + before[m - 1] + d;
	if ( m > 2 && ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0) ) n++;
	return n;
}

static double value(int k, long t)
{
	return sin(2 * PI * (double)t / R[k].days);
}

// 위험일: 리듬이 0 을 지나는 날 (오늘과 내일 사이에 부호가 바뀜)
static int critical(int k, long t)
{
	double a = value(k, t), b = value(k, t + 1);
	if ( t % R[k].days == 0 ) return 1;
	return (a > 0 && b <= 0) || (a < 0 && b >= 0);
}

static const char *state(int k, long t)
{
	double v = value(k, t);
	if ( critical(k, t) ) return C_RED "위험일" C_WHITE;
	if ( v >= 0.6 ) return "아주 좋음";
	if ( v >= 0.15 ) return "좋음";
	if ( v > -0.15 ) return "보통";
	if ( v > -0.6 ) return "낮음";
	return "바닥";
}

// -100 ~ +100 막대: 왼쪽 10 칸(-), 가운데 |, 오른쪽 10 칸(+). 한 칸 = 10%, 2 칸 폭.
// 리듬 이름과 같은 (밝은) 글자색의 ■ 로 찍는다. 밝은 색을 배경으로 칠하면 이야기에서 깜빡일 수 있다.
static void bar(int k, double v)
{
	int n = (int)floor(fabs(v) * 10 + 0.5), i;
	if ( n > 10 ) n = 10;
	for ( i = 0; i < 10; i++ ) {
		if ( v < 0 && i >= 10 - n ) printf("%s■", R[k].color);
		else printf(C_DARK "..");		// 빈 칸은 흐린 점
	}
	printf(" " C_GRAY "|");
	for ( i = 0; i < 10; i++ ) {
		if ( v > 0 && i < n ) printf("%s■", R[k].color);
		else printf(C_DARK "..");
	}
	printf(" " C_WHITE);
}

static void today_lines(long t)
{
	int k;
	printf(" " C_GRAY "     -100                 0                 +100" C_WHITE "\r\n");
	for ( k = 0; k < 4; k++ ) {
		double v = value(k, t);
		const char *arrow = value(k, t + 1) > v ? C_YELLOW "▲" C_WHITE : C_GRAY "▼" C_WHITE;
		printf(" %s%s" C_WHITE " ", R[k].color, R[k].name);
		bar(k, v);
		printf("%4d%% %s %s\r\n", (int)floor(v * 100 + (v < 0 ? -0.5 : 0.5)), arrow, state(k, t));
	}
}

// 오늘의 풀이 (최대 lines 줄): 위험일, 가장 좋은 리듬, 가장 나쁜 리듬 순
static void advice(long t, int lines)
{
	int k, best = 0, worst = 0, crit = -1, shown = 0;
	for ( k = 0; k < 4; k++ ) {
		if ( value(k, t) > value(best, t) ) best = k;
		if ( value(k, t) < value(worst, t) ) worst = k;
		if ( crit < 0 && critical(k, t) ) crit = k;
	}
	if ( crit >= 0 && shown < lines ) {
		printf(" " C_RED "※" C_WHITE " 오늘은 %s%s" C_WHITE " 위험일: 리듬이 바뀌는 날이라 실수하기 쉬워요.\r\n", R[crit].color, R[crit].name);
		shown++;
	}
	if ( value(best, t) >= 0.15 && best != crit && shown < lines ) {
		printf(" " C_YELLOW "☞" C_WHITE " %s%s" C_WHITE ": %s\r\n", R[best].color, R[best].name, R[best].good);
		shown++;
	}
	if ( value(worst, t) <= -0.15 && worst != crit && shown < lines ) {
		printf(" " C_GRAY "☞" C_WHITE " %s%s" C_WHITE ": %s\r\n", R[worst].color, R[worst].name, R[worst].bad);
		shown++;
	}
	if ( shown == 0 && lines > 0 ) printf(" " C_GRAY "☞" C_WHITE " 크게 좋지도 나쁘지도 않은 무난한 날입니다.\r\n");
}

// y-m-d 에서 n 일 뒤(앞)의 날짜. '일' 을 돌려주고 mon 에 '월'
static int shift_date(int y, int m, int d, int n, int *mon)
{
	struct tm tmv;
	memset(&tmv, 0, sizeof(tmv));
	tmv.tm_year = y - 1900; tmv.tm_mon = m - 1; tmv.tm_mday = d + n; tmv.tm_hour = 12;
	mktime(&tmv);
	if ( mon ) *mon = tmv.tm_mon + 1;
	return tmv.tm_mday;
}

// 앞뒤 2 주 그래프: 2 일 전 ~ 12 일 뒤 (15 일). 하루를 4 칸으로 나눠 * 로 선을 그린다 (세로 11 줄)
static void chart(long t, int y, int m, int d)
{
	enum { FROM = -2, DAYS = 15, STEP = 4, COLS = DAYS * STEP, ROWS = 11 };
	signed char who[ROWS][COLS];
	int r, x, k;
	memset(who, -1, sizeof(who));
	for ( x = 0; x < COLS; x++ ) {
		double tt = (double)t + FROM + (double)x / STEP;
		for ( k = 3; k >= 0; k-- ) {	// 겹치면 앞의 리듬(신체)이 보인다
			double v = sin(2 * PI * tt / R[k].days);
			who[(int)floor((1 - v) / 2 * (ROWS - 1) + 0.5)][x] = (signed char)k;
		}
	}
	for ( r = 0; r < ROWS; r++ ) {
		const char *label = r == 0 ? "+100" : r == ROWS / 2 ? "   0" : r == ROWS - 1 ? "-100" : "    ";
		int last = -2;
		printf(" " C_GRAY "%s |", label);
		for ( x = 0; x < COLS; x++ ) {
			int c = who[r][x];
			// 색이 바뀔 때만 색 코드를 보낸다
			if ( c != last ) { printf("%s", c >= 0 ? R[c].color : C_DARK); last = c; }
			if ( c >= 0 ) putchar('*');
			else if ( x == -FROM * STEP ) putchar(':');		// 오늘
			else if ( r == ROWS / 2 ) putchar(x % 2 ? ' ' : '.');	// 0 줄
			else putchar(' ');
		}
		printf(C_WHITE "\r\n");
	}
	// 날짜 줄: 그날이 시작하는 칸 아래
	printf(" " C_GRAY "      ");
	for ( x = 0; x < DAYS; x++ ) {
		int dd = shift_date(y, m, d, FROM + x, NULL);
		if ( x == -FROM ) printf(C_YELLOW "%-4d" C_GRAY, dd);
		else printf("%-4d", dd);
	}
	printf(C_WHITE "\r\n");
}

// 앞으로 2 주 안의 위험일
static void criticals(long t, int y, int m, int d)
{
	int k, i, any = 0;
	printf(" " C_GRAY "앞으로 2 주 위험일:" C_WHITE);
	for ( k = 0; k < 4; k++ ) {
		for ( i = 1; i <= 14; i++ ) {
			int mon, dd;
			if ( !critical(k, t + i) ) continue;
			dd = shift_date(y, m, d, i, &mon);
			printf(" %s%s" C_WHITE " %d/%d", R[k].color, R[k].name, mon, dd);
			any = 1;
			break;	// 리듬마다 가장 가까운 날 하나
		}
	}
	if ( !any ) printf(" 없음");
	printf("\r\n");
}

int main(int argc, char **argv)
{
	int uy, um, ud, ty, tm_, td, full;
	long t;
	time_t now;
	struct tm *lt;

	if ( argc < 4 ) {
		printf("usage: %s YYYY MM DD [full]\n", argv[0]);
		return 1;
	}
	uy = atoi(argv[1]); um = atoi(argv[2]); ud = atoi(argv[3]);
	full = (argc > 4 && !strcmp(argv[4], "full"));

	now = time(NULL);
	lt = localtime(&now);
	ty = lt->tm_year + 1900; tm_ = lt->tm_mon + 1; td = lt->tm_mday;

	if ( uy < 1900 || um < 1 || um > 12 || ud < 1 || ud > 31 || day_number(uy, um, ud) > day_number(ty, tm_, td) ) {
		printf(" 생일이 바르게 등록되어 있지 않아 바이오리듬을 볼 수 없습니다.\r\n");
		printf(" " C_GRAY "(pe 명령으로 회원 정보의 생일을 고쳐 주세요)" C_WHITE "\r\n");
		return 0;
	}
	t = day_number(ty, tm_, td) - day_number(uy, um, ud);	// 태어난 날이 0 일째

	if ( !full ) {
		printf(" " C_YELLOW "오늘의 바이오리듬" C_WHITE "  " C_GRAY "%d년 %d월 %d일 / 태어난 지 %ld 일째" C_WHITE "\r\n", ty, tm_, td, t);
		today_lines(t);
		advice(t, 1);
		return 0;
	}

	{
		int k;
		printf(" %d년 %d월 %d일생, 태어난 지 " C_YELLOW "%ld" C_WHITE " 일째  ", uy, um, ud, t);
		for ( k = 0; k < 4; k++ ) printf("  %s*%s", R[k].color, R[k].name);
		printf(C_WHITE "\r\n");
	}
	today_lines(t);
	chart(t, ty, tm_, td);
	criticals(t, ty, tm_, td);
	advice(t, 1);
	return 0;
}
