#include "main.h"
#include <dirent.h>

std::string random_string(const int len)
{
	char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!@#$%^&*()_+";
	std::stringstream ss;

	// ¿¹ÃøÇÒ ¼ö ¾øµµ·Ï /dev/urandom »ç¿ë
	unsigned char rnd[256];
	int n = 0;
	int fd = open("/dev/urandom", O_RDONLY);
	if ( fd >= 0 ) {
		n = read(fd, rnd, sizeof(rnd));
		close(fd);
	}

	int k = 0;
	for (int i = 0; i < len; ++i) {
		unsigned int r;
		// ³ª¸ÓÁö ÆíÇâÀ» ¾ø¾Ö±â À§ÇØ ¹üÀ§¸¦ ³Ñ´Â °ªÀº ¹ö¸²
		const unsigned int count = sizeof(alphanum) - 1;
		const unsigned int limit = 256 - (256 % count);
		do {
			if ( k < n ) {
				r = rnd[k++];
			} else {
				// /dev/urandom À» ¸ø ÀĞÀº °æ¿ìÀÇ ¿¹ºñ ¼ö´Ü
				static bool seeded = false;
				if ( !seeded ) { srand(time(NULL) ^ (getpid() << 16)); seeded = true; }
				r = rand() % limit;
			}
		} while ( r >= limit );
		ss << alphanum[r % count];
	}
	return ss.str();
}

std::string centered(const std::string s, const int w) 
{
    std::stringstream ss, spaces;
    int pad = w - s.length();                  // count excess room to pad
    for(int i=0; i<pad/2; ++i)
        spaces << " ";
    ss << spaces.str() << s << spaces.str(); // format with padding
    if(pad>0 && pad%2!=0)                    // if pad odd #, add 1 more space
        ss << " ";
    return ss.str();
}

std::string repeat(const std::string s, int n) 
{
    std::ostringstream os;
    for(int i = 0; i < n; i++)
        os << s;
    return os.str();
}

std::string read_file(const char *path)
{
	std::ifstream file(path);

    if (file) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
		return buffer.str();
    }

	return "";
}

bool dir_exists(char *dir)
{
	 struct stat sb;
    if (stat(dir, &sb) == 0 && S_ISDIR(sb.st_mode))
		return true;
    else
		return false;
}

bool file_exists(char *file)
{
	if( access( file, F_OK ) != -1 )
		return true;
	else
		return false;
}


std::vector<std::string> exec_command(char *command, bool *success)
{
	std::vector<std::string> lines;
	char line[1035];

	/* Open the command for reading. */
	FILE *fp = popen(command, "r");
	if (fp == NULL) {
		*success = false;
		return lines;
	}

	/* Read the output a line at a time - output it. */
	while (fgets(line, sizeof(line)-1, fp) != NULL) {
		lines.push_back(std::string(line));
	}

	/* close: Á¾·á ÄÚµå°¡ 0 ÀÏ ¶§¸¸ ¼º°ø */
	int status = pclose(fp);

	*success = ( status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0 );
	return lines;
}

std::vector<std::string> find_files(const std::string& path)
{
    glob_t glob_result;
    glob(path.c_str(), GLOB_TILDE, NULL, &glob_result);
	std::vector<std::string> ret;
    for(unsigned int i=0; i<glob_result.gl_pathc; ++i){
        ret.push_back(std::string(glob_result.gl_pathv[i]));
    }
    globfree(&glob_result);
    return ret;
}

// ½Ã°£¼øÀ¸·Î Á¤·ÄµÈ ÆÄÀÏ ¸ñ·ÏÀ» ¾òÀ½
std::vector<std::string> find_files_time_sorted(char *dir) 
{
	std::vector<std::string> files = find_files(dir);
	FileTimeComparator comparator;
	std::sort (files.begin(), files.end(), comparator);

	return files;
}

// $HANULSO/tmp ¿¡ ºó ÀÓ½Ã ÆÄÀÏÀ» ¸¸µé¾î (mkstemp, 0600) ±× °æ·Î¸¦ µ¹·ÁÁØ´Ù. ¸ø ¸¸µé¸é ""
// (tempnam Àº ÀÌ¸§¸¸ °í¸£°í ¸¸µéÁö ¾Ê¾Æ, ³²ÀÌ °°Àº ÀÌ¸§À¸·Î ½Éº¼¸¯ ¸µÅ©¸¦ ¸ÕÀú ³õÀ» ¼ö ÀÖ´Ù)
std::string make_private_tmpfile(const char *prefix)
{
	char path[1024];
	snprintf(path, sizeof(path), "%s/tmp/%s.XXXXXX", getenv("HANULSO") ? getenv("HANULSO") : ".", prefix);
	int fd = mkstemp(path);
	if ( fd < 0 ) return "";
	fchmod(fd, 0600);
	close(fd);
	return path;
}

std::string query_luck(int yy, int mm, int dd, int sex, int birth_yy, int birth_mm, int birth_dd, int lunar, int yun)
{
	std::string tmp_path = make_private_tmpfile("luck");
	if ( tmp_path.empty() ) return "";
	char tmp_file[1024];
	snprintf(tmp_file, sizeof(tmp_file), "%s", tmp_path.c_str());

	char www[1024];
	sprintf(www, "http://today.freeunse.funstory.biz/sub/tradition1.php?unse_mode=1"
		// ³¯Â¥
		"&yy=%d&mm=%d&dd=%d"
		// ³²(1)/¿©(2)
		"&unse1_sex=%d"
		// »ı³â¿ù/ÀÏ
		"&unse1_yy=%d&unse1_mm=%d&unse1_dd=%d"
		// »ı³â½Ã
		"&unse1_hh=&"
		// ¾ç·Â(1)/À½·Â(2)
		"unse1_solun=%d"
		// Æò´Ş(1)/À±´Ş(2)
		"&unse1_lun_yn=%d", 
		yy, mm, dd, sex, birth_yy, birth_mm, birth_dd, lunar, yun);

	char buf[2048];
	snprintf(buf, sizeof(buf), "lynx -dump -nomargins -width=70 -assume_charset=euc-kr -display_charset=euc-kr %s"
		"| sed 's/¤ı/* /g' > %s", shell_quote(www).c_str(), shell_quote(tmp_file).c_str());
	system(buf);

	std::string txt = read_file(tmp_file);

	unlink(tmp_file);

	return txt;
}

std::string replace_all(std::string str, const std::string& from, const std::string& to) 
{
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}

bool is_binary(const void *data, size_t len)
{
    return memchr(data, '\0', len) != NULL;
}

std::string string_convert(std::vector<std::string> lines)
{
	std::string str;
	for(unsigned int i=0; i<lines.size(); i++) {
		str += lines[i];
		str += std::string("\r\n");
	}

	return str;
}

void date_now(int *year, int *month, int *day, std::string &week)
{
	struct tm *t;
	const char *weeks[] = {"ÀÏ", "¿ù", "È­", "¼ö", "¸ñ", "±İ", "Åä" };

	time_t now;
	time ( &now );
	t = localtime(&now);

	*year = t->tm_year+1900;
	*month = t->tm_mon+1;
	*day = t->tm_mday;
	week = std::string(weeks[t->tm_wday]);
}

void time_now(int *hour, int *min, int *sec)
{
	struct tm *t;

	time_t now;
	time ( &now );
	t = localtime(&now);

	*hour = t->tm_hour;
	*min = t->tm_min;
	*sec = t->tm_sec;
}

std::string date_now_string(bool week)
{
	int year, month, day;
	std::string week_s;
	date_now(&year, &month, &day, week_s);

	char buf[256];
	sprintf(buf, "%04d-%02d-%02d", year, month, day);
	if ( week ) {
		size_t l = strlen(buf);
		snprintf(buf + l, sizeof(buf) - l, " (%s)", week_s.c_str());
	}

	return std::string(buf);
}

std::string time_now_string(void)
{
	int hour, min, sec;
	time_now(&hour, &min, &sec);

	char buf[256];
	sprintf(buf, "%02d:%02d:%02d", hour, min, sec);

	return std::string(buf);
}

std::string datetime_now_string(bool week)
{
	int year, month, day;
	std::string week_s;
	date_now(&year, &month, &day, week_s);

	int hour, min, sec;
	time_now(&hour, &min, &sec);

	char buf[256];
	if ( week ) {
		sprintf(buf, "%04d-%02d-%02d (%s) %02d:%02d:%02d", year, month, day, week_s.c_str(), hour, min, sec);
	} else {
		sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, min, sec);
	}

	return std::string(buf);
}

#if 0
std::vector<std::string> split_string(std::string str, std::string delimiter)
{
    std::vector<std::string> strings;

    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    while ((pos = str.find(delimiter, prev)) != std::string::npos)
    {
        strings.push_back(str.substr(prev, pos - prev));
        prev = pos + 1;
    }

    // To get the last substring (or only, if delimiter is not found)
    strings.push_back(str.substr(prev));

    return strings;
}
#else
std::vector<std::string> split_string(std::string str, char delimiter)
{
	std::stringstream test(str);
	std::string segment;
	std::vector<std::string> result;

	while(std::getline(test, segment, delimiter))
	   result.push_back(segment);
	
	return result;
}

std::vector<std::string> split_string_with_width(std::string str, char delimiter, int width)
{
	std::stringstream test(str);
	std::string segment;
	std::vector<std::string> result;

	while(std::getline(test, segment, delimiter)) {
        if(segment.length() > width) {
            int start = 0;
            std::string line;
            while (1) {
                if(start > segment.length()) break;
                std::string sub = segment.substr(start, 1);
                char c = sub.c_str()[0];

                // ÇÑ±ÛÀÌ¸é 2¹ÙÀÌÆ®¸¦ ÀĞÀ½
                if(!isascii(c)) {
                    // Áö±İ±îÁö ÀĞÀº°Å¿¡ 2¸¦ ´õÇØ¼­ widthº¸´Ù Å©¸é ¶óÀÎ push_back
                    if(line.length() + 2 > width) {
                        result.push_back(line);
                        line = "";
                    }

                    sub = segment.substr(start, 2);
                    start+=2;

                // ¿µ¹®ÀÌ¸é 1¹ÙÀÌÆ®¸¦ ÀĞÀ½
                } else {
                    // Áö±İ±îÁö ÀĞÀº°Å¿¡ 1¸¦ ´õÇØ¼­ widthº¸´Ù Å©¸é ¶óÀÎ push_back
                    if(line.length() + 1 > width) {
                        result.push_back(line);
                        line = "";
                    }

                    start+=1;
                }

                line += sub;
            }
            if(line.length() > 0) {
                //printf("%s\n", line.c_str());
                result.push_back(line);
            }
        } else {
            //printf("%s\n", segment.c_str());
            result.push_back(segment);
        }
    }
	
	return result;
}
#endif

bool is_number(std::string s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

long long ms_time_now (void)
{
    struct timeval t; 
    gettimeofday(&t, NULL); // get current time
    long long milliseconds = t.tv_sec*1000LL + t.tv_usec/1000; // caculate milliseconds
    // printf("milliseconds: %lld\n", milliseconds);
    return milliseconds;
}

long file_size(std::string filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

long file_mtime(std::string filename)
{
	struct stat stat_buf;
 
	stat(filename.c_str(), &stat_buf);
	return (long)stat_buf.st_mtime;
}

long file_ms_mtime(std::string filename)
{
	struct stat stat_buf;
 
	stat(filename.c_str(), &stat_buf);
	long ms = stat_buf.st_mtime * 1000 + stat_buf.st_mtim.tv_nsec / 1000000;
	return ms;
}

std::string ms_time_to_string(long ms_time)
{
    int milliseconds = int((ms_time%1000)/100)
		, seconds = int((ms_time/1000)%60)
		, minutes = int((ms_time/(1000*60))%60)
		, hours = int((ms_time/(1000*60*60))%24);

	char buf[1024];
	sprintf(buf, "%02d:%02d:%02d.%03d", hours, minutes, seconds, milliseconds);
	return std::string(buf);
}

std::string convert_to_string(double num) 
{
	std::ostringstream convert;
    convert << num; 
    return convert.str();
}

double round_off(double n) {
    double d = n * 100.0;
    int i = d + 0.5;
    d = (float)i / 100.0;
    return d;
}

std::string human_file_size(long size)
{
	static const char *SIZES[] = { "B", "KB", "MB", "GB" };
	unsigned int div = 0;
	size_t rem = 0;

	while (size >= 1024 && div < (sizeof SIZES / sizeof *SIZES) - 1) {
		rem = (size % 1024);
		div++;
		size /= 1024;
	}

	double size_d = (float)size + (float)rem / 1024.0;
	std::string result;
	if ( round_off(size_d) < 0 ) {
		result = convert_to_string(0) + SIZES[div];
	} else {
		result = convert_to_string(round_off(size_d)) + SIZES[div];
	}
	return result;
}

std::vector<std::string> split(const std::string& str, const std::string& delim)
{
	std::vector<std::string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == std::string::npos) pos = str.length();
		std::string token = str.substr(prev, pos-prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    }
    while (pos < str.length() && prev < str.length());
    return tokens;
}

std::string split_file_path (const std::string& str) 
{
	unsigned found = str.find_last_of("/\\");
	//std::cout << " path: " << str.substr(0,found) << '\n';
	//std::cout << " file: " << str.substr(found+1) << '\n';
	return str.substr(0, found);
}

std::string split_file_name (const std::string& str) 
{
	unsigned found = str.find_last_of("/\\");
	//std::cout << " path: " << str.substr(0,found) << '\n';
	//std::cout << " file: " << str.substr(found+1) << '\n';
	return str.substr(found+1);
}

bool is_han(char c)
{
	if((c & 0x80)==0) {
		return false;
	} else {
		return true;
	}
}

// ¿Ï¼ºÇü ÇÑ±ÛÀÌ µÎ ¹ÙÀÌÆ® Â¦À» ÀÌ·çÁö ¸øÇÑ ¹ÙÀÌÆ®¸¦ »«´Ù.
// (¿¹Àü¿¡ ¹é½ºÆäÀÌ½º°¡ ÇÑ±Û ¹İÂÊ¸¸ Áö¿ö ÀúÀåµÈ ±ÛÀ» Ãâ·ÂÇÒ ¶§ ÁÙÀÌ ¹Ğ¸®Áö ¾Êµµ·Ï)
std::string fix_hangul(const std::string &s)
{
	std::string r;
	unsigned int k = 0;
	while ( k < s.size() ) {
		if ( is_han(s[k]) ) {
			if ( k + 1 < s.size() && is_han(s[k+1]) ) {
				r += s[k];
				r += s[k+1];
				k += 2;
			} else {
				k += 1;
			}
		} else {
			r += s[k];
			k += 1;
		}
	}
	return r;
}

// ±× pid °¡ »ì¾Æ ÀÖ´Â BBS(main) ÇÁ·Î¼¼½ºÀÎÁö.
// °­Á¦ Á¾·á·Î ³²Àº Á¢¼ÓÀÚ ÆÄÀÏÀÇ pid ¸¦ ´Ù¸¥ ÇÁ·Î±×·¥ÀÌ ¹Ş¾ÒÀ» ¼ö ÀÖÀ¸¹Ç·Î ÀÌ¸§±îÁö º»´Ù.
// (ctime ÀÌ execl("bin/main", "main", ...) À¸·Î ¶ç¿ì¹Ç·Î argv[0] ÀÌ main)
bool is_bbs_process(int pid)
{
	if ( pid <= 0 ) return false;
	if ( kill(pid, 0) != 0 && errno == ESRCH ) return false;
	char path[64];
	snprintf(path, sizeof(path), "/proc/%d/cmdline", pid);
	std::string argv0 = read_file(path).c_str();
	if ( argv0.empty() ) return true;		// /proc ¸¦ ¸ø ÀĞÀ¸¸é »ì¾Æ ÀÖ´Â °ÍÀ¸·Î
	std::string::size_type slash = argv0.rfind('/');
	if ( slash != std::string::npos ) argv0 = argv0.substr(slash + 1);
	return argv0 == "main";
}

// Á¢¼ÓÀÚ ÆÄÀÏ(tmp/<tty>.tty: "¾ÆÀÌµğ pid") À» ÀĞ´Â´Ù.
// ·Î±×ÀÎ Àü(ºó ÆÄÀÏ)ÀÌ¸é false, ÇÁ·Î¼¼½º°¡ ÀÌ¹Ì Á×¾úÀ¸¸é (°­Á¦ Á¾·á µîÀ¸·Î ³²Àº ÆÄÀÏ) Áö¿ì°í false.
bool read_tty_file(const std::string &path, std::string &user_id)
{
	std::string txt = trim(read_file(path.c_str()));
	char id[256] = "";
	int pid = 0;
	if ( sscanf(txt.c_str(), "%255s %d", id, &pid) < 1 ) {
		return false;
	}

	// pid °¡ ¾ø´Â ¿¹Àü Çü½Ä: Áö±İ ÄÚµå´Â ´Ã pid ¸¦ ÀûÀ¸¹Ç·Î ¿¹Àü¿¡ ³²Àº ÆÄÀÏÀÌ´Ù
	if ( pid <= 0 || !is_bbs_process(pid) ) {
		unlink(path.c_str());
		return false;
	}

	user_id = id;
	return true;
}

// Á¢¼Ó ÁßÀÎ È¸¿ø¿¡°Ô ¾Ë¸² ÇÑ ÁÙ. ±× È¸¿øÀÇ BBS ¼¼¼Ç¸¶´Ù tmp/<tty>.notice ¿¡ µ¡ºÙÀÎ´Ù.
// (´ëÈ­¹æÃ³·³ ÇÁ·ÒÇÁÆ®°¡ ¾Æ´Ñ È­¸é¿¡¼­ º¸¿© ÁÖ·Á°í. ÇÁ·ÒÇÁÆ®·Î µ¹¾Æ¿À¸é Áö¿î´Ù)
// signal_bbs ¸é BBS ÇÁ·Î¼¼½º¿¡ SIGUSR1 (ÇÁ·ÒÇÁÆ®¿¡¼­ Àüº¸¸¦ ¹Ù·Î ¶ç¿ì°Ô). Á¢¼ÓÇÑ ¼¼¼Ç ¼ö¸¦ µ¹·ÁÁØ´Ù.
int notify_online(const std::string &user_id, const std::string &line, bool signal_bbs)
{
	char pattern[1024];
	snprintf(pattern, sizeof(pattern), "%s/tmp/*.tty", getenv("HANULSO"));
	std::vector<std::string> files = find_files(pattern);
	int n = 0;
	for ( unsigned int i = 0; i < files.size(); i++ ) {
		std::string id;
		if ( !read_tty_file(files[i], id) || id != user_id ) continue;
		char sid[256] = "";
		int pid = 0;
		sscanf(trim(read_file(files[i].c_str())).c_str(), "%255s %d", sid, &pid);
		if ( !is_bbs_process(pid) ) continue;
		std::string notice = files[i].substr(0, files[i].size() - 4) + ".notice";	// .tty -> .notice
		FILE *fp = fopen(notice.c_str(), "a");
		if ( fp != NULL ) {
			fprintf(fp, "%s\n", line.c_str());
			fclose(fp);
		}
		if ( signal_bbs ) kill(pid, SIGUSR1);
		n++;
	}
	return n;
}

// ³²ÀÌ ¾´ ±Û (º»¹® / Á¦¸ñ) À» ´Ù¸¥ È¸¿ø È­¸é¿¡ Âï±â Àü¿¡: »ö ¹Ù²Ù±â (ESC [ ... m / F / G) ¸¸ µÎ°í
// ´Ù¸¥ ESC ¿­ (Ä¿¼­ ¿Å±â±â, ±Û¼è ¹Ù²Ù±â ...) °ú Á¦¾î ¹®ÀÚ (Æ¯È÷ CAN 0x18 - ZMODEM ½ÃÀÛ ½ÅÈ£·Î ÀĞ´Â ÂÊ
// Åë½Å ÇÁ·Î±×·¥ÀÌ ÆÄÀÏ ¹Ş±â / º¸³»±â¸¦ ½ÃÀÛÇÒ ¼ö ÀÖ´Ù) ´Â »«´Ù. ÅÇÀº ºóÄ­ ÇÏ³ª·Î. ¿Ï¼ºÇü µî ³ôÀº ¹ÙÀÌÆ®´Â ±×´ë·Î
std::string safe_terminal_text(const std::string &s)
{
	std::string r;
	for ( unsigned int i = 0; i < s.size(); i++ ) {
		unsigned char c = s[i];
		if ( c == 0x1b ) {
			// ESC [ (¼ıÀÚ ; = ?)* ³¡ ±ÛÀÚ
			unsigned int k = i + 1;
			if ( k < s.size() && s[k] == '[' ) {
				k++;
				while ( k < s.size() && (isdigit((unsigned char)s[k]) || s[k] == ';' || s[k] == '=' || s[k] == '?') ) k++;
				if ( k < s.size() ) {
					char f = s[k];
					bool color = (f == 'm' || f == 'F' || f == 'G') && s.find('?', i) != i + 2;
					if ( color ) r.append(s, i, k - i + 1);
					i = k;
					continue;
				}
			}
			continue;		// È¥ÀÚ ÀÖ´Â ESC ³ª ´Ù¸¥ ¿­Àº ¹ö¸°´Ù
		}
		if ( c == '\t' ) { r += ' '; continue; }
		if ( c < 0x20 || c == 0x7f ) continue;
		r += (char)c;
	}
	return r;
}

// È­¸é¿¡ º¸ÀÌ´Â ±ÛÀÚ¸¸ ³²±ä´Ù: ÀÏ¹İ ASCII ¿Í ¿Ï¼ºÇü(KS X 1001) 2 ¹ÙÀÌÆ® ±ÛÀÚ.
// Á¦¾î ¹®ÀÚ, ¿Ï¼ºÇü ¹ÛÀÇ ±ÛÀÚ(ÅÍ¹Ì³Î¿¡ ¾È º¸ÀÌ°í Ä­¸¸ ¾î±ß³²), Â¦ ¾ø´Â ¹ÙÀÌÆ®´Â »«´Ù.
std::string display_text(const std::string &s)
{
	std::string r;
	unsigned int k = 0;
	while ( k < s.size() ) {
		unsigned char c = s[k];
		if ( c >= 0x80 ) {
			unsigned char c2 = (k + 1 < s.size()) ? (unsigned char)s[k+1] : 0;
			if ( c >= 0xA1 && c <= 0xFE && c2 >= 0xA1 && c2 <= 0xFE ) {
				r += s[k];
				r += s[k+1];
			}
			k += (c2 >= 0x41) ? 2 : 1;
		} else {
			if ( c >= 0x20 && c < 0x7F ) r += s[k];
			k += 1;
		}
	}
	return trim(r);
}

// ÅÍ¹Ì³Î¿¡ Ä¿¼­ À§Ä¡¸¦ ¹°¾î(ESC[6n) ÀÀ´ä ESC[Çà;¿­R À» ÀĞ´Â´Ù
static bool query_cursor(int *row, int *col)
{
	printf("\033[6n");
	fflush(stdout);

	char buf[32];
	int len = 0;
	while ( len < (int)sizeof(buf) - 1 ) {
		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(0, &fds);
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 800000;
		if ( select(1, &fds, NULL, NULL, &tv) <= 0 ) break;
		char ch;
		if ( read(0, &ch, 1) != 1 ) break;
		buf[len++] = ch;
		if ( ch == 'R' ) break;
	}
	buf[len] = 0;

	char *p = strrchr(buf, '[');
	return p != NULL && sscanf(p + 1, "%d;%d", row, col) == 2;
}

// ÅÍ¹Ì³Î ÁÙ ¼ö. Ä¿¼­¸¦ ¸Ç ¾Æ·¡·Î º¸³» À§Ä¡¸¦ ¹°¾îº¸°í ¿ø·¡ ÀÚ¸®·Î µ¹·Á ³õ´Â´Ù.
// À§Ä¡¸¦ ¾Ë·Á ÁÖÁö ¾Ê´Â ÅÍ¹Ì³ÎÀÌ¸é telnet ÀÌ ¾Ë·Á ÁØ Ã¢ Å©±â, ±×°Íµµ ¾øÀ¸¸é 24 ÁÙ
int terminal_rows(void)
{
	int rows = 0;
	int r0, c0;
	if ( query_cursor(&r0, &c0) ) {
		printf("\033[999;999H");
		int r, c;
		if ( query_cursor(&r, &c) ) rows = r;
		printf("\033[%d;%dH", r0, c0);
		fflush(stdout);
	}
	if ( rows < 10 ) {
		struct winsize ws;
		if ( ioctl(0, TIOCGWINSZ, &ws) == 0 && ws.ws_row >= 10 ) rows = ws.ws_row;
	}
	if ( rows < 10 || rows > 200 ) rows = 24;
	return rows;
}

// ÀÔ·Â ¹öÆÛ(stdio)³ª ¼ÒÄÏ¿¡ ¹Ù·Î ÀÌ¾î¼­ µé¾î¿Â ±ÛÀÚ°¡ ÀÖ´ÂÁö
static bool input_pending(int msec)
{
	if ( stdin->_IO_read_ptr < stdin->_IO_read_end ) return true;

	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(0, &fds);
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = msec * 1000;
	return select(1, &fds, NULL, NULL, &tv) > 0;
}

// ÀÔ·ÂÀ» ±â´Ù¸®´Â µ¿¾È 1 ÃÊ¸¶´Ù ºÒ¸®´Â ÇÔ¼ö (ÇÁ·ÒÇÁÆ®¿¡¼­ Àüº¸¸¦ ¶ç¿ï ¶§).
// Áö±İ±îÁö È­¸é¿¡ Ä£ ±ÛÀÚ¸¦ ³Ñ°Ü, ¸Ş½ÃÁö¸¦ ¶ç¿î µÚ ÀÔ·Â ÁÙÀ» ´Ù½Ã ±×¸± ¼ö ÀÖ°Ô ÇÑ´Ù.
void (*line_input_wait_hook)(const char *typed) = NULL;

// ÁÙ ÆíÁı±â¿ë ÀÚµ¿ ÁÙ¹Ù²Ş (line_input_wrap): ÁÙÀÌ Â÷¸é ¸¶Áö¸· ³¹¸»À» ´ÙÀ½ ÁÙ·Î ³Ñ±ä´Ù
static bool wrap_enabled = false;
static bool wrap_done = false;
static std::string wrap_carry;

// ÁÙÀÌ Ã¡À» ¶§: ¸¶Áö¸· ¶ç¾î¾²±â µÚÀÇ ³¹¸»À» È­¸é°ú ¹öÆÛ¿¡¼­ Áö¿ì°í extra ¿Í ÇÔ²² wrap_carry ¿¡.
// ¶ç¾î¾²±â°¡ ¾øÀ¸¸é (±ä ³¹¸») ÁÙÀº ±×´ë·Î µÎ°í extra ¸¸ ³Ñ±ä´Ù.
static void wrap_line(char *str, int *i, const std::string &extra, int echo)
{
	int k = *i;
	while ( k > 0 && str[k - 1] != ' ' ) k--;
	if ( k > 0 ) {
		wrap_carry = std::string(str + k, *i - k) + extra;
		if ( echo != 0 ) {
			for ( int n = k; n < *i; n++ ) { putchar('\b'); putchar(' '); putchar('\b'); }
		}
		*i = k - 1;		// ¶ç¾î¾²±âµµ ÁÙ ³¡¿¡¼­ »«´Ù
	} else {
		wrap_carry = extra;
	}
	wrap_done = true;
}

// Ä¿¼­Å° µûÀ§ (ESC [ ... ³¡ ±ÛÀÚ, ESC O x) ¸¦ ÀĞ¾î Å°·Î.  ¸ğ¸£´Â ¿­Àº ÅëÂ°·Î ¹ö¸°´Ù
// (¹ö¸®Áö ¾ÊÀ¸¸é ESC ¸¸ ºüÁö°í µÚÀÇ "[C" °¡ ±ÛÀÚ·Î µé¾î°£´Ù).  ESC ÇÏ³ª¸¸ ´­·¶À¸¸é °ğ ¾Æ¹«°Íµµ ¿ÀÁö ¾Ê´Â´Ù.
enum { KEY_NONE, KEY_LEFT, KEY_RIGHT, KEY_HOME, KEY_END, KEY_DEL };
static int read_escape_key(void)
{
	if ( !input_pending(50) ) return KEY_NONE;
	int c = getchar();
	if ( c == '[' ) {
		int num = 0;
		while ( input_pending(50) ) {
			c = getchar();
			if ( c == EOF ) return KEY_NONE;
			if ( c >= '0' && c <= '9' ) { num = num * 10 + (c - '0'); continue; }
			if ( c >= 0x40 && c <= 0x7e ) break;		// ³¡ ±ÛÀÚ
		}
		switch ( c ) {
		case 'D': return KEY_LEFT;
		case 'C': return KEY_RIGHT;
		case 'H': return KEY_HOME;
		case 'F': return KEY_END;
		case 'P': return KEY_DEL;		// ÀÌ¾ß±âÀÇ Del Àº ESC [ P
		case '~':
			if ( num == 1 || num == 7 ) return KEY_HOME;
			if ( num == 4 || num == 8 ) return KEY_END;
			if ( num == 3 ) return KEY_DEL;
			return KEY_NONE;
		}
		return KEY_NONE;
	} else if ( c == 'O' ) {
		if ( !input_pending(50) ) return KEY_NONE;
		c = getchar();
		if ( c == 'D' ) return KEY_LEFT;
		if ( c == 'C' ) return KEY_RIGHT;
		if ( c == 'H' ) return KEY_HOME;
		if ( c == 'F' ) return KEY_END;
		return KEY_NONE;
	} else if ( c != EOF ) {
		ungetc(c, stdin);
	}
	return KEY_NONE;
}

// ÁÙ ÀÔ·ÂÀÇ È­¸é ÂÊ: str[a..b) ¸¦ echo ¹æ½Ä´ë·Î Âï´Â´Ù (1 ±ÛÀÚ ±×´ë·Î, 2 ºóÄ­, 3 '*', 0 ¾È ÂïÀ½)
static void li_put(const char *str, int a, int b, int echo)
{
	for ( int k = a; k < b; k++ ) {
		if      (echo == 0) ;
		else if (echo == 2) putchar(' ');
		else if (echo == 3) putchar('*');
		else                putchar(str[k]);
	}
}

// Ä¿¼­¸¦ ¿ŞÂÊÀ¸·Î n Ä­.  ¹é½ºÆäÀÌ½º (\b) ´Â ÀÌ¾ß±â °°Àº ÅÍ¹Ì³Î¿¡¼­ ±ÛÀÚ¸¦ Áö¿ì¸ç ¿Å±â¹Ç·Î
// Ä¿¼­¸¸ ¿Å±â´Â ESC [ n D ¸¦ ¾´´Ù
static void li_back(int n, int echo)
{
	if ( echo == 0 || n <= 0 ) return;
	char seq[16];
	snprintf(seq, sizeof(seq), "\033[%dD", n);
	for ( const char *q = seq; *q; q++ ) putchar(*q);
}

// cur ¹Ù·Î ¾Õ ±ÛÀÚÀÇ ¹ÙÀÌÆ® ¼ö (¿Ï¼ºÇü ÇÑ±ÛÀº 2)
static int li_prev_len(const char *str, int cur)
{
	int k = 0, last = 1;
	while ( k < cur ) {
		last = ( is_han(str[k]) && k + 1 < cur ) ? 2 : 1;
		k += last;
	}
	return last;
}

// cur ÀÚ¸® ±ÛÀÚÀÇ ¹ÙÀÌÆ® ¼ö
static int li_cur_len(const char *str, int cur, int n)
{
	return ( is_han(str[cur]) && cur + 1 < n ) ? 2 : 1;
}

// ------------------------------------------------------------------------
// ÇÑ ÁÙ ÀÔ·Â.  ¡ç/¡æ ·Î Ä¿¼­¸¦ ¿Å±â°í (ÇÑ±ÛÀº ÇÑ ±ÛÀÚ¾¿), Home/End, Delete,
// Ctrl+A / Ctrl+E ´Â ÁÙ Ã³À½ / ³¡ (ÀÌ¾ß±â´Â End ¸¦ º¸³»Áö ¾Ê´Â´Ù),
// °¡¿îµ¥¿¡¼­ Ä¡¸é ³¢¿ö ³Ö°í ¹é½ºÆäÀÌ½º´Â Ä¿¼­ ¾Õ ±ÛÀÚ¸¦ Áö¿î´Ù.
// ÁÙ ÆíÁı±â (line_input_wrap) ´Â ÁÙÀÌ Â÷¸é ¸¶Áö¸· ³¹¸»À» ´ÙÀ½ ÁÙ·Î ³Ñ±ä´Ù (Ä¿¼­°¡ ³¡¿¡ ÀÖÀ» ¶§¸¸).
void _line_input(char *str, char *init_str, int len, int echo)
{
	int n = strlen(init_str);		// ±ÛÀÚ ¼ö (¹ÙÀÌÆ®)
	int cur;						// Ä¿¼­ ÀÚ¸® (¹ÙÀÌÆ®, ´Ã ±ÛÀÚ °æ°è)
	int c;
	// ÇÑ±Û Ã¹ ¹ÙÀÌÆ®°¡ µé¾î¿Í µÎ ¹øÂ° ¹ÙÀÌÆ®¸¦ ±â´Ù¸®´Â Áß
	bool wait_trail = false;
	char pending_lead = 0;

	// ÃÊ±â ¹®ÀÚ¿­ÀÌ ¹öÆÛº¸´Ù ±æ¸é Àß¶ó³¿.  Á¦¾î ±ÛÀÚ ('\r' µûÀ§) ´Â »«´Ù (ÂïÀ¸¸é È­¸é Ä¿¼­°¡ ¾î±ß³­´Ù)
	n = 0;
	for ( const char *q = init_str; *q && n < len; q++ ) {
		if ( (unsigned char)*q < 0x20 || *q == 0x7f ) continue;
		str[n++] = *q;
	}
	str[n] = 0;
	li_put(str, 0, n, 1);		// Ã³À½ ±ÛÀº ±×´ë·Î º¸ÀÎ´Ù (¿¹ÀüÃ³·³)
	cur = n;

	while ( 1 ) {
		if ( line_input_wait_hook != NULL ) {
			// getchar() ´Â ÀĞ±â Àü¿¡ È­¸é Ãâ·Â(stdout)À» ³»º¸³»Áö¸¸ select ·Î ±â´Ù¸± ¶§´Â
			// ³»º¸³»Áö ¾ÊÀ¸¹Ç·Î, ¸ÕÀú ³»º¸³»¾ß ÇÁ·ÒÇÁÆ®°¡ º¸ÀÎ´Ù
			fflush(stdout);
			while ( !input_pending(1000) ) {
				std::string typed(str, n);
				line_input_wait_hook(typed.c_str());		// Àüº¸¸¦ ¶ç¿ì°í ÁÙÀ» ´Ù½Ã ±×¸°´Ù (Ä¿¼­´Â ÁÙ ³¡)
				li_back(n - cur, echo);
			}
		}
		c = getchar();
		// Á¢¼ÓÀÌ ²÷±â¸é (EOF) ¹«ÇÑ ·çÇÁ¿¡ ºüÁöÁö ¾Êµµ·Ï Á¾·á
		if ( c == EOF ) {
			host_close();
			exit(1);
		}
		if ( c == '\r' ) break;
		char ch = (char)c;

		// ÇÑ±Û µÎ ¹øÂ° ¹ÙÀÌÆ®
		if ( wait_trail ) {
			wait_trail = false;
			if ( is_han(ch) ) {
				if ( n + 2 <= len ) {
					memmove(str + cur + 2, str + cur, n - cur);
					str[cur] = pending_lead;
					str[cur + 1] = ch;
					n += 2;
					li_put(str, cur, n, echo);
					cur += 2;
					li_back(n - cur, echo);
				} else if ( wrap_enabled && cur == n ) {
					// ÁÙÀÌ Ã¡À¸¸é (ÀÚµ¿ ÁÙ¹Ù²Ş) ÀÌ ±ÛÀÚ¿Í ¸¶Áö¸· ³¹¸»À» ´ÙÀ½ ÁÙ·Î
					wrap_line(str, &n, std::string(1, pending_lead) + ch, echo);
					break;
				}
				continue;
			}
			// ÇÑ±Û µÎ ¹øÂ° ¹ÙÀÌÆ®°¡ ¿ÀÁö ¾Ê¾ÒÀ¸¸é Ã¹ ¹ÙÀÌÆ®´Â ¹ö¸°´Ù (ÀÌ ±ÛÀÚ´Â ¾Æ·¡¿¡¼­ ±×´ë·Î)
		}

		if ( ch == '\b' || c == 0x7f ) {
			if ( cur > 0 ) {
				// Ä¿¼­ ¾Õ ±ÛÀÚ (ÇÑ±ÛÀÌ¸é 2 ¹ÙÀÌÆ®) ¸¦ Áö¿ì°í µÚ¸¦ ´ç±ä´Ù
				int last = li_prev_len(str, cur);
				memmove(str + cur - last, str + cur, n - cur);
				cur -= last;
				n -= last;
				li_back(last, echo);
				li_put(str, cur, n, echo);
				if ( echo != 0 ) for ( int k = 0; k < last; k++ ) putchar(' ');
				li_back(n - cur + last, echo);

				// ÇÑ±Û ÇÑ ±ÛÀÚ¿¡ ¹é½ºÆäÀÌ½º¸¦ ¹ÙÀÌÆ® ¼ö¸¸Å­(2 ¹ø) º¸³»´Â ÅÍ¹Ì³ÎÀÌ¸é
				// ¹Ù·Î ÀÌ¾î¼­ µé¾î¿Â µÎ ¹øÂ° ¹é½ºÆäÀÌ½º´Â ¹ö¸°´Ù
				if ( last == 2 && input_pending(10) ) {
					int c2 = getchar();
					if ( c2 != '\b' && c2 != EOF ) {
						ungetc(c2, stdin);
					}
				}
			}
		}
		else if ( ch == 0x1b || ch == 0x01 || ch == 0x05 ) {
			int key = ch == 0x01 ? KEY_HOME : ch == 0x05 ? KEY_END : read_escape_key();
			switch ( key ) {
			case KEY_LEFT:
				if ( cur > 0 ) {
					int last = li_prev_len(str, cur);
					cur -= last;
					li_back(last, echo);
				}
				break;
			case KEY_RIGHT:
				if ( cur < n ) {
					int l = li_cur_len(str, cur, n);
					li_put(str, cur, cur + l, echo);
					cur += l;
				}
				break;
			case KEY_HOME:
				li_back(cur, echo);
				cur = 0;
				break;
			case KEY_END:
				li_put(str, cur, n, echo);
				cur = n;
				break;
			case KEY_DEL:
				if ( cur < n ) {
					int l = li_cur_len(str, cur, n);
					memmove(str + cur, str + cur + l, n - cur - l);
					n -= l;
					li_put(str, cur, n, echo);
					if ( echo != 0 ) for ( int k = 0; k < l; k++ ) putchar(' ');
					li_back(n - cur + l, echo);
				}
				break;
			}
		}
		else if ( is_han(ch) ) {
			// ÇÑ±Û Ã¹ ¹ÙÀÌÆ®: µÎ ¹øÂ° ¹ÙÀÌÆ®¿Í ÇÔ²² ³Ö´Â´Ù
			wait_trail = true;
			pending_lead = ch;
		}
		else if ( ch <= 0x1F ) {
			// Ç¥½Ã °¡´ÉÇÏÁö ¾ÊÀº ¹®ÀÚ´Â pass
		}
		else if ( isascii(ch) ) {
			if ( n < len ) {
				memmove(str + cur + 1, str + cur, n - cur);
				str[cur] = ch;
				n++;
				li_put(str, cur, n, echo);
				cur++;
				li_back(n - cur, echo);
			} else if ( wrap_enabled && cur == n ) {
				// ÁÙÀÌ Ã¡´Ù (ÀÚµ¿ ÁÙ¹Ù²Ş): ¶ç¾î¾²±â¸é ±×³É ´ÙÀ½ ÁÙ, ±ÛÀÚ¸é ¸¶Áö¸· ³¹¸»°ú ÇÔ²²
				if ( ch == ' ' ) {
					wrap_carry.clear();
					wrap_done = true;
				} else {
					wrap_line(str, &n, std::string(1, ch), echo);
				}
				break;
			}
		}
	}

	// ¿£ÅÍ: Ä¿¼­°¡ °¡¿îµ¥¿©µµ ÁÙ ³¡À¸·Î (´ÙÀ½ ÁÙ¹Ù²ŞÀÌ ±ÛÀ» ÀÚ¸£Áö ¾Ê°Ô)
	if ( cur < n ) li_put(str, cur, n, echo);
	str[n] = 0;
}

#if 0
void _line_input(char *str, char *init_str, int len, int echo)
{
	int j;
    int i = strlen(init_str);
    char ch;
	sprintf(str, "%s", init_str);
	for(j=0; j<strlen(init_str); j++) {
		putchar(init_str[j]);
	}

    while((ch=getchar()) != '\r' ) {
        if(ch == '\b') {
            if(i > 0) {
				if (is_han(str[i-1])) {
					putchar(ch); putchar(' '); putchar(ch);
					if(i > 0) i--;
					putchar(ch); putchar(' '); putchar(ch);
					if(i > 0) i--;
				} else if (isascii(str[i-1])) {
					putchar(ch); putchar(' '); putchar(ch);
					if(i > 0) i--;
				}
            }
        }
		else if(ch == 27) {
			char ch2 = getchar();
			char ch3 = getchar();
#if 0
			if (ch2 == 91) {
				/*
				if (ch3 == 65) printf("Up Key");
				if (ch3 == 66) printf("Down Key");
				if (ch3 == 67) printf("Right Key");
				*/
				// Left Key
				if (ch3 == 68) {
					if(i > 0) {
						// ÇÑ±ÛÀÌ¸é..
						if (is_han(str[i-1])) {
							printf("[2D");
							if(i > 0) i--;
							if(i > 0) i--;
						} else {
							printf("[1D");
							if(i > 0) i--;
						}
					}
				}
				// Right Key
				if (ch3 == 67) {
					if(i < strlen(str)) {
						// ÇÑ±ÛÀÌ¸é..
						if (is_han(str[i+1])) {
							printf("[2C");
							if(i < strlen(str)) i++;
							if(i < strlen(str)) i++;
						} else {
							printf("[1C");
							if(i < strlen(str)) i++;
						}
					}
				}
			}
#endif
		}
        else if((ch == 0x1b) | (ch == 0x18) | (ch == 0x0f));
        else if(i < len) {
            str[i++] = ch;
            if     (echo==0) ;
            else if(echo==2) putchar(' ');
            else if(echo==3) putchar('*');
            else             putchar(ch);
        }
    }

    str[i] = 0;
}
#endif

void line_input_edit(char *str, char *init_str, int len)
{
    _line_input(str, init_str, len, 1);
}

void line_input(char *str, int len)
{
    _line_input(str, (char*)"", len, 1);
}

// ÁÙ ÆíÁı±â¿ë: init_str ¿¡ ÀÌ¾î¼­ ¹Ş°í, ÁÙÀÌ Â÷¸é ¸¶Áö¸· ³¹¸»À» carry ¿¡ ´ã¾Æ true (´ÙÀ½ ÁÙ¿¡¼­ ÀÌ¾î ¹Ş´Â´Ù)
bool line_input_wrap(char *str, const char *init_str, int len, std::string &carry)
{
	wrap_enabled = true;
	wrap_done = false;
	wrap_carry.clear();
	_line_input(str, (char*)init_str, len, 1);
	wrap_enabled = false;
	carry = wrap_done ? wrap_carry : "";
	return wrap_done;
}

void line_input2(char *mess, char *str, int len)
{
    printf(mess);
    _line_input(str, (char*)"", len, 1);
}

void line_input_echo(char *str, int len)
{
    _line_input(str, (char*)"", len, 3);
}

void press_enter(void) 
{
	char buff[1];
	line_input(buff, 0);
}

int yesno(int defaultkey)
{
	char buff[10];
    line_input(buff, 2);

	printf(ESC_ENG);

	if (buff[0] == 0) 
		return defaultkey;

	if (buff[0] == 'Y' || buff[0] == 'y' || buff[0] == '1') 
		return YES;

	return NO;
}

void goto_screen(int row, int col)
{
	printf("\033[%d;%dH", row, col);
}

void clear_screen(void)
{
    printf("\033[2J\033[;H");
}

// ANSI ÄÚµå »èÁ¦
static char newline1[9072];
char *strip_ansi_codes(const char *line)
{
	const char *tstr = line;
	char *nstr = newline1;
	int gotansi = 0;

	char *nend = newline1 + sizeof(newline1) - 1;

	while (*tstr && nstr < nend)
	{
		/* Note that we use '\x9b' here, rather than 0x9b, because the 
		 * former will have the correct value whether or not char is
		 * signed.
		 */
		if (*tstr == '\x1b' || *tstr == '\x9b') 
			gotansi = 1;
		if (gotansi && isalpha((unsigned char)*tstr)) 
			gotansi = 0;
		else if (!gotansi) 
		{
			*nstr = *tstr;
			nstr++;
		}
		tstr++;
	}
	*nstr = 0;
	return newline1;
}

int days_in_month(int m, int y) 
{ 
    switch (m) {
        case 2 :
            return (y % 4 == 0 && y % 100) || y % 400 == 0 ? 29 : 28;
        case 9 : case 4 : case 6 : case 11 :
            return 30;
        default :
            return 31;
    }
}

bool is_date_valid(int d, int m, int y) 
{
    return m >= 0 && m <= 12 && d > 0 && d <= days_in_month(m, y);
}

bool is_dot_dash ( const char c )
{
	switch ( c )
	{
		case '.':
		case '-':
			return true;
	}
	return false;
} 


#if 0
bool is_email_valid(const char *address) 
{
	char *pattern = "^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\.[a-zA-Z0-9-.]+$";
	int i = re_match(pattern, (char*)address);
	if ( i == -1 ) 
		return false;
	else 
		return true;
}
#else
bool is_email_valid(const char *address) 
{
    char str[1024];
    sprintf(str, "%s", address);

    int size, pos=0, pos1=0, c=0;
    size=strlen(str);

    //first char should be an alphabet
    if((str[0]>='a'&& str[0]<='z')||(str[0]>='A'&& str[0]<='Z'))  {
        for(int i=0;i<size;i++) {
            //combination of characters allowed
            if((str[i]>='a'&& str[i]<='z') || (str[i]>='0' && str[i]<='9') || 
                str[i]=='.'||str[i]=='_'||str[i]=='-'||str[i]=='#'||str[i]=='@') {
                // symbol encountered 
                if(str[i]=='.'||str[i]=='_'||str[i]=='-'||str[i]=='#'||str[i]=='@') {
                    //no 2 repeated symbols
                    if((str[i+1]>='a'&&str[i+1]<='z')||(str[i+1]>='0' &&str[i+1]<='9')) {
                        //@ encountered, so domain part begins
                        if(str[i]=='@')                       
                            //pos is the position where domain begins
                            pos=i; 
                    } else {
                        return 0;
                    }
                }
            } else {
                return 0;
            }
        }
    } else {
        return 0;
    }

    if(pos==0) {
        return 0;
    } else {
        for(int j=pos+1;j<size;j++) {
            if(str[pos+1]>='a'&&str[pos+1]<='z') {
                if(str[j]=='.') {
                    pos1=j;
                }
            } else {
                return 0;
            }
        }
    }
    if(pos1==0) {
        return 0;

    } else {
        for(int k=pos1+1;k<size;k++) {
            if(str[k]>='a'&&str[k]<='z') {
                c++;
            } else {
                return 0;
            }
        }
        if(c>=2) {
            return 1;
        } else {
            return 0;
        }
    }                                           
}
#endif


// Æú´õ¸¦ »ı¼ºÇÑ´Ù.
bool mkdir2 (char *dir)
{
	// »õ·Î ¸¸µé ¶§¸¸ (ÀÌ¹Ì ÀÖ´Â ÀÌ¸§ = ³²ÀÌ ¸ÕÀú ³õÀº Æú´õ³ª ½Éº¼¸¯ ¸µÅ©ÀÏ ¼ö ÀÖ´Ù). ÁÖÀÎ¸¸ ¾²°Ô 0700.
	// ¿¹Àü¿¡´Â mkdir -p (ÀÖ¾îµµ ¼º°ø) µÚ chmod 777 ÀÌ¶ó ¹Ì¸® ³õÀº ¸µÅ©¸¦ µû¶ó°¡ ´Ù¸¥ Æú´õ¸¦ ¿­¾î ¹ö¸± ¼ö ÀÖ¾ú´Ù
	if ( mkdir(dir, 0700) != 0 ) {
		printf("\r\nÀÓ½Ã Æú´õ¸¦ ¸¸µéÁö ¸øÇß½À´Ï´Ù: %s\r\n", strerror(errno));
		return false;
	}
	int fd = open(dir, O_RDONLY | O_NOFOLLOW);
	if ( fd >= 0 ) {
		fchmod(fd, 0700);		// umask °¡ x ¸¦ Áö¿ü¾îµµ µé¾î°¥ ¼ö ÀÖ°Ô
		close(fd);
	}
	return true;
}

// ------------------------------------------------------------------------------------------
//¾Õ¿¡ ÀÖ´Â °³Çà ¹®ÀÚ Á¦°Å
std::string ltrim(std::string s) 
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), 
				std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

//µÚ¿¡ ÀÖ´Â °³Çà ¹®ÀÚ Á¦°Å
std::string rtrim(std::string s) 
{
	s.erase(std::find_if(s.rbegin(), s.rend(), 
				std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

//¾çÂÊ ³¡ÀÇ °³Çà ¹®ÀÚ Á¦°Å
std::string trim(std::string s) 
{
	return ltrim(rtrim(s));
}

// ------------------------------------------------------------------------------------------
// ÇöÀç Á¢¼ÓÀÚ¿¡ ÀÇÇØ »ı¼ºµÈ ÀÓ½Ã ÆÄÀÏµé ¸ñ·Ï Ãß°¡
// ÇÑ ÁÙÀ» width Ä­ ¾È¿¡¼­ ´Ü¾î(°ø¹é) ´ÜÀ§·Î ³ª´«´Ù. (EUC-KR 2 ¹ÙÀÌÆ® ±ÛÀÚ´Â ±úÁö ¾Ê´Â´Ù)
// °ø¹é ¾øÀÌ width º¸´Ù ±ä ºÎºĞÀº ±ÛÀÚ ´ÜÀ§·Î ÀÚ¸¥´Ù.
std::vector<std::string> wrap_words(const std::string &line, int width)
{
	std::vector<std::string> out;
	std::string rest = line;
	while ( (int)rest.size() > width ) {
		// width ¾È¿¡¼­ 2 ¹ÙÀÌÆ® ±ÛÀÚ¸¦ ±úÁö ¾Ê´Â ¸¶Áö¸· À§Ä¡
		int cut = 0;
		while ( cut < (int)rest.size() ) {
			int w = ((unsigned char)rest[cut] >= 0x80) ? 2 : 1;
			if ( cut + w > width ) break;
			cut += w;
		}
		// ±× ¾ÈÀÇ ¸¶Áö¸· °ø¹é¿¡¼­ ²÷´Â´Ù
		int sp = -1;
		for (int i = cut; i > 0; i--) {
			if ( rest[i] == ' ' ) { sp = i; break; }
		}
		if ( sp > 0 ) {
			out.push_back(rest.substr(0, sp));
			rest = rest.substr(sp + 1);
		} else {
			out.push_back(rest.substr(0, cut));
			rest = rest.substr(cut);
		}
	}
	out.push_back(rest);
	return out;
}

// È­¸é ÆíÁı±â(pico) ¿ë ÀÓ½Ã ÆÄÀÏ. ÆíÁıÇÒ ¶§¸¶´Ù µû·Î ¸¸µç µğ·ºÅÍ¸® ¾È¿¡ µĞ´Ù.
// pico ´Â -o ·Î ÀÌ µğ·ºÅÍ¸® ¹ÛÀÇ ÆÄÀÏÀ» ÀĞ°Å³ª(^R, ÆÄÀÏ ¸ñ·Ï) ¾²Áö(^O) ¸øÇÑ´Ù.
// ÆÄÀÏÀº add_user_tmpfile ·Î µî·ÏµÇ¾î Á¢¼ÓÀ» ³¡³¾ ¶§ µğ·ºÅÍ¸®¿Í ÇÔ²² Áö¿öÁø´Ù.
bool make_editor_tmpfile(char *dir, char *file, size_t size)
{
	snprintf(dir, size, "%s/tmp/editXXXXXX", getenv("HANULSO"));
	if ( mkdtemp(dir) == NULL ) return false;
	// BBS ´Â umask(0111) ·Î µ¹¾Æ¼­ mkdtemp ÀÇ 0700 ÀÌ 0600 ÀÌ µÈ´Ù (µğ·ºÅÍ¸®¿¡ µé¾î°¥ ¼ö ¾øÀ½)
	chmod(dir, 0700);
	snprintf(file, size, "%s/text.txt", dir);
	FILE *fp = fopen(file, "w");
	if ( fp == NULL ) return false;
	fclose(fp);
	// Æú´õÂ° Àû¾î µĞ´Ù (ÆíÁı±â°¡ ´Ù¸¥ ÆÄÀÏÀ» ¸¸µé¾îµµ Á¢¼ÓÀ» ³¡³¾ ¶§ ÇÔ²² Áö¿öÁöµµ·Ï)
	add_user_tmpfile(dir);
	return true;
}

// È­¸é ÆíÁı±â ½ÇÇà ¸í·É.
//   -o : ÀÛ¾÷ µğ·ºÅÍ¸® ¹ÛÀº ÀĞ°í ¾µ ¼ö ¾ø°Ô (»©¸é ¼­¹öÀÇ ´Ù¸¥ ÆÄÀÏÀÌ º¸ÀÎ´Ù)
//   -r76 : 76 Ä­¿¡¼­ ÀÚµ¿ ÁÙ¹Ù²Ş (°Ô½ÃÆÇ ±Û º¸±â Æø¿¡ ¸ÂÃã)
std::string screen_editor_command(const char *dir, const char *file)
{
	return std::string(getenv("HANULSO")) + "/bin/pico -r76 -o " + shell_quote(dir) + " " + shell_quote(file);
}

// $HANULSO/tmp ¾ÈÀÇ ÀÓ½Ã Æú´õ¸¦ ÅëÂ°·Î Áö¿î´Ù (´Ù¸¥ °÷Àº ½Ç¼ö·Î¶óµµ Áö¿ìÁö ¾Êµµ·Ï È®ÀÎ)
void remove_tmp_dir(const char *dir)
{
	std::string base = std::string(getenv("HANULSO")) + "/tmp/";
	std::string d = dir;
	if ( d.compare(0, base.size(), base) != 0 || d.size() <= base.size() || d.find("..") != std::string::npos ) {
		return;
	}
	std::string cmd = "rm -rf " + shell_quote(d);
	system(cmd.c_str());
}

// °­Á¦ Á¾·á, ¼­¹ö Àç½ÃÀÛÃ³·³ Á¤¸®ÇÏÁö ¸øÇÏ°í ³²Àº ÀÓ½Ã Æú´õ/ÆÄÀÏÀ» Áö¿î´Ù.
// (tmp/file*: ÆÄÀÏ ÁÖ°í¹Ş±â, tmp/edit*: ±Û ÆíÁı, tmp/*.file: Á¢¼ÓÀÚº° ÀÓ½Ã ÆÄÀÏ ¸ñ·Ï)
// ÇÏ·ç°¡ Áö³­ °Í¸¸, ÇÑ ½Ã°£¿¡ ÇÑ ¹ø¸¸ (Á¢¼ÓÇÒ ¶§¸¶´Ù tmp ¸¦ µÚÁöÁö ¾Êµµ·Ï)
// Æú´õ ¾ÈÀÇ º¸Åë ÆÄÀÏ ¼ö (limit ±îÁö¸¸ ¼¾´Ù)
int count_dir_files(const std::string &dir, int limit)
{
	DIR *d = opendir(dir.c_str());
	if ( d == NULL ) return 0;
	int n = 0;
	struct dirent *e;
	while ( n < limit && (e = readdir(d)) != NULL ) {
		if ( e->d_name[0] == '.' ) continue;
		struct stat st;
		if ( stat((dir + "/" + e->d_name).c_str(), &st) == 0 && S_ISREG(st.st_mode) ) n++;
	}
	closedir(d);
	return n;
}

// »õ Ã·ºÎ ÆÄÀÏÀ» ³ÖÀ» Æú´õ ("000", "001", ...): file/ ¾Æ·¡ ¹øÈ£ Æú´õ Áß ¸¶Áö¸· °Í,
// 1000 °³°¡ Ã¡À¸¸é ´ÙÀ½ ¹øÈ£¸¦ ¸¸µç´Ù. ¸ø ¸¸µé¸é "" (¿¹ÀüÃ³·³ file/ ¹Ù·Î ¾Æ·¡)
std::string attachment_bucket(void)
{
	std::string base = std::string(getenv("HANULSO")) + "/file";
	int last = -1;
	DIR *d = opendir(base.c_str());
	if ( d != NULL ) {
		struct dirent *e;
		while ( (e = readdir(d)) != NULL ) {
			const char *n = e->d_name;
			if ( strlen(n) < 3 || strspn(n, "0123456789") != strlen(n) ) continue;
			struct stat st;
			if ( stat((base + "/" + n).c_str(), &st) == 0 && S_ISDIR(st.st_mode) && atoi(n) > last ) last = atoi(n);
		}
		closedir(d);
	}
	if ( last < 0 ) last = 0;
	char name[16];
	snprintf(name, sizeof(name), "%03d", last);
	if ( count_dir_files(base + "/" + name, 1000) >= 1000 ) snprintf(name, sizeof(name), "%03d", last + 1);
	std::string dir = base + "/" + name;
	struct stat st;
	if ( stat(dir.c_str(), &st) != 0 ) {
		if ( mkdir(dir.c_str(), 0755) != 0 && errno != EEXIST ) return "";
		chmod(dir.c_str(), 0755);		// umask °¡ x ¸¦ Áö¿öµµ µé¾î°¥ ¼ö ÀÖ°Ô
	}
	return name;
}

void sweep_stale_tmp(void)
{
	char marker[1024];
	snprintf(marker, sizeof(marker), "%s/tmp/.sweep", getenv("HANULSO"));
	struct stat st;
	if ( stat(marker, &st) == 0 && time(NULL) - st.st_mtime < 3600 ) {
		return;
	}
	FILE *fp = fopen(marker, "w");
	if ( fp != NULL ) fclose(fp);

	// ÇÏ·ç Áö³­ ÀÓ½Ã ÆÄÀÏ/Æú´õ: ÆÄÀÏ ÁÖ°í¹Ş±â(file*), ±Û ÆíÁı(edit*), Á¢¼ÓÀÚº° ¸ñ·Ï(*.file),
	// ³¯¾¾/È¯À²/´º½º/¿À´Ã(*.csv, *.json, *.xml ...) µî. ¿¹Àü ÄÚµå°¡ ½ÇÆĞÇÒ ¶§ Áö¿ìÁö ¾Ê°í ³²±ä °Íµµ.
	// Á¢¼ÓÀÚ ÆÄÀÏ(*.tty), ¼û±è ÆÄÀÏ(.db_index_v1 °°Àº Ç¥½Ã), stats.cache, ·Î±×(*.log) ´Â ³²±ä´Ù.
	std::string tmp = shell_quote(std::string(getenv("HANULSO")) + "/tmp");
	std::string cmd = "find " + tmp + " -mindepth 1 -maxdepth 1 "
		"! -name '*.tty' ! -name '.*' ! -name 'stats.cache' ! -name 'mailsend.log' -mmin +1440 "
		"-exec rm -rf {} + > /dev/null 2>&1";
	system(cmd.c_str());

	// Á¢¼ÓÀÚ ÆÄÀÏ: ÇÁ·Î¼¼½º°¡ ¾ø´Â °Í (read_tty_file ÀÌ Áö¿î´Ù)
	char pattern[1024];
	snprintf(pattern, sizeof(pattern), "%s/tmp/*.tty", getenv("HANULSO"));
	std::vector<std::string> ttys = find_files(pattern);
	for ( unsigned int i = 0; i < ttys.size(); i++ ) {
		std::string id;
		read_tty_file(ttys[i], id);
	}
	// ·Î±×ÀÎ Àü(ºó) Á¢¼ÓÀÚ ÆÄÀÏÀÌ ÇÑ ½Ã°£ ³Ñ°Ô ³²Àº °Í: ·Î±×ÀÎ È­¸éÀº 10 ºĞ ÀÔ·ÂÀÌ ¾øÀ¸¸é ²÷±â¹Ç·Î
	// (°¡ÀÔ/ºñ¹Ğ¹øÈ£ Ã£±â Áß ²÷±è µîÀ¸·Î ³²Àº °Í)
	cmd = "find " + tmp + " -mindepth 1 -maxdepth 1 -name '*.tty' -empty -mmin +60 "
		"-exec rm -f {} + > /dev/null 2>&1";
	system(cmd.c_str());
}

void add_user_tmpfile(char *path)
{
	char buf[1024];
	snprintf(buf, sizeof(buf), "%s/tmp/%s.file", getenv("HANULSO"), tty);

	// ÁÖÀÎ¸¸ ÀĞ°í ¾²°Ô (¿¹Àü¿¡´Â umask 0111 ¶§¹®¿¡ 0666 ÀÌ¶ó ³²ÀÌ Áö¿ï °æ·Î¸¦ ³¢¿ö ³ÖÀ» ¼ö ÀÖ¾ú´Ù)
	int fd = open(buf, O_WRONLY | O_CREAT | O_APPEND | O_NOFOLLOW, 0600);
	if ( fd < 0 ) return;
	fchmod(fd, 0600);
	FILE *fp = fdopen(fd, "a");
	if ( fp == NULL ) { close(fd); return; }
	fprintf(fp, "%s\r\n", path);
	fclose(fp);
}

// ÇöÀç Á¢¼ÓÀÚ¿¡ ÀÇÇØ »ı¼ºµÈ ÀÓ½Ã ÆÄÀÏµé ¸ñ·Ï
std::vector<std::string> user_tmpfiles(void)
{
	std::vector<std::string> files;

	char buf[1024];
	sprintf(buf, "%s/tmp/%s.file", getenv("HANULSO"), tty);

	if ( file_exists(buf) ) {
		FILE *fp = fopen(buf, "r");

		char line[1024];
		while (!feof(fp)) {
			fgets( line, 1024, fp );
			if (strlen(line) > 0) {
				files.push_back(trim(std::string(line)));
			}
		}

		fclose(fp);
    }

	return files;
}

// ÇöÀç Á¢¼ÓÀÚ¿¡ ÀÇÇØ ÀÓ½Ã·Î »ı¼º(º¹»ç)µÈ ÆÄÀÏÀÌ ÀÖ´Ù¸é »èÁ¦.. 
void del_user_tmpfiles(void)
{
	char buf[1024];

	std::vector<std::string> files = user_tmpfiles();
	std::string tmp_root = std::string(getenv("HANULSO")) + "/tmp/";
	for(unsigned int i=0; i<files.size(); i++) {
		struct stat st;
		// $HANULSO/tmp ¾Æ·¡ÀÇ °Í¸¸ (¸ñ·Ï¿¡ ´Ù¸¥ °æ·Î°¡ ³¢¾î ÀÖ¾îµµ Áö¿ìÁö ¾Ê´Â´Ù)
		if ( files[i].compare(0, tmp_root.size(), tmp_root) != 0 || files[i].find("..") != std::string::npos ) {
			continue;
		}
		// lstat: ´Ù¿î·Îµå¿ë ½Éº¼¸¯ ¸µÅ©´Â °¡¸®Å°´Â ÆÄÀÏÀÌ ¾ø¾îµµ Áö¿ö¾ß ÇÏ¹Ç·Î
		if ( files[i].empty() || lstat(files[i].c_str(), &st) != 0 ) {
			continue;
		}
		// ÆÄÀÏ ÁÖ°í¹Ş±â ÀÓ½Ã Æú´õ´Â ¹Ş´Ù ¸¸ ÆÄÀÏÂ° Áö¿î´Ù
		if ( S_ISDIR(st.st_mode) ) {
			remove_tmp_dir(files[i].c_str());
			continue;
		}
		{
			// ÆÄÀÏ »èÁ¦
			unlink((char*)files[i].c_str());

			// Æú´õ°¡ ºñ¾î ÀÖÀ¸¸é Æú´õµµ »èÁ¦
			std::string dir = split_file_path(files[i]);
			sprintf(buf, "%s/*", dir.c_str());

			std::vector<std::string> files = find_files_time_sorted(buf);
			if (files.size() == 0 && dir + "/" != tmp_root && dir.compare(0, tmp_root.size(), tmp_root) == 0) {
				rmdir(dir.c_str());		// ºó Æú´õ¸¸ (¼ĞÀ» °ÅÄ¡Áö ¾Ê´Â´Ù)
			}
		}
	}

	// ÆÄÀÏ ±â·Ï Á¤º¸ ÆÄÀÏµµ »èÁ¦
	sprintf(buf, "%s/tmp/%s.file", getenv("HANULSO"), tty);
	unlink(buf);
}



std::string string_truncate_center (std::string str, int max, std::string sep ) 
{
    int len = str.length();
    if(len > max){
        int seplen = sep.length();

        // If seperator is larger than character limit,
        // well then we don't want to just show the seperator,
        // so just show right hand side of the string.
        if(seplen > max) {
            return str.substr(len - max);
        }

        // Half the difference between max and string length.
        // Multiply negative because small minus big.
        // Must account for length of separator too.
        int n = -0.5 * (max - len - seplen);

        // This gives us the centerline.
        int center = len/2;

        std::string front = str.substr(0, center - n);
        std::string back = str.substr(len - center + n); // without second arg, will automatically go to end of line.

        return front + sep + back;
    }

    return str;
}

#if 0
std::string string_truncate (std::string str, int length, std::string suffix ) 
{
    if(length < str.length()) {
		length = length - suffix.length();

		unsigned int i;
		for(i=0; i<str.length(); i++) {
			char c = ((char*)str.c_str())[i];
			if (is_han(c)) {
                if ( i+1 >= length ) break;
				i++;
			} else {
                if ( i+1 >= length ) break;
            }
			if ( i >= length ) break;
		}

        std::string front = str.substr(0, i+1);
        return front + suffix;
    }

    return str;
}
#else
std::string string_truncate (std::string str, int width, std::string suffix )
{
	// Â¦ÀÌ ¸ÂÁö ¾Ê´Â ÇÑ±Û ¹ÙÀÌÆ®°¡ ÀÖÀ¸¸é È­¸é Ä­ÀÌ ¾î±ß³ª¹Ç·Î ¸ÕÀú Á¤¸®
	str = fix_hangul(str);

    if(width < strlen(str.c_str())) {
        width = width - strlen(suffix.c_str());

        int start = 0;
        std::string line;
        std::string result;
        while (1) {
            if(start > strlen(str.c_str())-1) break;
            if(start > width-1) break;
            std::string sub = str.substr(start, 1);
            char c = sub.c_str()[0];

            // ÇÑ±ÛÀÌ¸é 2¹ÙÀÌÆ®¸¦ ÀĞÀ½
            if(!isascii(c)) {
                // Áö±İ±îÁö ÀĞÀº°Å¿¡ 2¸¦ ´õÇØ¼­ widthº¸´Ù Å©¸é ¶óÀÎ push_back
                if(strlen(line.c_str()) + 2 > width) {
                    return line + suffix;
                }
                sub = str.substr(start, 2);
                start+=2;

            // ¿µ¹®ÀÌ¸é 1¹ÙÀÌÆ®¸¦ ÀĞÀ½
            } else {
                // Áö±İ±îÁö ÀĞÀº°Å¿¡ 1¸¦ ´õÇØ¼­ widthº¸´Ù Å©¸é ¶óÀÎ push_back
                if(strlen(line.c_str()) + 1 > width) {
                    return line + suffix;
                }
                start+=1;
            }

            line += sub;
        }

        return line + suffix;
    }

    return str;
}
#endif

char *iconv_convert(char *tgt, char *src, char *input, float rate)
{
	iconv_t it = iconv_open(tgt, src);
	if(it == (iconv_t) -1)
	{
		fprintf(stderr, "iconv open error");
		return NULL;
	}
	size_t nSrc = strlen(input) + 1;  // for '\0'
	size_t nTgt = nSrc * rate;
	char * output = (char *)malloc(nTgt);
	char * pOutput = output;
	// printf("s:%lu\tt:%lu\n", nSrc, nTgt);
	//
	if(iconv(it, (char **)&input, &nSrc, &pOutput, &nTgt) == (size_t) -1)
	{
		fprintf(stderr, "iconv error\n");
		return NULL;
	}
	// printf("s:%lu\tt:%lu\n", nSrc, nTgt);
	return output;    // Don't forget to 'free()'!!
}

char *utf8_to_cp949(char * input)
{ 
	return (char*)iconv_convert((char*)"CP949//TRANSLIT//IGNORE", 
		(char*)"UTF-8//TRANSLIT//IGNORE", input, 1); 
}

char *cp949_to_utf8(char * input)
{ 
	return (char*)iconv_convert((char*)"UTF-8//TRANSLIT//IGNORE", 
		(char*)"CP949//TRANSLIT//IGNORE", input, 3); 
}

std::string file_ext_name(std::string file)
{
	std::size_t found = file.find_last_of(".");
	if ( found == std::string::npos ) return "";
	return file.substr(found);
}

std::string filename_only(std::string file)
{
	std::size_t found = file.find_last_of("/");
	return file.substr(found+1);    
}

bool check_used_port(int port)
{
	struct sockaddr_in sin;
	int socket_fd;

	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_fd == -1)
	  return -1;

	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = 0;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_family = AF_INET;

	if (bind(socket_fd, (struct sockaddr *)&sin, sizeof(struct sockaddr_in)) == -1) {
		if (errno == EADDRINUSE) {
			close(socket_fd);
			//printf("Port in use");
			return true;
		}
	}

	close(socket_fd);
	return false;
}

// URL À» path ·Î ¹Ş¾Æ ¿Â´Ù.
// CentOS 6 ÀÇ wget Àº SNI ¸¦ Áö¿øÇÏÁö ¾Ê¾Æ https »çÀÌÆ®°¡ ¸¹ÀÌ ½ÇÆĞÇÏ¹Ç·Î curl À» ¸ÕÀú ¾´´Ù.
// ¿À·¡µÈ ÀÎÁõ¼­ ¹­À½ ¶§¹®¿¡ ÀÎÁõ¼­ °ËÁõÀº ÇÏÁö ¾Ê´Â´Ù (´º½º/³¯¾¾ µî ÀĞ±â Àü¿ë ÀÚ·á).
// ÀÏºÎ »çÀÌÆ®´Â ´Ü¼øÇÑ User-Agent ¸¦ ¸·À¸¹Ç·Î compatible Çü½ÄÀ» ¾´´Ù.
bool download_url(const std::string &url, const std::string &path)
{
	const char *ua = "Mozilla/5.0 (compatible; OldDosBBS/1.0)";
	char buf[4096];

	snprintf(buf, sizeof(buf), "curl -s -L -k --max-time 20 -A %s -o %s %s",
			shell_quote(ua).c_str(), shell_quote(path).c_str(), shell_quote(url).c_str());
	int a = system(buf);
	if ( WEXITSTATUS(a) == 0 && file_size(path) > 0 ) {
		return true;
	}

	snprintf(buf, sizeof(buf), "wget -q -T 20 -t 1 --no-check-certificate -U %s -O %s %s",
			shell_quote(ua).c_str(), shell_quote(path).c_str(), shell_quote(url).c_str());
	a = system(buf);
	if ( WEXITSTATUS(a) == 0 && file_size(path) > 0 ) {
		return true;
	}

	return false;
}

// ¸í·ÉÀÇ Ãâ·ÂÀ» ¸ğµÎ ÀĞ´Â´Ù
static bool read_command_output(const std::string &cmd, std::string &out)
{
	out.clear();
	FILE *fp = popen(cmd.c_str(), "r");
	if ( fp == NULL ) return false;
	char buf[8192];
	size_t n;
	while ( (n = fread(buf, 1, sizeof(buf), fp)) > 0 ) {
		out.append(buf, n);
	}
	int st = pclose(fp);
	return st != -1 && WIFEXITED(st) && WEXITSTATUS(st) == 0;
}

// URL ÀÇ ³»¿ëÀ» ÀÓ½Ã ÆÄÀÏ ¾øÀÌ ¹Ş¾Æ ¿Â´Ù (curl, ¾È µÇ¸é wget).
// ¹Ş´Â Áß¿¡ Á¢¼ÓÀÌ ²÷°Üµµ tmp ¿¡ Âî²¨±â°¡ ³²Áö ¾Ê´Â´Ù.
bool download_text(const std::string &url, std::string &out)
{
	const char *ua = "Mozilla/5.0 (compatible; OldDosBBS/1.0)";

	if ( read_command_output("curl -s -L -k --max-time 20 -A " + shell_quote(ua) + " " + shell_quote(url), out)
			&& !out.empty() ) {
		return true;
	}
	if ( read_command_output("wget -q -T 20 -t 1 --no-check-certificate -U " + shell_quote(ua)
				+ " -O - " + shell_quote(url), out) && !out.empty() ) {
		return true;
	}
	out.clear();
	return false;
}

// UTF-8 À» ¿Ï¼ºÇü(CP949) À¸·Î. ¹Ù²Ü ¼ö ¾ø´Â ±ÛÀÚ´Â ¹ö¸°´Ù (iconv -c ¿Í °°À½)
std::string utf8_to_cp949(const std::string &in)
{
	iconv_t cd = iconv_open("CP949//TRANSLIT", "UTF-8");
	if ( cd == (iconv_t)-1 ) return in;

	std::string out;
	char *src = (char*)in.data();
	size_t left = in.size();
	char buf[8192];
	while ( left > 0 ) {
		char *dst = buf;
		size_t room = sizeof(buf);
		size_t r = iconv(cd, &src, &left, &dst, &room);
		out.append(buf, dst - buf);
		if ( r == (size_t)-1 && errno != E2BIG ) {
			// ¹Ù²Ü ¼ö ¾ø´Â ±ÛÀÚ³ª ±úÁø ±ÛÀÚ: ÇÑ ¹ÙÀÌÆ®¾¿ °Ç³Ê¶Ú´Ù
			// (³²Àº µŞ ¹ÙÀÌÆ®µéµµ ±úÁø ±ÛÀÚ·Î ´Ù½Ã °É·Á Â÷·Ê·Î °Ç³Ê¶Ù¾îÁø´Ù)
			src++;
			left--;
		}
	}
	iconv_close(cd);
	return out;
}

// ¼Ğ ¸í·É ÀÎÀÚ¸¦ ÀÛÀºµû¿ÈÇ¥·Î °¨½Ñ´Ù (' ´Â '\'' ·Î ¹Ù²Ş)
std::string shell_quote(const std::string &str)
{
	std::string ret = "'";
	for (unsigned int i=0; i<str.length(); i++) {
		if ( str[i] == '\'' ) {
			ret += "'\\''";
		} else {
			ret += str[i];
		}
	}
	ret += "'";
	return ret;
}

std::string add_slashes(std::string str)
{
	std::string new_str;

	std::set<char> needs_slash;

	//needs_slash.insert('\'');
	needs_slash.insert('\"');
	needs_slash.insert('\0');
	needs_slash.insert('\\');

	for (unsigned int i=0; i<str.length(); i++) {
		if (needs_slash.find(str[i]) != needs_slash.end())
			new_str.push_back('\\');
		new_str.push_back(str[i]);
	}

	return new_str;
}

// return 0: weak
// return 1: moderate
// return 2: strong
int check_password_strongness(std::string str)
{
    int n = str.length();
 
    // Checking lower alphabet in string
    bool hasLower = false, hasUpper = false;
    bool hasDigit = false, specialChar = false;
    std::string normalChars = "abcdefghijklmnopqrstu"
        "vwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890 ";
 
    for (int i = 0; i < n; i++) {
        if (islower(str[i]))
            hasLower = true;
        if (isupper(str[i]))
            hasUpper = true;
        if (isdigit(str[i]))
            hasDigit = true;
 
        size_t special = str.find_first_not_of(normalChars);
        if (special != std::string::npos)
            specialChar = true;
    }
 
    // Strength of password
    //cout << "Strength of password:-";
    if (hasLower && hasUpper && hasDigit && 
        specialChar && (n >= 8))
        //cout << "Strong" << endl;
		return 2;
    else if ((hasLower || hasUpper) && 
             specialChar && (n >= 6))
		return 1;
        //cout << "Moderate" << endl;
    else
		return 0;
        //cout << "Weak" << endl;
}


std::string html2text(std::string html)
{
	std::string hp = make_private_tmpfile("html");
	std::string pp = make_private_tmpfile("html_out");
	if ( hp.empty() || pp.empty() ) {
		if ( !hp.empty() ) unlink(hp.c_str());
		if ( !pp.empty() ) unlink(pp.c_str());
		return "";
	}
	char html_file[1024];
	snprintf(html_file, sizeof(html_file), "%s", hp.c_str());

	FILE *fp = fopen(html_file, "w");
	if ( fp == NULL ) return "";
	fputs(html.c_str(), fp);
	fclose(fp);

	char plain_file[1024];
	snprintf(plain_file, sizeof(plain_file), "%s", pp.c_str());

	char buf[1024];
	snprintf(buf, sizeof(buf), "lynx -dump -nomargins -assume_charset=euc-kr -display_charset=euc-kr %s"
		"> %s", shell_quote(html_file).c_str(), shell_quote(plain_file).c_str());
	system(buf);

	std::string text = read_file(plain_file);

	unlink(html_file);
	unlink(plain_file);

	return text;
}


