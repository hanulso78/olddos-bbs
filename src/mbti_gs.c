/*
   ÆÄÀÏ¸í:MBTI GS
   ¸¸µç»ç¶÷: ÀÌÀ±¼º
   ¸¸µç³¯Â¥ 2012³â 4¿ù 2ÀÏ

   2018/1/10  	BBS¿ëÀ¸·Î ¼öÁ¤(¹ÎÀÎÇÐ)
   2012/4/23  	fflush Ãß°¡
   2012/4/24 	QWE·Î ¸¸µë 
   24¹ø ¿£ÅÍ°ª
   2012/10/09	Ãß°¡¼³¸í Ãß°¡

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void istj();
void isfj();
void infj();
void intj();
void istp();
void isfp();
void infp();
void intp();
void estp();
void esfp();
void enfp();
void entp();
void estj();
void esfj();
void enfj();
void entj();

void print_header(char *str)
{
	unsigned int i;
	printf("\r\n[=15G[=0F");
	for(i=0; i<strlen(str); i++) {
		if(*(str+i) != '\r' && *(str+i) != '\n') {
			printf("%c", *(str+i));
		}
	}
	printf("[=1G[=15F\r\n");
}

void _line_input(char *str, int len)
{
    int i = 0;
    char ch;
    while((ch=getchar()) != '\r' ) {
        if(ch == '\b') {
            if(i > 0) {
				putchar(ch); putchar(' '); putchar(ch);
				if(i > 0) i--;
            }
        }
        else if((ch == 0x1b) | (ch == 0x18) | (ch == 0x0f));
        else if(i < len) {
            str[i++] = ch;
            putchar(ch);
        }
    }

    str[i] = 0;
}

void input(char *c)
{
	char buff[10];
    _line_input(buff, 1);
	printf("\r\n");
	if ( buff[0] == 'x' || buff[0] == 'X' ) {
		printf("\r\nÇÁ·Î±×·¥À» Á¾·áÇÕ´Ï´Ù.");
		exit(0);
	}
	*c = buff[0];
}

void input_two(char *c, char *c2)
{
	char a=0;
	char b=0;
	char buff[10];
    _line_input(buff, 3);
	printf("\r\n");
	sscanf(buff, "%c %c", &a, &b);
	if ( (a == 'x' || a == 'X') || (b == 'x' || b == 'X') ) {
		printf("\r\nÇÁ·Î±×·¥À» Á¾·áÇÕ´Ï´Ù.");
		exit(0);
	}
	*c = a;
	*c2 = b;
}

int main (void)
{
	int van=0; // ¹Ýº¹ º¯¼ö									
	char men=0; // ¼ºº° º¯¼ö
	char lant=0; // Ç×»ó º¯ÇÏ´Â º¯¼ö		
	char lant24=0; // 24¹ø º¯¼ö

	//MBTI º¯¼ö
	int E=0;
	int I=0;
	int S=0;
	int N=0;
	int T=0;
	int F=0;
	int J=0;
	int P=0;

	int mbe=0;
	int mbi=0;
	int mbs=0;
	int mbn=0;
	int mbf=0;
	int mbt=0;
	int mbj=0;
	int mbp=0;

	printf("[2J[H\r\n");
	printf("   [=15G[=0F                                                                         [=1G[=15F    \r\n");
	printf("   [=15G[=0F                          MBTI GS ¼º°Ý À¯Çü °Ë»ç                         [=1G[=15F    \r\n");
	printf("   [=15G[=0F                                                                         [=1G[=15F    \r\n\r\n");

	printf("                                                  ¼­°­´ëÇÐ±³ ±è Á¤ ÅÃ\r\n");
	printf("                                                  ºÎ»ê´ëÇÐ±³ ½É Çý ¼÷\r\n");
	printf("                                                  ÇÁ·Î±×·¡¸Ó ÀÌ À± ¼º\r\n\r\n\r\n");

	printf("[=15G[=0F¡Ù ÁÖÀÇ »çÇ× ¡Ù[=1G[=15F\r\n");
	printf("¡Ø24¹ø ¹®Á¦´Â ¼±ÅÃÀ» 2°³ ÇÒ ¼ö ÀÖÀ¸´Ï ÁÖÀÇÇÏ½Ã±â ¹Ù¶ø´Ï´Ù.\r\n");
	printf("¡Øºü¸£°Ô ÀÔ·Â½Ã ¹«ÇÑ¹Ýº¹ÀÌ µÉ ¼öµµ ÀÖÀ¸´Ï ÃµÃµÈ÷ ÀÔ·ÂÇØÁÖ½Ã±â ¹Ù¶ø´Ï´Ù.\r\n");
	printf("¡ØÁßº¹ ÀÔ·Â½Ã Ã³À½ ¹®ÀÚÀÇ °ªÀÌ ÀÔ·ÂµË´Ï´Ù. ex)qwe´Â qÀÇ °ªÀ» °®°ÔµË´Ï´Ù.\r\n");
	printf("¡Ø°Ë»çÃë¼Ò´Â 'X' ¸¦ ÀÔ·ÂÇÏ¼¼¿ä.\r\n\r\n");

	do
	{
		printf("°Ë»ç¸¦ ¹Þ±â Àü¿¡ ÀÐ¾î º¼ »çÇ×À» ÀÐ¾îº¸½Ã°Ú½À´Ï±î?(Y/N): ");
		input(&lant);
		//fflush(stdin);

		if(lant=='Y'||lant=='y')
		{
			van=0;
			printf("\r\nÀÌ °Ë»ç´Â ¼º°ÝÀ» Áø´ÜÇÏ°Å³ª\r\n°³ÀÎÀÇ ´É·ÂÀ» Æò°¡ÇÏ´Â ½É¸®°Ë»ç°¡ Àý´ë ¾Æ´Õ´Ï´Ù.\r\n¼±ÃµÀûÀÎ ½É¸®°æÇâÀ» ¾Ë¾Æº¸´Â ºñÁø´Ü °Ë»çÀÔ´Ï´Ù.\r\n\r\n");

			printf("ÀÌ °Ë»ç´Â °³ÀÎ¸¶´Ù Å¸°í³ª´Â ½É¸®Àû °æÇâ°ú\r\n±× °æÇâÀÌ Áö´Ï´Â ¼º°ÝÀÇ ¿ªµ¿À» ¾Ë¾Æ³»¾î\r\nÀÚ±â¿Í Å¸ÀÎÀ» ÀÌÇØÇÏ°í °¡Á¤°ú »çÈ¸¿Í ÇÐ±³¿Í Á÷Àå ¹× ½Ç»ýÈ°¿¡\r\nµµ¿òÀ» ÁÖ°íÀÚ ÇÏ´Âµ¥ ±× ¸ñÀûÀÌ ÀÖ½À´Ï´Ù.\r\n\r\n");

			printf("ÀÌ °Ë»çÀÇ ¹®Ç×¿¡ ¡°¸Â´Â´ä¡±°ú ¡°Æ²¸°´ä¡±ÀÌ ¾ø½À´Ï´Ù.\r\n°¢ ¹®Ç×À» ÀÐÀº ´ÙÀ½ ÀÚ½Å¿¡°Ô ½À°üÃ³·³ Æí¾ÈÇÏ°í\r\nÀÚ¿¬½º·´°í ÀÚÁÖ¾²´Â °æÇâ. Áï ÀÚ½Å¿¡°Ô Á» ´õ °¡±õ´Ù°í\r\n»ý°¢µÇ´Â °ÍÀ» °ñ¶ó º°µµÀÇ ´ä¾È¿ëÁö¿¡ Ç¥½ÃÇÏ¸é µË´Ï´Ù.\r\n\r\n");

			printf("½Ã°£Á¦ÇÑÀº ¾øÀ¸³ª ¾î´À ÇÑ ¹®Ç×À» ³Ê¹« ¿À·¡ »ý°¢ÇÏÁö ¸¶½Ê½Ã¿À.\r\nÀÇ½ÄÀûÀ¸·Î ÀÏ°ü¼º ÀÖ°Ô ÀÀ´äÇÏ·Á ÇÏÁö ¸»°í\r\nÀÚ¿¬½º·¯¿î ¸¶À½°¡ÁüÀ» °¡Áö°í ÀÀ´äÇÏµµ·Ï ÇÏ½Ê½Ã¿À.\r\n\r\n");

			printf("ÀÚ½ÅÀÌ ÀÌ»óÀûÀ¸·Î ¹Ù¶ó´Â °Í¿¡ ´äÇÏÁö ¸¶½Ã°í\r\nÀÚ½ÅÀÌ ½À°üÃ³·³ Æí¾ÈÇÏ°í, ÀÚ¿¬½º·´°Ô\r\nÀÚÁÖ ´À³¢°í Çàµ¿ÇÏ´Â °æÇâ¿¡ Ç¥½ÃÇÏ½Ê½Ã¿À.\r\n");

		}
		else if(lant=='N'||lant=='n')
		{
			van=0;
		}
		else
		{
			van=1;
		}

	}while(van>=1);

	do
	{
		printf("\r\n´ç½ÅÀº ¿©¼ºÀÔ´Ï±î ³²¼ºÀÔ´Ï±î?(³²¼º=Q,¿©¼º=W): ");
		input(&men);
		//fflush(stdin);
		if(men=='Q'||men=='q')
		{
			men='Q';
			van=0;
		}
		else if(men=='W'||men=='w')
		{
			men='W';
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	// 1ºÎ-----
	print_header("¡Ø Á¦ 1ºÎ ¡Ø");
	printf("\r\nÀÚ½Å¿¡°Ô ÀÚ¿¬½º·´°í, ½À°üÃ³·³ Æí¾ÈÇÏ°Ô ´À²¸Áö°í,\r\nÀÚÁÖ Çàµ¿ÇÏ´Â °æÇâ°ú °¡±õ´Ù°í »ý°¢µÇ´Â °ÍÀ» ¼±ÅÃÇÏ¿© ´ä¾ÈÁö¿¡ Ç¥½ÃÇÏ½Ê½Ã¿À.\r\n");

	do
	{
		printf("\r\n1. ³ª´Â ´ëÃ¼·Î\r\n");
		printf("(Q)´Ù¸¥ »ç¶÷°ú ½±°Ô ¾î¿ï¸®´Â ÆíÀÌ´Ù.\r\n(W)Á¶¿ëÇÏ°í ¸»ÀÌ ¾ø´Â ÆíÀÌ´Ù. : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			E=E+2;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			I=I+2;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n2. ¸¸¾à ÀÚ½ÅÀÌ ±³»ç¶ó¸é\r\n");
		printf("(Q)½ÇÁ¦ÀûÀÎ »ç°í¸¦ ´Ù·ç´Â °ú¸ñÀ» °¡¸£Ä¡°í ½Í´Ù.\r\n(W)ÀÌ·ÐÀûÀÎ °ú¸ñÀ» °¡¸£Ä¡°í ½Í´Ù. : ");
		input(&lant);
		//fflush(stdin);	
		if(lant=='Q'||lant=='q')
		{
			S=S+2;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			N=N+2;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n3. ³ª´Â\r\n");	
		printf("(Q)°¨¼ºÀ» ÀÌ¼ºº¸´Ù ´õ ÀÚÁÖ ³»¼¼¿ì´Â ÆíÀÌ´Ù.\r\n(W)ÀÌ¼ºÀ» °¨¼ºº¸´Ù ´õ ÀÚÁÖ ³»¼¼¿ì´Â ÆíÀÌ´Ù. : ");
		input(&lant);
		//fflush(stdin);	
		if(lant=='Q'||lant=='q')
		{	
			if(men=='Q'||men=='q')
			{
				T=T+1;
				van=0;
			}	
			else if(men=='W'||men=='w')
			{
				T=T+2;
				van=0;
			}
		}		
		else if(lant=='W'||lant=='w')
		{
			if(men=='Q'||men=='q')
			{
				F=F+1;
				van=0;
			}
			else if(men=='W'||men=='w')
			{
				F=F+2;
				van=0;
			}
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n4. ÇÏ·çÁ¤µµ ´Ù³à¿À°í ½ÍÀ» ¶§ ³ª´Â\r\n");
		printf("(Q)¾ðÁ¦ ¹«¾ùÀ» ÇÒ °ÍÀÎ°¡¸¦ °èÈ¹ÇÏ´Â ÆíÀÌ´Ù.\r\n(W)º° °èÈ¹¾øÀÌ ÈÇÂ½ ¶°³ª´Â ÆíÀÌ´Ù. : ");
		input(&lant);
		//fflush(stdin);	
		if(lant=='Q'||lant=='q')
		{
			J=J+2;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			P=P+2;
			van=0;
		}

		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n5. ³ª´Â ¿©·¯»ç¶÷µé°ú ÇÔ²² ÀÖÀ» ¶§ ÀÏ¹Ý ÀûÀ¸·Î\r\n");
		printf("(Q)¿©·¯»ç¶÷µé°ú ÇÔ²² ³¢¾î ´ëÈ­¸¦ ³ª´©´Â ÆíÀÌ´Ù.\r\n(W)ÇÑ¹ø¿¡ ÇÑ »ç¶÷¾¿ ´ëÈ­¸¦ ³ª´©´Â ÆíÀÌ´Ù. : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			E=E+1;
			van=0;	
		}
		else if(lant=='W'||lant=='w')
		{
			I=I+2;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n6. ÀÚ½ÅÀº Æò¼Ò¿¡ ¾î¶°ÇÑ »ç¶÷µé°ú Àß ¾î¿ï¸³´Ï±î\r\n");
		printf("(Q)»ó»ó·ÂÀÌ Ç³ºÎÇÑ »ç¶÷\r\n(W)Çö½Ç°¨°¢ÀÌ ÀÖ´Â »ç¶÷ : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			S=S+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			N=N+2;
			van=0;
		}						
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n7. ÀÚ½ÅÀº ¾î¶²°ÍÀ» ´õ ÁÁÀº ÄªÂùÀÌ¶ó »ý°¢ÇÏ½Ê´Ï±î?\r\n");
		printf("(Q)¼ÖÁ÷ÇÏ°í °¨Á¤ÀûÀÎ »ç¶÷À¸·Î ºÒ¸®¿ì´Â °Í\r\n(W)²ÙÁØÇÏ°í ÇÕ¸®ÀûÀÎ »ç¶÷À¸·Î ºÒ¸®¿ì´Â °Í : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			if(men=='Q'||men=='q')
			{
				T=T+2;
				van=0;
			}
			else if(men=='W'||men=='w')
			{
				T=T+2;
				van=0;
			}
		}

		else if(lant=='W'||lant=='w')
		{
			if(men=='Q'||men=='q')
			{
				F=F+1;
				van=0;
			}
			else if(men=='W'||men=='w')
			{
				F=F+1;
				van=0;
			}
		}	
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n8. ³ª´Â\r\n");
		printf("(Q)¸ðÀÓÀÌ³ª ÆÄÆ¼µîÀ» ¿©À¯·Ó°Ô °èÈ¹ÇÏ´Â °ÍÀ» ¿øÇÏ´Â ÆíÀÌ´Ù.\r\n(W)±×¶§ »óÈ²¿¡ µû¶ó Àç¹ÌÀÖ¾î º¸ÀÌ´Â °ÍÀ» \r\n   ÇÒ ¼ö ÀÖµµ·Ï º° °èÈ¹ ¾øÀÌ ÀÚÀ¯½º·¯¿î °ÍÀ» ¿øÇÏ´Â ÆíÀÌ´Ù. : ");
		input(&lant);
		//fflush(stdin);	
		if(lant=='Q'||lant=='q')
		{
			J=J+2;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			P=P+1;
			van=0;
		}		
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n9. ³ª´Â »ç¶÷µéÀÌ ¸¹Àº ±×·ì³»¿¡¼­\r\n");
		printf("(Q)ÁÖ·Î ³»°¡ ´Ù¸¥»ç¶÷À» ¼Ò°³ÇÏ´Â ÆíÀÌ´Ù.\r\n(W)ÁÖ·Î ´Ù¸¥»ç¶÷ÀÌ ³ª¸¦ ¼Ò°³ÇÏ´Â ÆíÀÌ´Ù. : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			E=E+2;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			I=I+2;
			van=0;
		}		
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n10. ÀÚ½ÅÀÌ ¾î´À ÂÊÀ¸·Î ÀÎÁ¤ ¹Þ±æ ¿øÇÏ½Ê´Ï±î?\r\n");
		printf("(Q)½ÇÁ¦ÀûÀ¸·Î Çö½Ç°¨°¢ÀÌ ÀÖ´Â »ç¶÷\r\n(W)Àç´É°ú Ã¢ÀÇ·ÂÀÌ ÀÖ´Â »ç¶÷ : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			S=N+2;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			N=N+2;
			van=0;	
		}	
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n11. ³ª´Â Æò¼Ò¿¡\r\n");
		printf("(Q)³í¸®º¸´Ù °¨»ó(¼¾Æ¼¸àÆ®)À» ÁÖ·Î Áß¿ä½ÃÇÏ´Â ÆíÀÌ´Ù.\r\n(W)°¨»ó(¼¾Æ®¸àÆ®)º¸´Ù´Â ³í¸®¸¦ ´õ Áß¿ä½ÃÇÏ´Â ÆíÀÌ´Ù. : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			T=T+2;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			F=F+2;
			van=0;
		}		
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n12. ³»°¡ ÀÏÀ» º¸´Ù ¼º°øÀûÀ¸·Î Ã³¸®ÇÏ´Â °æ¿ì¿¡´Â\r\n");
		printf("(Q)±â´ëÄ¡ ¾Ê´Â ÀÏÀ» ´Ù·ç°Å³ª ÀÏÀ» ½Å¼ÓÇÏ°Ô Ã³¸®ÇØ¾ß ÇÒ °æ¿ìÀÌ´Ù.\r\n(W)Ä¡¹ÐÇÏ°Ô Â¥¿©Áø °èÈ¹¿¡ µû¶ó ÀÏÀ» Ã³¸®ÇÏ´Â °æ¿ìÀÌ´Ù. : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			J=J+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			P=P+1;
			van=0;
		}		
		else
			van=1;
	}while(van>=1);


	do
	{
		printf("\r\n13. ³ª´Â\r\n");
		printf("(Q)¼Ò¼öÀÇ »ç¶÷µé°ú ±íÀº ¿ìÁ¤À» ¸Î´Â ÆíÀÌ´Ù.\r\n(W)´Ù¾çÇÑ »ç¶÷µé°ú Æø ³ÐÀº ¿ìÁ¤À» ¸Î´Â ÆíÀÌ´Ù. : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			E=E+2;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			I=I+1;
			van=0;
		}		
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n14. ÀÚ½ÅÀº ¾î´ÀÇüÀÇ »ç¶÷À» ´õ ¿ì·¯·¯ º¾´Ï±î?\r\n");
		printf("(Q)Àû´çÈ÷ º¸¼öÀûÀÌ¾î¼­ ´ëÃ¼·Î ³²¿¡°Ô ÀÚ½ÅÀ» µå·¯³»Áö ¾Ê´Â »ç¶÷\r\n(W)µ¶Ã¢ÀûÀ¸·Î °³ÀÎÀûÀÌ¾î¼­\r\n   ÀÚ½ÅÀ» µå·¯³ª°Å³ª ¸»°Å³ª º°·Î ½Å°æÀ» ¾²Áö ¾Ê´Â »ç¶÷. : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			S=S+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			N=N+2;
			van=0;
		}		
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n15. ÀÚ½ÅÀº ¾î´ÀÂÊÀÌ ´õ ÇüÆí¾ø´Â °áÁ¡ÀÌ¶ó ´À³§´Ï±î?\r\n");
		printf("(Q)µ¿Á¤½ÉÀÌ ¾ø´Â ¸é\r\n(W)ºñÇÕ¸®ÀûÀÎ ¸é : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			T=T+0;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			if(men=='Q'||men=='q')
			{
				F=F+1;
				van=0;
			}
			else if(men=='W'||men=='w')
			{
				F=F+2;
				van=0;
			}
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n16. ³ª¿¡°Ô ÀÖ¾î¼­ Â¥¿©Áø ½Ã°£Ç¥¸¦ µû¸£´Â ÀÏÀº\r\n");
		printf("(Q)¸¾¿¡ µç´Ù.\r\n(W)´ä´äÇÏ°Ô ´À²¸Áø´Ù. : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			J=J+2;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			P=P+2;
			van=0;
		}		
		else
			van=1;
	}while(van>=1);


	do
	{
		printf("\r\n17. ³ª´Â Ä£±¸µé »çÀÌ¿¡¼­\r\n");
		printf("(Q)¼Ò½ÄÀÌ³ª ¼Ò¹®À» Á¦ÀÏ ´Ê°Ô ¾ò¾î µè´Â ÆíÀÌ´Ù.\r\n(W)¸¹Àº »ç¶÷µé¿¡ ´ëÇÑ ¼Ò½ÄÀÌ³ª ¼Ò¹®¿¡ ¹àÀº ÆíÀÌ´Ù. : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			E=E+2;	
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			I=I+1;
			van=0;
		}		
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n18. ³»°¡ Ä£±¸·Î »ç±Í°í ½ÍÀº »ç¶÷Àº\r\n");
		printf("(Q)¾ðÁ¦³ª »õ·Î¿î ¾ÆÀÌµð¾î¸¦ ÀÚ¾Æ³»´Â »ç¶÷\r\n(W)Çö½Ç°¨°¢ÀÌ ÀÖ´Â »ç¶÷ : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			S=S+2;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			N=N+1;
			van=0;
		}		
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n19. ÀÚ½ÅÀº ¾î¶°ÇÑ ÇüÀÇ »ç¶÷ÀÇ ¾Æ·¡¿¡¼­ ÀÏÇÏ´Â °ÍÀ» ´õ ÁÁ¾ÆÇÕ´Ï±î?\r\n");
		printf("(Q)Ç×»ó Ä£ÀýÇÑ »ç¶÷\r\n(W)Ç×»ó °øÁ¤ÇÑ »ç¶÷ : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			T=T+0;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			if(men=='Q'||men=='q')
			{
				T=T+1;
				van=0;
			}
			else if(men=='W'||men=='w')
			{
				F=F+2;
				van=0;
			}
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n20. ÁÖ¸»¿¡ ¸¶ÃÄ¾ß µÇ´Â ÀÏÀÇ ¸ñ·ÏÀ» ÀÛ¼ºÇØ¾ß ÇÑ´Ù´Â »ý°¢Àº, ³ª¿¡°Ô ÀÖ¾î¼­\r\n");
		printf("(Q)½±°Ô È£°¨ÀÌ °£´Ù.\r\n(W)º°·Î ¸¶À½ÀÌ ³»Å°Áö ¾Ê´Â´Ù.\r\n(E)»ý°¢ÇÏ¸é ¿ì¿ïÇØÁø´Ù. : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			J=J+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			P=P+1;
			van=0;
		}
		else if(lant=='E'||lant=='e')
		{
			P=P+1;
			van=0;
		}	
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n21. ³»°¡ ´ëÈ­¸¦ ³ª´©¾î¾ß ÇÑ´Ù¸é\r\n");
		printf("(Q)´©±¸ÇÏ°í³ª ½±°Ô ´ëÈ­¸¦ ³ª´©´Â ÆíÀÌ´Ù.\r\n(W)¾î´À ÀÏÁ¤ÇÑ »ç¶÷ÀÌ³ª\r\n   È¤Àº ¾î¶°ÇÑ »óÈ²ÀÌ µÇ¾úÀ» ¶§ ´õ ½±°Ô ´ëÈ­¸¦ ³ª´©´Â ÆíÀÌ´Ù. : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			E=E+2;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			I=I+2;
			van=0;
		}	
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n22. ³ª´Â Ãë¹Ì·Î µ¶¼­¸¦ ÇÒ ¶§\r\n");
		printf("(Q)±â¹¦ÇÏ°Å³ª µ¶Ã¢ÀûÀÎ ÀÛ°¡ÀÇ Ç¥ÇöÀ» Áñ±â´Â ÆíÀÌ´Ù.\r\n(W)ÀÇµµÇÑ ¹Ù°¡ Á¤È®ÇÑ ÀÛ°¡ÀÇ Ç¥ÇöÀ» ÁÁ¾ÆÇÏ´Â ÆíÀÌ´Ù. : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			S=S+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			N=N+0;
			van=0;
		}		
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n23. ÀÚ½ÅÀº ¾î´ÀÂÊÀÌ ´õ ÇüÆí¾ø´Â °áÁ¡ÀÌ¶ó°í ´À³§´Ï±î?\r\n");
		printf("(Q)Áö³ªÄ£ ¿ÂÁ¤À» º¸ÀÌ´Â °Í\r\n(W)Àû´çÇÑ ¿ÂÁ¤À» º¸ÀÌÁö ¾Ê´Â °Í : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			T=T+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			F=F+0;
			van=0;
		}		
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n¡ØÀÌ ¹®Á¦¸¸ 2°³¸¦ ¼±ÅÃ°¡´É(¹®ÀÚ space ¹®ÀÚ)");
		printf("\r\n¡ØÁÖÀÇ»çÇ×: ¿£ÅÍ¸¦ ¸ÕÀú Ä¡Áö¸¶¼¼¿ä.");
		printf("\r\n¡Øex) q q/ q w/ q e/ w q/ w w/ w e/ e q/ e w/ e e\r\n");
		printf("\r\n24. ³ª´Â ÀÏ»ó»ýÈ°¿¡¼­\r\n");
		printf("(Q)½Ã°£À» ´ÙÅõ´Â ºñ»ó»óÅÂ¿¡¼­ ÀÏÇÏ´Â °ÍÀ» Áñ±â´Â ÆíÀÌ´Ù.\r\n(W)½Ã°£ÀÇ ¾Ð¹ÚÀ» ¹ÞÀ¸¸é¼­ ÀÏÇÏ´Â °ÍÀ» ¾ÆÁÖ ½È¾îÇÏ´Â ÆíÀÌ´Ù.\r\n(E)½Ã°£ÀÇ ¾Ð¹ÚÀ» ÇÇÇÏ±â À§ÇØ¼­ ´ëÃ¼·Î °èÈ¹À» ¹Ì¸® Â¥µÎ´Â ÆíÀÌ´Ù. : ");	
		input_two(&lant,&lant24);
		//fflush(stdin);

		if((lant=='Q'||lant=='q') && (lant24=='W'||lant24=='w'))//AB
		{
			P=P+1;
			van=0;
		}
		else if((lant=='Q'||lant=='q') && (lant24=='E'||lant24=='e'))//AC	
		{
			P=P+1;
			J=J+1;
			van=0;
		}
		else if((lant=='W'||lant=='w') && (lant24=='E'||lant24=='e'))//BC
		{
			J=J+1;
			van=0;
		}
		else if((lant=='Q'||lant=='q') && (lant24=='Q'||lant24=='q'))//A
		{
			P=P+1;
			van=0;
		}
		else if((lant=='W'||lant=='w') && (lant24=='W'||lant24=='w'))//B
		{
			J=J+0;
			van=0;
		}
		else if((lant=='E'||lant=='e') && (lant24=='E'||lant24=='e'))//C
		{
			J=J+1;
			van=0;
		}

		else if((lant!='Q'||lant!='q') && (lant24!='Q'||lant24=='q'))
		{
			van=1;
		}

		else if((lant!='W'||lant!='w') && (lant24!='W'||lant24=='w'))
		{
			van=1;
		}

		else if((lant!='E'||lant!='e') && (lant24!='E'||lant24=='e'))
		{
			van=1;
		}


		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n25. Ã³À½¸¸³ª´Â »ç¶÷µéÀÌ ³ªÀÇ °ü½ÉÀÌ ¹«¾ùÀÎ°¡¸¦\r\n");
		printf("(Q)¸¸³ªÀÚ¸¶ÀÚ ¹Ù·Î ¾Æ´Â ÆíÀÌ´Ù.\r\n(W)³ª¿Í ±íÀÌ ¾Ë°í ³­ ÈÄ¿¡¶ó¾ß ¾Æ´Â ÆíÀÌ´Ù. : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			E=E+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			I=I+1;
			van=0;
		}		
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n26. ´ëºÎºÐÀÇ »ç¶÷µéÀÌ ÀÏ»óÀûÀ¸·Î ÇÏ´Â ÀÏÀ» ³»°¡ ÇÏ°ÔµÉ ¶§\r\n");
		printf("(Q)ÀÏ¹ÝÀûÀÎ °ü½ÀÀ» µû¶ó Ã³¸®ÇÏ´Â °ÍÀ» ´õ ÁÁ¾ÆÇÑ´Ù.\r\n(W)µ¶Ã¢ÀûÀÎ Ã³¸®¹æ¹ýÀ» »ý°¢ÇØ ³»´Â°ÍÀ» ´õ ÁÁ¾ÆÇÑ´Ù. : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			S=S+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			N=N+1;
			van=0;	
		}	
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n27. ´ç½ÅÀº ¾î´À ÂÊ¿¡ ´õ ¸¶À½À» ¾¹´Ï±î?\r\n");
		printf("(Q)»ç¶÷µéÀÇ °¨Á¤À» Á¸ÁßÇÏ´Â ÀÏ\r\n(W)»ç¶÷µéÀÇ ±Ç¸®¸¦ º¸È£ÇÏ´Â ÀÏ : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			if(men=='Q'||men=='q')
			{
				T=T+2;
				van=0;
			}
			else if(men=='W'||lant=='w')
			{
				T=T+1;
				van=0;
			}
		}
		else if(lant=='W'||lant=='w')
		{
			F=F+0;
			van=0;
		}	
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n28. ³ª´Â ¾î¶² Æ¯º°ÇÑ ÀÏÀ» Ã³¸®ÇÒ ¶§\r\n");
		printf("(Q)½ÃÀÛÇÏ±âÀü¿¡ ¹Ì¸® Á¶½É½º·´°Ô °èÈ¹À» ¼¼¿ì´Â ÆíÀÌ´Ù.\r\n(W)ÀÏÀ» Ã³¸®ÇØ³ª°¡¸é¼­ »óÈ²¿¡ µû¶ó ÇÊ¿äÇÑ ´ëÃ¥À» ¼¼¿ì´Â ÆíÀÌ´Ù. : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			J=J+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			P=P+2;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n29. ³ª´Â Æò¼Ò¿¡\r\n");
		printf("(Q)ÀÚ½ÅÀÇ °¨Á¤°ú ´À³¦À» ÀÚÀ¯½º·¯ÀÌ Ç¥ÇöÇÏ´Â ÆíÀÌ´Ù.\r\n(W)°¨Á¤°ú ´À³¦À» Ç¥ÇöÇÏ±â º¸´Ù´Â ÀÚ½Å¾È¿¡ ¹¯¾îµÎ´Â ÆíÀÌ´Ù. : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			E=E+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			I=I+0;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n30. ³ªÀÇ »ýÈ° ¾ç½ÄÀº\r\n");
		printf("(Q)µ¶Ã¢ÀûÀÎ ÆíÀÌ´Ù.\r\n(W)°ü·ÊÀûÀÎ ÆíÀÌ´Ù. : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			S=S+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			N=N+0;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	//2ºÎ-----

	do
	{
		print_header("¡Ø Á¦ 2ºÎ ¡Ø");
		printf("\r\nµÎ °³ÀÇ ³¹¸»ÀÌ ÀÖ´Â ¹®Ç×¿¡¼­´Â,\r\nÀÚ½Å¿¡°Ô ´õ °¡±õ´Ù°í »ý°¢µÇ´Â ¸»À» ¼±ÅÃÇÏ¿© ´ä¾ÈÁö¿¡ Ç¥½ÃÇÏ½Ê½Ã¿À.\r\n");

		printf("\r\n31. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)À¯¼øÇÑ\r\n(W)°­°ÇÇÑ : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			T=T+2;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			if(men=='Q'||men=='q')
			{
				F=F+1;
				van=0;
			}
			else if(men=='W'||men=='w')
			{
				F=F+0;
				van=0;
			}
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n32. ¾î¶² ÀÏÀ» ¾ðÁ¦ ÇÒ °ÍÀÌ¶ó´Â °èÈ¹ÀÌ ¹Ì¸®¹Ì¸® Â¥¿©Á³À» ¶§ ³ª´Â\r\n");
		printf("(Q)±×°Í¿¡ ¸ÂÃß¾î¼­ ´Ù¸¥ °èÈ¹À» ¼¼¿ï ¼ö ÀÖ¾î ÁÁ´Ù°í ´À³¢´Â ÆíÀÌ´Ù.\r\n(W)ÀÏÁ¤¿¡ ¸ÞÀÌ´Â °Í ¶§¹®¿¡ ½ÈÀº ÆíÀÌ´Ù. : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			J=J+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			P=P+1;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n33. ³ª´Â ÀÏ¹ÝÀûÀ¸·Î ¾î¶² ÀÏ¿¡ ´ëÇÏ¿© ´Ù¸¥»ç¶÷µéº¸´Ù\r\n");
		printf("(Q)´õ ¿­¼ºÀûÀÎ ÆíÀÌ´Ù.\r\n(W)´ú ¿­¼ºÀûÀÎ ÆíÀÌ´Ù. : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			E=E+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			I=I+1;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n34. ³ªÀÇ »ý°¢¿¡ ´õ ÁÁÀº ÄªÂùÀÌ¶ó°í »ý°¢µÇ´Â °ÍÀº?\r\n");
		printf("(Q)¼±°ßÁö¸íÀ» °¡Á³´Ù°í ¸»ÇØÁÖ´Â °ÍÀÌ´Ù.\r\n(W)»ó½ÄÀÌ ÀÖ´Â »ç¶÷ÀÌ¶ó°í ¸»ÇØ ÁÖ´Â °ÍÀÌ´Ù. : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			S=S+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			N=N+2;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n35. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)»ý°¢ÇÔ\r\n(W)´À³¦ : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			T=T+2;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			if(men=='Q'||men=='q')
			{
				F=F+2;
				van=0;
			}
			else if(men=='W'||men=='w')
			{
				F=F+1;
				van=0;
			}
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n36. ³ª´Â\r\n");
		printf("(Q)¸¶Áö¸· ¼ø°£¿¡ °¡¼­ ÀÏÀ» Ã³¸®ÇÏ±â¸¦ ÁÁ¾ÆÇÑ´Ù.\r\n(W)¸¶Áö¸· ¼ø°£¿¡ °¡¼­ ÀÏÀ» Ã³¸®ÇÏ´Â °ÍÀÌ ºÒ¾ÈÇÏ°í ¼º¹Ì¿¡ ¸ÂÁö ¾Ê´Â ÆíÀÌ´Ù. : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			J=J+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			P=P+1;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n37. ³ª´Â Ä£¸ñÈ¸³ª ÆÄÆ¼ ¼®»ó¿¡¼­\r\n");
		printf("(Q)¶§·Î Áö·çÇÏ°Ô ´À³¤´Ù.\r\n(W)¾ðÁ¦µçÁö Àç¹ÌÀÖ´Ù. : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			E=E+2;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			I=I+1;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n38. ÀÚ½ÅÀº ¾î´À ÂÊÀÌ ´õ Áß¿äÇÏ´Ù°í º¾´Ï±î?\r\n");
		printf("(Q)¾î¶² »óÈ²ÀÌ ¾È°í ÀÖ´Â °¡´É¼ºÀ» º¸´Â ÀÏ.\r\n(W)ÀÖ´Â ±×´ë·ÎÀÇ »ç½Ç¿¡ ½ÇÁ¦ÀûÀ¸·Î ÀûÀÀÇÏ´Â ÀÏ. : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			S=S+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			N=N+0;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n39. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)³³µæ½ÃÅ´\r\n(W)°¨µ¿½ÃÅ´ : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			T=T+2;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			if(men=='Q'||men=='q')
			{
				F=F+2;
				van=0;
			}
			else if(men=='W'||men=='w')
			{
				F=F+1;
				van=0;
			}
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n40. ³ª´Â Æò»óÀûÀÎ ÇÏ·çÀÇ ÀÏ°ú¸¦ ¾Ë°í ÀÖ´Â °ÍÀÌ\r\n");
		printf("(Q)ÀÏ»óÀûÀÎ ÀÏÀ» Ã³¸®ÇÏ´Âµ¥ Æí¸®ÇÏ´Ù°í »ý°¢ÇÑ´Ù.\r\n(W)ÇÊ¿äÇÏ´Ù°í´Â »ý°¢ÇÏ³ª ±×°ÍÀ» ¸ÅÀÏ µû¸£´Â °ÍÀÌ °íÅë½º·´´Ù. : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			J=J+0;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			P=P+2;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n41. ³ª´Â ÀÏ¹ÝÀûÀ¸·Î »õ·Î¿î À¯ÇàÀÌ ½ÃÀÛµÉ ¶§\r\n");
		printf("(Q)¾ÕÀå¼­¼­ ½ÃµµÇØº¸´Â ÃàÀÇ ÇÑ »ç¶÷ÀÌ´Ù.\r\n(W)º° °ü½ÉÀÌ ¾ø´Â ÆíÀÌ´Ù. : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			E=E+0;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			I=I+2;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{	
		printf("\r\n42. ³ª´Â\r\n");
		printf("(Q)¼±ÀÍÀ» À§ÇØ¼­ ÀÌ¹Ì ¸¸µé¾îÁø ±âÁ¸Ã¼Á¦ ¹æ½ÄÀ» ÁöÁöÇÏ´Â ÆíÀÌ´Ù.\r\n(W)±âÁ¸ Ã¼Á¦¹æ½Ä¿¡ ¹«¾ùÀÌ Àß¸øµÇ¾î ¿Ô´ÂÁö¸¦ ºÐ¼®ÇÏ°í\r\n   ÇØ°áµÇÁö ¾ÊÀº ¹®Á¦¿¡ µµÀüÇÏ´Â ÆíÀÌ´Ù. : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			S=S+2;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			N=N+0;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n43. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)ºÐ¼®ÇÏ´Ù.\r\n(W)°ø°¨ÇÏ´Ù. : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			if(men=='Q'||men=='q')
			{
				T=T+1;
				van=0;
			}
			else if(men=='W'||men=='w')
			{
				T=T+2;
				van=0;
			}
		}
		else if(lant=='W'||lant=='w')
		{
			F=F+2;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{	
		printf("\r\n44. ³»°¡ Ã³¸®ÇØ¾ß µÉ »ç¼ÒÇÑ ÀÏÀÌ³ª ±¸ÀÔÇØ¾ß ÇÒ ¹°°ÇÀÌ »ý°¢¿¡ ¶° ¿Ã¶úÀ» ¶§\r\n");
		printf("(Q)ÀÚÁÖ ÀØ°í ÀÖ´Ù°¡ ³ªÁß¿¡¾ß ±â¾ïÇÏ´Â ÆíÀÌ´Ù.\r\n(W)±â¾ïÇÏ±â À§ÇØ¼­ ¸Þ¸ðÁö¿¡ Àû¾î µÎ´Â ÆíÀÌ´Ù.\r\n(E)±×¶§ ±×¶§ Áï½Ã Ã³¸®ÇÏ´Â ÆíÀÌ´Ù. : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='W'||lant=='w')
		{
			P=P+1;
			van=0;
		}
		else if(lant=='E'||lant=='e')
		{
			P=P+0;
			van=0;
		}
		else if(lant=='Q'||lant=='q')
		{
			J=J+1;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n45. ³ª´Â\r\n");
		printf("(Q)´Ù¸¥ »ç¶÷µéÀÌ ½±°Ô »ç±Ð ¼ö ÀÖ´Â ÆíÀÌ´Ù.\r\n(W)´Ù¸¥ »ç¶÷µéÀÌ ½±°Ô »ç±Ð ¼ö ÀÖ´Â ÆíÀÌ ¾Æ´Ï´Ù. : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			E=E+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			I=I+2;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{	
		printf("\r\n46. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)Çö½Ç\r\n(W)ÀÌ³ä : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			S=S+2;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			N=N+1;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n47. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)Á¤ÀÇ\r\n(W)ÀÚºñ : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			T=T+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			F=F+2;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{	
		printf("\r\n48. ³ª¿¡°Ô ÀûÀÀÇÏ±â ´õ ¾î·Á¿î ÀÏÀº\r\n");
		printf("(Q)Æ²¿¡ ¹ÚÈù ÀÏ»óÀûÀÎ ÀÏ\r\n(W)°è¼ÓÀûÀÎ º¯È­ : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			J=J+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			P=P+1;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{	
		printf("\r\n49. ³ª´Â ´ëÈ­ µµÁß¿¡ ´çÈ¤ÇÑ »óÈ²¿¡ Ã³ÇßÀ» ¶§\r\n");
		printf("(Q)´ëÈ­ÀÇ ÁÖÁ¦¸¦ ¹Ù²Û´Ù.\r\n(W)³ó´ãÀ¸·Î µ¹¸°´Ù.\r\n(E)¸çÄ¥ ÈÄ¿¡¾ß ±×¶§ ¹«½¼ ¾ê±â¸¦ Çß¾î¾ß µÇ¾ú´Ù°í »ý°¢ÇÑ´Ù. : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			I=I+0;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			E=E+1;
			van=0;
		}
		else if(lant=='E'||lant=='e')
		{
			I=I+2;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n50. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)»ç½ÇÀûÀÎ Áø¼ú\r\n(W)Ãß»óÀûÀÎ °³³ä : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			S=S+2;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			N=N+1;
			van=0;			
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n51. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)ÃøÀºÁö½É(ö³ëßñýãý)\r\n(W)¼±°ßÁö¸í(à»Ì¸ñýÙ¥) : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			T=T+2;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			F=F+1;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{	
		printf("\r\n52. ÀÏÁÖÀÏ¾È¿¡ ¸¶¹«¸® Áö¾î¾ß ÇÒ Å« ÀÏÀ» ½ÃÀÛÇÒ ¶§\r\n");
		printf("(Q)½Ã°£À» ³»¾î ÀÏÀÇ ¸ñ·ÏÀ» ÀÛ¼ºÇÏ°í ±× Ã³¸®¼ø¼­¸¦ Á¤ÇÑ´Ù.\r\n(W)±×³É ¶Ù¾îµé¾î ½ÃÀÛÇÑ´Ù. : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			J=J+2;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			P=P+1;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n53. ³ª¸¦ °¡±îÀÌ ¾Æ´Â »ç¶÷µéÀº\r\n");
		printf("(Q)´ëºÎºÐÀÇ ÀÏ¿¡ ÀÖ¾î¼­ ³»°¡ ¾î¶»°Ô ´À³¢´Â °¡¸¦ ¾Æ´Â ÆíÀÌ´Ù.\r\n(W)³»°¡ ¾î¶»°Ô ´À³¢´Â °¡¸¦ ¼³¸íÇßÀ» ¶§¸¸ ¾Æ´Â ÆíÀÌ´Ù. : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			E=E+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			I=I+1;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n54. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)ÀÌ·Ð\r\n(W)È®½Å : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			S=S+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			N=N+2;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n55. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)ÇýÅÃ\r\n(W)Ãàº¹ : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			T=T+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			F=F+1;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n56. ³ª´Â ÀÏÀ» Ã³¸®ÇÏ´Â µ¥ ÀÖ¾î¼­\r\n");
		printf("(Q)¹Ì¸® ½ÃÀÛÇØ¼­ ¿©À¯ÀÖ°Ô ¸¶¹«¸®ÇÏ´Â ÆíÀÌ´Ù.\r\n(W)¸·¹ÙÁö¿¡ °¡¼­ ¼­µÑ·¯ ³¡³»´Â ÆíÀÌ´Ù. : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			J=J+0;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			P=P+1;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n57. ³ª´Â ÆÄÆ¼ ¼®»ó¿¡¼­?\r\n");
		printf("(Q)ÆÄÆ¼°¡ ÁøÇàµÇµµ·Ï µ½´Â ÀÏÀ» ÁÁ¾ÆÇÑ´Ù.\r\n(W)»ç¶÷µéÀ» ÀÚ±âµé ³ª¸§´ë·Î Áñ±âµµ·Ï µÎ´Â ÀÏÀ» ÁÁ¾ÆÇÑ´Ù. : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			E=E+1;	
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			I=I+2;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n58. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)¹®ÀÚ ±×´ë·Î\r\n(W)ºñÀ¯ÀûÀ¸·Î : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			S=S+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			N=N+1;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n59. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)°á½É\r\n(W)Çå½Å : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			T=T+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			if(men=='Q'||men=='q')
			{
				F=F+1;
				van=0;
			}
			else if(men=='W'||men=='w')
			{
				F=F+2;
				van=0;
			}
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n60. ³»°¡ ¸¸¾à ¾î´À ÀÏ¿äÀÏ ¾ÆÄ§¿¡ ±×³¯ ¹«¾ùÀ» ÇÒ °ÍÀÌ³Ä°í Áú¹®À» ¹Þ¾Ò´Ù¸é\r\n");
		printf("(Q)¹«¾ùÀ» ÇÒ °ÍÀÎ°¡¸¦ ¼ú¼ú ¾ê±âÇØ ÁÙ ¼ö ÀÖÀ» °ÍÀÌ´Ù\r\n(W)½ÇÁ¦ ÇÒ ¼ö ÀÖ´Â ÀÏº¸´Ù °ÅÀÇ µÎ¹è³ª µÉ Á¤µµÀÇ °èÈ¹À» ³ª¿­ÇÒ °ÍÀÌ´Ù.\r\n(E)±â´Ù·È´Ù°¡ ±×³¯ Áö³ªº¸¾Æ¾ß ¹«¾ùÀ» ÇÒÁö ¾Ë¼ö ÀÖ´Ù°í ¸»ÇÒ °ÍÀÌ´Ù. : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			J=J+0;
			van=0;	
		}
		else if(lant=='W'||lant=='w')
		{
			P=P+1;
			van=0;
		}
		else if(lant=='E'||lant=='e')
		{
			P=P+1;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n61. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)±â¿îÂù\r\n(W)Â÷ºÐÇÔ : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			E=E+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			I=I+2;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n62. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)»ó»óÀûÀÎ\r\n(W)»ç½ÇÀûÀÎ : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			S=S+2;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			N=N+0;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n63. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)±»°ÇÇÑ ÀÇÁö\r\n(W)µû¶æÇÑ ¸¶À½ : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			T=T+2;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			F=F+0;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n64. ³ª¿¡°Ô ÀÖ¾î¼­ ÀÏ»óÀûÀÎ ÀÏÀº\r\n");
		printf("(Q)½±°í Æí¾ÈÇÏ´Ù.\r\n(W)Áö·çÇÏ´Ù. : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			J=J+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			P=P+0;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n65. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)¸»ÀÌ º°·Î ¾øÀ½\r\n(W)¸»·Î ½±°Ô Ç¥ÇöÇÔ : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			E=E+0;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			I=I+1;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n66. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)Á¦ÀÛÇÏ´Ù\r\n(W)Ã¢ÀÛÇÏ´Ù : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			S=S+2;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			N=N+0;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{		
		printf("\r\n67. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)ÁßÀçÀÚ\r\n(W)½ÉÆÇ°ü : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			T=T+2;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			F=F+0;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{	
		printf("\r\n68. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)°èÈ¹\r\n(W)¹«°èÈ¹ : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			J=J+2;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			P=P+2;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n69. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)Ä§ÂøÇÑ\r\n(W)¹ß¶öÇÑ : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			E=E+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			I=I+1;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{	
		printf("\r\n70. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)ÀçÄ¡ÀÖ´Â\r\n(W)Èï¹ÌÁøÁøÇÑ : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			S=S+2;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			N=N+0;
			van=0;
		}
		else
			van=1;
	}while(van>=1);


	do
	{
		printf("\r\n71. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)ºÎµå·¯¿ò\r\n(W)°­ÇÔ : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			T=T+2;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			F=F+0;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n72. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)Ã¼°èÀû\r\n(W)ÀÚ¹ßÀû : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			J=J+2;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			P=P+2;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n73. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)¸»ÇÏ±â\r\n(W)¾²±â : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			E=E+0;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			E=E+1;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n74. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)Á¦ÀÛ\r\n(W)¼³°è : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			S=S+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			N=N+0;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n75. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)¿ë¼­ÇÏ´Ù\r\n(W)Âü¾ÆÁÖ´Ù : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			T=T+2;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			F=F+0;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n76. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)°èÈ¹¿¡ µû¸¥\r\n(w)»óÈ²¿¡ µû¸¥ : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			J=J+2;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			P=P+2;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n77. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)»ç±³ÀûÀÎ\r\n(W)ÃÊ¿¬ÇÑ : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			E=E+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			I=I+1;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n78. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)±¸Ã¼ÀûÀÎ\r\n(W)Ãß»óÀûÀÎ : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			S=S+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			N=N+2;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n79. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)´©±¸¸¦\r\n(W)¹«¾ùÀ» : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			T=T+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			F=F+0;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{	
		printf("\r\n80. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)Ãæµ¿\r\n(W)°á½É : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			J=J+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			P=P+2;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n81. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)Ä£¸ñ¸ðÀÓ\r\n(W)µ¶¼­ : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			E=E+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			I=I+0;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n82. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)°Ç¼³ÇÏ´Ù\r\n(W)¹ß¸íÇÏ´Ù : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			S=S+2;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			N=N+1;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{	
		printf("\r\n83. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)¹«ºñÆÇÀûÀÎ\r\n(W)ºñÆÇÀûÀÎ : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			T=T+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			F=F+1;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n84. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)½Ã°£ÀûÀ¸·Î Á¤È®ÇÑ\r\n(W)À¯À¯ÀÚÀûÇÑ : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			J=J+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			P=P+1;
			van=0;
		}
		else	
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n85. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)ÃÊ¼®\r\n(W)±Í°¨ : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			S=S+0;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			N=N+2;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n86. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)Á¶½É½º·¯ÀÌ ´ëÇÏ´Â\r\n(W)Çã¹°¾øÀÌ ´ëÇÏ´Â : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			T=T+2;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			F=F+0;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{	
		printf("\r\n87. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)º¯È­ÀûÀÎ\r\n(W)¿µ±¸ÀûÀÎ : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			J=J+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			P=P+0;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n88. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)ÀÌ·Ð\r\n(W)°æÇè : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			S=S+0;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			N=N+2;
			van=0;
		}
		else	
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n89. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)µ¿ÀÇÇÏ´Ù.\r\n(W)Åä·ÐÇÏ´Ù. : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			T=T+0;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			if(men=='Q'||men=='q')
				F=F+0;
			else if(men=='W'||men=='w')
				F=F+1;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n90. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)±ÔÄ¢ÀûÀÎ\r\n(W)´À±ßÇÑ : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			J=J+2;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			P=P+1;
			van=0;
		}
		else	
			van=1;
	}while(van>=1);

	do
	{
		printf("\r\n91. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)Ç¥½Ä\r\n(W)»óÂ¡ : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			S=S+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			N=N+0;
			van=0;
		}
		else
		{
			van=1;
		}
	}while(van>=1);

	do
	{
		printf("\r\n92. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)½Å¼ÓÇÑ\r\n(W)½ÅÁßÇÑ : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			J=J+0;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			P=P+1;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{	
		printf("\r\n93. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)¼ö¿ëÇÏ´Ù.\r\n(W)º¯°æÇÏ´Ù. : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			S=S+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			N=N+0;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	do
	{	
		printf("\r\n94. ¹«¾ùÀÌ ´õ °¡±õ½À´Ï±î?\r\n");
		printf("(Q)¾Ë·ÁÁø\r\n(W)¹ÌÁöÀÇ : ");
		input(&lant);
		//fflush(stdin);
		if(lant=='Q'||lant=='q')
		{
			S=S+1;
			van=0;
		}
		else if(lant=='W'||lant=='w')
		{
			N=N+1;
			van=0;
		}
		else
			van=1;
	}while(van>=1);

	// °á°ú
	printf("\r\n[=15G[=0FMBTI GSÀÇ °á°ú¸¦ È­¸é¿¡ Ãâ·ÂÇÕ´Ï´Ù.[=1G[=15F");


	printf("\r\n\r\nE=¿ÜÇâ I=³»Çâ\r\nS=°¨°¢ N=°¨Á¤\r\nT=»ç°í F=°¨Á¤\r\nJ=ÆÇ´Ü P=ÀÎ½Ä\r\n");

	if(E>=I)// EIÃâ·Â
	{
		printf("\r\nE°ª:%d, I°ª:%d\r\n",E,I);
		printf("E\r\n");

	}
	else
	{
		printf("\r\nE°ª:%d, I°ª:%d\r\n",E,I);
		printf("I\r\n");

	}

	if(N>=S)// SNÃâ·Â
	{
		printf("\r\nS°ª:%d, N°ª:%d\r\n",S,N);
		printf("N\r\n");

	}
	else
	{
		printf("\r\nS°ª:%d, N°ª:%d\r\n",S,N);
		printf("S\r\n");

	}

	if(F>=T)// TFÃâ·Â
	{
		printf("\r\nT°ª:%d, F°ª:%d\r\n",T,F);
		printf("F\r\n");

	}
	else
	{
		printf("\r\nT°ª:%d, F°ª:%d\r\n",T,F);
		printf("T\r\n");

	}

	if(P>=J)// JPÃâ·Â
	{
		printf("\r\nJ°ª:%d, P°ª:%d\r\n",J,P);
		printf("P\r\n");

	}
	else
	{
		printf("\r\nJ°ª:%d, P°ª:%d\r\n",J,P);
		printf("J\r\n");

	}


	// MBTI°ü°è¿¬»ê-------------------------------------

	if(E>=I)
		mbe=1;
	if(I>E)
		mbi=1;
	if(N>=S)
		mbn=1;
	if(S>N)
		mbs=1;
	if(F>=T)
		mbf=1;
	if(T>F)
		mbt=1;
	if(P>=J)
		mbp=1;
	if(J>P)
		mbj=1;

	// ---------------------------------------------------

	if(mbi==1 && mbs==1 && mbt==1 && mbj==1)
	{
		print_header("\r\nISTJ ¼¼»óÀÇ ¼Ò±ÝÇü\r\n");

		printf("\r\n½ÅÁßÇÏ°í Á¶¿ëÇÏ¸ç ÁýÁß·ÂÀÌ °­ÇÏ°í ¸Å»ç¿¡ Ã¶ÀúÇÏ¸ç »ç¸®ºÐº°·ÂÀÌ ¶Ù¾î³ª´Ù\r\n");

		printf("\r\n½ÇÁ¦ »ç½Ç¿¡ ´ëÇÏ¿© Á¤È®ÇÏ°í Ã¼°èÀûÀ¸·Î ±â¾ïÇÏ¸ç ÀÏÃ³¸®¿¡ ÀÖ¾î¼­µµ ½ÅÁßÇÏ¸ç Ã¥ÀÓ°¨ÀÌ °­ÇÏ´Ù. ÁýÁß·ÂÀÌ °­ÇÑ Çö½Ç°¨°¢À» Áö³æÀ¸¸ç Á¶Á÷ÀûÀÌ°í Ä§ÂøÇÏ´Ù. º¸¼öÀûÀÎ °æÇâÀÌ ÀÖÀ¸¸ç, ¹®Á¦¸¦ ÇØ°áÇÏ´Âµ¥ °ú°ÅÀÇ °æÇèÀ» Àß Àû¿ëÇÏ¸ç, ¹Ýº¹µÇ´Â ÀÏ»óÀûÀÎ ÀÏ¿¡ ´ëÇÑ ÀÎ³»·ÂÀÌ °­ÇÏ´Ù. ÀÚ½Å°ú Å¸ÀÎÀÇ °¨Á¤°ú ±âºÐÀ» ¹è·ÁÇÏ¸ç, ÀüÃ¼ÀûÀÌ°í Å¸ÇùÀû ¹æ¾ÈÀ» °í·ÁÇÏ´Â ³ë·ÂÀÌ ¶§·Î ÇÊ¿äÇÏ´Ù. Á¤È®¼º°ú Á¶Á÷·ÂÀ» ¹ßÈÖÇÏ´Â ºÐ¾ßÀÇ ÀÏÀ» ¼±È£ÇÑ´Ù. Áï È¸°è, »ý»ê, °ÇÃà, ÀÇ·á, »ç¹«Á÷, °ü¸®Á÷ µî¿¡¼­ ´É·ÂÀ» ¹ßÈÖÇÏ¸ç, À§±â»óÈ²¿¡¼­µµ ¾ÈÁ¤µÇ¾î ÀÖ´Ù.\r\n");

		istj();

		printf("\r\n»ç½ÇÀûÀÎ, Ã¶ÀúÇÑ, Ã¼°èÀûÀÎ, ½Å·ÚÇÒ ¼ö ÀÖ´Â, È®°íºÎµ¿ÇÑ, ½ÇÁ¦ÀûÀÎ, Á¶Á÷È­µÈ, ÀÇ¹«ÀûÀÎ, ºÐº°ÀÖ´Â, ±Ù¸éÇÑ, ¹ÏÀ»¸¸ÇÑ, ¼º½ÇÇÑ\r\n");
	}

	if(mbi==1 && mbs==1 && mbf==1 && mbj==1)
	{
		print_header("\r\nISFJ ÀÓ±Ý µÞÆíÀÇ ±Ç·ÂÇü\r\n");

		printf("\r\nÁ¶¿ëÇÏ°í Â÷ºÐÇÏ¸ç Ä£±ÙÇÏ°í  Ã¥ÀÓ°¨ÀÌ ÀÖÀ¸¸ç Çå½ÅÀûÀÌ´Ù.\r\n");

		printf("\r\nÃ¥ÀÓ°¨ÀÌ °­ÇÏ°í ¿ÂÁ¤ÀûÀÌ¸ç Çå½ÅÀûÀÌ°í, Ä§ÂøÇÏ¸ç, ÀÎ³»·ÂÀÌ °­ÇÏ´Ù. ´Ù¸¥ »ç¶÷ÀÇ »çÁ¤À» °í·ÁÇÏ¸ç ÀÚ½Å°ú Å¸ÀÎÀÇ °¨Á¤¿¡ ¹Î°¨ÇÏ¸ç, ÀÏÃ³¸®¿¡ ÀÖ¾î¼­ Çö½Ç°¨°¢À» °®°í ½ÇÁ¦ÀûÀÌ°í Á¶Á÷ÀûÀ¸·Î Ã³¸®ÇÑ´Ù. °æÇèÀ» ÅëÇØ¼­ ÀÚ½ÅÀÌ Æ²·È´Ù°í ÀÎÁ¤ÇÒ ¶§ ±îÁö ¾î¶°ÇÑ ³­°üÀÌ ÀÖ¾îµµ ²ÙÁØÈ÷ ¹Ð°í ³ª°¡´Â ÇüÀÌ´Ù. ¶§·Î ÀÇÁ¸ÀûÀÌ°í µ¶Ã¢¼ºÀÌ ¿ä±¸µÇ¸ç Å¸ÀÎ¿¡°Ô ÀÚ½ÅÀ» ÃæºÐÈ÷ ¸íÈ®ÇÏ°Ô Ç¥ÇöÇÏ´Â °ÍÀÌ ÇÊ¿äÇÒ ¶§°¡ ÀÖ´Ù. Å¸ÀÎÀÇ °ü½É°ú °üÂû·ÂÀÌ ÇÊ¿äÇÑ ºÐ¾ß, Áï ÀÇ·á, °£È£, ±³Á÷, »ç¹«Á÷, »çÈ¸»ç¾÷¿¡ ÀûÇÕÇÏ´Ù. ÀÌµéÀÌ ÀÏÀ» ÇÏ°í, ¼¼»ó¿¡ ´ëÃ³ÇÒ ¶§ ±×µéÀÇ Çàµ¿Àº ºÐº°·ÂÀÌ ÀÖ´Ù.\r\n");

		isfj();

		printf("\r\n»ó¼¼ÇÑ, ¼º½ÇÇÑ, ÀüÅëÀûÀÎ, Ãæ½ÇÇÑ, ÂüÀ»¼ºÀÖ´Â, Á¶Á÷È­µÈ, ºÀ»çÀûÀÎ, Çå½ÅÀûÀÎ, º¸È£ÇÏ´Â, ¸Å¿ì ¼¶¼¼ÇÑ, Ã¥ÀÓÁú ¼ö ÀÖ´Â, µ¿Á¤ÀûÀÎ\r\n");
	}	

	if(mbi==1 && mbn==1 && mbf==1 && mbj==1)
	{
		print_header("\r\nINFJ ¿¹¾ðÀÚÇü\r\n");

		printf("\r\nÀÎ³»½ÉÀÌ ¸¹°í ÅëÂû·Â°ú Á÷°ü·ÂÀÌ ¶Ù¾î³ª¸ç ¾ç½ÉÀÌ ¹Ù¸£°í È­ÇÕÀ» Ãß±¸ÇÑ´Ù.\r\n");

		printf("\r\nÃ¢ÀÇ·Â°ú ÅëÂû·ÂÀÌ ¶Ù¾î³ª¸ç, °­ÇÑ Á÷°ü·ÂÀ¸·Î ¸»¾øÀÌ Å¸ÀÎ¿¡°Ô ¿µÇâ·ÂÀ» ³¢Ä£´Ù. µ¶Ã¢¼º°ú ³»Àûµ¶¸³½ÉÀÌ °­ÇÏ¸ç, È®°íÇÑ ½Å³ä°ú ¿­Á¤À¸·Î ÀÚ½ÅÀÇ ¿µ°¨À» ±¸Çö½ÃÄÑ ³ª°¡´Â Á¤½ÅÀû ÁöµµÀÚµéÀÌ ¸¹´Ù. Á÷°ü·Â°ú »ç¶÷Áß½ÉÀÇ °¡Ä¡¸¦ Áß½ÃÇÏ´Â ºÐ¾ß Áï, ¼ºÁ÷, ½É¸®ÇÐ, ½É¸®Ä¡·á¿Í »ó´ã, ¿¹¼ú°ú ¹®ÇÐºÐ¾ßÀÌ´Ù. Å×Å©´ÏÄÃÇÑ ºÐ¾ß·Î´Â ¼ø¼ö°úÇÐ, ¿¬±¸ °³¹ßºÐ¾ß·Î½á »õ·Î¿î ½Ãµµ¿¡ ´ëÇÑ ¿­¼ºÀÌ ´ë´ÜÇÏ´Ù. ÇÑ °÷¿¡ ¸ôµÎÇÏ´Â °æÇâÀ¸·Î ¸ñÀû´Þ¼º¿¡ ÇÊ¿äÇÑ ÁÖº¯ÀûÀÎ Á¶°ÇµéÀ» °æ½ÃÇÏ±â ½±°í, ÀÚ±â¾ÈÀÇ °¥µîÀÌ ¸¹°í º¹ÀâÇÏ´Ù. ÀÌµéÀº Ç³ºÎÇÑ ³»ÀûÀÎ »ýÈ°À» ¼ÒÀ¯ÇÏ°í ÀÖÀ¸¸ç ³»¸éÀÇ ¹ÝÀÀÀ» Á»Ã³·³ ³²°ú °øÀ¯ÇÏ±â ¾î·Á¿ö ÇÑ´Ù.\r\n");

		infj();

		printf("\r\nÇå½ÅÀûÀÎ, Ãæ½ÇÇÑ, ÀÚºñ·Î¿î, Ã¢ÀÇÀûÀÎ, ¿­Á¤ÀûÀÎ, ±íÀÌ ÀÖ´Â, °á½ÉÀÌ ±»Àº, °³³äÀûÀÎ, ÀüÃ¼ÀûÀÎ, ÀÌ»óÀûÀÎ, ½Åºñ·Î¿î\r\n");
	}

	if(mbi==1 && mbn==1 && mbt==1 && mbj==1)
	{
		print_header("\r\nINTJ °úÇÐÀÚÇü\r\n");		

		printf("\r\n»ç°í°¡ µ¶Ã¢ÀûÀÌ¸ç Ã¢ÀÇ·Â°ú ºñÆÇºÐ¼®·ÂÀÌ ¶Ù¾î³ª¸ç ³»Àû½Å³äÀÌ °­ÇÏ´Ù.\r\n");

		printf("\r\nÇàµ¿°ú »ç°í¿¡ ÀÖ¾î µ¶Ã¢ÀûÀÌ¸ç °­ÇÑ Á÷°ü·ÂÀ» Áö³æ´Ù. ÀÚ½ÅÀÌ °¡Áø ¿µ°¨°ú ¸ñÀûÀ» ½ÇÇö½ÃÅ°·Á´Â ÀÇÁö¿Í °á´Ü·Â°ú ÀÎ³»½ÉÀ» °¡Áö°í ÀÖ´Ù. ÀÚ½Å°ú Å¸ÀÎÀÇ ´É·ÂÀ» Áß¿ä½ÃÇÏ¸ç, ¸ñÀû ´Þ¼ºÀ» À§ÇÏ¿© ¿Â ½Ã°£°ú ³ë·ÂÀ» ¹ÙÃÄ ÀÏÇÑ´Ù. Á÷°ü·Â°ú ÅëÂû·ÂÀÌ È°¿ëµÇ´Â ºÐ¾ß, Áï °úÇÐ, ¿£Áö´Ï¾î¸µ, ¹ß¸í, Á¤Ä¡, Ã¶ÇÐ ºÐ¾ß µî¿¡¼­ ´É·ÂÀ» ¹ßÈÖÇÑ´Ù. ³ÃÃ¶ÇÑ ºÐ¼®·Â ¶§¹®¿¡ ÀÏ°ú »ç¶÷À» ÀÖ´Â ±×´ë·ÎÀÇ »ç½ÇÀûÀÎ ¸éÀ» º¸°íÀÚÇÏ´Â ³ë·ÂÀÌ ÇÊ¿äÇÏ¸ç Å¸ÀÎÀÇ °¨Á¤À» °í·ÁÇÏ°í Å¸ÀÎÀÇ °üÁ¡¿¡ ÁøÁöÇÏ°Ô ±Í±â¿ïÀÌ´Â °ÍÀÌ ¹Ù¶÷Á÷ÇÏ´Ù.\r\n");

		intj();

		printf("\r\nµ¶¸³ÀûÀÎ, ³í¸®ÀûÀÎ, ºñÆÇÀûÀÎ, µ¶Ã¢ÀûÀÎ, Ã¼°èÀûÀÎ ¸¶À½, È®°íÇÑ, ºñÁ¯ÀÌ ÀÖ´Â, ÀÌ·ÐÀûÀÎ, ±âÁØÀÌ ³ôÀº, °´°üÀûÀÎ, ÀüÃ¼ÀûÀÎ\r\n");
	}

	if(mbi==1 && mbs==1 && mbt==1 && mbp==1)
	{
		print_header("\r\nISTP ¹é°ú»çÀüÇü\r\n");

		printf("\r\nÁ¶¿ëÇÏ°í °ú¹¬ÇÏ°í ÀýÁ¦µÈ È£±â½ÉÀ¸·Î ÀÎ»ýÀ» °üÂûÇÏ¸ç »óÈ²À» ÆÄ¾ÇÇÏ´Â ¹Î°¨¼º°ú µµ±¸¸¦ ´Ù·ç´Â ¶Ù¾î³­ ´É·ÂÀÌ ÀÖ´Ù.\r\n");

		printf("\r\n¸»ÀÌ ¾øÀ¸¸ç, °´°üÀûÀ¸·Î ÀÎ»ýÀ» °üÂûÇÏ´Â ÇüÀÌ´Ù. ÇÊ¿äÀÌ»óÀ¸·Î ÀÚ½ÅÀ» ¹ßÈÖÇÏÁö ¾ÊÀ¸¸ç, ÀÏ°ú °ü°èµÇÁö ¾Ê´Â ÀÌ»ó ¾î¶² »óÈ²ÀÌ³ª ÀÎ°£ °ü°è¿¡ Á÷Á¢ ¶Ù¾îµéÁö ¾Ê´Â´Ù. °¡´ÉÇÑ ¿¡³ÊÁö ¼Òºñ¸¦ ÇÏÁö ¾ÊÀ¸·Á ÇÏ¸ç, »ç¶÷¿¡ µû¶ó »ç½ÇÀûÀÚ·á¸¦ Á¤¸®, Á¶Á÷ÇÏ±æ ÁÁ¾ÆÇÏ¸ç ±â°è¸¦ ¸¸Áö°Å³ª ÀÎ°ú °ü°è³ª °´°üÀû ¿ø¸®¿¡ °ü½ÉÀÌ ¸¹´Ù. ¿¬Àå, µµ±¸, ±â°è¸¦ ´Ù·ç´Âµ¥ ¶Ù¾î³ª¸ç »ç½ÇµéÀ» Á¶Á÷È­ÇÏ´Â Àç´ÉÀÌ ¸¹À½À¸·Î ¹ý·ü, °æÁ¦, ¸¶ÄÉÆÃ, ÆÇ¸ÅÅë°è ºÐ¾ß¿¡ ´É·ÂÀ» ¹ßÈÖÇÑ´Ù. ¹ÎÃ¸ÇÏ°Ô »óÈ²À» ÆÄ¾ÇÇÏ´Â ´É·ÂÀÌ ÀÖ´Ù. ´À³¦ÀÌ³ª °¨Á¤, Å¸ÀÎ¿¡ ´ëÇÑ ¸¶À½À» Ç¥ÇöÇÏ±â ¾î·Á¿öÇÑ´Ù.\r\n");

		istp();

		printf("\r\n°´°üÀûÀÎ, ÆíÀÇÀûÀÎ, ½ÇÁ¦ÀûÀÎ, Çö½ÇÀûÀÎ, »ç½ÇÀûÀÎ, ÀÀ¿ëÀûÀÎ, µ¶¸³ÀûÀÎ, ¸ðÇèÀûÀÎ, ÀÚ¹ßÀûÀÎ, À¶Åë¼º ÀÖ´Â, ÀÚ±â°áÁ¤¿¡ ÀÇÇÑ\r\n");
	}	

	if(mbi==1 && mbs==1 && mbf==1 && mbp==1)
	{
		print_header("\r\nISFP ¼ºÀÎ±ºÀÚÇü\r\n");

		printf("\r\n¸»¾øÀÌ ´ÙÁ¤ÇÏ°í ¿ÂÈ­ÇÏ¸ç Ä£ÀýÇÏ°í ¿¬±â·ÂÀÌ ¶Ù¾î³ª¸ç °â¼ÕÇÏ´Ù.\r\n");

		printf("\r\n¸»¾øÀÌ ´ÙÁ¤ÇÏ°í, ¾çÅÐ ¾È°¨À» ³Ö´Â ¿À¹öÄÚÆ®Ã³·³ ¼Ó¸¶À½ÀÌ µû¶æÇÏ°í Ä£ÀýÇÏ´Ù. ±×·¯³ª »ó´ë¹æÀ» Àß ¾Ë°Ô µÉ ¶§±îÁö ÀÌ µû¶æÇÔÀ» Àß µå·¯³»Áö ¾Ê´Â´Ù. µ¿Á¤ÀûÀÌ¸ç ÀÚ±â ´É·Â¿¡ ´ëÇØ¼­ ¸ðµç ¼º°Ý À¯Çü Áß¿¡¼­ °¡Àå °â¼ÕÇÏ°í ÀûÀÀ·Â°ú °ü¿ë¼ºÀÌ ¸¹´Ù. ÀÚ½ÅÀÇ ÀÇ°ßÀÌ³ª °¡Ä¡¸¦ Å¸ÀÎ¿¡°Ô °­¿äÇÏÁö ¾ÊÀ¸¸ç ¹Ý´ëÀÇ°ßÀÌ³ª Ãæµ¹À» ÇÇÇÏ°í, ÀÎÈ­¸¦ Áß½ÃÇÑ´Ù. ÀÎ°£°ú °ü°èµÇ´Â ÀÏÀ» ÇÒ¶§ ÀÚ½Å°ú Å¸ÀÎÀÇ °¨Á¤¿¡ Áö³ªÄ¡°Ô ¹Î°¨ÇÏ°í, °áÁ¤·Â°ú ÃßÁø·ÂÀÌ ÇÊ¿äÇÒ ¶§°¡ ¸¹À» °ÍÀÌ´Ù. ÀÏ»óÈ°µ¿¿¡ ÀÖ¾î¼­ °ü¿ëÀû, °³¹æÀû, À¶Åë¼º, ÀûÀÀ·ÂÀÌ ÀÖ´Ù.\r\n");

		isfp();

		printf("\r\nµ¹º¸´Â, ºÎµå·¯¿î, ¿ÂÈ­ÇÑ, À¶Åë¼º ÀÖ´Â, ¹Î°¨ÇÑ, ¿¹¸®ÇÑ, Çùµ¿ÀûÀÎ, Ãæ¼º½º·¯¿î, ½Å·ÚÇÏ´Â, ÀÚ¹ßÀûÀÎ, ÀÌÇØÇÏ´Â, Á¶È­·Î¿î, °â¼ÕÇÑ\r\n");
	}

	if(mbi==1 && mbn==1 && mbf==1 && mbp==1)
	{
		print_header("\r\nINFP ÀÜ´Ù¸£Å©Çü\r\n");

		printf("\r\nÁ¤¿­ÀûÀÌ°í Ãæ½ÇÇÏ¸ç ¸ñ°¡ÀûÀÌ°í, ³¶¸¸ÀûÀÌ¸ç ³»Àû½Å³äÀÌ ±í´Ù.\r\n");

		printf("\r\n¸¶À½ÀÌ µû¶æÇÏ°í Á¶¿ëÇÏ¸ç ÀÚ½ÅÀÌ °ü°èÇÏ´Â ÀÏÀÌ³ª »ç¶÷¿¡ ´ëÇÏ¿© Ã¥ÀÓ°¨ÀÌ °­ÇÏ°í ¼º½ÇÇÏ´Ù. ÀÌÇØ½ÉÀÌ ¸¹°í °ü´ëÇÏ¸ç ÀÚ½ÅÀÌ ÁöÇâÇÏ´Â ÀÌ»ó¿¡ ´ëÇÏ¿© Á¤¿­ÀûÀÎ ½Å³äÀ» °¡Á³À¸¸ç, ³²À» Áö¹èÇÏ°Å³ª ÁÁÀº ÀÎ»óÀ» ÁÖ°íÀÚÇÏ´Â °æÇâÀÌ °ÅÀÇ ¾ø´Ù. ¿Ïº®ÁÖÀÇÀû °æÇâÀÌ ÀÖÀ¸¸ç, ³ëµ¿ÀÇ ´ñ°¡¸¦ ³Ñ¾î¼­ ÀÚ½ÅÀÌ ÇÏ´Â ÀÏ¿¡ Èï¹Ì¸¦ Ã£°íÀÚÇÏ´Â °æÇâÀÌ ÀÖÀ¸¸ç, ÀÎ°£ÀÌÇØ¿Í ÀÎ°£º¹Áö¿¡ ±â¿©ÇÒ ¼ö ÀÖ´Â ÀÏÀ» ÇÏ±â¸¦ ¿øÇÑ´Ù. ¾ð¾î, ¹®ÇÐ, »ó´ã, ½É¸®ÇÐ, °úÇÐ, ¿¹¼úºÐ¾ß¿¡¼­ ´É·ÂÀ» ¹ßÈÖÇÑ´Ù. ÀÚ½ÅÀÇ ÀÌ»ó°ú Çö½ÇÀÌ ¾È°í ÀÖ´Â ½ÇÁ¦ »óÈ²À» °í·ÁÇÏ´Â ´É·ÂÀÌ ÇÊ¿äÇÏ´Ù.\r\n");

		infp();

		printf("\r\nÀÚºñ·Î¿î, ºÎµå·¯¿î, °í°áÇÑ, À¶Åë¼º ÀÖ´Â, Çå½ÅÀûÀÎ, ¸ðÇè½ÉÀÌ ÀÖ´Â, Ã¢ÀÇÀûÀÎ, Ãæ¼º½º·¯¿î, Çå½ÅÇÏ´Â, ±íÀÌ ÀÖ´Â, °ú¹¬ÇÑ, °ø°¨ÇÏ´Â\r\n");
	}

	if(mbi==1 && mbn==1 && mbt==1 && mbp==1)
	{
		print_header("\r\nINTP ¾ÆÀÌµð¾î¹ðÅ©Çü\r\n");

		printf("\r\nÁ¶¿ëÇÏ°í °ú¹¬ÇÏ¸ç ³í¸®¿Í ºÐ¼®À¸·Î ¹®Á¦¸¦ ÇØ°áÇÏ±â ÁÁ¾ÆÇÑ´Ù.\r\n");

		printf("\r\n°ú¹¬ÇÏ³ª °ü½ÉÀÌ ÀÖ´Â ºÐ¾ß¿¡ ´ëÇØ¼­´Â ¸»À» ÀßÇÏ¸ç ÀÌÇØ°¡ ºü¸£°í ³ôÀº Á÷°ü·ÂÀ¸·Î ÅëÂûÇÏ´Â Àç´É°ú ÁöÀû È£±â½ÉÀÌ ¸¹´Ù. °³ÀÎÀûÀÎ ÀÎ°£°ü°è³ª Ä£¸ñÈ¸ È¤Àº Àâ´ã µî¿¡ º°·Î °ü½ÉÀÌ ¾øÀ¸¸ç ¸Å¿ì ºÐ¼®ÀûÀÌ°í ³í¸®ÀûÀÌ¸ç °´°üÀû ºñÆòÀ» Àß ÇÑ´Ù. ÁöÀû È£±â½ÉÀ» ¹ßÈÖÇÒ ¼ö ÀÖ´Â ºÐ¾ß Áï ¼ø¼ö°úÇÐ, ¿¬±¸, ¼öÇÐ, ¿£Áö´Ï¾î¸µºÐ¾ß³ª Ãß»óÀû °³³äÀ» ´Ù·ç´Â °æÁ¦, Ã¶ÇÐ, ½É¸®ÇÐ ºÐ¾ßÀÇ ÇÐ¹®À» ÁÁ¾ÆÇÑ´Ù. Áö³ªÄ¡°Ô Ãß»óÀûÀÌ°í ºñÇö½ÇÀûÀÌ¸ç »ç±³¼ºÀÌ °á¿©µÇ±â ½¬¿î °æÇâÀÌ ÀÖ°í, ¶§·Î´Â ÀÚ½ÅÀÇ ÁöÀû ´É·ÂÀ» Àº±ÙÈ÷ °ú½ÃÇÏ´Â ¼ö°¡ ÀÖ±â ¶§¹®¿¡ °Å¸¸ÇÏ°Ô º¸ÀÏ ¼ö ÀÖ´Ù.\r\n");

		intp();

		printf("\r\n³í¸®ÀûÀÎ, È¸ÀÇÀûÀÎ, ÀÎÁöÀûÀÎ, ÃÊ¿¬ÇÑ, ÀÌ·ÐÀûÀÎ, Á¤È®ÇÑ, µ¶¸³ÀûÀÎ, »ç»öÀûÀÎ, µ¶Ã¢ÀûÀÎ, ÀÚÀ²ÀûÀÎ, ÀÚ±â°áÁ¤¿¡ ÀÇÇÑ\r\n");
	}	

	if(mbe==1 && mbs==1 && mbt==1 && mbp==1)
	{
		print_header("\r\nESTP ¼ö¿ÏÁÁÀº È°µ¿°¡Çü\r\n");

		printf("\r\nÇö½ÇÀûÀÎ ¹®Á¦ÇØ°á¿¡ ´ÉÇÏ¸ç ÀûÀÀ·ÂÀÌ °­ÇÏ°í °ü¿ëÀûÀÌ´Ù.\r\n");

		printf("\r\n»ç½ÇÀûÀÌ°í °ü´ëÇÏ¸ç, °³¹æÀûÀÌ°í »ç¶÷ÀÌ³ª ÀÏ¿¡ ´ëÇÑ ¼±ÀÔ°üÀÌ º°·Î ¾ø´Ù. °­ÇÑ Çö½Ç°¨°¢À¸·Î Å¸ÇùÃ¥À» ¸ð»öÇÏ°í ¹®Á¦¸¦ ÇØ°áÇÏ´Â ´É·ÂÀÌ ¶Ù¾î³ª´Ù. ÀûÀÀÀ» ÀßÇÏ°í Ä£±¸¸¦ ÁÁ¾ÆÇÏ¸ç ±ä¼³¸íÀ» ½È¾îÇÏ°í, ¿îµ¿, À½½Ä, ´Ù¾çÇÑ È°µ¿µî ÁÖ·Î ¿À°üÀ¸·Î º¸°í, µè°í, ¸¸Áú ¼ö ÀÖ´Â »ýÈ°ÀÇ ¸ðµç °ÍÀ» Áñ±â´Â ÇüÀÌ´Ù. ¼ø¹ß·ÂÀÌ ¶Ù¾î³ª¸ç ¸¹Àº »ç½ÇµéÀ» ½±°Ô ±â¾ïÇÏ°í, ¿¹¼úÀûÀÎ ¸Ú°ú ÆÇ´Ü·ÂÀ» Áö´Ï°í ÀÖÀ¸¸ç, ¿¬ÀåÀÌ³ª Àç·á¸¦ ´Ù·ç´Âµ¥ ´É¼÷ÇÏ´Ù. ³í¸® ºÐ¼®ÀûÀ¸·Î ÀÏÀ» Ã³¸®ÇÏ°í, Ãß»óÀûÀÎ ¾ÆÀÌµð¾î³ª °³³ä¿¡ ´ëÇØ º°·Î Èï¹Ì°¡ ¾ø´Ù.\r\n");

		estp();

		printf("\r\nÇàµ¿ÁöÇâÀûÀÎ, À¶Åë¼º ÀÖ´Â, Àç¹Ì¸¦ ÁÁ¾ÆÇÏ´Â, ÀçÁÖ°¡ ¸¹Àº, ¿­Á¤ÀûÀÎ, ³«ÃµÀûÀÎ. ¹ÎÃ¸ÇÑ, ÀÚ¹ßÀûÀÎ, ½Ç¿ëÀûÀÎ, ´À±ßÇÑ, ¼³µæÀûÀÎ, °³¹æÀûÀÎ\r\n");
	}	

	if(mbe==1 && mbs==1 && mbf==1 && mbp==1)
	{
		print_header("\r\nESFP »ç±³ÀûÀÎ À¯Çü\r\n");

		printf("\r\n»ç±³ÀûÀÌ°í È°µ¿ÀûÀÌ¸ç ¼ö¿ëÀûÀÌ°í Ä£ÀýÇÏ¸ç ³«ÃµÀûÀÌ´Ù.\r\n");

		printf("\r\nÇö½ÇÀûÀÌ°í ½ÇÁ¦ÀûÀÌ¸ç Ä£ÀýÇÏ´Ù. ¾î¶² »óÈ²ÀÌµç Àß ÀûÀÀÇÏ¸ç ¼ö¿ë·ÂÀÌ °­ÇÏ°í »ç±³ÀûÀÌ´Ù. ÁÖÀ§ÀÇ »ç¶÷ÀÌ³ª »ç¹°À» ´Ù·ç´Â »ç½ÇÀûÀÎ »ó½ÄÀÌ Ç³ºÎÇÏ´Ù. ¹°ÁúÀû ¼ÒÀ¯³ª ¿îµ¿µîÀÇ ½Ç»ýÈ°À» Áñ±â¸ç, »ó½Ä°ú ½ÇÁ¦Àû ´É·ÂÀ» ÇÊ¿ä·Î ÇÏ´Â ºÐ¾ßÀÇ ÀÏ Áï, ÀÇ·á, ÆÇ¸Å, ±³Åë, À¯Èï¾÷, °£È£Á÷, ºñ¼­Á÷, »ç¹«Á÷, °¨µ¶Á÷, ±â°è¸¦ ´Ù·ç´Â ºÐ¾ß¸¦ ¼±È£ÇÑ´Ù. ¶§·Î´Â Á¶±Ý ¼ö´Ù½º·´°í, ±íÀÌ°¡ °á¿©µÇ°Å³ª ¸¶¹«¸®¸¦ µîÇÑ½ÃÇÏ´Â °æÇâÀÌ ÀÖÀ¸³ª, ¾î¶² Á¶Á÷Ã¼³ª °øµ¿Ã¼¿¡¼­ ¹à°í Àç¹ÌÀÖ´Â ºÐÀ§±â Á¶¼º ¿ªÇÒÀ» ÀßÇÑ´Ù.\r\n");

		esfp();

		printf("\r\n¿­¼ºÀûÀÎ, À¶Åë¼º ÀÖ´Â, ÄèÈ°ÇÑ, ¿ìÈ£ÀûÀÎ, ¸í¶ûÇÑ, »ç±³ÀûÀÎ, Ç¥ÇöÀûÀÎ, Çùµ¿ÀûÀÎ, ´À±ßÇÑ, °ü¿ëÀûÀÎ, °³¹æÀûÀÎ, ³«ÃµÀûÀÎ\r\n");
	}

	if(mbe==1 && mbn==1 && mbf==1 && mbp==1)
	{
		print_header("\r\nENFP ½ºÆÄÅ©Çü\r\n");

		printf("\r\nµû¶æÇÏ°í Á¤¿­ÀûÀÌ°í È°±â¿¡ ³ÑÄ¡¸ç Àç´ÉÀÌ ¸¹°í »ó»ó·ÂÀÌ Ç³ºÎÇÏ´Ù.\r\n");

		printf("\r\n¿ÂÁ¤ÀûÀÌ°í Ã¢ÀÇÀûÀÌ¸ç Ç×»ó »õ·Î¿î °¡´É¼ºÀ» Ã£°í ½ÃµµÇÏ´Â ÇüÀÌ´Ù. ¹®Á¦ ÇØ°á¿¡ Àçºü¸£°í °ü½ÉÀÌ ÀÖ´Â ÀÏÀº ¹«¾ùÀÌµçÁö ¼öÇàÇØ³»´Â ´É·Â°ú ¿­¼ºÀÌ ÀÖ´Ù. ´Ù¸¥ »ç¶÷µé¿¡°Ô °ü½ÉÀ» ½ñÀ¸¸ç »ç¶÷µéÀ» Àß ´Ù·ç°í ¶Ù¾î³­ ÅëÂû·ÂÀ¸·Î µµ¿òÀ» ÁØ´Ù. »ó´ã, ±³À° °úÇÐ, Àú³Î¸®½ºÆ®, ±¤°í, ÆÇ¸Å, ¼ºÁ÷, ÀÛ°¡ µîÀÇ ºÐ¾ß¿¡¼­ ¶Ù¾î³­ Àç´ÉÀ» º¸ÀÎ´Ù. ¹Ýº¹µÇ´Â ÀÏ»óÀûÀÎ ÀÏÀ» ÂüÁö ¸øÇÏ°í ¿­¼ºÀÌ ³ªÁö ¾Ê´Â´Ù. ¶ÇÇÑ ÇÑ °¡Áö ÀÏÀ» ³¡³»±âµµ Àü¿¡ ¸î °¡Áö ´Ù¸¥ ÀÏÀ» ¶Ç ¹ú¸®´Â °æÇâÀ» °¡Áö°í ÀÖ´Ù. ÅëÂû·Â°ú Ã¢ÀÇ·ÂÀÌ ¿ä±¸µÇÁö ¾Ê´Â ÀÏ¿¡´Â Èï¹Ì¸¦ ´À³¢Áö ¸øÇÏ°í ¿­¼ºÀ» ºÒ·¯ÀÏÀ¸Å°Áö ¸øÇÑ´Ù.\r\n");

		enfp();

		printf("\r\nÃ¢ÀÇÀûÀÎ, È£±â½É ÀÖ´Â, ¿­¼ºÀûÀÎ, ÀçÁÖ°¡ ¸¹Àº, ÀÚ¹ßÀûÀÎ, Ç¥ÇöÀûÀÎ, µ¶¸³ÀûÀÎ, ¿ìÈ£ÀûÀÎ, ¿­Á¤ÀûÀÎ, »ó»óÀûÀÎ, È°µ¿ÀûÀÎ\r\n");
	}

	if(mbe==1 && mbn==1 && mbt==1 && mbp==1)
	{
		print_header("\r\nENTP ¹ß¸í°¡Çü\r\n");

		printf("\r\n¹ÎÃ¸ÇÏ°í µ¶Ã¢ÀûÀÌ¸ç ¾È¸ñÀÌ ³ÐÀ¸¸ç ´Ù¹æ¸é¿¡ °ü½É°ú Àç´ÉÀÌ ¸¹´Ù.\r\n");

		printf("\r\nµ¶Ã¢ÀûÀÌ¸ç Ã¢ÀÇ·ÂÀÌ Ç³ºÎÇÏ°í ³ÐÀº ¾È¸ñÀ» °®°í ÀÖÀ¸¸ç ´Ù¹æ¸é¿¡ Àç´ÉÀÌ ¸¹´Ù. Ç³ºÎÇÑ »ó»ó·Â°ú »õ·Î¿î ÀÏÀ» ½ÃµµÇÏ´Â ¼Ö¼±·ÂÀÌ °­ÇÏ¸ç ³í¸®ÀûÀÌ´Ù. »õ·Î¿î ¹®Á¦³ª º¹ÀâÇÑ ¹®Á¦¿¡ ÇØ°á ´É·ÂÀÌ ¶Ù¾î³ª¸ç »ç¶÷µéÀÇ µ¿Çâ¿¡ ´ëÇØ ±â¹ÎÇÏ°í ¹Ú½ÄÇÏ´Ù. ±×·¯³ª ÀÏ»óÀûÀÌ°í ¼¼ºÎÀûÀÎ ÀÏÀ» °æ½ÃÇÏ°í ÅÂ¸¸ÇÏ±â ½±´Ù. Áï, »õ·Î¿î µµÀüÀÌ ¾ø´Â ÀÏ¿¡´Â Èï¹Ì°¡ ¾øÀ¸³ª °ü½ÉÀ» °®°í ÀÖ´Â ÀÏ¿¡´Â ´ë´ÜÇÑ ¼öÇà ´É·ÂÀ» °¡Áö°í ÀÖ´Ù. ¹ß¸í°¡, °úÇÐÀÚ, ¹®Á¦ÇØ°á»ç, Àú³Î¸®½ºÆ®, ¸¶ÄÉÆÃ, ÄÄÇ»ÅÍ ºÐ¼® µî¿¡ Å¹¿ùÇÑ ´É·ÂÀÌ ÀÖ´Ù. ¶§·Î °æÀïÀûÀÌ¸ç Çö½Çº¸´Ù´Â ÀÌ·Ð¿¡ ´õ ¹àÀº ÆíÀÌ´Ù.\r\n");

		entp();

		printf("\r\nÁøÃëÀûÀÎ, µ¶¸³ÀûÀÎ, ¼ÖÁ÷ÇÑ, Àü·«ÀûÀÎ, Ã¢ÀÇÀûÀÎ, À¶Åë¼º ÀÖ´Â, µµÀüÀûÀÎ, ºÐ¼®ÀûÀÎ, ¿µ¸®ÇÑ, ÀÚ¿øÀÌ Ç³ºÎÇÑ, ÀÇ½É½º·¯¿î, ÀÌ·ÐÀûÀÎ\r\n");
	}

	if(mbe==1 && mbs==1 && mbt==1 && mbj==1)
	{
		print_header("\r\nESTJ »ç¾÷°¡Çü\r\n");

		printf("\r\n±¸Ã¼ÀûÀÌ°í Çö½ÇÀûÀÌ°í »ç½ÇÀûÀÌ¸ç È°µ¿À» Á¶Á÷È­ÇÏ°í ÁÖµµÇØ³ª°¡´Â Áöµµ·ÂÀÌ ÀÖ´Ù.\r\n");

		printf("\r\n½ÇÁúÀûÀÌ°í Çö½Ç°¨°¢ÀÌ ¶Ù¾î³ª¸ç ÀÏÀ» Á¶Á÷ÇÏ°í °èÈ¹ÇÏ¿© ÃßÁø½ÃÅ°´Â ´É·ÂÀÌ ÀÖ´Ù. ±â°èºÐ¾ß³ª ÇàÁ¤ ºÐ¾ß¿¡ Àç´ÉÀ» °¡Á³À¸¸ç, Ã¼°èÀûÀ¸·Î »ç¾÷Ã¼³ª Á¶Á÷Ã¼¸¦ ÀÌ²ø¾î ³ª°£´Ù. Å¸°í³­ ÁöµµÀÚ·Î½á ÀÏÀÇ ¸ñÇ¥¸¦ ¼³Á¤ÇÏ°í, Áö½ÃÇÏ°í, °áÁ¤ÇÏ°í ÀÌÇàÇÏ´Â ´É·ÂÀÌ ÀÖ´Ù. °á°ú¸¦ ´«À¸·Î º¼ ¼ö ÀÖ´Â ÀÏ, Áï, »ç¾÷°¡, ÇàÁ¤°ü¸®, »ý»ê°ÇÃà µîÀÇ ºÐ¾ß¿¡¼­ ´É·ÂÀ» ¹ßÈÖÇÒ ¼ö ÀÖ´Ù. ¼Ó´Ü ¼Ó°áÇÏ´Â °æÇâ°ú Áö³ªÄ¡°Ô ¾÷¹«À§ÁÖ·Î »ç¶÷À» ´ëÇÏ´Â °æÇâÀÌ ÀÖÀ¸¹Ç·Î ÀÎ°£Áß½ÉÀÇ °¡Ä¡¿Í Å¸ÀÎÀÇ °¨Á¤À» ÃæºÐÈ÷ °í·ÁÇØ¾ß ÇÑ´Ù. ¶Ç ¹Ì·¡ÀÇ °¡´É¼ºº¸´Ù ÇöÀçÀÇ »ç½ÇÀ» Ãß±¸ÇÏ±â ¶§¹®¿¡ Çö½ÇÀû, ½Ç¿ëÀûÀÎ ¸éÀÌ °­ÇÏ´Ù.\r\n");

		estj();

		printf("\r\n³í¸®ÀûÀÎ, °áÁ¤ÀûÀÎ, Ã¼°èÀûÀÎ, È¿À²ÀûÀÎ, °´°üÀûÀÎ, ½ÇÁ¦ÀûÀÎ, Á¶Á÷È­µÈ, ºñ°³ÀÎÀûÀÎ, Ã¥ÀÓÁú ¼ö ÀÖ´Â, ±¸Á¶È­µÈ, ¼º½ÇÇÑ, Áöµµ·ÂÀÌ ÀÖ´Â\r\n");
	}

	if(mbe==1 && mbs==1 && mbf==1 && mbj==1)
	{
		print_header("\r\nESFJ Ä£¼±µµ¸ðÇü\r\n");

		printf("\r\n¸¶À½ÀÌ µû¶æÇÏ°í ÀÌ¾ß±âÇÏ±â ÁÁ¾ÆÇÏ°í, ¾ç½É¹Ù¸£°í ÀÎÈ­¸¦ Àß ÀÌ·é´Ù.\r\n");

		printf("\r\nµ¿Á¤½ÉÀÌ ¸¹°í ´Ù¸¥ »ç¶÷¿¡°Ô °ü½ÉÀ» ½ñ°í ÀÎÈ­¸¦ Áß½ÃÇÑ´Ù. Å¸°í³­ Çù·ÂÀÚ·Î½á µ¿·á¾Ö°¡ ¸¹°í Ä£ÀýÇÏ¸ç ´Éµ¿ÀûÀÎ ±¸¼º¿øÀÌ´Ù. ÀÌ¾ß±âÇÏ±â¸¦ Áñ±â¸ç Á¤¸®Á¤µ·À» ÀßÇÏ°í ÂüÀ»¼ºÀÌ ¸¹À¸¸ç ´Ù¸¥ »ç¶÷À» Àß µµ¿ÍÁØ´Ù. »ç¶÷À» ´Ù·ç°í Çàµ¿À» ¿ä±¸ÇÏ´Â ºÐ¾ß, ¿¹¸¦ µé¸é, ±³Á÷, ¼ºÁ÷, ÆÇ¸Å Æ¯È÷ µ¿Á¤½ÉÀ» ÇÊ¿ä·ÎÇÏ´Â °£È£³ª ÀÇ·á ºÐ¾ß¿¡ ÀûÇÕÇÏ´Ù. ÀÏÀÌ³ª »ç¶÷µé¿¡ ´ëÇÑ ¹®Á¦¿¡ ´ëÇÏ¿© ³ÃÃ¶ÇÑ ÀÔÀåÀ» ÃëÇÏ´Â °ÍÀ» ¾î·Á¿ö ÇÑ´Ù. ¹Ý´ë ÀÇ°ß¿¡ ºÎµúÃÆÀ» ¶§³ª ÀÚ½ÅÀÇ ¿ä±¸°¡ °ÅÀý´çÇßÀ» ¶§ ¸¶À½ÀÇ »óÃ³¸¦ ¹Þ´Â´Ù.\r\n");

		esfj();

		printf("\r\n¼º½ÇÇÑ, Ãæ¼º½º·¯¿î, »ç±³ÀûÀÎ, °³ÀÎÀûÀÎ, Ã¥ÀÓÁú ¼ö ÀÖ´Â, Á¶È­·Î¿î, Çùµ¿ÀûÀÎ, ÀçÄ¡ÀÖ´Â, Ã¶ÀúÇÑ, °¨µ¿ÇÏ±â ½¬¿î, ÀüÅëÀûÀÎ, µ¿Á¤ÀûÀÎ\r\n");
	}

	if(mbe==1 && mbn==1 && mbf==1 && mbj==1)
	{
		print_header("\r\nENFJ ¾ðº¯´É¼÷Çü\r\n");

		printf("\r\nµû¶æÇÏ°í Àû±ØÀûÀÌ¸ç Ã¥ÀÓ°¨ÀÌ °­ÇÏ°í »ç±³¼ºÀÌ Ç³ºÎÇÏ°í µ¿Á¤½ÉÀÌ ¸¹´Ù.\r\n");

		printf("\r\n¹ÎÃ¸ÇÏ°í µ¿Á¤½ÉÀÌ ¸¹°í »ç±³ÀûÀÌ¸ç ÀÎÈ­¸¦ Áß¿ä½ÃÇÏ°í ÂüÀ»¼ºÀÌ ¸¹´Ù. ´Ù¸¥ »ç¶÷µéÀÇ »ý°¢ÀÌ³ª ÀÇ°ß¿¡ ÁøÁöÇÑ °ü½ÉÀ» °¡Áö°í °øµ¿¼±À» À§ÇÏ¿© ´Ù¸¥ »ç¶÷ÀÇ ÀÇ°ß¿¡ ´ëÃ¼·Î µ¿ÀÇÇÑ´Ù. ÇöÀç º¸´Ù´Â ¹Ì·¡ÀÇ °¡´É¼ºÀ» Ãß±¸ÇÏ¸ç Æí¾ÈÇÏ°í ´É¶õÇÏ°Ô °èÈ¹À» Á¦½ÃÇÏ°í Áý´ÜÀ» ÀÌ²ø¾î°¡´Â ´É·ÂÀÌ ÀÖ´Ù. »ç¶÷À» ´Ù·ç´Â ±³Á÷, ¼ºÁ÷, ½É¸® »ó´ãÄ¡·á, ¿¹¼ú, ¹®ÇÐ, ¿Ü±³, ÆÇ¸Å µî¿¡ ÀûÇÕÇÏ´Ù. ¶§·Î ´Ù¸¥ »ç¶÷µéÀÇ ÁÁÀº Á¡À» Áö³ªÄ¡°Ô ÀÌ»óÈ­ÇÏ°í ¸Í¸ñÀû Ãæ¼ºÀ» º¸ÀÌ´Â °æÇâÀÌ ÀÖÀ¸¸ç ´Ù¸¥ »ç¶÷µé¿¡ ´ëÇØ¼­µµ ÀÚ±â¿Í °°À» °ÍÀÌ¶ó°í »ý°¢ÇÏ´Â °æÇâÀÌ ÀÖ´Ù.\r\n");

		enfj();

		printf("\r\nÃæ¼º½º·¯¿î, ÀÌ»óÀûÀÎ, °³ÀÎÀûÀÎ, ¾ð¾îÀûÀÎ, Ã¥ÀÓÁú ¼ö ÀÖ´Â, Ç¥ÇöÀûÀÎ, ¿­¼ºÀûÀÎ, ¿­Á¤ÀûÀÎ, ¿Ü±³ÀûÀÎ, ¿°·ÁÇÏ´Â, ÁöÁöÀûÀÎ, ¸¶À½ÀÌ ¸Â´Â\r\n");
	}

	if(mbe==1 && mbn==1 && mbt==1 && mbj==1)
	{
		print_header("\r\nENTJ ÁöµµÀÚÇü\r\n");

		printf("\r\n¿­¼ºÀÌ ¸¹°í ¼ÖÁ÷ÇÏ°í ´ÜÈ£ÇÏ°í Áöµµ·Â°ú Åë¼Ö·ÂÀÌ ÀÖ´Ù.\r\n");

		printf("\r\nÈ°µ¿ÀûÀÌ°í ¼ÖÁ÷ÇÏ¸ç, °áÁ¤·Â°ú Åë¼Ö·ÂÀÌ ÀÖ°í, Àå±âÀû °èÈ¹°ú °Å½ÃÀû ¾È¸ñÀ» ¼±È£ÇÑ´Ù. Áö½Ä¿¡ ´ëÇÑ ¿å±¸¿Í °ü½ÉÀÌ ¸¹À¸¸ç Æ¯È÷ ÁöÀûÀÎ ÀÚ±ØÀ» ÁÖ´Â »õ·Î¿î ¾ÆÀÌµð¾î¿¡ ³ôÀº °ü½ÉÀ» °¡Á³´Ù. ÀÏÃ³¸®¿¡ ÀÖ¾î »çÀüÁØºñ¸¦ Ã¶ÀúÈ÷ ÇÏ¸ç ³í¸® ºÐ¼®ÀûÀ¸·Î °èÈ¹ÇÏ°í Á¶Á÷ÇÏ¿© Ã¼°èÀûÀ¸·Î ÃßÁøÇØ ³ª°¡´Â ÇüÀÌ´Ù. ´Ù¸¥ »ç¶÷ÀÇ ÀÇ°ß¿¡ ±Í¸¦ ±â¿ïÀÏ ÇÊ¿ä°¡ ÀÖÀ¸¸ç, ÀÚ½Å°ú Å¸ÀÎÀÇ °¨Á¤¿¡ Ãæ½ÇÇÒ ÇÊ¿ä°¡ ÀÖ´Ù. ÀÚ½ÅÀÇ ´À³¦ÀÌ³ª °¨Á¤À» ÀÎÁ¤ÇÏ°í Ç¥ÇöÇÔÀÌ Áß¿äÇÏ¸ç, ¼º±ÞÇÑ ÆÇ´ÜÀÌ³ª °á·ÐÀº ÇÇÇØ¾ß ÇÑ´Ù. ±×·¸Áö ¾ÊÀ¸¸é ´©ÀûµÈ °¨Á¤ÀÌ Å©°Ô Æø¹ßÇÒ °¡´É¼ºµµ ÀÖ´Ù.\r\n");

		entj();

		printf("\r\n³í¸®ÀûÀÎ, °áÁ¤ÀûÀÎ, °èÈ¹ÀÌ ¸¹Àº, °­ÀÎÇÑ, Àü·«ÀûÀÎ, ºñÆÇÀûÀÎ, Á¶ÀýµÈ, µµÀüÀûÀÎ, Á÷¼±ÀûÀÎ, °´°üÀûÀÎ, °øÁ¤ÇÑ, ÀÌ·ÐÀûÀÎ\r\n");
	}

	printf("\r\n´Ù¸¥ À¯ÇüÀÇ °á°ú¹°À» º¸½Ã°Ú½À´Ï±î?(Y/N): ");
	input(&lant);
	//fflush(stdin);

	if(lant=='Y'||lant=='y')
	{
		mbe=1;
		mbi=1;
		mbs=1;
		mbn=1;
		mbf=1;
		mbt=1;
		mbj=1;
		mbp=1;
	}

	else 
	{
		mbe=0;
		mbi=0;
		mbs=0;
		mbn=0;
		mbf=0;
		mbt=0;
		mbj=0;
		mbp=0;
	}


	if(mbi==1 && mbs==1 && mbt==1 && mbj==1)
	{
		print_header("\r\nISTJ ¼¼»óÀÇ ¼Ò±ÝÇü\r\n");

		printf("\r\n½ÅÁßÇÏ°í Á¶¿ëÇÏ¸ç ÁýÁß·ÂÀÌ °­ÇÏ°í ¸Å»ç¿¡ Ã¶ÀúÇÏ¸ç »ç¸®ºÐº°·ÂÀÌ ¶Ù¾î³ª´Ù\r\n");

		printf("\r\n½ÇÁ¦ »ç½Ç¿¡ ´ëÇÏ¿© Á¤È®ÇÏ°í Ã¼°èÀûÀ¸·Î ±â¾ïÇÏ¸ç ÀÏÃ³¸®¿¡ ÀÖ¾î¼­µµ ½ÅÁßÇÏ¸ç Ã¥ÀÓ°¨ÀÌ °­ÇÏ´Ù. ÁýÁß·ÂÀÌ °­ÇÑ Çö½Ç°¨°¢À» Áö³æÀ¸¸ç Á¶Á÷ÀûÀÌ°í Ä§ÂøÇÏ´Ù. º¸¼öÀûÀÎ °æÇâÀÌ ÀÖÀ¸¸ç, ¹®Á¦¸¦ ÇØ°áÇÏ´Âµ¥ °ú°ÅÀÇ °æÇèÀ» Àß Àû¿ëÇÏ¸ç, ¹Ýº¹µÇ´Â ÀÏ»óÀûÀÎ ÀÏ¿¡ ´ëÇÑ ÀÎ³»·ÂÀÌ °­ÇÏ´Ù. ÀÚ½Å°ú Å¸ÀÎÀÇ °¨Á¤°ú ±âºÐÀ» ¹è·ÁÇÏ¸ç, ÀüÃ¼ÀûÀÌ°í Å¸ÇùÀû ¹æ¾ÈÀ» °í·ÁÇÏ´Â ³ë·ÂÀÌ ¶§·Î ÇÊ¿äÇÏ´Ù. Á¤È®¼º°ú Á¶Á÷·ÂÀ» ¹ßÈÖÇÏ´Â ºÐ¾ßÀÇ ÀÏÀ» ¼±È£ÇÑ´Ù. Áï È¸°è, »ý»ê, °ÇÃà, ÀÇ·á, »ç¹«Á÷, °ü¸®Á÷ µî¿¡¼­ ´É·ÂÀ» ¹ßÈÖÇÏ¸ç, À§±â»óÈ²¿¡¼­µµ ¾ÈÁ¤µÇ¾î ÀÖ´Ù.\r\n");

		istj();

		printf("\r\n»ç½ÇÀûÀÎ, Ã¶ÀúÇÑ, Ã¼°èÀûÀÎ, ½Å·ÚÇÒ ¼ö ÀÖ´Â, È®°íºÎµ¿ÇÑ, ½ÇÁ¦ÀûÀÎ, Á¶Á÷È­µÈ, ÀÇ¹«ÀûÀÎ, ºÐº°ÀÖ´Â, ±Ù¸éÇÑ, ¹ÏÀ»¸¸ÇÑ, ¼º½ÇÇÑ\r\n");
	}

	if(mbi==1 && mbs==1 && mbf==1 && mbj==1)
	{
		print_header("\r\nISFJ ÀÓ±Ý µÞÆíÀÇ ±Ç·ÂÇü\r\n");

		printf("\r\nÁ¶¿ëÇÏ°í Â÷ºÐÇÏ¸ç Ä£±ÙÇÏ°í  Ã¥ÀÓ°¨ÀÌ ÀÖÀ¸¸ç Çå½ÅÀûÀÌ´Ù.\r\n");

		printf("\r\nÃ¥ÀÓ°¨ÀÌ °­ÇÏ°í ¿ÂÁ¤ÀûÀÌ¸ç Çå½ÅÀûÀÌ°í, Ä§ÂøÇÏ¸ç, ÀÎ³»·ÂÀÌ °­ÇÏ´Ù. ´Ù¸¥ »ç¶÷ÀÇ »çÁ¤À» °í·ÁÇÏ¸ç ÀÚ½Å°ú Å¸ÀÎÀÇ °¨Á¤¿¡ ¹Î°¨ÇÏ¸ç, ÀÏÃ³¸®¿¡ ÀÖ¾î¼­ Çö½Ç°¨°¢À» °®°í ½ÇÁ¦ÀûÀÌ°í Á¶Á÷ÀûÀ¸·Î Ã³¸®ÇÑ´Ù. °æÇèÀ» ÅëÇØ¼­ ÀÚ½ÅÀÌ Æ²·È´Ù°í ÀÎÁ¤ÇÒ ¶§ ±îÁö ¾î¶°ÇÑ ³­°üÀÌ ÀÖ¾îµµ ²ÙÁØÈ÷ ¹Ð°í ³ª°¡´Â ÇüÀÌ´Ù. ¶§·Î ÀÇÁ¸ÀûÀÌ°í µ¶Ã¢¼ºÀÌ ¿ä±¸µÇ¸ç Å¸ÀÎ¿¡°Ô ÀÚ½ÅÀ» ÃæºÐÈ÷ ¸íÈ®ÇÏ°Ô Ç¥ÇöÇÏ´Â °ÍÀÌ ÇÊ¿äÇÒ ¶§°¡ ÀÖ´Ù. Å¸ÀÎÀÇ °ü½É°ú °üÂû·ÂÀÌ ÇÊ¿äÇÑ ºÐ¾ß, Áï ÀÇ·á, °£È£, ±³Á÷, »ç¹«Á÷, »çÈ¸»ç¾÷¿¡ ÀûÇÕÇÏ´Ù. ÀÌµéÀÌ ÀÏÀ» ÇÏ°í, ¼¼»ó¿¡ ´ëÃ³ÇÒ ¶§ ±×µéÀÇ Çàµ¿Àº ºÐº°·ÂÀÌ ÀÖ´Ù.\r\n");

		isfj();

		printf("\r\n»ó¼¼ÇÑ, ¼º½ÇÇÑ, ÀüÅëÀûÀÎ, Ãæ½ÇÇÑ, ÂüÀ»¼ºÀÖ´Â, Á¶Á÷È­µÈ, ºÀ»çÀûÀÎ, Çå½ÅÀûÀÎ, º¸È£ÇÏ´Â, ¸Å¿ì ¼¶¼¼ÇÑ, Ã¥ÀÓÁú ¼ö ÀÖ´Â, µ¿Á¤ÀûÀÎ\r\n");
	}	

	if(mbi==1 && mbn==1 && mbf==1 && mbj==1)
	{
		print_header("\r\nINFJ ¿¹¾ðÀÚÇü\r\n");

		printf("\r\nÀÎ³»½ÉÀÌ ¸¹°í ÅëÂû·Â°ú Á÷°ü·ÂÀÌ ¶Ù¾î³ª¸ç ¾ç½ÉÀÌ ¹Ù¸£°í È­ÇÕÀ» Ãß±¸ÇÑ´Ù.\r\n");

		printf("\r\nÃ¢ÀÇ·Â°ú ÅëÂû·ÂÀÌ ¶Ù¾î³ª¸ç, °­ÇÑ Á÷°ü·ÂÀ¸·Î ¸»¾øÀÌ Å¸ÀÎ¿¡°Ô ¿µÇâ·ÂÀ» ³¢Ä£´Ù. µ¶Ã¢¼º°ú ³»Àûµ¶¸³½ÉÀÌ °­ÇÏ¸ç, È®°íÇÑ ½Å³ä°ú ¿­Á¤À¸·Î ÀÚ½ÅÀÇ ¿µ°¨À» ±¸Çö½ÃÄÑ ³ª°¡´Â Á¤½ÅÀû ÁöµµÀÚµéÀÌ ¸¹´Ù. Á÷°ü·Â°ú »ç¶÷Áß½ÉÀÇ °¡Ä¡¸¦ Áß½ÃÇÏ´Â ºÐ¾ß Áï, ¼ºÁ÷, ½É¸®ÇÐ, ½É¸®Ä¡·á¿Í »ó´ã, ¿¹¼ú°ú ¹®ÇÐºÐ¾ßÀÌ´Ù. Å×Å©´ÏÄÃÇÑ ºÐ¾ß·Î´Â ¼ø¼ö°úÇÐ, ¿¬±¸ °³¹ßºÐ¾ß·Î½á »õ·Î¿î ½Ãµµ¿¡ ´ëÇÑ ¿­¼ºÀÌ ´ë´ÜÇÏ´Ù. ÇÑ °÷¿¡ ¸ôµÎÇÏ´Â °æÇâÀ¸·Î ¸ñÀû´Þ¼º¿¡ ÇÊ¿äÇÑ ÁÖº¯ÀûÀÎ Á¶°ÇµéÀ» °æ½ÃÇÏ±â ½±°í, ÀÚ±â¾ÈÀÇ °¥µîÀÌ ¸¹°í º¹ÀâÇÏ´Ù. ÀÌµéÀº Ç³ºÎÇÑ ³»ÀûÀÎ »ýÈ°À» ¼ÒÀ¯ÇÏ°í ÀÖÀ¸¸ç ³»¸éÀÇ ¹ÝÀÀÀ» Á»Ã³·³ ³²°ú °øÀ¯ÇÏ±â ¾î·Á¿ö ÇÑ´Ù.\r\n");

		infj();

		printf("\r\nÇå½ÅÀûÀÎ, Ãæ½ÇÇÑ, ÀÚºñ·Î¿î, Ã¢ÀÇÀûÀÎ, ¿­Á¤ÀûÀÎ, ±íÀÌ ÀÖ´Â, °á½ÉÀÌ ±»Àº, °³³äÀûÀÎ, ÀüÃ¼ÀûÀÎ, ÀÌ»óÀûÀÎ, ½Åºñ·Î¿î\r\n");
	}

	if(mbi==1 && mbn==1 && mbt==1 && mbj==1)
	{
		print_header("\r\nINTJ °úÇÐÀÚÇü\r\n");		

		printf("\r\n»ç°í°¡ µ¶Ã¢ÀûÀÌ¸ç Ã¢ÀÇ·Â°ú ºñÆÇºÐ¼®·ÂÀÌ ¶Ù¾î³ª¸ç ³»Àû½Å³äÀÌ °­ÇÏ´Ù.\r\n");

		printf("\r\nÇàµ¿°ú »ç°í¿¡ ÀÖ¾î µ¶Ã¢ÀûÀÌ¸ç °­ÇÑ Á÷°ü·ÂÀ» Áö³æ´Ù. ÀÚ½ÅÀÌ °¡Áø ¿µ°¨°ú ¸ñÀûÀ» ½ÇÇö½ÃÅ°·Á´Â ÀÇÁö¿Í °á´Ü·Â°ú ÀÎ³»½ÉÀ» °¡Áö°í ÀÖ´Ù. ÀÚ½Å°ú Å¸ÀÎÀÇ ´É·ÂÀ» Áß¿ä½ÃÇÏ¸ç, ¸ñÀû ´Þ¼ºÀ» À§ÇÏ¿© ¿Â ½Ã°£°ú ³ë·ÂÀ» ¹ÙÃÄ ÀÏÇÑ´Ù. Á÷°ü·Â°ú ÅëÂû·ÂÀÌ È°¿ëµÇ´Â ºÐ¾ß, Áï °úÇÐ, ¿£Áö´Ï¾î¸µ, ¹ß¸í, Á¤Ä¡, Ã¶ÇÐ ºÐ¾ß µî¿¡¼­ ´É·ÂÀ» ¹ßÈÖÇÑ´Ù. ³ÃÃ¶ÇÑ ºÐ¼®·Â ¶§¹®¿¡ ÀÏ°ú »ç¶÷À» ÀÖ´Â ±×´ë·ÎÀÇ »ç½ÇÀûÀÎ ¸éÀ» º¸°íÀÚÇÏ´Â ³ë·ÂÀÌ ÇÊ¿äÇÏ¸ç Å¸ÀÎÀÇ °¨Á¤À» °í·ÁÇÏ°í Å¸ÀÎÀÇ °üÁ¡¿¡ ÁøÁöÇÏ°Ô ±Í±â¿ïÀÌ´Â °ÍÀÌ ¹Ù¶÷Á÷ÇÏ´Ù.\r\n");

		intj();

		printf("\r\nµ¶¸³ÀûÀÎ, ³í¸®ÀûÀÎ, ºñÆÇÀûÀÎ, µ¶Ã¢ÀûÀÎ, Ã¼°èÀûÀÎ ¸¶À½, È®°íÇÑ, ºñÁ¯ÀÌ ÀÖ´Â, ÀÌ·ÐÀûÀÎ, ±âÁØÀÌ ³ôÀº, °´°üÀûÀÎ, ÀüÃ¼ÀûÀÎ\r\n");
	}

	if(mbi==1 && mbs==1 && mbt==1 && mbp==1)
	{
		print_header("\r\nISTP ¹é°ú»çÀüÇü\r\n");

		printf("\r\nÁ¶¿ëÇÏ°í °ú¹¬ÇÏ°í ÀýÁ¦µÈ È£±â½ÉÀ¸·Î ÀÎ»ýÀ» °üÂûÇÏ¸ç »óÈ²À» ÆÄ¾ÇÇÏ´Â ¹Î°¨¼º°ú µµ±¸¸¦ ´Ù·ç´Â ¶Ù¾î³­ ´É·ÂÀÌ ÀÖ´Ù.\r\n");

		printf("\r\n¸»ÀÌ ¾øÀ¸¸ç, °´°üÀûÀ¸·Î ÀÎ»ýÀ» °üÂûÇÏ´Â ÇüÀÌ´Ù. ÇÊ¿äÀÌ»óÀ¸·Î ÀÚ½ÅÀ» ¹ßÈÖÇÏÁö ¾ÊÀ¸¸ç, ÀÏ°ú °ü°èµÇÁö ¾Ê´Â ÀÌ»ó ¾î¶² »óÈ²ÀÌ³ª ÀÎ°£ °ü°è¿¡ Á÷Á¢ ¶Ù¾îµéÁö ¾Ê´Â´Ù. °¡´ÉÇÑ ¿¡³ÊÁö ¼Òºñ¸¦ ÇÏÁö ¾ÊÀ¸·Á ÇÏ¸ç, »ç¶÷¿¡ µû¶ó »ç½ÇÀûÀÚ·á¸¦ Á¤¸®, Á¶Á÷ÇÏ±æ ÁÁ¾ÆÇÏ¸ç ±â°è¸¦ ¸¸Áö°Å³ª ÀÎ°ú °ü°è³ª °´°üÀû ¿ø¸®¿¡ °ü½ÉÀÌ ¸¹´Ù. ¿¬Àå, µµ±¸, ±â°è¸¦ ´Ù·ç´Âµ¥ ¶Ù¾î³ª¸ç »ç½ÇµéÀ» Á¶Á÷È­ÇÏ´Â Àç´ÉÀÌ ¸¹À½À¸·Î ¹ý·ü, °æÁ¦, ¸¶ÄÉÆÃ, ÆÇ¸ÅÅë°è ºÐ¾ß¿¡ ´É·ÂÀ» ¹ßÈÖÇÑ´Ù. ¹ÎÃ¸ÇÏ°Ô »óÈ²À» ÆÄ¾ÇÇÏ´Â ´É·ÂÀÌ ÀÖ´Ù. ´À³¦ÀÌ³ª °¨Á¤, Å¸ÀÎ¿¡ ´ëÇÑ ¸¶À½À» Ç¥ÇöÇÏ±â ¾î·Á¿öÇÑ´Ù.\r\n");

		istp();

		printf("\r\n°´°üÀûÀÎ, ÆíÀÇÀûÀÎ, ½ÇÁ¦ÀûÀÎ, Çö½ÇÀûÀÎ, »ç½ÇÀûÀÎ, ÀÀ¿ëÀûÀÎ, µ¶¸³ÀûÀÎ, ¸ðÇèÀûÀÎ, ÀÚ¹ßÀûÀÎ, À¶Åë¼º ÀÖ´Â, ÀÚ±â°áÁ¤¿¡ ÀÇÇÑ\r\n");
	}	

	if(mbi==1 && mbs==1 && mbf==1 && mbp==1)
	{
		print_header("\r\nISFP ¼ºÀÎ±ºÀÚÇü\r\n");

		printf("\r\n¸»¾øÀÌ ´ÙÁ¤ÇÏ°í ¿ÂÈ­ÇÏ¸ç Ä£ÀýÇÏ°í ¿¬±â·ÂÀÌ ¶Ù¾î³ª¸ç °â¼ÕÇÏ´Ù.\r\n");

		printf("\r\n¸»¾øÀÌ ´ÙÁ¤ÇÏ°í, ¾çÅÐ ¾È°¨À» ³Ö´Â ¿À¹öÄÚÆ®Ã³·³ ¼Ó¸¶À½ÀÌ µû¶æÇÏ°í Ä£ÀýÇÏ´Ù. ±×·¯³ª »ó´ë¹æÀ» Àß ¾Ë°Ô µÉ ¶§±îÁö ÀÌ µû¶æÇÔÀ» Àß µå·¯³»Áö ¾Ê´Â´Ù. µ¿Á¤ÀûÀÌ¸ç ÀÚ±â ´É·Â¿¡ ´ëÇØ¼­ ¸ðµç ¼º°Ý À¯Çü Áß¿¡¼­ °¡Àå °â¼ÕÇÏ°í ÀûÀÀ·Â°ú °ü¿ë¼ºÀÌ ¸¹´Ù. ÀÚ½ÅÀÇ ÀÇ°ßÀÌ³ª °¡Ä¡¸¦ Å¸ÀÎ¿¡°Ô °­¿äÇÏÁö ¾ÊÀ¸¸ç ¹Ý´ëÀÇ°ßÀÌ³ª Ãæµ¹À» ÇÇÇÏ°í, ÀÎÈ­¸¦ Áß½ÃÇÑ´Ù. ÀÎ°£°ú °ü°èµÇ´Â ÀÏÀ» ÇÒ¶§ ÀÚ½Å°ú Å¸ÀÎÀÇ °¨Á¤¿¡ Áö³ªÄ¡°Ô ¹Î°¨ÇÏ°í, °áÁ¤·Â°ú ÃßÁø·ÂÀÌ ÇÊ¿äÇÒ ¶§°¡ ¸¹À» °ÍÀÌ´Ù. ÀÏ»óÈ°µ¿¿¡ ÀÖ¾î¼­ °ü¿ëÀû, °³¹æÀû, À¶Åë¼º, ÀûÀÀ·ÂÀÌ ÀÖ´Ù.\r\n");

		isfp();

		printf("\r\nµ¹º¸´Â, ºÎµå·¯¿î, ¿ÂÈ­ÇÑ, À¶Åë¼º ÀÖ´Â, ¹Î°¨ÇÑ, ¿¹¸®ÇÑ, Çùµ¿ÀûÀÎ, Ãæ¼º½º·¯¿î, ½Å·ÚÇÏ´Â, ÀÚ¹ßÀûÀÎ, ÀÌÇØÇÏ´Â, Á¶È­·Î¿î, °â¼ÕÇÑ\r\n");
	}

	if(mbi==1 && mbn==1 && mbf==1 && mbp==1)
	{
		print_header("\r\nINFP ÀÜ´Ù¸£Å©Çü\r\n");

		printf("\r\nÁ¤¿­ÀûÀÌ°í Ãæ½ÇÇÏ¸ç ¸ñ°¡ÀûÀÌ°í, ³¶¸¸ÀûÀÌ¸ç ³»Àû½Å³äÀÌ ±í´Ù.\r\n");

		printf("\r\n¸¶À½ÀÌ µû¶æÇÏ°í Á¶¿ëÇÏ¸ç ÀÚ½ÅÀÌ °ü°èÇÏ´Â ÀÏÀÌ³ª »ç¶÷¿¡ ´ëÇÏ¿© Ã¥ÀÓ°¨ÀÌ °­ÇÏ°í ¼º½ÇÇÏ´Ù. ÀÌÇØ½ÉÀÌ ¸¹°í °ü´ëÇÏ¸ç ÀÚ½ÅÀÌ ÁöÇâÇÏ´Â ÀÌ»ó¿¡ ´ëÇÏ¿© Á¤¿­ÀûÀÎ ½Å³äÀ» °¡Á³À¸¸ç, ³²À» Áö¹èÇÏ°Å³ª ÁÁÀº ÀÎ»óÀ» ÁÖ°íÀÚÇÏ´Â °æÇâÀÌ °ÅÀÇ ¾ø´Ù. ¿Ïº®ÁÖÀÇÀû °æÇâÀÌ ÀÖÀ¸¸ç, ³ëµ¿ÀÇ ´ñ°¡¸¦ ³Ñ¾î¼­ ÀÚ½ÅÀÌ ÇÏ´Â ÀÏ¿¡ Èï¹Ì¸¦ Ã£°íÀÚÇÏ´Â °æÇâÀÌ ÀÖÀ¸¸ç, ÀÎ°£ÀÌÇØ¿Í ÀÎ°£º¹Áö¿¡ ±â¿©ÇÒ ¼ö ÀÖ´Â ÀÏÀ» ÇÏ±â¸¦ ¿øÇÑ´Ù. ¾ð¾î, ¹®ÇÐ, »ó´ã, ½É¸®ÇÐ, °úÇÐ, ¿¹¼úºÐ¾ß¿¡¼­ ´É·ÂÀ» ¹ßÈÖÇÑ´Ù. ÀÚ½ÅÀÇ ÀÌ»ó°ú Çö½ÇÀÌ ¾È°í ÀÖ´Â ½ÇÁ¦ »óÈ²À» °í·ÁÇÏ´Â ´É·ÂÀÌ ÇÊ¿äÇÏ´Ù.\r\n");

		infp();

		printf("\r\nÀÚºñ·Î¿î, ºÎµå·¯¿î, °í°áÇÑ, À¶Åë¼º ÀÖ´Â, Çå½ÅÀûÀÎ, ¸ðÇè½ÉÀÌ ÀÖ´Â, Ã¢ÀÇÀûÀÎ, Ãæ¼º½º·¯¿î, Çå½ÅÇÏ´Â, ±íÀÌ ÀÖ´Â, °ú¹¬ÇÑ, °ø°¨ÇÏ´Â\r\n");
	}

	if(mbi==1 && mbn==1 && mbt==1 && mbp==1)
	{
		print_header("\r\nINTP ¾ÆÀÌµð¾î¹ðÅ©Çü\r\n");

		printf("\r\nÁ¶¿ëÇÏ°í °ú¹¬ÇÏ¸ç ³í¸®¿Í ºÐ¼®À¸·Î ¹®Á¦¸¦ ÇØ°áÇÏ±â ÁÁ¾ÆÇÑ´Ù.\r\n");

		printf("\r\n°ú¹¬ÇÏ³ª °ü½ÉÀÌ ÀÖ´Â ºÐ¾ß¿¡ ´ëÇØ¼­´Â ¸»À» ÀßÇÏ¸ç ÀÌÇØ°¡ ºü¸£°í ³ôÀº Á÷°ü·ÂÀ¸·Î ÅëÂûÇÏ´Â Àç´É°ú ÁöÀû È£±â½ÉÀÌ ¸¹´Ù. °³ÀÎÀûÀÎ ÀÎ°£°ü°è³ª Ä£¸ñÈ¸ È¤Àº Àâ´ã µî¿¡ º°·Î °ü½ÉÀÌ ¾øÀ¸¸ç ¸Å¿ì ºÐ¼®ÀûÀÌ°í ³í¸®ÀûÀÌ¸ç °´°üÀû ºñÆòÀ» Àß ÇÑ´Ù. ÁöÀû È£±â½ÉÀ» ¹ßÈÖÇÒ ¼ö ÀÖ´Â ºÐ¾ß Áï ¼ø¼ö°úÇÐ, ¿¬±¸, ¼öÇÐ, ¿£Áö´Ï¾î¸µºÐ¾ß³ª Ãß»óÀû °³³äÀ» ´Ù·ç´Â °æÁ¦, Ã¶ÇÐ, ½É¸®ÇÐ ºÐ¾ßÀÇ ÇÐ¹®À» ÁÁ¾ÆÇÑ´Ù. Áö³ªÄ¡°Ô Ãß»óÀûÀÌ°í ºñÇö½ÇÀûÀÌ¸ç »ç±³¼ºÀÌ °á¿©µÇ±â ½¬¿î °æÇâÀÌ ÀÖ°í, ¶§·Î´Â ÀÚ½ÅÀÇ ÁöÀû ´É·ÂÀ» Àº±ÙÈ÷ °ú½ÃÇÏ´Â ¼ö°¡ ÀÖ±â ¶§¹®¿¡ °Å¸¸ÇÏ°Ô º¸ÀÏ ¼ö ÀÖ´Ù.\r\n");

		intp();

		printf("\r\n³í¸®ÀûÀÎ, È¸ÀÇÀûÀÎ, ÀÎÁöÀûÀÎ, ÃÊ¿¬ÇÑ, ÀÌ·ÐÀûÀÎ, Á¤È®ÇÑ, µ¶¸³ÀûÀÎ, »ç»öÀûÀÎ, µ¶Ã¢ÀûÀÎ, ÀÚÀ²ÀûÀÎ, ÀÚ±â°áÁ¤¿¡ ÀÇÇÑ\r\n");
	}	

	if(mbe==1 && mbs==1 && mbt==1 && mbp==1)
	{
		print_header("\r\nESTP ¼ö¿ÏÁÁÀº È°µ¿°¡Çü\r\n");

		printf("\r\nÇö½ÇÀûÀÎ ¹®Á¦ÇØ°á¿¡ ´ÉÇÏ¸ç ÀûÀÀ·ÂÀÌ °­ÇÏ°í °ü¿ëÀûÀÌ´Ù.\r\n");

		printf("\r\n»ç½ÇÀûÀÌ°í °ü´ëÇÏ¸ç, °³¹æÀûÀÌ°í »ç¶÷ÀÌ³ª ÀÏ¿¡ ´ëÇÑ ¼±ÀÔ°üÀÌ º°·Î ¾ø´Ù. °­ÇÑ Çö½Ç°¨°¢À¸·Î Å¸ÇùÃ¥À» ¸ð»öÇÏ°í ¹®Á¦¸¦ ÇØ°áÇÏ´Â ´É·ÂÀÌ ¶Ù¾î³ª´Ù. ÀûÀÀÀ» ÀßÇÏ°í Ä£±¸¸¦ ÁÁ¾ÆÇÏ¸ç ±ä¼³¸íÀ» ½È¾îÇÏ°í, ¿îµ¿, À½½Ä, ´Ù¾çÇÑ È°µ¿µî ÁÖ·Î ¿À°üÀ¸·Î º¸°í, µè°í, ¸¸Áú ¼ö ÀÖ´Â »ýÈ°ÀÇ ¸ðµç °ÍÀ» Áñ±â´Â ÇüÀÌ´Ù. ¼ø¹ß·ÂÀÌ ¶Ù¾î³ª¸ç ¸¹Àº »ç½ÇµéÀ» ½±°Ô ±â¾ïÇÏ°í, ¿¹¼úÀûÀÎ ¸Ú°ú ÆÇ´Ü·ÂÀ» Áö´Ï°í ÀÖÀ¸¸ç, ¿¬ÀåÀÌ³ª Àç·á¸¦ ´Ù·ç´Âµ¥ ´É¼÷ÇÏ´Ù. ³í¸® ºÐ¼®ÀûÀ¸·Î ÀÏÀ» Ã³¸®ÇÏ°í, Ãß»óÀûÀÎ ¾ÆÀÌµð¾î³ª °³³ä¿¡ ´ëÇØ º°·Î Èï¹Ì°¡ ¾ø´Ù.\r\n");

		estp();

		printf("\r\nÇàµ¿ÁöÇâÀûÀÎ, À¶Åë¼º ÀÖ´Â, Àç¹Ì¸¦ ÁÁ¾ÆÇÏ´Â, ÀçÁÖ°¡ ¸¹Àº, ¿­Á¤ÀûÀÎ, ³«ÃµÀûÀÎ. ¹ÎÃ¸ÇÑ, ÀÚ¹ßÀûÀÎ, ½Ç¿ëÀûÀÎ, ´À±ßÇÑ, ¼³µæÀûÀÎ, °³¹æÀûÀÎ\r\n");
	}	

	if(mbe==1 && mbs==1 && mbf==1 && mbp==1)
	{
		print_header("\r\nESFP »ç±³ÀûÀÎ À¯Çü\r\n");

		printf("\r\n»ç±³ÀûÀÌ°í È°µ¿ÀûÀÌ¸ç ¼ö¿ëÀûÀÌ°í Ä£ÀýÇÏ¸ç ³«ÃµÀûÀÌ´Ù.\r\n");

		printf("\r\nÇö½ÇÀûÀÌ°í ½ÇÁ¦ÀûÀÌ¸ç Ä£ÀýÇÏ´Ù. ¾î¶² »óÈ²ÀÌµç Àß ÀûÀÀÇÏ¸ç ¼ö¿ë·ÂÀÌ °­ÇÏ°í »ç±³ÀûÀÌ´Ù. ÁÖÀ§ÀÇ »ç¶÷ÀÌ³ª »ç¹°À» ´Ù·ç´Â »ç½ÇÀûÀÎ »ó½ÄÀÌ Ç³ºÎÇÏ´Ù. ¹°ÁúÀû ¼ÒÀ¯³ª ¿îµ¿µîÀÇ ½Ç»ýÈ°À» Áñ±â¸ç, »ó½Ä°ú ½ÇÁ¦Àû ´É·ÂÀ» ÇÊ¿ä·Î ÇÏ´Â ºÐ¾ßÀÇ ÀÏ Áï, ÀÇ·á, ÆÇ¸Å, ±³Åë, À¯Èï¾÷, °£È£Á÷, ºñ¼­Á÷, »ç¹«Á÷, °¨µ¶Á÷, ±â°è¸¦ ´Ù·ç´Â ºÐ¾ß¸¦ ¼±È£ÇÑ´Ù. ¶§·Î´Â Á¶±Ý ¼ö´Ù½º·´°í, ±íÀÌ°¡ °á¿©µÇ°Å³ª ¸¶¹«¸®¸¦ µîÇÑ½ÃÇÏ´Â °æÇâÀÌ ÀÖÀ¸³ª, ¾î¶² Á¶Á÷Ã¼³ª °øµ¿Ã¼¿¡¼­ ¹à°í Àç¹ÌÀÖ´Â ºÐÀ§±â Á¶¼º ¿ªÇÒÀ» ÀßÇÑ´Ù.\r\n");

		esfp();

		printf("\r\n¿­¼ºÀûÀÎ, À¶Åë¼º ÀÖ´Â, ÄèÈ°ÇÑ, ¿ìÈ£ÀûÀÎ, ¸í¶ûÇÑ, »ç±³ÀûÀÎ, Ç¥ÇöÀûÀÎ, Çùµ¿ÀûÀÎ, ´À±ßÇÑ, °ü¿ëÀûÀÎ, °³¹æÀûÀÎ, ³«ÃµÀûÀÎ\r\n");
	}

	if(mbe==1 && mbn==1 && mbf==1 && mbp==1)
	{
		print_header("\r\nENFP ½ºÆÄÅ©Çü\r\n");

		printf("\r\nµû¶æÇÏ°í Á¤¿­ÀûÀÌ°í È°±â¿¡ ³ÑÄ¡¸ç Àç´ÉÀÌ ¸¹°í »ó»ó·ÂÀÌ Ç³ºÎÇÏ´Ù.\r\n");

		printf("\r\n¿ÂÁ¤ÀûÀÌ°í Ã¢ÀÇÀûÀÌ¸ç Ç×»ó »õ·Î¿î °¡´É¼ºÀ» Ã£°í ½ÃµµÇÏ´Â ÇüÀÌ´Ù. ¹®Á¦ ÇØ°á¿¡ Àçºü¸£°í °ü½ÉÀÌ ÀÖ´Â ÀÏÀº ¹«¾ùÀÌµçÁö ¼öÇàÇØ³»´Â ´É·Â°ú ¿­¼ºÀÌ ÀÖ´Ù. ´Ù¸¥ »ç¶÷µé¿¡°Ô °ü½ÉÀ» ½ñÀ¸¸ç »ç¶÷µéÀ» Àß ´Ù·ç°í ¶Ù¾î³­ ÅëÂû·ÂÀ¸·Î µµ¿òÀ» ÁØ´Ù. »ó´ã, ±³À° °úÇÐ, Àú³Î¸®½ºÆ®, ±¤°í, ÆÇ¸Å, ¼ºÁ÷, ÀÛ°¡ µîÀÇ ºÐ¾ß¿¡¼­ ¶Ù¾î³­ Àç´ÉÀ» º¸ÀÎ´Ù. ¹Ýº¹µÇ´Â ÀÏ»óÀûÀÎ ÀÏÀ» ÂüÁö ¸øÇÏ°í ¿­¼ºÀÌ ³ªÁö ¾Ê´Â´Ù. ¶ÇÇÑ ÇÑ °¡Áö ÀÏÀ» ³¡³»±âµµ Àü¿¡ ¸î °¡Áö ´Ù¸¥ ÀÏÀ» ¶Ç ¹ú¸®´Â °æÇâÀ» °¡Áö°í ÀÖ´Ù. ÅëÂû·Â°ú Ã¢ÀÇ·ÂÀÌ ¿ä±¸µÇÁö ¾Ê´Â ÀÏ¿¡´Â Èï¹Ì¸¦ ´À³¢Áö ¸øÇÏ°í ¿­¼ºÀ» ºÒ·¯ÀÏÀ¸Å°Áö ¸øÇÑ´Ù.\r\n");

		enfp();

		printf("\r\nÃ¢ÀÇÀûÀÎ, È£±â½É ÀÖ´Â, ¿­¼ºÀûÀÎ, ÀçÁÖ°¡ ¸¹Àº, ÀÚ¹ßÀûÀÎ, Ç¥ÇöÀûÀÎ, µ¶¸³ÀûÀÎ, ¿ìÈ£ÀûÀÎ, ¿­Á¤ÀûÀÎ, »ó»óÀûÀÎ, È°µ¿ÀûÀÎ\r\n");
	}

	if(mbe==1 && mbn==1 && mbt==1 && mbp==1)
	{
		print_header("\r\nENTP ¹ß¸í°¡Çü\r\n");

		printf("\r\n¹ÎÃ¸ÇÏ°í µ¶Ã¢ÀûÀÌ¸ç ¾È¸ñÀÌ ³ÐÀ¸¸ç ´Ù¹æ¸é¿¡ °ü½É°ú Àç´ÉÀÌ ¸¹´Ù.\r\n");

		printf("\r\nµ¶Ã¢ÀûÀÌ¸ç Ã¢ÀÇ·ÂÀÌ Ç³ºÎÇÏ°í ³ÐÀº ¾È¸ñÀ» °®°í ÀÖÀ¸¸ç ´Ù¹æ¸é¿¡ Àç´ÉÀÌ ¸¹´Ù. Ç³ºÎÇÑ »ó»ó·Â°ú »õ·Î¿î ÀÏÀ» ½ÃµµÇÏ´Â ¼Ö¼±·ÂÀÌ °­ÇÏ¸ç ³í¸®ÀûÀÌ´Ù. »õ·Î¿î ¹®Á¦³ª º¹ÀâÇÑ ¹®Á¦¿¡ ÇØ°á ´É·ÂÀÌ ¶Ù¾î³ª¸ç »ç¶÷µéÀÇ µ¿Çâ¿¡ ´ëÇØ ±â¹ÎÇÏ°í ¹Ú½ÄÇÏ´Ù. ±×·¯³ª ÀÏ»óÀûÀÌ°í ¼¼ºÎÀûÀÎ ÀÏÀ» °æ½ÃÇÏ°í ÅÂ¸¸ÇÏ±â ½±´Ù. Áï, »õ·Î¿î µµÀüÀÌ ¾ø´Â ÀÏ¿¡´Â Èï¹Ì°¡ ¾øÀ¸³ª °ü½ÉÀ» °®°í ÀÖ´Â ÀÏ¿¡´Â ´ë´ÜÇÑ ¼öÇà ´É·ÂÀ» °¡Áö°í ÀÖ´Ù. ¹ß¸í°¡, °úÇÐÀÚ, ¹®Á¦ÇØ°á»ç, Àú³Î¸®½ºÆ®, ¸¶ÄÉÆÃ, ÄÄÇ»ÅÍ ºÐ¼® µî¿¡ Å¹¿ùÇÑ ´É·ÂÀÌ ÀÖ´Ù. ¶§·Î °æÀïÀûÀÌ¸ç Çö½Çº¸´Ù´Â ÀÌ·Ð¿¡ ´õ ¹àÀº ÆíÀÌ´Ù.\r\n");

		entp();

		printf("\r\nÁøÃëÀûÀÎ, µ¶¸³ÀûÀÎ, ¼ÖÁ÷ÇÑ, Àü·«ÀûÀÎ, Ã¢ÀÇÀûÀÎ, À¶Åë¼º ÀÖ´Â, µµÀüÀûÀÎ, ºÐ¼®ÀûÀÎ, ¿µ¸®ÇÑ, ÀÚ¿øÀÌ Ç³ºÎÇÑ, ÀÇ½É½º·¯¿î, ÀÌ·ÐÀûÀÎ\r\n");
	}

	if(mbe==1 && mbs==1 && mbt==1 && mbj==1)
	{
		print_header("\r\nESTJ »ç¾÷°¡Çü\r\n");

		printf("\r\n±¸Ã¼ÀûÀÌ°í Çö½ÇÀûÀÌ°í »ç½ÇÀûÀÌ¸ç È°µ¿À» Á¶Á÷È­ÇÏ°í ÁÖµµÇØ³ª°¡´Â Áöµµ·ÂÀÌ ÀÖ´Ù.\r\n");

		printf("\r\n½ÇÁúÀûÀÌ°í Çö½Ç°¨°¢ÀÌ ¶Ù¾î³ª¸ç ÀÏÀ» Á¶Á÷ÇÏ°í °èÈ¹ÇÏ¿© ÃßÁø½ÃÅ°´Â ´É·ÂÀÌ ÀÖ´Ù. ±â°èºÐ¾ß³ª ÇàÁ¤ ºÐ¾ß¿¡ Àç´ÉÀ» °¡Á³À¸¸ç, Ã¼°èÀûÀ¸·Î »ç¾÷Ã¼³ª Á¶Á÷Ã¼¸¦ ÀÌ²ø¾î ³ª°£´Ù. Å¸°í³­ ÁöµµÀÚ·Î½á ÀÏÀÇ ¸ñÇ¥¸¦ ¼³Á¤ÇÏ°í, Áö½ÃÇÏ°í, °áÁ¤ÇÏ°í ÀÌÇàÇÏ´Â ´É·ÂÀÌ ÀÖ´Ù. °á°ú¸¦ ´«À¸·Î º¼ ¼ö ÀÖ´Â ÀÏ, Áï, »ç¾÷°¡, ÇàÁ¤°ü¸®, »ý»ê°ÇÃà µîÀÇ ºÐ¾ß¿¡¼­ ´É·ÂÀ» ¹ßÈÖÇÒ ¼ö ÀÖ´Ù. ¼Ó´Ü ¼Ó°áÇÏ´Â °æÇâ°ú Áö³ªÄ¡°Ô ¾÷¹«À§ÁÖ·Î »ç¶÷À» ´ëÇÏ´Â °æÇâÀÌ ÀÖÀ¸¹Ç·Î ÀÎ°£Áß½ÉÀÇ °¡Ä¡¿Í Å¸ÀÎÀÇ °¨Á¤À» ÃæºÐÈ÷ °í·ÁÇØ¾ß ÇÑ´Ù. ¶Ç ¹Ì·¡ÀÇ °¡´É¼ºº¸´Ù ÇöÀçÀÇ »ç½ÇÀ» Ãß±¸ÇÏ±â ¶§¹®¿¡ Çö½ÇÀû, ½Ç¿ëÀûÀÎ ¸éÀÌ °­ÇÏ´Ù.\r\n");

		estj();

		printf("\r\n³í¸®ÀûÀÎ, °áÁ¤ÀûÀÎ, Ã¼°èÀûÀÎ, È¿À²ÀûÀÎ, °´°üÀûÀÎ, ½ÇÁ¦ÀûÀÎ, Á¶Á÷È­µÈ, ºñ°³ÀÎÀûÀÎ, Ã¥ÀÓÁú ¼ö ÀÖ´Â, ±¸Á¶È­µÈ, ¼º½ÇÇÑ, Áöµµ·ÂÀÌ ÀÖ´Â\r\n");
	}

	if(mbe==1 && mbs==1 && mbf==1 && mbj==1)
	{
		print_header("\r\nESFJ Ä£¼±µµ¸ðÇü\r\n");

		printf("\r\n¸¶À½ÀÌ µû¶æÇÏ°í ÀÌ¾ß±âÇÏ±â ÁÁ¾ÆÇÏ°í, ¾ç½É¹Ù¸£°í ÀÎÈ­¸¦ Àß ÀÌ·é´Ù.\r\n");

		printf("\r\nµ¿Á¤½ÉÀÌ ¸¹°í ´Ù¸¥ »ç¶÷¿¡°Ô °ü½ÉÀ» ½ñ°í ÀÎÈ­¸¦ Áß½ÃÇÑ´Ù. Å¸°í³­ Çù·ÂÀÚ·Î½á µ¿·á¾Ö°¡ ¸¹°í Ä£ÀýÇÏ¸ç ´Éµ¿ÀûÀÎ ±¸¼º¿øÀÌ´Ù. ÀÌ¾ß±âÇÏ±â¸¦ Áñ±â¸ç Á¤¸®Á¤µ·À» ÀßÇÏ°í ÂüÀ»¼ºÀÌ ¸¹À¸¸ç ´Ù¸¥ »ç¶÷À» Àß µµ¿ÍÁØ´Ù. »ç¶÷À» ´Ù·ç°í Çàµ¿À» ¿ä±¸ÇÏ´Â ºÐ¾ß, ¿¹¸¦ µé¸é, ±³Á÷, ¼ºÁ÷, ÆÇ¸Å Æ¯È÷ µ¿Á¤½ÉÀ» ÇÊ¿ä·ÎÇÏ´Â °£È£³ª ÀÇ·á ºÐ¾ß¿¡ ÀûÇÕÇÏ´Ù. ÀÏÀÌ³ª »ç¶÷µé¿¡ ´ëÇÑ ¹®Á¦¿¡ ´ëÇÏ¿© ³ÃÃ¶ÇÑ ÀÔÀåÀ» ÃëÇÏ´Â °ÍÀ» ¾î·Á¿ö ÇÑ´Ù. ¹Ý´ë ÀÇ°ß¿¡ ºÎµúÃÆÀ» ¶§³ª ÀÚ½ÅÀÇ ¿ä±¸°¡ °ÅÀý´çÇßÀ» ¶§ ¸¶À½ÀÇ »óÃ³¸¦ ¹Þ´Â´Ù.\r\n");

		esfj();

		printf("\r\n¼º½ÇÇÑ, Ãæ¼º½º·¯¿î, »ç±³ÀûÀÎ, °³ÀÎÀûÀÎ, Ã¥ÀÓÁú ¼ö ÀÖ´Â, Á¶È­·Î¿î, Çùµ¿ÀûÀÎ, ÀçÄ¡ÀÖ´Â, Ã¶ÀúÇÑ, °¨µ¿ÇÏ±â ½¬¿î, ÀüÅëÀûÀÎ, µ¿Á¤ÀûÀÎ\r\n");
	}

	if(mbe==1 && mbn==1 && mbf==1 && mbj==1)
	{
		print_header("\r\nENFJ ¾ðº¯´É¼÷Çü\r\n");

		printf("\r\nµû¶æÇÏ°í Àû±ØÀûÀÌ¸ç Ã¥ÀÓ°¨ÀÌ °­ÇÏ°í »ç±³¼ºÀÌ Ç³ºÎÇÏ°í µ¿Á¤½ÉÀÌ ¸¹´Ù.\r\n");

		printf("\r\n¹ÎÃ¸ÇÏ°í µ¿Á¤½ÉÀÌ ¸¹°í »ç±³ÀûÀÌ¸ç ÀÎÈ­¸¦ Áß¿ä½ÃÇÏ°í ÂüÀ»¼ºÀÌ ¸¹´Ù. ´Ù¸¥ »ç¶÷µéÀÇ »ý°¢ÀÌ³ª ÀÇ°ß¿¡ ÁøÁöÇÑ °ü½ÉÀ» °¡Áö°í °øµ¿¼±À» À§ÇÏ¿© ´Ù¸¥ »ç¶÷ÀÇ ÀÇ°ß¿¡ ´ëÃ¼·Î µ¿ÀÇÇÑ´Ù. ÇöÀç º¸´Ù´Â ¹Ì·¡ÀÇ °¡´É¼ºÀ» Ãß±¸ÇÏ¸ç Æí¾ÈÇÏ°í ´É¶õÇÏ°Ô °èÈ¹À» Á¦½ÃÇÏ°í Áý´ÜÀ» ÀÌ²ø¾î°¡´Â ´É·ÂÀÌ ÀÖ´Ù. »ç¶÷À» ´Ù·ç´Â ±³Á÷, ¼ºÁ÷, ½É¸® »ó´ãÄ¡·á, ¿¹¼ú, ¹®ÇÐ, ¿Ü±³, ÆÇ¸Å µî¿¡ ÀûÇÕÇÏ´Ù. ¶§·Î ´Ù¸¥ »ç¶÷µéÀÇ ÁÁÀº Á¡À» Áö³ªÄ¡°Ô ÀÌ»óÈ­ÇÏ°í ¸Í¸ñÀû Ãæ¼ºÀ» º¸ÀÌ´Â °æÇâÀÌ ÀÖÀ¸¸ç ´Ù¸¥ »ç¶÷µé¿¡ ´ëÇØ¼­µµ ÀÚ±â¿Í °°À» °ÍÀÌ¶ó°í »ý°¢ÇÏ´Â °æÇâÀÌ ÀÖ´Ù.\r\n");

		enfj();

		printf("\r\nÃæ¼º½º·¯¿î, ÀÌ»óÀûÀÎ, °³ÀÎÀûÀÎ, ¾ð¾îÀûÀÎ, Ã¥ÀÓÁú ¼ö ÀÖ´Â, Ç¥ÇöÀûÀÎ, ¿­¼ºÀûÀÎ, ¿­Á¤ÀûÀÎ, ¿Ü±³ÀûÀÎ, ¿°·ÁÇÏ´Â, ÁöÁöÀûÀÎ, ¸¶À½ÀÌ ¸Â´Â\r\n");
	}

	if(mbe==1 && mbn==1 && mbt==1 && mbj==1)
	{
		print_header("\r\nENTJ ÁöµµÀÚÇü\r\n");

		printf("\r\n¿­¼ºÀÌ ¸¹°í ¼ÖÁ÷ÇÏ°í ´ÜÈ£ÇÏ°í Áöµµ·Â°ú Åë¼Ö·ÂÀÌ ÀÖ´Ù.\r\n");

		printf("\r\nÈ°µ¿ÀûÀÌ°í ¼ÖÁ÷ÇÏ¸ç, °áÁ¤·Â°ú Åë¼Ö·ÂÀÌ ÀÖ°í, Àå±âÀû °èÈ¹°ú °Å½ÃÀû ¾È¸ñÀ» ¼±È£ÇÑ´Ù. Áö½Ä¿¡ ´ëÇÑ ¿å±¸¿Í °ü½ÉÀÌ ¸¹À¸¸ç Æ¯È÷ ÁöÀûÀÎ ÀÚ±ØÀ» ÁÖ´Â »õ·Î¿î ¾ÆÀÌµð¾î¿¡ ³ôÀº °ü½ÉÀ» °¡Á³´Ù. ÀÏÃ³¸®¿¡ ÀÖ¾î »çÀüÁØºñ¸¦ Ã¶ÀúÈ÷ ÇÏ¸ç ³í¸® ºÐ¼®ÀûÀ¸·Î °èÈ¹ÇÏ°í Á¶Á÷ÇÏ¿© Ã¼°èÀûÀ¸·Î ÃßÁøÇØ ³ª°¡´Â ÇüÀÌ´Ù. ´Ù¸¥ »ç¶÷ÀÇ ÀÇ°ß¿¡ ±Í¸¦ ±â¿ïÀÏ ÇÊ¿ä°¡ ÀÖÀ¸¸ç, ÀÚ½Å°ú Å¸ÀÎÀÇ °¨Á¤¿¡ Ãæ½ÇÇÒ ÇÊ¿ä°¡ ÀÖ´Ù. ÀÚ½ÅÀÇ ´À³¦ÀÌ³ª °¨Á¤À» ÀÎÁ¤ÇÏ°í Ç¥ÇöÇÔÀÌ Áß¿äÇÏ¸ç, ¼º±ÞÇÑ ÆÇ´ÜÀÌ³ª °á·ÐÀº ÇÇÇØ¾ß ÇÑ´Ù. ±×·¸Áö ¾ÊÀ¸¸é ´©ÀûµÈ °¨Á¤ÀÌ Å©°Ô Æø¹ßÇÒ °¡´É¼ºµµ ÀÖ´Ù.\r\n");

		entj();

		printf("\r\n³í¸®ÀûÀÎ, °áÁ¤ÀûÀÎ, °èÈ¹ÀÌ ¸¹Àº, °­ÀÎÇÑ, Àü·«ÀûÀÎ, ºñÆÇÀûÀÎ, Á¶ÀýµÈ, µµÀüÀûÀÎ, Á÷¼±ÀûÀÎ, °´°üÀûÀÎ, °øÁ¤ÇÑ, ÀÌ·ÐÀûÀÎ\r\n");
	}



	//printf("\r\n¹®Á¦ÀÇ ´ä¾ÈÀ» Q·Î¸¸ Àû¾úÀ» °æ¿ì PÀÇ °ªÀÌ 1ÀÌ ³ª¿À´Â °ÍÀº 24¹ø¹®Á¦¿¡ ÀÇÇÑ °ÍÀ¸·Î ÇÁ·Î±×·¥»ó ¿À·ù°¡ ¾øÀ½À» ¹àÈü´Ï´Ù.");
	//

	printf("\r\nÇÁ·Î±×·¥À» Á¾·áÇÕ´Ï´Ù.");


	return 0;
}
//http://mbtitest.net/sub/mbti6.php

void istj()
{
	printf("\r\nISTJ ÀÏ¹ÝÀûÀÎ Æ¯¼º");
	printf("\r\n¿À·¡µÈ Á¶Á÷À» ÁÁ¾ÆÇÑ´Ù. ºÎÇÏÁ÷¿øÀ» ºÎ¸ð¿Í ÀÚ³à°ü°è°°ÀÌ µ¹º¸·Á°í ÇÑ´Ù. ¼±ÀÔ°ßÀÌ °­ÇÏ´Ù. Ä£¼÷ÇÏÁö ¾ÊÀº Àå¼Ò¿¡ ³ª¼­±â¸¦ ÁÖÀúÇÑ´Ù. Áö³ª°í ³­ ´ÙÀ½¿¡ µûÁö´Â ÆíÀÌ´Ù. ÁÖ¾îÁø ¾÷¹«³ª Ã¥ÀÓÀ» ³¡±îÁö ¿Ï¼öÇÑ´Ù. ¿ì¸®³ª¶ó¿¡¼­´Â Àå³² °°´Ù, Àå³à°°´Ù ¶ó´Â ¼Ò¸®¸¦ Àß µè´Â´Ù. º¯È­¿¡ ÀûÀÀÀÌ ´õµð´Ù. ¿ø¸® ¿øÄ¢ÀûÀÌ´Ù. ±³ÅëÃ¼ÁõÀ» ¹Ì¸® °è»êÇØ¼­ ¾à¼Ó½Ã°£À» ÁöÅ²´Ù. ÀÌÀ¯¾øÀÌ µ¹¾Æ´Ù´ÏÁö ¾Ê´Â´Ù. ´ëÀÎ°ü°è ÆøÀÌ ÀÚ²Ù Á¼¾ÆÁö°í ´ë½Å ÇÒ ÀÏÀÌ ´Ã¾î ³­´Ù. ½Ç¼ö ÇÑ °ÍÀ» ÂüÁö ¸øÇÏ°í Áï°¢ ¼öÁ¤ÇÏ±â¸¦ ¿øÇÑ´Ù. ³²µéÀÌ ¼ÓÀ» ¸ð¸¥´Ù¶ó°í ¸»ÇÔ Æ²¿¡ ¹ÚÈù ±ÔÄ¢ÀûÀÎ ÀÏÀ» ÁÁ¾ÆÇÑ´Ù. ÈÞÀÏ¿¡µµ Áý¿¡¼­ ÁÖ·Î Áö³½´Ù. Æò¼Ò¿¡ ¸¹ÀÌ Âü´Ù°¡ Æø¹ßÇÏ¸é »ó´çÈ÷ ¹«¼·´Ù. ³í¸®Àû, ÇÕ¸®ÀûÀÌÁö ¾ÊÀ¸¸é ÀÎÁ¤ÇÏÁö ¾ÊÀ½. ¿ôÀ½ÀÌ Àû´Ù. ¹Ý´ë¼ºÇâÀ» Áö´Ñ »ç¶÷°ú Ã³À½¿¡´Â ¿ø¸¸È÷ Áö³»³ª °á±¹ ¸Ö¾îÁü Àß¸øÇß´Ù´Â °Ç ÀÎÁ¤ÇÏ¸é¼­µµ ¹Ì¾ÈÇÏ´Ù, Àß¸øÇß´Ù´Â ¸»À» Àß ¸øÇÑ´Ù. Á¤¸®Á¤µ·À» ÇØ ³õ´Â °ÍÀÌ ¿ì¼±ÀÌ´Ù. Á÷¼³ÀûÀÎ Ç¥ÇöÀ» ¸¹ÀÌ ÇÏ´Â ÆíÀÌ´Ù.");

	printf("\r\n\r\nISTJ °³¹ßÇØ¾ßÇÒ Á¡");
	printf("\r\n¾ó±¼ Ç¥Á¤ÀÌ º¯È­°¡ ¾ø¾î¼­ »ç¶÷µé Ã³À½ ´ëÇÒ ¶§ Èûµé¾î ÇÒ ¼ö ÀÖ±â ¶§¹®¿¡ ¸ÕÀú ¸»À» ºÙÀÌ°í, ¿ô´Â ¿¬½ÀÀÌ ÇÊ¿ä. Æò¼Ò¿¡ ²À ÇÊ¿äÇÑ ¸»¸¸ÇÏ±â ¶§¹®¿¡ ºÐÀ§±â¸¦ ½ä··ÇÏ°Ô ¸¸µé ¼ö ÀÖÀ¸¹Ç·Î À¯¸Ó°¡ ÇÊ¿ä. °øÈÞÀÏ¿¡ Áý¿¡ ÀÖ±âº¸´Ù´Â °¡Á·°ú ÇÔ²² ³ªµéÀÌ°¡ ÇÊ¿ä.");
	printf("\r\n");
}

void isfj()
{
	printf("\r\nISFJ ÀÏ¹ÝÀûÀÎ Æ¯¼º");
	printf("\r\nÀÚ±â ÀÇ°ßÀ» ³¡°¡Áö ÁÖÀåÇÏÁö ¸øÇÏ°í ´Ù¼ö ÀÇ°ß¿¡ µû¸£°Ô µÈ´Ù. ¿©·¯ »ç¶÷ ¾Õ¿¡¼­ ¸»ÇÏ±â Èûµé¾îÇÑ´Ù. ²ö±â ÀÖ°í ¼º½ÇÇÏ¸ç, ¾ÈÁ¤°¨ÀÌ ÀÖ´Ù. Ä¡¹Ð¼º°ú ¹Ýº¹À» ¿äÇÏ´Â ÀÏÀ» ³¡±îÁö ÇØ³ª°¡´Â ÀÎ³»·ÂÀÌ ÀÖ´Ù. º¸¼öÀûÀÌ¸ç »õ·Î¿î º¯È­¸¦ ÁÁ¾ÆÇÏÁö ¾Ê´Â´Ù. Á¶Á÷¿¡ ¾ÈÁ¤°¨À» ÁØ´Ù. ÀÚ±âÁÖÀåÀÌ °­ÇÑµ¥ ºñÇÏ¿© Ç¥ÇöÀÌ Àû¾î ¼Óº´ÀÌ ¸¹´Ù.(À§Àåº´, ½ÉÀåº´ µî). ¸¹Àº °ÍÀ» °¡½¿¿¡ ¹¯¾î µÐ´Ù. ³²µéÀº ÁÁÀ¸³ª º»ÀÎÀÌ Èûµé´Ù. ³²¿¡°Ô ÀÇÁ¸ÇÏ´Â °ÍÀ» ÁÁ¾ÆÇÑ´Ù. Çö¸ð¾çÃ³ °¨ÀÌ´Ù. ³ª¿Í Å¸ÀÎÀÇ °¨Á¤¿¡ ¹Î°¨ÇÏ´Ù. Ã¥À» ¸ñÂ÷¼­ºÎÅÍ ÀÏ±â ½ÃÀÛÇÏ¿© ³¡±îÁö ÀÐ´Â´Ù. Áý¿¡ ÀÖ´Â °ÍÀÌ ÆíÇÏ´Ù. ¹«½¼ ÀÏÀ» ÇÒ ¶§ ¸ÕÀú ÁÖº¯ Á¤¸®ºÎÅÍ ÇÑ´Ù. ¿©·µÀÌ ¸ð¿© ¶°µå´Â °Í º¸´Ù´Â 1 : 1 ´ëÈ­°¡ ÁÁ´Ù. ¸ðÇèÀ» ÇÏÁö ¾Ê°í ¾Æ´Â ±æ·Î¸¸ °£´Ù. ³²¿¡°Ô »óÃ³ ÁÙ±îºÁ ¸»Á¶½ÉÇÑ´Ù. ³²¿¡°Ô ½ÈÀº ¼Ò¸® Àß ¸øÇÏ°í ½ÈÀº ¼Ò¸®¸¦ µéÀ¸¸é »óÃ³¸¦ ¸¹ÀÌ ¹Þ´Â´Ù. ¿©·µÀÇ ´ëÈ­ ½Ã Ä§¹¬À» ÁöÅ²´Ù. ¿©Çà ½Ã ÁüÀÌ ¸¹´Ù. ¾î¸¥µéÀÌ ÁÁ¾ÆÇÏ³ª º»ÀÎÀº Èûµé´Ù. ¸º¸ç´À¸® °¨ÀÌ´Ù. °¡Á¤ÀûÀÎ ¾Æºü´Ù.");

	printf("\r\n\r\nISFJ °³¹ßÇØ¾ßÇÒ Á¡");
	printf("\r\n¼ú,´ã¹è´Â ÀüÇô µµ¿òÀÌ µÇÁö ¾ÊÀ½. ¼ú ¾È ¸Ô°í ³ë·¡¹æ°¡¼­ Å« ¼Ò¸®·Î ³ë·¡ ºÎ¸£°í ÃãÃß´Â °ÍÀÌ ÇÊ¿ä. ¿¡¾î·Îºò °°Àº È°¹ßÇÑ ¿îµ¿ÀÌ ¼º°Ý°³Á¶¿¡ ÁÁ´Ù.");
	printf("\r\n");
}

void infj()
{
	printf("\r\nINFJ ÀÏ¹ÝÀûÀÎ Æ¯¼º");
	printf("\r\n¿µ°¨·ÂÀÌ ¶Ù¾î³ª°í ±íÀÌ ÀÖ´Â ÅëÂû·ÂÀÌ ÀÖ´Ù. Çö½ÇÀÇ À¯Çà¿¡ ´ë´ÜÈ÷ µÐ°¨ÇÏ°í Çö½Ç°ú °Å¸®°¡ ¸Ö´Ù. º¸ÀÌÁö ¾Ê´Â Á¤½Å¼¼°è¸¦ Ãß±¸ÇÑ´Ù. ÀÇ¹ÌºÎ¿©. ¿Ö »ç³ª? µî¿¡ °ü½ÉÀÌ ¸¹´Ù. ÃÊ, Áß, °í, »ýµéÀÌ ¹æÈ²À» ÇÒ ¼ö°¡ ÀÖ´Ù. »ý°¢ÀÌ ¸¹¾Æ Çö½ÇÀûÀÀÀÌ ¾î·Á¿ï ¼ö ÀÖ´Ù. °°Àº ³ªÀÌ¿¡ ºñÇØ¼­ Á¶¼÷ÇØ º¸ÀÎ´Ù. ¹®Á¦ÀÇ º»ÁúÀ» »ý°¢ÇÑ´Ù. ºñÀ¯¿Í ÀºÀ¯¸¦ ÀßÇÑ´Ù. Á¾±³ÀûÀÎ ½Å³äÀÌ °­ÇÏ´Ù. ³²ÀÇ ½Ã¼±À» ³Ê¹« ÀÇ½ÄÇØ¼­ ºÒ¾ÈÇÏ´Ù. »ç¶÷°úÀÇ ±³Á¦ ½ÃÀÛÀÌ ¾î·Æ´Ù. Àâ³ä ¶§¹®¿¡ ¼ö¸é ÁöÀåÀ» °¡Á®¿Â´Ù. º»ÀÎÀÌ ÇÏ´Â ¸»À» ³²µéÀÌ Àß ¾Ë¾ÆµèÁö ¸øÇÏ´Â °æ¿ì°¡ ÀÖ´Ù. Çö½Ç°ú Å¸ÇùÀÌ Èûµé´Ù. ½ÈÀº ³»»öÀ» ¸øÇÏ¸ç ¸¶À½ÀÇ »óÃ³µµ Àß ¹Þ´Â´Ù. ³ª¼­±âº¸´Ù´Â ÇùÁ¶ÀÚ·Î Àû±ØÀûÀ¸·Î µ½´Â´Ù. ÀÚ¾Æ¿ÍÀÇ °¥µîÀÌ ¸¹´Ù. »ç¶÷¿¡ ´ëÇÑ ÅëÂû·ÂÀ» Áö³æ´Ù. ¿Ç´Ù°í È®½ÅÀÌ »ý±ä ½Å³äÀº ³¡±îÁö ¹Ð°í ³ª°£´Ù. ÀÇ¹Ì ¾ø´Ù°í ´À³¢´Â ÀÏ¿¡ \" ?\" °¡ µû¸¥´Ù. Çö½Ç¿¡¼­µµ ÀÌ»óÀ» ²Þ²Û´Ù. ±âµµ³ª ±â ¼ö·Ã¿¡ °ü½ÉÀÌ ¸¹´Ù. Á¶¿ëÈ÷ Ã¥º¸´Â °ÍÀ» ÁÁ¾Æ ÇÑ´Ù. ´Ã Á¸Àç¿¡ ´ëÇØ »ý°¢ÇØ º¸°í »ý °ú »ç ¿µÀûÀÎ ¹®Á¦¿¡ °ü½ÉÀÌ ¸¹´Ù. ¾ðÇàÀÌ °í»óÇÑ °ÍÀ» ÁÁ¾ÆÇÑ´Ù.");

	printf("\r\n\r\nINFJ °³¹ßÇØ¾ßÇÒ Á¡");
	printf("\r\nÇö½Ç°¨À» Å°¿ï ÇÊ¿ä°¡ ÀÖ´Ù.¿ô´Â ¿¬½ÀÀÌ ÇÊ¿ä (Ç×»ó ½É°¢ÇØ º¸ÀÏ ¼ö ÀÖ´Ù.).°¡½¿¿¡ ¹¯¾î µÎÁö ¸»°í Ç®¾î³»´Â °ÍÀÌ ÇÊ¿ä.");
	printf("\r\n");
}

void intj()
{
	printf("\r\nINTJ ÀÏ¹ÝÀûÀÎ Æ¯¼º");
	printf("\r\n¾ÆÁÖ ÀÌ·ÐÀûÀÌ´Ù. ¾î·Á¿î ³­°üÀº ÀÚ±ØÁ¦°¡ µÇ¸ç, Ã¢ÀÇ¼ºÀ» ¸¹ÀÌ ¿äÇÏ´Â µµÀü¿¡ ÀÀÇÏ´Â °ÍÀ» Áñ±è. ºñÈ¿À²ÀûÀ¸·Î ½Ã°£À» ²ô´Â °ÍÀ» ½È¾îÇÑ´Ù. ³î±â À§ÇØ¼­ ³ë´Â ±â¼úÀ» ¹è¿î´Ù. °ÑÀº °­ÇÏ³ª ¼ÓÀº ¾ÆÁÖ ¿©¸®´Ù. ÁÖºÎÀÎ °æ¿ì »ì¸²ÇÏ±â Èûµé¾îÇÑ´Ù. ÀÎ°£°ü°è¿¡¼­ ÀÎ°£Àû Ãø¸é ºÎÁ·À¸·Î ÀÚ±âÀÇ ´É·ÂÀ» ÀÎÁ¤¹ÞÁö ¸øÇÏ´Â °æ¿ì°¡ ÀÖÀ½. ÅëÂû·Â°ú ºÐ¼®·ÂÀ¸·Î °íÁýÀÌ ´ë´ÜÇÏ´Ù. Àç´ÉÀº ¸¹À¸³ª ½±°Ô ÀûÀÀÇÏ±â ¾î·Á¿ö ÇÒ ¼ö ÀÖ´Ù. ÇÐ±¸¿­ÀÌ °­ÇÏ´Ù. (°øºÎÇÏ±â ÁÁ¾ÆÇÑ´Ù. ¼ø¼öÇÐ¹®). ´ëÀÎ°ü°è ¿¡¼­ Â÷°©´Ù´Â ´À³¦À» ÁÙ ¼ö ÀÖ´Ù. ÇÏ°íÀÚÇÏ´Â ÀÇÁö°¡ °­ÇÏ´Ù. Àâ´ãÀ» ½Ã°£Àû ³¶ºñ·Î º»´Ù. ÀÏÇÏ´Â È¯°æ¿¡¼­ »ç¶÷À» ½±°Ô »ç±Ï´Ù. ³ë´Â ÀÚ¸®¸¦ ¾î»öÇØ ÇÑ´Ù. ÁÖ¾îÁø ÀÏ¿¡ ¼º½ÇÇÏ¸ç ³ë·ÂÇÏ´Â ÆíÀÌ´Ù. ÀÇ¹Ì°¡ ÀÖ´Â ÀÏÀº ¿­¼ºÀ¸·Î ÇÑ´Ù. È¸½ÄÀ» º°·Î ÁÁ¾ÆÇÏÁö ¾Ê´Â´Ù. ¾Õ¿¡ ³ª¼­´Â °ÍÀ» ´ë´ÜÈ÷ ½È¾îÇÑ´Ù. È¥ÀÚ¼­ °ø»óÇÏ´Â °ÍÀ» Áñ±ä´Ù. »çÀûÀÎ ¾ê±â´Â °ÅÀÇ ÇÏÁö ¾Ê´Â´Ù. ¿µÈ­¸¦ º¼ ¶§µµ ¹«¾ð°¡ Àß¸øµÈ Àå¸éÀÌ ¾ø³ª Ã£À¸¸é¼­ º¼ ¶§°¡ ÀÖ´Ù. ÇÐ»ýÀÌ ¿Ç´Ù°í ±³¼öÇÏ°íµµ ½Î¿ï ¶§°¡ ÀÖ´Ù.");

	printf("\r\n\r\nINTJ °³¹ßÇØ¾ßÇÒ Á¡");
	printf("\r\nÀÇµµÀûÀ¸·Î °¡Á· ¿À¶ôÀÌ³ª °ÔÀÓ½Ã°£À» °®´Â °ÍÀÌ ÇÊ¿ä. Å¸ÀÎÀÇ ÀÇ°ß¿¡ ´ëÇØ¼­µµ °æÃ»ÇÏ´Â ÀÚ¼¼°¡ ÇÊ¿ä. È¥ÀÚ ÀÏÇÏ±âº¸´Ù´Â ÀÇµµÀûÀ¸·Î ÆÀÀ» ÅëÇØ¼­ ÇÔ²² ÇÏ´Â °ÍÀÌ ÇÊ¿ä.");
	printf("\r\n");
}

void istp()
{
	printf("\r\nISTP ÀÏ¹ÝÀûÀÎ Æ¯¼º");
	printf("\r\n¼Òºñ¼º °æÇâÀÌ ¸¹´Ù. ¸¶À½¿¡ ¾ø´Â ¾ê±â¸¦ »ó´ë¹æ ±âºÐ ¶§¹®¿¡ ÇÏÁö ¾Ê´Â´Ù. ÀÏ¹ÝÀûÀ¸·Î Á¶¿ëÇÑ ÆíÀÌ³ª ÇÊ¿ä¿¡ µû¶ó »ç±³ÀûÀÌ´Ù. ¼ÕÀçÁÖ°¡ ¶Ù¾î ³ª´Ù. °³ÀÎÁÖÀÇÀû ¼ºÇâÀÌ °­ÇÏ´Ù. Ãæµ¿¿¡ µû¶ó Çàµ¿ÇÏ±â ¶§¹®¿¡ ¾ðÁ¦¶óµµ ÀÏÀÚ¸®¸¦ ¹ÚÂ÷°í ¶°³¯ ¼ö ÀÖ´Ù. Æ²¿¡ ¹ÚÈù »ýÈ°À» ½È¾îÇÑ´Ù. °íÁýÀÌ ÀÖ°í ÁÖÀåÀÌ °­ÇÏ´Ù. ¸»ÀÌ ¾ø°í ³»»öÀ» ¾Ê´Â´Ù. °´°üÀû ¿ø¸®¿¡ °ü½ÉÀÌ ¸¹´Ù. µµ±¸¸¦ ´Ù·ç´Âµ¥ °ü½ÉÀÌ ÀÖ´Ù. ´À³¦°ú °¨Á¤, Å¸ÀÎ¿¡ ´ëÇÑ ¸¶À½À» Ç¥ÇöÇÏ±â ¾î·Á¿öÇÑ´Ù. Á¤ÀÇ°¨ÀÌ ÀÖÀ¸³ª Á÷¼³ÀûÀÎ ¸»·Î Å¸ÀÎÀÇ °¨Á¤À» »óÇÏ°Ô ÇÒ ¼ö ÀÖ´Ù. Ãæµ¿¿¡ µû¶ó Çàµ¿ÇÏ±â ¶§¹®¿¡ ¾ðÁ¦¶óµµ ÀÏÀÚ¸®¸¦ ¹ÚÂ÷°í ¶°³¯ ¼ö ÀÖ´Ù. Á¤¹ÐÀ» ¿äÇÏ´Â ÀÏÀ» Àß ÇØ³½´Ù. Å¸ÀÎÀÇ ÀÏ¿¡ ¹«°ü½ÉÇÑ ÆíÀÌ´Ù. ¸ðÇè°ú ½º¸±À» Áñ±ä´Ù. °ü½ÉºÐ¾ß°¡ ¾Æ´Ï¸é Ã³´Ù º¸Áöµµ ¾Ê´Â´Ù. »ý°¢Àº Àû±ØÀûÀÎµ¥ Çàµ¿Àº ¼Ò±ØÀû. ³ë·ÂÀ» Àý¾àÇÏ¸é¼­(°ÔÀ¸¸£´Ù´Â ¼Ò¸®¸¦ µéÀ½) ÀÏÀÇ ´É·üÀ» ³ôÀÎ´Ù."); 

	printf("\r\n\r\nISTP °³¹ßÇØ¾ßÇÒ Á¡");
	printf("\r\nÅ¸ÀÎ¿¡ ´ëÇØ¼­ ÀÇµµÀûÀ¸·Î °ü½ÉÀ» °®´Â °ÍÀÌ ÇÊ¿ä. È­°¡ ³­ ¾ó±¼ÀÌ Ã¢¹éÇÏ°Ô º¸ÀÏ ¼ö ÀÖÀ¸¹Ç·Î ¾ó±¼ °ü¸®¿¡ ½Å°æ ¾²´Â °ÍÀÌ ÇÊ¿ä. ÇÔ²² ÇÏ´Â ÀÏÀ»ÇÏ´Â ¿À¶ô½Ã°£À» °®´Â °ÍÀÌ ÇÊ¿ä.");
	printf("\r\n");
}

void isfp()
{
	printf("\r\nISFP ÀÏ¹ÝÀûÀÎ Æ¯¼º"); 
	printf("\r\n»îÀÇ ÇöÀç¸¦ Áñ±â´Â »ç¶÷ÀÌ´Ù. ´Ù¸¥ »ç¶÷ÀÇ ºÎÅ¹À» °ÅÀýÇÏ±â ¾î·Á¿öÇÑ´Ù. ÀÚ½ÅÀ» ³»¼¼¿ìÁö ¾Ê´Â´Ù. ÀÚ±â ÀÚ¶ûÀÌ ¾ø´Ù. ¸¶À½ÀÌ ¼øÇÏ°í µû¶æÇÏ¸ç Á¤ÀÌ ¸¹´Ù. ³²À» Àß ¹Ï°í ÀÇ½ÉÇÏÁö ¾Ê´Â´Ù. »ç±â ´ç ÇÒ È®·üÀÌ ³ô´Ù. ´©±¸ÇÏ°í³ª ¾î¶² »çÈ¸¿¡¼­³ª ¸ÂÃß¾î °¡¸ç »ì ¼ö ÀÖ´Â »ç¶÷. ±ÔÄ¢ Æ²¿¡ ¹­ÀÌ´Â °ÍÀ» ½È¾îÇÑ´Ù. ÃßÁø·Â, °áÁ¤·ÂÀÌ ºÎÁ·ÇÏ´Ù. Á¶¿ëÈ÷ ÀÖ´Ù°¡ ¹«´ë¿¡¼­ ³¢¸¦ ¹ßÈÖÇÑ´Ù. - ¸ôÀÔÀÌ Æ¯Â¡. °á´Ü·ÂÀÌ ºÎÁ·ÇÏ°í ²÷°í ¸Î´Â ¸ÀÀÌ ¾ø´Ù. ÀÚ¿¬ÀûÀÎ °Í, ¸ñ°¡ÀûÀÎ °Í, Àü¿øÀûÀÎ °ÍÀ» °¥±¸. »ý°¢Àº ¸¹°í Çàµ¿Àº ºÎÁ·ÇÏ´Ù. Áö³ªÄ¡°Ô Å¸ÀÎÀ» ¹è·ÁÇÑ´Ù. ´ëÁß ¾Õ¿¡ ¼±¶æ ³ª¼­Áö ¸øÇÑ´Ù. ½Î¿ï ¶§ °¨Á¤ÀÌ ¾Õ¼­ ³í¸®ÀûÀÌÁö ¸øÇÏ´Ù. °èÈ¹¼ºÀÌ ¾ø´Ù. ³²¿¡°Ô ½ÈÀº ¼Ò¸® ¸øÇÏ°í ¼ÓÀ¸·Î »èÀÎ´Ù. Å¸ÀÎÀ» ¹«Á¶°Ç ÀÌÇØÇØ ÁÖ°í ÀÚ±â ÀÇ°ß°ú´Â »ó°ü¾øÀÌ µû¶ó°¡ ÁØ´Ù. Áñ±â´Â °Í¿¡ ´ëÇÑ È£±â½ÉÀÌ ¸¹´Ù. ¿¹¼úÀûÀÎ ±âÁúÀÌ ÀÖ´Ù. (¿¬±Ø¹è¿ì, °¡¼ö, ÇÇ¾Æ´Ï½ºÆ® µî). Æ÷¿ë·Â°ú ÀÌÇØ·ÂÀÌ ¸¹´Ù. °æÀïÇÏ´Â ºÐÀ§±âº¸´Ù´Â Æí¾ÈÇÑ ºÐÀ§±â¿¡¼­ ´É·ÂÀ» ¹ßÈÖÇÑ´Ù. Á¶Á÷¿¡¼­ ½Ã°£ÀÌ ¿À·¡ Áö³ª¾ß ÀÎÁ¤À» ¹Þ´Â´Ù. µüµüÇÏ°í »ç¹«ÀûÀÎ »ç¶÷À» ½È¾îÇÑ´Ù.");

	printf("\r\n\r\nISFP °³¹ßÇØ¾ßÇÒ Á¡");
	printf("\r\nÀû±ØÀûÀÎ »ç°í¿Í Àû±ØÀûÀÌ Çàµ¿ÀÌ ÇÊ¿ä. ÀÚ±â¸¦ µå·¯³»´Â ¿¬½ÀÀÌ ÇÊ¿ä. Áñ°Å¿ò¿¡ ´ëÇÑ È£±â½ÉÀ» ÀÚÁ¦ÇÏ°í ¿ì¼± ¼øÀ§¿¡ ¸ÂÃç ÀÏÇÏ´Â ¿¬½ÀÀÌ ÇÊ¿ä.");
	printf("\r\n");
}

void infp()
{
	printf("\r\nINFP ÀÏ¹ÝÀûÀÎ Æ¯¼º");
	printf("\r\nÇö½Ç°¨°¢ÀÌ µÐÇÏ´Ù. - °¡°èºÎ¸¦ ¼Ò¼³·Î ¾´´Ù. ¸ù»ó°¡Àû ±âÁúÀÌ ¸¹´Ù. ÀÎ°£°ú Á¾±³(Á¤½Å¼¼°è)¿¡ °ü½ÉÀÌ ¸¹´Ù. ºÐÀ§±â¸¦ Àß Åº´Ù. (ºÐÀ§±â°¡ ÁÁÀ¸¸é ³¡±îÁö ³²´Â´Ù). ¾Æ¸§´Ù¿ò°ú ÃßÇÔ, ¼± °ú ¾Ç, µµ´ö°ú ºñµµ´ö¿¡ ¹Î°¨ÇÏ°Ô ¹ÝÀÀÇÑ´Ù. ½Å³äÀÌ ¶Ñ·ÇÇÏ¿© °ÑÀ¸·Î´Â ÁÖÀåÀ» ¾ÈÇØµµ ¼ÓÀ¸·Î´Â ¿­Á¤ÀÌ ÀÖ´Ù. °¡Ä¡ ÀÖ´Â ÀÏ¿¡´Â »ý¸íµµ ¹ÙÄ£´Ù. ³»¸éÀÇ ¼¼°è¸¦ Ãß±¸ÇÏ¿© ´Ã ¹«¾ùÀ» °¥±¸ÇÏ°í Ãß±¸ÇØ ³ª°£´Ù. ±ÔÄ¢À» ¸ö¼­¸® Ä¡µí ½È¾îÇÏ¸ç ¹Ýº¹µÇ´Â ÀÏ»óÀûÀÎ »ýÈ°À» ½È¾îÇÑ´Ù. ¸Ã°ÜÁø ÀÏ¿¡ ´ëÇØ¼­´Â Áö³ªÄ¡°Ô ¿Ïº®ÁÖÀÇ ÀûÀ¸·Î ³ª°¡´Â °æÇâÀÌ ÀÖ´Ù. ÁïÈïÀûÀÌ¸ç º¯È­°¡ ºñ½ÁÇÏ´Ù. ³»¸éÀÇ °¥µîÀÌ ½ÉÇÏ¿© °¨Á¤ÀÇ ±âº¹ÀÌ ½ÉÇÏ´Ù. ÀÏÀ» Àß ¹úÀÌ³ª ¸¶·ç¸®°¡ ¼­Åø´Ù. ¿©ÇàÀ» ÁÁ¾ÆÇÏ°í, ¿µÈ­, À½¾Ç, Ã¥À» ÁÁ¾ÆÇÑ´Ù. °èÀýÀÇ º¯È­¿¡ ¹Î°¨ÇÏ´Ù. »ó´ë¹æÀÇ ¸»¿¡ ¹Î°¨ÇÏ´Ù. ¾î´À ºÎºÐ¿¡ ´ëÇØ¼­´Â À¶Åë¼ºÀÌ ¾ÆÁÖ ¾ø´Â ÆíÀÌ´Ù. »ó´ë¹æÀ» ¹è·ÁÇØ¼­ ºùºù µ¹·Á¼­ ÀºÀ¯ÀûÀ¸·Î ÀÇ»ç Ç¥ÇöÇÑ´Ù. ¸¾¿¡ ¸Â´Â »ç¶÷ ¸¸³ª¸é ¹ãÀ» »õ¿ö°¡¸ç ÀÌ¾ß±âÇÑ´Ù. ³í¸®ÀûÀÌÁö ¸øÇÏ°í °¨Á¤ÀûÀÌ´Ù. °¨Á¤ Á¶ÀýÀÌ ¹Ì¼º¼÷ÇÏ´Ù. ¾ÆÀÌµð¾î°¡ ¸¹À¸³ª ½ÇÇà¿¡ Àß ¿Å±âÁö ¸øÇÑ´Ù.");

	printf("\r\n\r\nINFP °³¹ßÇØ¾ßÇÒ Á¡");
	printf("\r\nÇö½Ç°ú ÀÌ»óÀ» ±¸ºÐÇÏ´Â ´É·ÂÀÌ ÇÊ¿ä. ´ëÀÎ°ü°è¿¡¼­ °¡Ä¡°ü¿¡ ¸ÂÁö ¾Ê´Â °ÍÀÌ¶óµµ À¶Åë¼ºÀ» º¸ÀÏ ÇÊ¿ä°¡ ÀÖ´Ù. ²ÙÁØÇÔÀ» ±â¸£±â À§ÇØ¼­ ¾ÆÁÖ ÀÛÀº ÀÏºÎÅÍ ÅëÁ¦·ÂÀ» °®´Â °ÍÀÌ ÇÊ¿ä.");
	printf("\r\n");
}

void intp()
{
	printf("\r\nINTP ÀÏ¹ÝÀûÀÎ Æ¯¼º");
	printf("\r\nÇàµ¿ÇÏ±â º¸´Ù Ã¥À» ÅëÇØ¼­ ¹è¿î´Ù. (Ã¥Áßµ¶). ³ôÀº Á÷°ü·ÂÀ¸·Î ÅëÂûÇÏ´Â Àç´É°ú ÁöÀû°ü½ÉÀÌ ¸¹´Ù. Á¶¿ëÇÏ°í ¸»ÀÌ ¾øÀ¸³ª ÀÚ±âÀÇ °ü½É ºÐ¾ß¿¡¼­´Â ¸»À» ¸¹ÀÌ ÇÑ´Ù. Á¤¼­Ç¥ÇöÀÌ º°·Î ¾ø¾î Ä£ÇØÁö±â Àü¿¡´Â ÀÌÇØÇÏ±â ¾î·Æ´Ù. ±âºÐÀÌ³ª °¨Á¤µµ »ý°¢À» ÅëÇØ¼­ ÇÑ´Ù. Áö³ªÄ¡°Ô ÁöÀûÀÌ°í Ãß»óÀûÀÌ¸ç ¼³¸íÀÌ ³Ê¹« ÀÌ·ÐÀûÀÌ´Ù. È²´ç¹«°ÔÇÑ °ø»óÀ» Àß ÇÑ´Ù. ºñÇö½ÇÀûÀÌ¸ç ºñ¾àÀÌ ½ÉÇÏ´Ù. Å¸ÀÎ¿¡°Ô º°·Î °ü½ÉÀÌ ¾ø´Ù. »ý°¢Àº Ã¢ÀÇÀûÀÎµ¥ ½ÇÃµÀÌ ºÎÁ·ÇÏ´Ù. Á¶Á÷ÀÌ³ª ´Ü°è, °èÅë µî¿¡ ¾àÇÏ´Ù. Ãæµ¿ÀûÀÌ´Ù. ¸Å´º¾ó º¸±â ½È¾îÇÑ´Ù. Á¤ÀåÀ» ½È¾îÇÑ´Ù. ÆÐ¼Ç°¨°¢ÀÌ µÐÇÏ´Ù. µå¶ó¸¶, ÇÑ±¹¿µÈ­ Àß ¾È º»´Ù. »·ÇÑ ÀÌ¾ß±â³ª ¼­·ÐÀÌ ±ä °Í Âü±â ¾î·Æ´Ù. ÁÖ°üÀÌ ¶Ñ·ÇÇÏ°í ÀÚ½ÅÀº ÇÕ¸®ÀûÀÌ¶ó »ý°¢ÇÑ´Ù. ²À ÇÊ¿äÇÑ °Í ¾Æ´Ï¸é ÀØ¾î¹ö¸°´Ù. (°Ç¸ÁÁõ). ¸Ó¸´¼Ó¿¡ ÀÖ´Ù°¡ ¸·ÆÇ¿¡ ÈÄ´Ù´Ú ÀÏ Ã³¸®¸¦ ÇÑ´Ù. ÇÑ³¢ ¶¼¿ì¸é µÈ´Ù. ³²µé ÁÁ¾ÆÇÏ´Â ¿¬¿¹ÀÎ, ¾Ç¼¼»ç¸® µî¿¡ °ü½É ¾ø´Ù. Ãß¸®¼Ò¼³ ÁÁ¾ÆÇÑ´Ù. Àâ´ã ¸ðÀÓ ÈÄ¿¡´Â Çã¹«°¨À» ´À³¤´Ù. Ä£ÇÑ Ä£±¸¶óµµ ÀÏ ¾øÀ¸¸é ¿¬¶ô ¾È´Ù. °ø»ó°ú »ó»ó¼Ó¿¡ ÀÖÀ» ¶§°¡ ¸¹´Ù."); 

	printf("\r\n\r\nINTP °³¹ßÇØ¾ßÇÒ Á¡");
	printf("\r\nÅ¸ÀÎÀÇ ³ë·ÂÀ» ÀÎÁ¤ÇÏ´Â ÅÂµµ ÇÊ¿ä. ÆÀÀ¸·Î ÇÏ´Â ÀÏ¿¡ Àû±ØÀûÀ¸·Î ¶Ù¾îµé ÇÊ¿ä°¡ ÀÖ´Ù. »ý°¢º¸´Ù Á¾ÀÌ¿¡ Àû¾îº¸°í Á¤¸®ÇØ º¸´Â ¿¬½ÀÀÌ ÇÊ¿ä.");
	printf("\r\n");
}

void estp()
{
	printf("\r\nESTP ÀÏ¹ÝÀûÀÎ Æ¯¼º");
	printf("\r\nÁ¤º¸ÅëÀÌ´Ù. ³»±â¸¦ ÁÁ¾ÆÇÑ´Ù. »îÀ» Áñ±â¸ç »ê´Ù. »ç¶÷ÀÌ³ª, »ç°Ç¿¡ ´ëÇØ ¼±ÀÔ°¨ÀÌ ¾ø°í °³¹æÀûÀÌ´Ù. ½º¸±À» ÁÁ¾ÆÇÑ´Ù. Ã¥À» ÅëÇØ¼­ º¸´Ù´Â Á÷Á¢ °æÇèÀ» ¼±È£ÇÑ´Ù. ÁïÈïÀûÀÎ Çàµ¿¿¡ ÀÇÁ¸. ÀÏÀ» ¸¶Áö¸·¿¡ Æø¹ßÀûÀ¸·Î ÇÑ´Ù. Èï¹Ì À§ÁÖÀÇ ¿å±¸°¡ ¸¹´Ù. (°æÀï, °ÔÀÓ). ¹­¿© ÀÖÁö ¾ÊÀº ÀÚÀ¯·Î¿î »óÅÂÀÏ ¶§ ÀÏÀÇ ´É·üÀÌ ¿À¸¥´Ù. ÀÏ¹Ý º¸º´º¸´Ù Æ¯°ø´ë Ã¼Áú. Çö½ÇÀûÀÎ °è»êÀÌ ´«¿¡ º¸ÀÎ´Ù. Á¦¸Ú´ë·Î ÀÚÀ¯ºÐ¹æ. Çö ¿ì¸®³ª¶óÀÇ ÇÐ±³ ºÐÀ§±â¿¡ ÀûÀÀÇÏ±â ¾î·Á¿òÀ» ´À³¤´Ù. ÀÚ½Å°¨ÀÌ Ç×»ó Ã¶Ã¶ ³ÑÄ£´Ù. °øºÎº¸´Ù´Â ½ºÆ÷Ã÷¿Í °°Àº È°µ¿ÀûÀÎ °Í¿¡ ÁýÁß·ÂÀÌ ÀÖ´Ù. ÁÖÀ§ÀÇ »ç¶÷ÀÌ³ª ÀÏ¾î³ª´Â ÀÏ¿¡ °ü½ÉÀÌ ¸¹´Ù. Á¶±Ý ±í°Ô »ý°¢ÇÏ´Â °ÍÀ» ½È¾îÇÏ´Â °æÇâÀÌ ÀÖ´Ù. ÀÚ±ØÀûÀÎ °ÍÀ» ÁÁ¾ÆÇÑ´Ù. ¿À´Ã ÇÒ ÀÏÀ» ³»ÀÏ·Î ¹Ì·é´Ù. ¼ºÃë¿åÀÌ °­ÇÏ´Ù. »õ·Î¿î °Í¿¡ ´ëÇÑ µµÀü ¿å±¸°¡ °­ÇÔ. Å¸ÀÎ¿¡°Ô ¼±ÀÔ°ßÀÌ º°·Î ¾ø°í °³¹æÀûÀÌ´Ù. ÀÏÀ» ÇÑ²¨¹ø¿¡ Ã³¸®ÇÑ´Ù. °¨Á¤À» ÀÖ´Â ±×´ë·Î Ç¥ÇöÇÏ¿© »ó´ë¹æÀÌ »óÃ³¸¦ ¹ÞÀ» ¼ö ÀÖ´Ù. µûÁö°í ºÐ¼®ÇÏ°í ÀÌÇØ°¡ µÇ¾ß ¼ö±àÇÑ´Ù.");

	printf("\r\n\r\nESTP °³¹ßÇØ¾ßÇÒ Á¡");
	printf("\r\nÁ÷¼³ÀûÀÎ ¸»º¸´Ù´Â ¹è·ÁÇÏ´Â ¸»ÀÌ ÇÊ¿äÇÏ´Ù. Á¤½Å¼¼°è¿¡ ´ëÇØ¼­ °ü½ÉÀ» °®À» ÇÊ¿ä°¡ ÀÖ´Ù. ÀÏÀ» ¹ú¸®±â º¸´Ù´Â ¸¶¹«¸® Áþ´Âµ¥ ³ë·ÂÀ» ±â¿ïÀÏ ÇÊ¿ä°¡ ÀÖ´Ù.");
	printf("\r\n");
}

void esfp()
{
	printf("\r\nESFP ÀÏ¹ÝÀûÀÎ Æ¯¼º");
	printf("\r\nÅ¸ÀÎÀ» ±â»Ú°Ô ÇØÁÙ ±ôÂ¦¼î¸¦ ÁØºñÇÏ¸é¼­ Áñ°Å¿öÇÑ´Ù. ³» ÀÚ½ÅÀÇ ÀÌ¾ß±â¸¦ »ó´ë¹æÀ» °¡¸®Áö ¾Ê°í ¾Æ¹«¿¡°Ô³ª Àß ÅÐ¾î³õ´Â´Ù. ÀÚ½Å¿¡°Ô ³Ê±×·´°í ´Ù¸¥ ´ë¿ÜÀûÀÎ Ã¥ÀÓ°¨ÀÌ µû¸£´Â ÀÏÀº ¿­¼ºÀ» °¡Áö°í ÀÏÇÑ´Ù. ¹«°èÈ¹ÀûÀÌ°í Ãæµ¿ÀûÀÎ ¿©ÇàÀ» Áñ±ä´Ù. ¹ßµî¿¡ ºÒÀÌ ¶³¾îÁ®¾ß Çàµ¿¿¡ ¿Å±ä´Ù. Á÷¼³ÀûÀ¸·Î ¸»ÇÏ°í ÈïºÐÀ» ÀßÇÏ¸ç ¸ñ¼Ò¸®°¡ Å©´Ù. Áý¿¡ ÀÖÀ¸¸é ¹«±â·ÂÇÑ ´À³¦ÀÌ µé°í ½¬´Â ³¯Àº °ÅÀÇ ¿ÜÃâ. Ã»¼Ò³ª »¡·¡¸¦ Á¦½Ã°£¿¡ ÇÏÁö ¾Ê°í ¸ô¾Æ¼­ ²À ÇØ¾ß ÇÒ ¶§ ÇÑ´Ù. ¡®³»ÀÏÀº ³»ÀÏÀÇ ÅÂ¾çÀÌ ¶á´Ù ¡¯. ½È°í ÁÁÀº »ç¶÷ÀÌ ºÐ¸íÇÏ¸ç Ç¥Á¤¿¡ ³ªÅ¸³­´Ù. Á¤ÀÌ ¸¹°í °Ç¸ÁÁõÀÌ ½ÉÇÏ´Ù. °ÅÀýÀ» Àß ¸øÇÑ´Ù. Àß ¸Ô°í, Àß ÀÚ°í, »ý°¢ÀÌ ´Ü¼øÇÏ´Ù. °í¹ÎÇÏ´Ù°¡ ±×³É Àáµç´Ù. È¥ÀÚ ÀÖ´Â °ÍÀ» Èûµé¾îÇÑ´Ù. Á¶Á÷»ýÈ° º¸´Ù´Â ÀÚÀ¯·Î¿ï ¶§ ´É·ÂÀ» ¹ßÈÖÇÑ´Ù. Á¤ÀÛ ÇÏ°í ½ÍÀº ¸»Àº ¸øÇÑ´Ù. ±âºÐÆÄ, µ· ÀÖÀ¸¸é ÀÏ´Ü ¾²°í º»´Ù. ÀÌ¾ß±â ÇÒ ¶§ ¿äÁ¡°ú ´õºÒ¾î ºÎ¿¬ ¼³¸íÀ» ¸¹ÀÌ µ¡ºÙÀÎ´Ù. ±Í°¡ ¾ã´Ù.( »óÈ²¿¡ µû¶ó, ÁÖÀ§ »ç¶÷µéÀÇ ¹ÝÀÀ¿¡ µû¶ó Àß º¯ÇÑ´Ù.). Æ²¿¡ ¹ÚÈù °Í ½È¾îÇÏ°í, °èÈ¹¿¡ µû¶ó ÇÏ´Â °Í Èûµé¾îÇÑ´Ù.");

	printf("\r\n\r\nESFP °³¹ßÇØ¾ßÇÒ Á¡");
	printf("\r\nÀÇ²÷°í ¸Î´Â °ÍÀ» °³¹ß ÇÒ ÇÊ¿ä°¡ ÀÖ´Ù. °èÈ¹¼º ÀÖ´Â ÀÏÃ³¸® ¹æ½ÄÀÌ ÇÊ¿ä. °¡²û È¥ÀÚ¼­ ÀÚ±âÀÇ ³»¸éÀ» ¼ºÂûÀÇ ½Ã°£ÀÌ ÇÊ¿ä.");
	printf("\r\n");
}

void enfp()
{
	printf("\r\nENFP ÀÏ¹ÝÀûÀÎ Æ¯¼º");
	printf("\r\n°¨Á¤ÀÌ ¾ó±¼¿¡ Àß µé¾î ³­´Ù. »õ·Î¿î ½Ãµµ¸¦ ÁÁ¾ÆÇÑ´Ù. °èÈ¹ÇÏ±âº¸´Ù´Â ±×¶§±×¶§ ÀÏÀ» Ã³¸®ÇÏ´Â ÆíÀÌ´Ù. »õ·Î¿î »ç¶÷ ¸¸³ª±â¸¦ ÁÁ¾ÆÇÑ´Ù. °¨µ¿À» ÀßÇÏ°í ´«¹°µµ Àß Èê¸°´Ù. µ· °³³äÀÌ Èñ¹ÚÇÏ´Ù. µ·À» ¸ðÀ¸±â Èûµé ¼ö ÀÖ´Ù. °¨Á¤ÀÇ ±âº¹ÀÌ ½ÉÇÏ´Ù. °æÀïÀÇ½ÄÀÌ ¾ø´Ù. »ó´ë¹æÀÇ ¸»¿¡ ¹Î°¨ÇÏ³ª ±âºÐÀÌ ³ª»ÚÁö ¾ÊÀº Ã´ ÇÑ´Ù. ³»¸é¿¡ ¿­Á¤À» Áö³æ´Ù. À§±â ´ëÃ³´É·ÂÀÌ ¶Ù¾î³­´Ù. »ç¶÷À» ±â»Ú°Ô ÇØÁÖ´Â Å¸°í³­ ´É·ÂÀÌ ÀÖ´Ù. Çà»ç³ª ÀÏÀ» Àß ÁÖ¼±ÇÑ´Ù. ³î´Ù°¡µµ ¸ôÀÔÀÌ ¾ÈµÇ°í Áö±Ý ¹«¾ùÇÏ°í ÀÖ´Â °Ç°¡? ¶ó´Â »ý°¢ÀÌ µé ¶§°¡ ÀÖ´Ù. ¸Ú ³»´Â °ÍÀ» ÁÁ¾ÆÇÑ´Ù. ¾çº¸¸¦ ÀßÇÏ°í ½Î¿òÀ» ÇÒ·Á¸é ½ÉÀåºÎÅÍ ¶Ú´Ù. ´Ü¼ø¾Ï±â¿¡ ¾àÇÏ´Ù. ÀÎ»ýÀ» Áñ°Ì°Ô »ì·Á°í ÇÑ´Ù. ¼±»ý´ÔÀÌ ¸¶À½¿¡ µé¸é ÇÏ±â ½ÈÀº °ú¸ñµµ ÀßÇÑ´Ù. ÇÏ±â ½ÈÀº °Í¿¡ ´ëÇÑ ÀÎ³»·ÂÀÌ ºÎÁ·ÇÏ´Ù. ÁÁ¾ÆÇÏ´Â »ç¶÷°ú ½È¾îÇÏ´Â »ç¶÷ÀÇ ±¸º°ÀÌ ½ÉÇÏ´Ù. ¹Ýº¹ÀûÀÎ ÀÏ»óÀ» Èûµé¾î ÇÑ´Ù. ºÐÀ§±â¸¦ Àß ¶ç¿î ÈÄ¿¡ ÀÚ±â´Â ºüÁø´Ù.");

	printf("\r\n\r\nENFP °³¹ßÇØ¾ßÇÒ Á¡");
	printf("\r\n°¨Á¤ÀÇ ±âº¹À» ÀÌ°Ü³»±â À§ÇØ¼­ ²ÙÁØÇÑ ¿îµ¿ÀÌ ÇÊ¿ä. ÁÁ¾ÆÇÏ´Â ÀÏ¸¸ ÇÏ±âº¸´Ù ¿ì¼±¼øÀ§¿¡ ¸ÂÃß¾î¼­ ÇÏ´Â °ÍÀÌ ÇÊ¿ä. »ó´ë¹æÀÇ ¸»¿¡ ´ëÇØ¼­ °´°üÈ­ ÀÛ¾÷ÀÌ ÇÊ¿ä. ±ÔÄ¢ÀûÀÌ°í Ã¼°èÀûÀÏ ÇÊ¿ä°¡ ÀÖ´Ù. Çö½Ç¿¡ Ãæ½ÇÇØ¾ß ÇÑ´Ù. ÀÎ³»½ÉÀ» ±æ·¯¾ß ÇÑ´Ù.");
	printf("\r\n");
}

void entp()
{
	printf("\r\nENTP ÀÏ¹ÝÀûÀÎ Æ¯¼º");
	printf("\r\nÇÑ¹øµéÀº ¾ê±â¸¦ ¶Ç µè´Â °Ç ½È¾îÇÑ´Ù. 5´ë¾ç 6´ëÁÖ°¡ È°µ¿ ¹«´ë ÀÌ°Ç ½È¾îÇÑ´Ù. ¿©ÀÚÀÎ °æ¿ì Ä¡¸¶¸¦ µÎ¸¥ ³²ÀÚ °°´Ù. º¹ÀâÇÑ ¹®Á¦ÀÏ¼ö·Ï ½±°Ô ÇØ°áÇÑ´Ù. ¸¶À½¸¸ ¸ÔÀ¸¸é ¸øÇÏ´Â °ÍÀÌ ¾ø´Ù. Àü°øÀÌ ¿©·¯ °¡ÁöÀÌ´Ù. ±²ÀåÈ÷ ´ÙÀç´Ù´ÉÇÏ°í ´É·ÂÀÌ ÀÖ´Ù. ´Ü¾î ÇÏ³ª·Î 2½Ã°£µµ ÀÌ¾ß±â ÇÑ´Ù. ÃÊ, Áß, °íµîÇÐ±³ ÀÇ ±ÔÄ¢»ýÈ°ÀÌ Èûµé ¼ö ÀÖ´Ù. ÀÏ»óÀûÀÎ ÀÏ¿¡ ½±°Ô ½ÈÁõÀ» ´À³¤´Ù. 007 Á¦ÀÓ½º º»µåÇü ÀÌ´Ù. ÀÎ°£°ü°è°¡ ÀÚÀ¯·Ó´Ù. ¶È°°Àº °­ÀÇ¸¦ ¹Ýº¹ ¸øÇÑ´Ù. °ü½ÉºÐ¾ß´Â ´ë´ÜÈ÷ ¹Ú½Ä °ü½É ¾ø´Â ºÐ¾ß´Â ´ë´ÜÈ÷ ¹«½Ä. °æÀï½ÉÀÌ ¸¹´Ù. ÀÏ»óÀûÀÌ°í ¹Ýº¹µÇ´Â ÀÏÀº Áö·çÇÏ°í Èûµé¾î ÇÑ´Ù. Ç×»ó »õ·Î¿î °ÍÀ» Ãß±¸ÇÑ´Ù. ºü¶ß¸®°Å³ª »©¸Ô´Â ÀÏÀÌ ¸¹´Ù. ´Ù¸¥ ´©±¸ÀÇ ±ÇÀ¯³ª Âü°ßÀº Áú»öÀÌ´Ù. ÀÚ±âÀÇ ÆÇ´Ü¿¡ µû¶ó Çàµ¿ÇÑ´Ù. ²ö±â ÀÖ°Ô ÇÑ °¡Áö ÀÏ¿¡ ¸ôµÎÇÏÁö ¸øÇÑ´Ù. ¸»À» ³ª¿À´Â ´ë·Î ¸·ÇÒ ¼ö ÀÖ´Ù. Ã³À½ º¸´Â »ç¶÷°úµµ ±Ý¹æ Ä£ÇØÁø´Ù. ÆÈ¹æ¹ÌÀÎ, ´«½ä¹Ì°¡ ÁÁ´Ù. ±æ°Ô ¼³¸íÇÏ´Â °Ç Â¥Áõ³­´Ù.");

	printf("\r\n\r\nENTP °³¹ßÇØ¾ßÇÒ Á¡");
	printf("\r\n¸»À» ÇÒ ¶§ Àú »ç¶÷ÀÌ ¾î¶»°Ô ´À³¥±î ¸ÕÀú »ý°¢ÇÏ´Â °ÍÀÌ ÇÊ¿ä. ÀÏÀÇ ³¡¸¶¹«¸®¿¡ ´ëÇÑ ÀÎ³»½ÉÀÌ ÇÊ¿ä. Å¸ÀÎ¿¡ ´ëÇÑ ÄªÂù, °Ý·Á, ÀÎÁ¤ÀÌ ÇÊ¿ä.");
	printf("\r\n");
}

void estj()
{
	printf("\r\nESTJ ÀÏ¹ÝÀûÀÎ Æ¯¼º");
	printf("\r\n°¨Á¤ÀÌ Àß µå·¯³ª Á÷¼³ÀûÀÎ ¾ð¾î·Î Ç¥ÇöÇÑ´Ù. ¼ÖÁ÷ÇÏ°í È­²öÇÏ´Ù. ³ª¼­±â¸¦ ÀßÇÑ´Ù. ÈÞÀÏ¿¡ ¾à¼ÓÀÌ ¾øÀ¸¸é ºÒ¾ÈÇÏ´Ù. ÁÖÀåÀÌ °­ÇÏ°í ³ª¼­±æ ÁÁ¾ÆÇÑ´Ù. ³í¸®Àû, ºÐ¼®Àû °´°üÀûÀÌ¸ç ºÐ¸íÇÑ ±ÔÄ¢À» Áß¿ä½ÃÇÏ°í ±×¿¡ µû¶ó Çàµ¿ÇÏ°í ÀÏÀ» ÃßÁøÇÏ°í ¿Ï¼ºÇÑ´Ù. °íÁýÀÌ ÀÖÁö¸¸ ³í¸®ÀûÀ¸·Î ±àÁ¤ÇÏ°Ô µÇ¸é ´õ ÀÌ»ó °íÁýÀ» ÇÇ¿ìÁö ¾Ê´Â´Ù. È¥ÀÚ¼­ ÀÏÇÏ±â ½È¾îÇÑ´Ù. ÀÏÀ» ÀßÇØ ³õ°íµµ Á¸°æ¹Þ´Â ÀÏÀÌ µå¹° ¼ö ÀÖ´Ù. Áý´Ü¿¡¼­ ºÐÀ§±â º¸´Ù´Â ¸ñÀûÀÇ½ÄÀ» Áß¿ä½ÃÇÑ´Ù. °æ¿µÀÚ ÀûÀÎ ÀçÁúÀ» Áö³æ´Ù. ÇÑ¹ø ½ÃÀÛÇÑ ÀÏÀ» Ã¶ÀúÇÏ°Ô »Ñ¸®»Ì´Â´Ù. ÀüÃ¶À» Å» ¶§µµ ¾î´À Ä­¿¡ Å¸¾ß °¥¾ÆÅ¸±â ÆíÇÑÁö °è»êÇÏ°í Åº´Ù. ¿¹¾à°ú °èÈ¹ÀÇ »ýÈ°È­. µ¶Ã¢·Â, Ã¢ÀÇ·ÂÀÌ ºÎÁ·ÇÏ´Ù. ¸»ÀÌ ºü¸£°í °ÉÀ½ÀÌ ºü¸£´Ù. È­²öÇÏ¸ç µÚ³¡ÀÌ ¾ø´Ù. Áö¹èÇÏ·Á´Â ¼º°ÝÀÌ ÀÖ¾îµµ ¾çº¸´Â Àß ÇÏÁö ¾Ê´Â´Ù. °¨µ¿½ÃÅ°±âº¸´Ù´Â ÀÌÇØ½ÃÅ°±æ ¿øÇÑ´Ù. ¿ÜÃâ ½Ã ²À ¸Þ¸ð¸¦ ÇÑ´Ù. °¡¸¸È÷ ÀÖÀ¸¸é ÇÇ°ïÇÏ´Ù.");

	printf("\r\n\r\nESTJ °³¹ßÇØ¾ßÇÒ Á¡");
	printf("\r\nÁö³ªÄ¡°Ô ÀÏ Áß½ÉÀûÀÌ±â ¶§¹®¿¡ °¡Á·¿¡°Ô ¼ÒÈ¦ ÇÒ ¼ö ÀÖÀ¸¹Ç·Î ÀÏº¸´Ù »ç¶÷ Áß½ÉÀûÀÎ »ýÈ°¿¡ ÇÊ¿ä. ±ÞÇÏ°Ô °áÁ¤ÇÏ±â Àü¿¡ ¿©·¯ »óÈ²À» »ý°¢ÇÒ ÇÊ¿ä°¡ ÀÖ´Ù. Å¸ÀÎÀÇ Á¤¼­ÀûÀÎ ¸é¿¡ °ü½ÉÀ» °¡Áú ÇÊ¿ä°¡ ÀÖ´Ù.");
	printf("\r\n");
}

void esfj()
{
	printf("\r\nESFJ ÀÏ¹ÝÀûÀÎ Æ¯¼º");
	printf("\r\n½Å³ª°í Àç¹ÌÀÖ´Â »ç¶÷ÀÌ´Ù. ½ºÆ®·¹½º¸¦ ¹ÞÀ¸¸é ´©±¸¸¦ ¸¸³ª¾ß ÇÑ´Ù. °­ÀÇ ¶§ °í°³¸¦ Á¦ÀÏ ¸¹ÀÌ ²ô´öÀÌ¾î °­»ç¸¦ Áñ°Ì°Ô ÇÑ´Ù. ÁØºñ¼ºÀÌ Ã¶ÀúÇÏ¸ç, ÂüÀ»¼ºÀÌ ¸¹°í Å¸ÀÎÀ» Àß µ½´Â´Ù. ³²¿¡°Ô µ¿Á¶ÇÏ´Â °æÇâÀÌ ¶Ù¾î³ª¸ç »ç¶÷µé°úÀÇ »óÈ£ È°µ¿¿¡¼­ ±â·ÂÀÌ »ý±ä´Ù. Å¸ÀÎÀÇ ÀÎÁ¤À» ¹Þ´Â °Í¿¡ ¾ÆÁÖ ¹Î°¨ÇÏ´Ù. º¸¼öÀûÀÌ¸ç, ÁÁÀº À½½ÄÀ» ÁÁ¾ÆÇÏ°í, ºÀ»ç¸¦ ÁÁ¾ÆÇÏ¸ç Àç¹°À» ¸ðÀ¸´Â °ÍÀ» Áñ±è. ÀÚ½ÅÀÌ Á¸°æÇÏ´Â »ç¶÷ÀÌ³ª ¹°°ÇÀ» ÀÌ»óÈ­ÇÏ´Â °æÇâÀÌ ÀÖ´Ù. Á¦Ç°À» º¸°í »ç´Â °ÍÀÌ ¾Æ´Ï¶ó ESFJÀÇ ÀÎ°£ÀûÀÎ ¸é¿¡ ¹ÝÇØ¼­ »ê´Ù. »ý°¢Àº Ã¢ÀÇÀûÀÎµ¥ ½ÇÃµÀÌ ºÎÁ·ÇÏ´Ù. Å¸ÀÎÀ» µ½°í ½ÍÀº ¿å±¸ ¶§¹®¿¡ ÀÚ½ÅÀÇ ¾÷¹«¸¦ ¼ÒÈ¦È÷ ÇÒ ¼ö ÀÖ´Ù. ÀÜ°ÆÁ¤ÀÌ ¸¹´Ù. °¡Á·µé¿¡°Ô ÀÜ¼Ò¸®°¡ ½ÉÇÏ´Ù. Áý´ÜÀÇ ÀÏÀÌ³ª ¸ñÀûÀ» °³ÀÎÀÇ °Íº¸´Ù ¾Õ¼¼¿î´Ù. Á¶È­¿Í ±ÕÇüÀ» Áß¿ä½ÃÇÑ´Ù. ½ÈÀº ¼Ò¸®ÇÏ±â ½È¾îÇÑ´Ù. »ç¶÷µé »çÀÌ¿¡¼­ ÁßÀçÀÚ ¿ªÇÒÀ» ÀßÇÑ´Ù. ÀÚ³à¿Í Á¤¼­ÀûÀÎ µ¶¸³ÀÌ ¾ÈµÇ¾î °ÆÁ¤ÀÌ ²÷ÀÌÁö°¡ ¾Ê´Â´Ù. º°¸íÀÌ ¼öµµ²ÀÁö - ´«¹°ÀÌ ¸¹´Ù. ¹Þ´Â Ä£ÀýÇÏ°í ÀçÄ¡ ÀÖ´Ù."); 

	printf("\r\n\r\nESTJ °³¹ßÇØ¾ßÇÒ Á¡");
	printf("\r\n°¡Á·µé°ú µ¶¸³ÀÌ ÇÊ¿äÇÏ´Ù. Å¸ÀÎÀÇ ÀÏ¿¡ ¶§·Î´Â ³ÃÁ¤ÇÏ°Ô »ý°¢ÇØ º¼ ¿©À¯°¡ ÇÊ¿äÇÏ´Ù. ÀÜ°ÆÁ¤À» ÁÙÀÌ±â À§ÇØ¼­ °ÆÁ¤À» °´°üÈ­ ½ÃÄÑ º¼ ÇÊ¿ä°¡ ÀÖ´Ù.");
	printf("\r\n");
}

void enfj()
{
	printf("\r\nENFJ ÀÏ¹ÝÀûÀÎ Æ¯¼º");
	printf("\r\n¸¶À½ÀÌ ¾àÇÏ°í ³²ÀÇ ÀÇ°ß¿¡ µ¿È­¸¦ ÀßÇÏ´Â ÆíÀÌ´Ù. ¸»·Î Ç¥ÇöÀ» ÀßÇÏ°í »ý°¢ÀÌ³ª ¸¶À½À» Àß ¿¬´Ù. Àû±ØÀûÀÌ°í ÃßÁø·ÂÀÌ °­ÇÑ ÆíÀÌ´Ù. Á» ¾î·Æ°Ô »ý°¢µÇ´Â ÀÏµµ µÇ´Â ÂÊÀ¸·Î ¸ô°í°£´Ù. ÀÏÀÇ °á°úº¸´Ù´Â »ç¶÷°úÀÇ ÀÎÈ­¸¦ ´õ Áß¿ä½ÃÇÏ´Â ÆíÀÌ´Ù. »çÀü °èÈ¹À» ¼¼¿ì°í ±×¿¡ µû¶ó ÇàÇÏ°íÀÚ ÇÑ´Ù. È­°¡³ª¸é °ÑÀ¸·Î Ç¥ÇöÇÏÁö ¾Ê°í ¹¬¹¬È÷ Âü¾Æ³½´Ù. Å¸ÀÎ¿¡°Ô ¹«Ã´ »ç±³ÀûÀÌÁö¸¸ Áý¿¡ ÀÖ´Â ¾ÆÀÌµé¿¡°Ô´Â ¾È ±×·² ¼ö ÀÖ´Ù. »óÃ³¸¦ ¹ÞÀ¸¸é ¿À·£½Ã°£ °¡½¿¾ÆÆÄÇÑ´Ù. Çö½Çº¸´Ù´Â ´õ ³ªÀº »î ÀÌ»óÀ» Ãß±¸ÇÑ´Ù. ¸Î°í ²÷´Â °ÍÀÌ ºÐ¸íÇÏÁö ¸øÇÔ. Á¤, ´«¹°, µ¿Á¤½ÉÀÌ ¸¹À½. »ó´ë¹æÀÇ ¸»¿¡ ¹Î°¨ÇÏ´Ù. »ç¶÷À» ¼¶±â±â À§ÇØ¼­ ¼¼»ó¿¡ ³ª¿ÔÀ½ . »ç¶÷À» ÁÁ¾ÆÇÏ°í ºñÆÇÀûÀÎ ½Ã°¢º¸´Ù´Â ±àÁ¤ÀûÀ¸·Î º¸·Á´Â ½Ã°¢ÀÌ °­ÇÔ. »ç¶÷¿¡ ´ëÇØ ¸Í¸ñÀû ÀÌ»óÈ­ °æÇâÀÌ ÀÖ´Ù. Å¸ÀÎ¿¡°Ô ÀÎÁ¤°ú ÄªÂùÀ» ¹Þ´Â ÀÏ¿¡¸¸ ¿­ÁßÇÒ ¼ö ÀÖ´Ù. »ç¶÷À» À§ÇØ ºÀ»çÇÏ´Â ºÐ¾ß¿¡ ´É·ÂÀÌ ÀÖ´Ù. »ç¶÷°ú »ç¶÷À» Àß ¿«´Â´Ù. °¨Á¤ÀÇ ±âº¹ÀÌ ½ÉÇÏ´Ù.");

	printf("\r\n\r\nENFJ °³¹ßÇØ¾ßÇÒ Á¡");
	printf("\r\nÇö½ÇÀûÀÎ ÀÏ°ú ¼¼ºÎ »çÇ×¿¡ ´ëÇÑ °ü½ÉÀÌ ÇÊ¿ä. ÀÎ°£°ü°è¸¦ Áß¿äÇÏ´Ù°¡ Å« ÀÏÀ» ¼ÒÈ¦È÷ ÇÒ ¼ö ÀÖ´Ù. Å¸ÀÎÀÇ ¸»¿¡ ´ëÇØ¼­ °´°üÀûÀÎ ÀÛ¾÷ÀÌ ÇÊ¿ä.");
	printf("\r\n");
}

void entj()
{
	printf("\r\nENTJ ÀÏ¹ÝÀûÀÎ Æ¯¼º");
	printf("\r\nÅ¸°í³­ ÁöµµÀÚÇü. Á¶Á÷Àû, Ã¼°èÀû, °èÈ¹ÀûÀÌ´Ù. ÀÏÀº ÀÏ´ë·Î ÇÏ°í ¿åÀº ¿å´ë·Î ¸ÔÀ» ¼ö ÀÖ´Ù. ³Ê¹« ¿Ïº®À» Ãß±¸ÇÏ±â ¶§¹®¿¡ ³²ÀÌ ºñÁý°í µé¾î°¥ Æ´ÀÌ ¾ø´Ù. »ó»óÀ» ¸¹ÀÌ ÇÑ´Ù. È¥ÀÚ ÀÖ´Â °ÍÀ» ½È¾îÇÑ´Ù. µ¿¹°À» º°·Î ÁÁ¾ÆÇÏÁö ¾Ê´Â´Ù. °íÁýÀÌ ¼¼°í, È£±â½ÉÀÌ ¸¹´Ù. ÁöÀû ¿å±¸°¡ °­ÇÏ´Ù. °¨Á¤Ç¥ÇöÀÌ ¼ÖÁ÷ÇØ¼­ Å¸ÀÎÀÌ »óÃ³¸¦ ¹ÞÀ» ¼ö ÀÖ´Ù. »ç¶÷º¸´Ù ÀÏÀ» Áß½ÃÇÑ´Ù. Ç×»ó °èÈ¹À» ÇÏ°í ½ÇÇàÇÑ´Ù. ÀÏ»óÀûÀÎ ¹Ýº¹µÇ´Â ÀÏÀ» ½È¾îÇÑ´Ù. °¡²û ¾û¶×ÇÒ ¶§°¡ ÀÖ´Ù. º¯È­ ÀÖ´Â »ýÈ°À» Áñ±ä´Ù. ¾î·Á¿î ÀÏÀ» ¸¸³ªµµ ÀÚ±ØÀÌ µÇ¾î ½±°Ô Ã³¸®ÇÑ´Ù. ÁöÀû ´É·Â Çâ»ó¿¡ ³ë·ÂÇÏ°íÀÚ ÇÑ´Ù. ±ÇÀ§ÀûÀÌÁö ¾Ê´Ù. ¹Ì·¡¿¡ ´ëÇÑ ²ÞÀÌ Å©´Ù. ¼ÖÁ÷ÇÏ°í °á´Ü·Â, Åë¼Ö·ÂÀÌ ÀÖÀ¸¸ç °Å½ÃÀû ¾È¸ñÀ¸·Î ÀÏÀ» ÃßÁøÇØ ³ª°¨.");

	printf("\r\n\r\nENTJ °³¹ßÇØ¾ßÇÒ Á¡");
	printf("\r\n¼Ó´Ü ¼Ó°áÇÏ¿© ¹è·ÁÇÏ±âº¸´Ù´Â °­¾ÐÀûÀ¸·Î ÇÏ±â ½¬¿ì¹Ç·Î Á÷¼³ÀûÀÎ ¸»À» »ï°¡¾ß ÇÒ ÇÊ¿ä°¡ ÀÖ´Ù. Å¸ÀÎÀÇ ¸»À» Àû±ØÀûÀ¸·Î °æÃ»ÇÒ ÇÊ¿ä°¡ ÀÖ´Ù. ÀÏ Áß½É º¸´Ù »ç¶÷Áß½ÉÀÇ »ýÈ°ÀÌ ÇÊ¿ä.");
	printf("\r\n");
}

