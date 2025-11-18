// 바이오리듬 계산 프로그램 V1.0 by TeIn
//http://blog.naver.com/bieemiho92
//Last Edit : 20150709

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#define PI 3.1415926535897932384626433832795

int Days(int year, int month, int day)
{
	// 출처 : http://kin.naver.com/qna/detail.nhn?d1id=1&dirId=1040101&docId=223459012&qb=7IK07JWE7JioIOuCoOynnOqzhOyCsA==&enc=utf8&section=kin&rank=2&search_sort=0&spq=0&pid=Si0htdoRR14ssalCQUssssssss4-171997&sid=jqbPrSPEXfBWIlflJ33P0g%3D%3D
	//날 값이 저장될 변수 
	int count = 0;
	//달 수의 계산할 배열 
	int mon[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	int i;
	//윤년이 아닐때 365일 
	//윤년일때 366일 
	for (i = 0; i<year; i++)
	{
		//윤년이면 
		if (i % 4 == 0)
		{
			count += 366;
		}
		else
		{
			count += 365;
		}
	}
	for (i = 0; i< month; i++)
	{
		//해당 년도가 윤년일때 
		if (year % 4 == 0)
		{
			//2월은 29일 
			if (i == 1)
			{
				count += (mon[i] + 1);
			}
			else
			{
				count += mon[i];
			}
		}
		//아닐때 
		else
		{
			count += mon[i];
		}
	}
	//남은 일수값 
	count += day;
	return count;
}
void GetBirth(int *yy, int *mm, int *dd)
{
	while (1)
	{
		printf("당신의 생일을 입력하세요(YYYY MM DD) : ");
		scanf("%d %d %d", yy, mm, dd);
		if (*mm < 1 || *mm>12 || *dd < 1 || *dd >31)
			printf("입력한 값이 올바르지 않습니다.\n");
		else
			return;
	}
}
void CalcRhythm(double *b, double *e, double *i, double *s, int surviveday)
{
	*b = sin((2 * PI* ((double)surviveday / 23)));
	*e = sin((2 * PI* ((double)surviveday / 28)));
	*i = sin((2 * PI* ((double)surviveday / 33)));
	*s = sin((2 * PI* ((double)surviveday / 38)));
}
void Print(double body, double emotion, double intelligent, double sense, int tyear, int tmonth, int tday, int uyear, int umonth, int uday)
{
	double bb, ee, ii, ss;
	int i;

	printf(" %d년 %d월 %d일생인 당신의 바이오리듬 입니다.\r\n\r\n", uyear, umonth, uday);
	
	printf(" 항목\t-100%%==============0%%==============+100%%\n");
	//body graph
	printf(" 신체\t");
	if (body*10 < 0)
	{
		bb = floor(body * 10 * -1);
		for (i = 10; i > bb; i--)
			printf("□");
		for (i = bb; i > 0; i--)
			printf("■");
		for (i = 0; i < 10; i++)
			printf("□");
	}
	else
	{ 
		bb = floor(body * 10);
		for (i = 0; i < 10; i++)
			printf("□");
		for (i = 0; i < bb; i++)
			printf("■");
		for (i = bb; i < 10; i++)
			printf("□");
	}
	printf("\t%.2lf%%\n", body * 100);
	
	//emotion graph
	printf(" 감성\t");
	if (emotion * 10 < 0)
	{
		ee = floor(emotion * 10 * -1);
		for (i = 10; i > ee; i--)
			printf("□");
		for (i = ee; i > 0; i--)
			printf("■");
		for (i = 0; i < 10; i++)
			printf("□");
	}
	else
	{
		ee = floor(emotion * 10);
		for (i = 0; i < 10; i++)
			printf("□");
		for (i = 0; i < ee; i++)
			printf("■");
		for (i = ee; i < 10; i++)
			printf("□");
	}
	printf("\t%.2lf%%\n", emotion * 100);

	//intelligent graph
	printf(" 지성\t");
	if (intelligent * 10 < 0)
	{
		ii = floor(intelligent * 10 * -1);
		for (i = 10; i > ii; i--)
			printf("□");
		for (i = ii; i > 0; i--)
			printf("■");
		for (i = 0; i < 10; i++)
			printf("□");
	}
	else
	{
		ii = floor(intelligent * 10);
		for (i = 0; i < 10; i++)
			printf("□");
		for (i = 0; i < ii; i++)
			printf("■");
		for (i = ii; i < 10; i++)
			printf("□");
	}
	printf("\t%.2lf%%\n", intelligent * 100);

	//sense graph
	printf(" 지각\t");
	if (sense * 10 < 0)
	{
		ss = floor(sense * 10 * -1);
		for (i = 10; i > ss; i--)
			printf("□");
		for (i = ss; i > 0; i--)
			printf("■");
		for (i = 0; i < 10; i++)
			printf("□");
	}
	else
	{
		ss = floor(sense * 10);
		for (i = 0; i < 10; i++)
			printf("□");
		for (i = 0; i < ss; i++)
			printf("■");
		for (i = ss; i < 10; i++)
			printf("□");
	}
	printf("\t%.2lf%%\n", sense * 100);
}

int main(int argc, char **argv)
{
	int uyear, umonth, uday;
	int tyear, tmonth, tday;
	int surviveday;
	double body, emotion, intelligent, sense;

	time_t current;
	struct tm *d;
	
	uyear = atoi(argv[1]);
	umonth = atoi(argv[2]);
	uday = atoi(argv[3]);

	current = time(NULL);
	d = localtime(&current);
	tyear = 1900 + (d->tm_year);	tmonth = (d->tm_mon) + 1;	tday = (d->tm_mday);

	printf(" 오늘은 %d년 %d월 %d일 입니다.\n", tyear, tmonth, tday);

	//GetBirth(&uyear, &umonth, &uday);

	//printf("당신의 생일은 %d년 %d월 %d일 입니다.\n", uyear, umonth, uday);

	surviveday = (Days(tyear, tmonth, tday) - Days(uyear, umonth, uday)) + 1; //함수가 기준일이 0일인 관계로 +1;

	/*printf("days from today %d\n", days(tyear, tmonth, tday));
	printf("days from birthday %d\n", days(uyear, umonth, uday));
	printf("지금까지 %d일 살았습니다.\n", surviveday);
	*/

	CalcRhythm(&body, &emotion, &intelligent, &sense, surviveday);
	//printf("body %lf, emotion %lf, intelligent %lf, sense %lf\n", body * 100, emotion * 100, intelligent * 100, sense * 100);

	Print(body, emotion, intelligent, sense, tyear, tmonth, tday, uyear, umonth, uday);

	return 0;

}// main fn end;
