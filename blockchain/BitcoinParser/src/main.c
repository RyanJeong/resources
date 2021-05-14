#include <stdio.h>      /*  puts, ...   */
#include <stdlib.h>     /*  atoi, ...   */
#include <string.h>     /*  strcmp, ... */
#include <stdint.h>     /*  size_t, ... */
#include <unistd.h>     /*  sleep() */
#include <time.h>       /*  measure running time    */

#include "main.h"   
#include "util.h"
#include "common.h"
#include "curl.h"
#include "getblock.h"   /*  requestGetBlock(), ...  */

static size_t flag = DEBUG_ON;

int main(int argc, char *argv[]) 
{
    /*
     *  func        : putsForDebug 에서 함수를 식별하기 위해 사용하는 포인터 변수
     *  param       : getblock에 사용할 매개변수
     *  i           : getblock을 자동 조회할 때 쓰는 인덱스 
     *  blockCount  : 프로그램이 동작될 시점에서의 블록 총 갯수
     *  temp        : 반복 작업 시 디버깅 메세지를 담을 때 사용하는 변수
     *  start       : 시작 시간
     *  end         : 종료 시간
     */
    static char *func = MAIN_MAIN;
    int         param;
    size_t      i, blockCount;
    char        temp[SIZE_32BYTES];
    clock_t     start, end;

    start = time(NULL);
    if (initCurl()) {
    
        return 1;
    }
    if (argc == 1) {
        puts(CMD_GUIDE);
    } else {
        if (!strcmp(*(argv + OFFSET_CMD), CMD_GETBLOCK)) {
            if (argc < 3) {
                puts(CMD_GUIDE);
            } else {
                param = atoi(*(argv + OFFSET_PARAM));
                if (param < 0) {
                    blockCount = requestGetBlockCount();
                    for (i = 0; i < blockCount; ++i) {
                        requestGetBlock(i);
                        sprintf(temp, "Index: %lu", i);
                        putsForDebug(__MAIN__, func, temp, flag);
                        /*  usleep(1000);   just for stableability  */
                    }   
                } else {
                    requestGetBlock(param);
                }
            }
        } else if (!strcmp(*(argv + OFFSET_CMD), CMD_TRANSACTION)) {
            puts(CMD_TRANSACTION);
        } else if (!strcmp(*(argv + OFFSET_CMD), CMD_ADDRESS)) {
            puts(CMD_ADDRESS);
        } else {
            puts(CMD_GUIDE);
        }
    }
    cleanUpCurl();  
    end = time(NULL);
    calcRunningTime(end - start);

    return 0;
}
