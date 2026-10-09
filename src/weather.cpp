#include "main.h"

struct termio sys_term;

char title[1024] = "날씨 정보";

char tty[10];

char host_name[256];

#include "regions.h"

// 시간별 예보
struct forecast {
	std::string time;	// YYYY-MM-DDTHH:MM
	double temp;
	int pop;
	int code;
	double ws;
	int wd;
};

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

	return;
}

/* 프로그램 종료 루틴 */
int host_close (void)
{
    ioctl(0, TCSETAF, &sys_term);
    exit(1);
}

void prompt(char *cmd)
{
	printf(ESC_ENG);
	printf("이동(번호) 상위메뉴(P) 종료(X)\r\n");
	printf("선택 >> ");
	line_input(cmd, 30);

	std::vector<std::string> args = split_string(std::string(cmd), ' ');
	if ( args.size() == 0 ) return;

	/* 입력이 명령 코드 */
	if( !is_number(cmd) ) {
		// 종료 명령
		if ( !strcasecmp(args[0].c_str(), "x") ) {
			host_close();
		}
	}
}

// 화면 상단 헤더 출력
void print_header(const char *head_title)
{
    printf("\033[1;1H");
	printf("\033[=9F\033[=1G%s\033[=15F\033[=1G", repeat("─", 40).c_str());
    printf("\033[1;1H");
	printf("\033[1A\033[7m%s\033[0m", host_name);
	// 타이틀 출력
	int center = (80-strlen(strip_ansi_codes(head_title)))/2;
	if ( center < 0 ) center = 0;
    printf("\033[2;1H");
	printf("\r\033[%dC%s", center, head_title);
    printf("\033[3;1H");
	printf("\033[=0F\033[=1G%s\033[=15F\033[=1G", repeat("━", 40).c_str());
}

// 목록에서 하나를 고른다. 선택한 번호(0 부터), 상위메뉴(P)면 -1
int select_menu(const std::string &head_title, const std::vector<std::string> &names)
{
	int n = names.size();

	int col = 2;
	int width = 30;
	if ( n <= 15 ) {
		col = 1;
		width = 30;
	} else if ( n > 40 ) {
		col = 3;
		width = 20;
	}
	int rows = (n + col - 1) / col;

	while (1) {
		printf(ESC_CLEAR);
		print_header(head_title.c_str());

        printf("\033[4;1H");
		for (int i=0; i<n; i++) {
			printf("%5d%s", i+1, centered(names[i], width).c_str());
			if ( (i+1) % col == 0 && i+1 < n ) {
				printf("\r\n");
			}
		}

        printf("\033[%d;1H", rows+4);
		printf("%s\r\n", repeat("━", 40).c_str());

		char cmd[1024];
		prompt(cmd);

		/* 입력이 숫자 */
		if( is_number(cmd) ) {
			int no = atoi(cmd);
			if ( no >= 1 && no <= n ) {
				return no - 1;
			}
		} else {
			if (!strcasecmp(cmd, "p")) {
				return -1;
			}
		}
	}
}

// 날씨 코드 (WMO) 를 우리말로
std::string weather_name(int code)
{
	switch (code) {
		case 0: return "맑음";
		case 1: return "구름조금";
		case 2: return "구름많음";
		case 3: return "흐림";
		case 45: case 48: return "안개";
		case 51: case 53: case 55: return "이슬비";
		case 56: case 57: return "어는 이슬비";
		case 61: return "약한 비";
		case 63: return "비";
		case 65: return "강한 비";
		case 66: case 67: return "어는 비";
		case 71: return "약한 눈";
		case 73: return "눈";
		case 75: return "강한 눈";
		case 77: return "싸락눈";
		case 80: case 81: return "소나기";
		case 82: return "강한 소나기";
		case 85: case 86: return "소낙눈";
		case 95: return "뇌우";
		case 96: case 99: return "뇌우/우박";
	}
	return "-";
}

// 풍향 (도) 을 16 방위로
std::string wind_direction(int deg)
{
	static const char *dirs[] = {
		"북", "북북동", "북동", "동북동", "동", "동남동", "남동", "남남동",
		"남", "남남서", "남서", "서남서", "서", "서북서", "북서", "북북서"
	};
	if ( deg < 0 ) deg = 0;
	int idx = (int)((deg % 360) / 22.5 + 0.5) % 16;
	return dirs[idx];
}

int round_int(double v)
{
	return (v < 0) ? (int)(v - 0.5) : (int)(v + 0.5);
}

// 오늘부터 days 일 뒤의 날짜 (YYYY-MM-DD)
std::string date_after(int days)
{
	time_t t = time(NULL) + (time_t)days * 86400;
	struct tm *tm = localtime(&t);
	char buf[32];
	strftime(buf, sizeof(buf), "%Y-%m-%d", tm);
	return buf;
}

// URL 의 내용을 받아 온다
bool fetch_csv(const char *url, std::string &csv)
{
	// 임시 파일 없이 받는다 (받는 중에 끊겨도 tmp 에 남지 않게)
	return download_text(url, csv);
}

// 미세먼지 등급 (환경부 기준)
const char *dust_grade(double v, bool pm25)
{
	if ( v < 0 ) return "-";
	if ( pm25 ) {
		if ( v <= 15 ) return "좋음";
		if ( v <= 35 ) return "보통";
		if ( v <= 75 ) return "나쁨";
		return "매우나쁨";
	}
	if ( v <= 30 ) return "좋음";
	if ( v <= 80 ) return "보통";
	if ( v <= 150 ) return "나쁨";
	return "매우나쁨";
}

// Open-Meteo 대기질 예보에서 지금 시각의 미세먼지(PM10), 초미세먼지(PM2.5)
bool get_air(const region &r, double *pm10, double *pm25)
{
	char url[1024];
	snprintf(url, sizeof(url), "http://air-quality-api.open-meteo.com/v1/air-quality"
			"?latitude=%.2f&longitude=%.2f&hourly=pm10,pm2_5"
			"&timezone=Asia%%2FSeoul&forecast_days=1&format=csv",
			r.lat, r.lon);

	*pm10 = -1;
	*pm25 = -1;

	std::string csv;
	if ( !fetch_csv(url, csv) ) {
		return false;
	}

	char now[32];
	time_t t = time(NULL);
	strftime(now, sizeof(now), "%Y-%m-%dT%H", localtime(&t));

	std::vector<std::string> lines = split_string(csv, '\n');
	for (unsigned int i=0; i<lines.size(); i++) {
		std::string line = trim(lines[i]);
		if ( line.compare(0, 13, now) != 0 ) continue;

		std::vector<std::string> cols = split_string(line, ',');
		if ( cols.size() >= 3 ) {
			if ( !cols[1].empty() ) *pm10 = atof(cols[1].c_str());
			if ( !cols[2].empty() ) *pm25 = atof(cols[2].c_str());
			return true;
		}
	}
	return false;
}

// Open-Meteo 에서 예보를 받아 온다 (CSV 형식)
bool get_forecast(const region &r, std::vector<forecast> &hourly,
		std::map<std::string, std::pair<double, double> > &daily)
{
	char url[1024];
	snprintf(url, sizeof(url), "http://api.open-meteo.com/v1/forecast"
			"?latitude=%.2f&longitude=%.2f"
			"&hourly=temperature_2m,precipitation_probability,weather_code,wind_speed_10m,wind_direction_10m"
			"&daily=temperature_2m_max,temperature_2m_min"
			"&timezone=Asia%%2FSeoul&forecast_days=3&wind_speed_unit=ms&format=csv",
			r.lat, r.lon);

	std::string csv;
	if ( !fetch_csv(url, csv) ) {
		return false;
	}

	// 시간별 표와 일별 표가 빈 줄로 나뉘어 온다
	std::vector<std::string> lines = split_string(csv, '\n');
	int section = 0;	// 1: 시간별, 2: 일별
	for (unsigned int i=0; i<lines.size(); i++) {
		std::string line = trim(lines[i]);
		if ( line.compare(0, 5, "time,") == 0 ) {
			section = ( line.find("_max") != std::string::npos ) ? 2 : 1;
			continue;
		}
		if ( line.empty() || !isdigit((unsigned char)line[0]) ) continue;

		std::vector<std::string> cols = split_string(line, ',');
		if ( section == 1 && cols.size() >= 6 ) {
			forecast f;
			f.time = cols[0];
			f.temp = atof(cols[1].c_str());
			f.pop = atoi(cols[2].c_str());
			f.code = cols[3].empty() ? -1 : atoi(cols[3].c_str());
			f.ws = atof(cols[4].c_str());
			f.wd = atoi(cols[5].c_str());
			hourly.push_back(f);
		} else if ( section == 2 && cols.size() >= 3 ) {
			daily[cols[0]] = std::make_pair(atof(cols[1].c_str()), atof(cols[2].c_str()));
		}
	}

	return hourly.size() > 0;
}

void show_info(const region &r)
{
	std::string head_title = std::string(title) + " : " + r.sido + " " + r.name;

    printf(ESC_CLEAR);
	print_header(head_title.c_str());

    printf("\033[4;1H");
    printf("자료를 받아오는 중입니다...");
    fflush(stdout);

	std::vector<forecast> hourly;
	std::map<std::string, std::pair<double, double> > daily;
	if ( !get_forecast(r, hourly, daily) ) {
		printf("\r\n\r\n날씨 정보를 받아오지 못했습니다.\r\n");
		printf("\r\n[Enter] 를 누르세요.");
		press_enter();
		return;
	}

	// 미세먼지
	double pm10, pm25;
	get_air(r, &pm10, &pm25);

	// -----------------------------------------------------------
    printf("\033[4;1H\033[K");
	if ( pm10 >= 0 || pm25 >= 0 ) {
		printf(" 지금 미세먼지 %d㎍/㎥ (%s)   초미세먼지 %d㎍/㎥ (%s)",
				round_int(pm10), dust_grade(pm10, false), round_int(pm25), dust_grade(pm25, true));
	} else {
		printf(" 미세먼지 정보를 받아오지 못했습니다.");
	}
    printf("\033[5;1H");
    printf("%5s %8s %13s %15s %10s %10s %10s\r\n",
            "시간", "날짜", "온도(고/저)", "날씨", "강수확률", "풍향", "풍속(m/s)");
    printf("\033[6;1H");
    printf("%s", repeat("─", 40).c_str());

	// 지금 시각 이후, 3 시간 간격으로 14 개 (이틀치)
	char now[32];
	time_t t = time(NULL);
	strftime(now, sizeof(now), "%Y-%m-%dT%H", localtime(&t));

	std::string today = date_after(0);
	std::string tomorrow = date_after(1);
	std::string after = date_after(2);

    printf("\033[7;1H");
	int shown = 0;
	for (unsigned int i=0; i<hourly.size() && shown < 14; i++) {
		const forecast &f = hourly[i];
		if ( f.time.size() < 13 ) continue;
		if ( f.time.compare(0, 13, now) < 0 ) continue;

		int hour = atoi(f.time.substr(11, 2).c_str());
		if ( hour % 3 != 0 ) continue;

		std::string date = f.time.substr(0, 10);
		std::string day;
		if ( date == today ) day = "오늘";
		else if ( date == tomorrow ) day = "내일";
		else if ( date == after ) day = "모레";
		else day = date.substr(5);

		char hour_s[16];
		snprintf(hour_s, sizeof(hour_s), "%d시", hour);

		char temp[64];
		if ( daily.count(date) ) {
			snprintf(temp, sizeof(temp), "%2d (%2d/%2d)", round_int(f.temp),
					round_int(daily[date].first), round_int(daily[date].second));
		} else {
			snprintf(temp, sizeof(temp), "%2d", round_int(f.temp));
		}

		char pop[16];
		snprintf(pop, sizeof(pop), "%d%%", f.pop);

		printf("%5s %8s %13s %15s %10s %10s %10.1f\r\n",
				hour_s,
				day.c_str(),
				temp,
				weather_name(f.code).c_str(),
				pop,
				wind_direction(f.wd).c_str(),
				f.ws);
		shown++;
	}

    printf("%s", repeat("─", 40).c_str());
    printf("\r\n 자료: Open-Meteo.com   [Enter] 를 누르세요.");
    press_enter();
}

// 지금 날씨 한 줄 "맑음 18℃" (서울 중구)
// [weather] 에 보일 짧은 지역 이름: 특별시/광역시는 시 이름 (서울, 부산), 나머지는 시/군 이름
static std::string short_place(const region &r)
{
	std::string sido = r.sido, name = r.name;
	std::string::size_type p = name.find('(');
	if ( p != std::string::npos ) name = name.substr(0, p);
	bool gu = name.size() >= 2 && name.compare(name.size() - 2, 2, "구") == 0;
	if ( sido == "전남광주통합특별시" ) return gu ? "광주" : name;
	if ( sido.find("광역시") != std::string::npos || sido.find("특별시") != std::string::npos ||
			sido.find("특별자치시") != std::string::npos ) {
		return sido.substr(0, 4);		// 완성형 두 글자
	}
	return name;
}

static int find_region(const std::string &sido, const std::string &name)
{
	for (int i=0; i<region_count; i++) {
		if ( sido == regions[i].sido && name == regions[i].name ) return i;
	}
	return -1;
}

// 접속한 곳의 IP (또는 호스트 이름) 로 가장 가까운 지역. "시도|시군구", 모르면 "-"
// ip-api.com (무료, 비상업, 분당 45 번. HTTP 라 오래된 TLS 에서도 된다). 한국이 아니면 모름
static std::string locate(const std::string &host)
{
	if ( host.empty() || host.size() > 64 ) return "-";
	for (unsigned int i=0; i<host.size(); i++) {
		char c = host[i];
		if ( !isalnum((unsigned char)c) && c != '.' && c != ':' && c != '-' ) return "-";
	}
	std::string txt;
	if ( !download_text("http://ip-api.com/line/" + host + "?fields=status,countryCode,lat,lon", txt) ) return "-";
	std::vector<std::string> l = split_string(txt, '\n');
	if ( l.size() < 4 || trim(l[0]) != "success" || trim(l[1]) != "KR" ) return "-";
	double lat = atof(l[2].c_str()), lon = atof(l[3].c_str());
	int best = -1;
	double bd = 0;
	for (int i=0; i<region_count; i++) {
		double dy = regions[i].lat - lat, dx = (regions[i].lon - lon) * 0.82;	// 경도 1 도는 위도 1 도의 cos(35도) 배
		double d = dx * dx + dy * dy;
		if ( best < 0 || d < bd ) { best = i; bd = d; }
	}
	if ( best < 0 ) return "-";
	return std::string(regions[best].sido) + "|" + regions[best].name;
}

// 지금 날씨 한 줄: "지역|맑음 13℃". r 이 없으면 서울
static bool now_line(const region *r, std::string &line)
{
	char url[512];
	snprintf(url, sizeof(url), "http://api.open-meteo.com/v1/forecast?latitude=%.2f&longitude=%.2f"
			"&current=temperature_2m,weather_code&timezone=Asia%%2FSeoul", r ? r->lat : 37.56, r ? r->lon : 127.00);
	std::string json;
	if ( !download_text(url, json) ) {
		return false;
	}
	// "current_units" 에도 같은 이름이 있으므로 "current":{ 뒤에서 찾는다
	std::string::size_type p = json.find("\"current\":{");
	if ( p == std::string::npos ) return false;
	std::string cur = json.substr(p);
	std::string::size_type t = cur.find("\"temperature_2m\":");
	std::string::size_type c = cur.find("\"weather_code\":");
	if ( t == std::string::npos || c == std::string::npos ) return false;
	double temp = atof(cur.c_str() + t + 17);	// "temperature_2m": 17 자
	int code = atoi(cur.c_str() + c + 15);		// "weather_code": 15 자
	char buf[128];
	snprintf(buf, sizeof(buf), "%s|%s %d℃", r ? short_place(*r).c_str() : "서울", weather_name(code).c_str(), round_int(temp));
	line = buf;
	return true;
}

int main(int argc, char **argv)
{
	// bin/weather --now [시도 시군구] : 지금 날씨 한 줄 "지역|맑음 13℃" (txt 파일의 [weather]). 지역이 없으면 서울
	if ( argc > 1 && !strcmp(argv[1], "--now") ) {
		int k = argc > 3 ? find_region(argv[2], argv[3]) : -1;
		std::string line;
		if ( !now_line(k >= 0 ? &regions[k] : NULL, line) ) return 1;
		printf("%s\n", line.c_str());
		return 0;
	}
	// bin/weather --locate 호스트 : 접속한 곳에서 가장 가까운 지역 "시도|시군구" (모르면 "-")
	if ( argc > 2 && !strcmp(argv[1], "--locate") ) {
		printf("%s\n", locate(argv[2]).c_str());
		return 0;
	}

	if ( argc > 1 ) {
		snprintf(host_name, sizeof(host_name), "%s", argv[1]);
	}

    signal(SIGQUIT, SIG_IGN);
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
    signal(SIGHUP, (__sighandler_t)host_close);
    signal(SIGSEGV, (__sighandler_t)host_close);
    signal(SIGBUS, (__sighandler_t)host_close);

    ioctl(0,TCGETA, &sys_term);
	raw_mode();

    umask(0111);

	// 시/도 목록
	std::vector<std::string> sidos;
	for (int i=0; i<region_count; i++) {
		if ( std::find(sidos.begin(), sidos.end(), regions[i].sido) == sidos.end() ) {
			sidos.push_back(regions[i].sido);
		}
	}

    while (1) {
		int s = select_menu(title, sidos);
		if ( s < 0 ) break;

		// 선택한 시/도의 지역 목록
		std::vector<std::string> names;
		std::vector<int> index;
		for (int i=0; i<region_count; i++) {
			if ( sidos[s] == regions[i].sido ) {
				names.push_back(regions[i].name);
				index.push_back(i);
			}
		}

		std::string sub_title = std::string(title) + " : " + sidos[s];
		while (1) {
			int n = select_menu(sub_title, names);
			if ( n < 0 ) break;

			show_info(regions[index[n]]);
		}
    }

	host_close();
}
