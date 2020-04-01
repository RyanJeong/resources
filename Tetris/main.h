#ifndef __MAIN_H__
#define __MAIN_H__

#define START_GAME          1
#define PRINT_GAME_MANUAL   2
#define PRINT_GAME_RESULT   3
#define EXIT_GAME           (-1)

/*  게임 플레이 정보를 기록하는 구조체  */
static struct rank {
	char    name[30];
	long    point;
	int      year;
	int     month;
	int     day;
	int     hour;
	int     min;
	int     rank;
} tetrisRank;

int getChoise(void);

#endif
