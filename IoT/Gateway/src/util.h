#ifndef __UTIL_H__
#define __UTIL_H__

#define MSG_LINE    "----------------------------------------"

#define SIZE_THING          64
#define SIZE_STR_SERIAL_ID  19
#define SIZE_SERIAL_ID      9
#define SIZE_SERIAL_ID_3    3
#define SIZE                1024
#define SIZE_TIME           32
#define SIZE_LABEL          60
#define SIZE_TX             6
#define SIZE_TX_LEN         3
#define SIZE_STR_MAC        13
#define SIZE_MAC            6
#define SIZE_INTERVAL       60 /*  60 seconds */

#define UTIL            "[util.c]"
#define UTIL_GETTIME    "[getTime(void)] "

typedef struct tm TIME;

/*
 *  Raspberry Pi 하나 당 최대 64대(0 ~ 63)의 아두이노를 관리할 수 있음
 *  즉, 아두이노의 ID는 0번부터 63번까지이다.
 *  THING 구조체를 64개의 배열로 선언하여 아두이노의 ID가 THING 구조체의 index가 됨
 *  serial과 time은 각각 '\0', 0으로 초기화 되며, 아두이노로부터 연결 요청을 받으면
 *  serial에는 아두이노의 serial을, time은 연결 요청이 이루어진 UNIX time 값을 입력한다.
 *  
 *  serial  : 아두이노의 serial ID를 의미
 *  time    : 아두이노와 Raspberry Pi와의 frame 송수신 중 마지막으로 성공한 시간
 *  test    : test 명령을 수신하였으면 1, 수신하지 않았으면 0
 *  LED     : LED 점등 명령을 수신하였으면 1, 수신하지 않았으면 0
 *  CRC     : thing의 CRC 저장하는 변수, 각 thing마다 이 값은 서로 다름
 *  
 *  time이 현재 시간으로부터 10분(600초)이상 차이가 날 경우, 재연결
 */
typedef struct thing {
    uint8_t serialID[SIZE_SERIAL_ID];
    char    strSerialID[SIZE_STR_SERIAL_ID];
    size_t  time;
    size_t  test;
    size_t  LED;
    uint8_t CRC;
} THING;
    
/*
 * size_t getUnixTime(void);
 * : a function that get UNIX time
 */
size_t getUnixTime(void);

/*
 * size_t initStructure(void);
 * : initialize the structure
 */
size_t initStructure(void);

/*
 * char *getTime(void);
 * : a function that get time
 */
char *getTime(void);

/*
 * size_t initFrame(void);
 * : frame 내부에 해당 gateway의 MAC 주소 입력
 */
size_t initFrame(void);

/*
 * void putsForDebug(const char *, const char *, char *);
 * : a function that print debug message
 */
void putsForDebug(const char *, const char *, char *);

#endif
