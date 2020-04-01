#include <stdio.h>
#include <stdlib.h>

#include "print.h"

void printManual(void)
{
	int ch;
		printf("\n\n\t\t\t       게임설명서");
		printf("\n\n왼쪽 : j | 아래 : k | 오른쪽 : l | 회전 :i | 바로떨구기 : a | 아이템 사용 : q");
		printf("\n\n\t한줄당 1000점 획득 및 5000점 넘길때마다 아이템 생성(최대3개)");
		printf("\n\n\t아이템 : 3줄즉시 제거");
		printf("\n\n\t블럭이 끝까지 쌓이면 gameover");
		printf("\n\n\thiddenkey 존재!");
		printf("\n\n\t게임 메뉴로 돌아가기 : M");
	while(1)
	{
		ch = getch();
		if(ch == 77 || ch == 109)
			break;
	}
	return 0;
}

void printRanking(void)
{
	FILE    *fp;
    int     c;

	system("clear");
    fp = NULL;
}
