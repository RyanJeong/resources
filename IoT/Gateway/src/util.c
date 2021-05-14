#include <time.h>
#include <stdio.h>  /*  sprintf */
#include <string.h> /*  strlen  */
#include <stdlib.h> /*  memcpy  */
#include <stdint.h> /*  uint8_t */

#include "util.h"

/*
 *  msg[SIZE](process.c)    : 디버깅 메세지를 담는 변수  
 *  thing[SIZE_THING]       : 사물에 대한 정보를 담는 자료구조, serialID, time, CRC, test, LED 멤버를 포함
 */
extern FILE *logFile;
THING       thing[SIZE_THING];

size_t getUnixTime(void)
{

    return time(NULL);
}

size_t initStructure(void)
{
    if (!memset(thing, 0, (size_t) SIZE_THING * sizeof(THING))) {
    
        return 1;
    } else {

        return 0;
    }
}

char *getTime(void)
{
    static char buffer[SIZE_TIME];
    time_t      timer;
    TIME        *t;

    timer   = time(NULL);
    t       = localtime(&timer);
    sprintf(buffer, "[%04d-%02d-%02d %02d:%02d:%02d]",
        (t->tm_year + 1900), 
        (t->tm_mon + 1),
        t->tm_mday,
        t->tm_hour,
        t->tm_min,
        t->tm_sec);

    return buffer;
}

size_t initFrame(void)
{

    return 0;
}

void putsForDebug(const char *file, const char *func, char *msg)
{
    /*
     * buffer       : puts에 사용할 문자열 배열, 화면에 출력할 메세지를 담는 배열
     * p            : buffer를 가리키는 포인터 
     * time         : 시간 정보를 받아오는 포인터
     * temp         : buffer에 추가할 문자열의 길이를 잠시 담는 변수
     */
    char    buffer[SIZE];
    char    *p, *time;
    size_t  temp;
    

    p       =  buffer;
    time    = getTime();
    memset(p, ' ', SIZE_LABEL);
    memcpy(p, time, (temp = strlen(time)));
    p += temp;
    memcpy(p, file, (temp = strlen(file)));
    p += temp;
    memcpy(p, func, strlen(func));

    p       =  buffer;
    p       += (SIZE_LABEL - 2);        /*  Label 안에 ':', ' ' 추가 위해   */  
    *p++    = ':';
    *p++    = ' ';
    memcpy(p, msg, (temp = strlen(msg)));
    p       += temp;
    *p      = '\0';
#ifdef LOG 
    fprintf(logFile, "%s\n", buffer);
    fflush(logFile);
#else 
    puts(buffer);
#endif

    return;
}   
