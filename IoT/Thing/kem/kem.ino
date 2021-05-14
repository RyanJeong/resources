/*
 *  KEM IoT Program for Arduino
 *  Munseong Jeong
 *  msjeong@daejin.ac.kr
 */

#include "kem.h"
#include "init.h"       /* for initThing(); */
#include "sensor.h"     /* for testing() and sensing(); */
#include "rf.h"         /* for sendMsg() and recvMSg(); */
#include "interrupt.h"  /* for sleep()  */
#include "util.h"       /* for SHUT_DOWN    */

/*
 *  commResult          : Gateway와 통신 결과를 저장하는 변수
 *  thingState          : 현재 thing의 상태를 저장하는 변수
 *  tempDbg[SIZE_BUF]   : Debug message를 담는 배열
 */
uint8_t     commResult, thingState;
char        tempDbg[SIZE_BUF];

/*
 *  void setup(void)
 *  ----------------------------------
 *  Thing 프로그램의 초기설정부
 *  initThing() 함수 수행 도중에 문제가 발생할 경우, Thing 프로그램을 강제로 종료
 *  initThung() 함수 수행 결과 문제가 없는 경우, loop() 함수로 진입
 */
void setup(void)
{
    /*  initThing()에서 문제가 발생할 경우 강제 종료     */
    if (initThing()) {
        SHUT_DOWN; 
    }
}

/*
 *  void loop(void)
 *  ----------------------------------
 *  Thing 프로그램의 동작부
 */
void loop(void)
{
#ifdef TEST_SENSOR
    testSensor();
#elif defined TEST_RF
    testCommunication();
#else
    if (!commResult) {
        switch (thingState) {
        case INIT_TEST_COMMAND:             /*  by init.    */
        case (RECEIVED_TEST_COMMAND + 0):   /*  by period   */
        case (RECEIVED_TEST_COMMAND + 1):   /*  by user */
            testing();
            break;
        default:
            sensing();
            break;
        }
    }
    communicate();
#endif
}
