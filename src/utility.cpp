#include "main.h"

std::string random_string(const int len)
{
	char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!@#$%^&*()_+";
	std::stringstream ss;
	srand(time(NULL));
	for (int i = 0; i < len; ++i) {
		ss << alphanum[rand() % (sizeof(alphanum) - 1)];
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
		//printf("Failed to run command\n" );
		//exit(1);
	}

	/* Read the output a line at a time - output it. */
	while (fgets(line, sizeof(line)-1, fp) != NULL) {
		lines.push_back(std::string(line));
	}

	/* close */
	pclose(fp);

	*success = true;
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

std::string query_luck(int yy, int mm, int dd, int sex, int birth_yy, int birth_mm, int birth_dd, int lunar, int yun)
{
	char tmp_file[1024];
	sprintf(tmp_file, "%s", tempnam("/tmp", "luck"));

	char www[1024];
	sprintf(www, "http://today.freeunse.funstory.biz/sub/tradition1.php?unse_mode=1"
		// ³¯Â¥
		"&yy=%d&mm=%d&dd=%d"
		// ³²(1)/¿©(2)
		"&unse1_sex=%d"
		// »ý³â¿ù/ÀÏ
		"&unse1_yy=%d&unse1_mm=%d&unse1_dd=%d"
		// »ý³â½Ã
		"&unse1_hh=&"
		// ¾ç·Â(1)/À½·Â(2)
		"unse1_solun=%d"
		// Æò´Þ(1)/À±´Þ(2)
		"&unse1_lun_yn=%d", 
		yy, mm, dd, sex, birth_yy, birth_mm, birth_dd, lunar, yun);

	char buf[1024];
	sprintf(buf, "lynx -dump -nomargins -width=70 -assume_charset=euc-kr -display_charset=euc-kr '%s'"
		"| sed 's/¤ý/* /g' > '%s'", www, tmp_file);
	system(buf);

	std::string txt = read_file(tmp_file);

	sprintf(buf, "rm -f %s", tmp_file);
	system(buf);

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
	const char *weeks[] = {"ÀÏ", "¿ù", "È­", "¼ö", "¸ñ", "±Ý", "Åä" };

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
		sprintf(buf, "%s (%s)", buf, week_s.c_str());
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

                // ÇÑ±ÛÀÌ¸é 2¹ÙÀÌÆ®¸¦ ÀÐÀ½
                if(!isascii(c)) {
                    // Áö±Ý±îÁö ÀÐÀº°Å¿¡ 2¸¦ ´õÇØ¼­ widthº¸´Ù Å©¸é ¶óÀÎ push_back
                    if(line.length() + 2 > width) {
                        result.push_back(line);
                        line = "";
                    }

                    sub = segment.substr(start, 2);
                    start+=2;

                // ¿µ¹®ÀÌ¸é 1¹ÙÀÌÆ®¸¦ ÀÐÀ½
                } else {
                    // Áö±Ý±îÁö ÀÐÀº°Å¿¡ 1¸¦ ´õÇØ¼­ widthº¸´Ù Å©¸é ¶óÀÎ push_back
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

	while (size >= 1024 && div < (sizeof SIZES / sizeof *SIZES)) {
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

// ------------------------------------------------------------------------
void _line_input(char *str, char *init_str, int len, int echo)
{
	unsigned int j;
    int i = strlen(init_str);
    char ch;

	sprintf(str, "%s", init_str);
	for(j=0; j<strlen(init_str); j++) {
		putchar(init_str[j]);
	}

    while((ch=getchar()) != '\r' ) {
        if(ch == '\b') {
            if(i > 0) {
				putchar(ch); putchar(' '); putchar(ch);
				if(i > 0) i--;
            }
        } else {
			// Ç¥½Ã °¡´ÉÇÏÁö ¾ÊÀº ¹®Á¦°¡ ÀÔ·ÂµÇ°í ÇÑ±ÛÀÌ ¾Æ´Ï¸é pass
			if (ch <= 0x1F && !is_han(ch)) {
				// pass
			}
			else if (is_han(ch) || isascii(ch)) {
				if(i < len) {
					str[i++] = ch;
					if     (echo==0) ;
					else if(echo==2) putchar(' ');
					else if(echo==3) putchar('*');
					else             putchar(ch);
				}
			}
		}
    }

    str[i] = 0;
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

	while (*tstr) 
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


// Æú´õ¸¦ »ý¼ºÇÑ´Ù.
bool mkdir2 (char *dir)
{
	char buf[9072];

	// Æú´õ »ý¼º
	sprintf(buf, "mkdir -p \"%s\"", dir);
	int a = system(buf);
	if ( WEXITSTATUS(a) != 0 ) {
		printf("\r\n¾÷·Îµå Æú´õ¸¦ »ý¼ºÇÏ´Âµ¥ ½ÇÆÐÇÏ¿´½À´Ï´Ù.\r\n");
		return false;
	}

	// ±ÇÇÑ º¯°æ
	sprintf(buf, "chmod 777 \"%s\"", dir);
	a = system(buf);
	if ( WEXITSTATUS(a) != 0 ) {
		printf("\r\n¾÷·Îµå Æú´õ ±ÇÇÑÀ» º¯°æÇÏ´Âµ¥ ½ÇÆÐÇÏ¿´½À´Ï´Ù.\r\n");
		return false;
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
// ÇöÀç Á¢¼ÓÀÚ¿¡ ÀÇÇØ »ý¼ºµÈ ÀÓ½Ã ÆÄÀÏµé ¸ñ·Ï Ãß°¡
void add_user_tmpfile(char *path)
{
	char buf[1024];
	sprintf(buf, "%s/tmp/%s.file", getenv("HANULSO"), tty);

	FILE *fp = fopen(buf, "a");
	fprintf(fp, "%s\r\n", path);
	fclose(fp);
}

// ÇöÀç Á¢¼ÓÀÚ¿¡ ÀÇÇØ »ý¼ºµÈ ÀÓ½Ã ÆÄÀÏµé ¸ñ·Ï
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

// ÇöÀç Á¢¼ÓÀÚ¿¡ ÀÇÇØ ÀÓ½Ã·Î »ý¼º(º¹»ç)µÈ ÆÄÀÏÀÌ ÀÖ´Ù¸é »èÁ¦.. 
void del_user_tmpfiles(void)
{
	char buf[1024];

	std::vector<std::string> files = user_tmpfiles();
	for(unsigned int i=0; i<files.size(); i++) {
		if ( file_exists((char*)files[i].c_str()) ) {
			// ÆÄÀÏ »èÁ¦
			unlink((char*)files[i].c_str());

			// Æú´õ°¡ ºñ¾î ÀÖÀ¸¸é Æú´õµµ »èÁ¦
			std::string dir = split_file_path(files[i]);
			sprintf(buf, "%s/*", dir.c_str());

			std::vector<std::string> files = find_files_time_sorted(buf);
			if (files.size() == 0) {
				sprintf(buf, "rm -rf \"%s\"", dir.c_str());
				system(buf);
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

            // ÇÑ±ÛÀÌ¸é 2¹ÙÀÌÆ®¸¦ ÀÐÀ½
            if(!isascii(c)) {
                // Áö±Ý±îÁö ÀÐÀº°Å¿¡ 2¸¦ ´õÇØ¼­ widthº¸´Ù Å©¸é ¶óÀÎ push_back
                if(strlen(line.c_str()) + 2 > width) {
                    return line + suffix;
                }
                sub = str.substr(start, 2);
                start+=2;

            // ¿µ¹®ÀÌ¸é 1¹ÙÀÌÆ®¸¦ ÀÐÀ½
            } else {
                // Áö±Ý±îÁö ÀÐÀº°Å¿¡ 1¸¦ ´õÇØ¼­ widthº¸´Ù Å©¸é ¶óÀÎ push_back
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
	char tmp_file[1024];
	sprintf(tmp_file, "%s", tempnam("/tmp", "html"));

	char html_file[1024];
	sprintf(html_file, "%s.html", tmp_file);

	FILE *fp = fopen(html_file, "w");
	fprintf(fp, html.c_str());
	fclose(fp);

	char plain_file[1024];
	sprintf(plain_file, "%s.out", html_file);

	char buf[1024];
	sprintf(buf, "lynx -dump -nomargins -assume_charset=euc-kr -display_charset=euc-kr '%s'"
		"> '%s'", html_file, plain_file);
	system(buf);

	std::string text = read_file(plain_file);

	sprintf(buf, "rm -f %s", html_file);
	system(buf);

	sprintf(buf, "rm -f %s", plain_file);
	system(buf);

	return text;
}


