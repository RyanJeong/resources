#include <stdio.h>
#include <stdint.h>     /*  uint8_t */
#include <sys/types.h>  /*  size_t  */

#include "process.h"
#include "util.h"       /*  SIZE_MSG    */
#include "rf.h"         /*  setupRF */
#include "server.h"     /*  setupThread */

/*
 *  logFile     : 프로그램 구동 방식이 log 방식일 경우, debug message들을 log로 남기기 위하여 저장하는 파일을 가리키는 변수
 *  msg[SIZE]   : 디버깅 메세지를 담는 변수  
 */
FILE    *logFile;
char    msg[SIZE];

size_t init(void)
{
    static const char *func = PROCESS_INIT;

#if defined PRINT
# if defined ALPHA
    putsForDebug(__PROCESS__, func, MSG_START_INFO1);
# elif defined BETA
    putsForDebug(__PROCESS__, func, MSG_START_INFO2);
# else
    putsForDebug(__PROCESS__, func, MSG_START_INFO3);
# endif
#else
    if ((logFile = fopen(LOG_DIRECTORY, "a")) == NULL) {
        puts(MSG_ERROR_INIT1);  /*  putsForDebug 사용해봐야 log 모드이므로 시리얼 모니터에 출력 안 됨   */

        return 1;
    } else {
        /*  Seperator   */
        putsForDebug(__PROCESS__, func, MSG_NULL);
        putsForDebug(__PROCESS__, func, MSG_NULL);
        putsForDebug(__PROCESS__, func, MSG_NULL);
    }
# if defined ALPHA
    putsForDebug(__PROCESS__, func, MSG_START_INFO4);
# elif defined BETA
    putsForDebug(__PROCESS__, func, MSG_START_INFO5);
# else
    putsForDebug(__PROCESS__, func, MSG_START_INFO6);
# endif
#endif
    if (setupUART()) {
        putsForDebug(__PROCESS__, func, MSG_ERROR_INIT2);

        return 1;
    }
    if (setupRF()) {
        putsForDebug(__PROCESS__, func, MSG_ERROR_INIT3);

        return 1;
    }
    if (setupThread()) {
        putsForDebug(__PROCESS__, func, MSG_ERROR_INIT4);

        return 1;
    }
/*  TODO: remove after test 
    if (initSocket()) {
        putsForDebug(__PROCESS__, func, MSG_ERROR_INIT5);

        return 1;
    }
*/
    if (initStructure()) {
        putsForDebug(__PROCESS__, func, MSG_ERROR_INIT6);

        return 1;
    }
    if (getMACAddrForPacket()) {
        putsForDebug(__PROCESS__, func, MSG_ERROR_INIT7);

        return 1;
    }
    putsForDebug(__PROCESS__, func, MSG_COMPLETE_INIT);

    return 0;
}

void process(void)
{
    static const char *func = PROCESS_PROCESS;

    putsForDebug(__PROCESS__, func, MSG_START_PROC);
    while (1) {
        recvAndSendMsg();
    }
    putsForDebug(__PROCESS__, func, MSG_FINISH_PROC);

    return;
}

size_t finish(void)
{
    static const char *func = PROCESS_FINISH;

    if (destroyThread()) {
        putsForDebug(__PROCESS__, func, MSG_ERROR_FINISH1);

        return 1;
    }
    if (closeUART()) {
        putsForDebug(__PROCESS__, func, MSG_ERROR_FINISH2);

        return 1;
    }
    /*  no return type  */
    closeSocket();
    putsForDebug(__PROCESS__, func, MSG_COMPLETE_FINISH);
#if defined PRINT
# if defined ALPHA
    putsForDebug(__PROCESS__, func, MSG_FINISH_INFO1);
# elif defined BETA
    putsForDebug(__PROCESS__, func, MSG_FINISH_INFO2);
# else
    putsForDebug(__PROCESS__, func, MSG_FINISH_INFO3);
# endif
#else
# if defined ALPHA
    putsForDebug(__PROCESS__, func, MSG_FINISH_INFO4);
# elif defined BETA
    putsForDebug(__PROCESS__, func, MSG_FINISH_INFO5);
# else
    putsForDebug(__PROCESS__, func, MSG_FINISH_INFO6);
# endif
#endif
    if (fclose(logFile)) {
        puts(MSG_ERROR_FINISH3);    /*  putsForDebug 사용해봐야 log 모드이므로 시리얼 모니터에 출력 안 됨   */

        return 1;
    }

    return 0;
}
