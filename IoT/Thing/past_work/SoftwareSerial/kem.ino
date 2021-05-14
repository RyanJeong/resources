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

/*
 *  commResult  : Gateway와 통신 결과를 저장하는 변수
 *  thingState  : 현재 thing의 상태를 저장하는 변수
 */
uint8_t commResult, thingState;

void setup(void)
{
    Serial.begin(9600);

    /*  if (initThing) {, 가독성 향상 위해 아래로 대체  */
    if (initThing() != OK) {

        return;
    }
}

void loop(void)
{
    /*  if (!commResult) {, 가독성 향상 위해 아래로 대체    */
    if (commResult == OK) {
        switch (thingState) {
        case INIT_TEST_COMMAND:
        case (RECEIVED_TEST_COMMAND + 0):   /*  by period   */
        case (RECEIVED_TEST_COMMAND + 1):   /*  by user */
            testing();
            break;
        default:
            sensing();
            break;
        }
    }
    sendMsg();
    /*  DO NOT ERASE THE NEXT DELAY FUNCTION    */
    delay(500);
    recvMsg();
    sleep();
}
