#ifndef _ANSI_H_
#define _ANSI_H_

#define ESC_CHAR	((char) 0x1B)

#define ESC_ENG "[?85l"
#define ESC_HAN "[?85h"
#define ESC_CLEAR "[;H[2J"

#define ESC_RESET "c"

#define ESCVTX "%Ac&6A"   //vtxON
#define VT100 "[61\"p"
#define CURSOR_ON "[?25h"
#define CURSOR_OFF "[?25l"

//	#define ESCVTX "&6A"   //vtxON
#define VTXOFF "&6@"     //vtxOFF
#define ESCWTX "[114d"      //±×¸²ON
#define WTXOFF "[133d[d"      //±×¸²OFF
#define C_SAVE "[s"
#define C_UNDO "[u"

//Ä®¶óÁöÁ¤ºÎºÐ
#define C_NORMAL  "[0m[1m[37m[0m"
#define C_BLACK "[0;30m"
#define C_RED "[0;31m"
#define C_GREEN "[0;32m"
#define C_BROWN "[0;33m"
#define C_BLUE "[0;34m"
#define C_MAGENTA "[0;35m"
#define C_CYAN "[0;36m"
#define C_WHITE "[0;37m"
#define C_GREY "[0;30m"
#define C_LT_RED "[1;31m"
#define C_LT_GREEN "[1;32m"
#define C_YELLOW "[1;33m"
#define C_LT_BLUE "[1;34m"
#define C_LT_MAGENTA "[1;35m"
#define C_LT_CYAN "[1;36m"
#define C_HI_WHITE "[1;37m"
#define C_GBLACK "[0;40m"
#define C_GRED "[0;41m"
#define C_GGREEN "[0;42m"
#define C_GBROWN "[0;43m"
#define C_GBLUE "[0;44m"
#define C_GMAGENTA "[0;45m"
#define C_GCYAN "[0;46m"
#define C_GWHITE "[0;47m"
#define C_BLINK "[5m"
#define C_INVERSE "[7m"
#define C_REVERSE "[0m[1m"

#endif
