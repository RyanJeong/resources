/*
 *  Heat Temperature Detector
 *  Munseong Jeong
 *  msjeong@daejin.ac.kr
 *  Last Update: DEC 2 2019
 */

#include "kem.h"
#include "sensor.h"     /* for testing() and sensing(); */
#include "rf.h"         /* for some macros  */

/*
 *  commResult  : Gateway와 통신 결과를 저장하는 변수
 *  thingState  : 현재 thing의 상태를 저장하는 변수
 *  connection  : Gateway와 연결 여부를 저장하는 변수
 */
size_t commResult, thingState, connection;

void setup(void)
{
    Serial.begin(9600);

    /*
     *  Serial default setting
     *  pinMode(0, INPUT);          RX
     *  pinMode(1, OUTPUT);         TX
     */

    pinMode(2, OUTPUT);     /*  HEAT_R  */
    pinMode(3, OUTPUT);     /*  Not used    */
    pinMode(4, OUTPUT);     /*  Not used    */
    pinMode(5, INPUT);      /*  GREEN LED   */
    pinMode(6, INPUT);      /*  YELLOW LED  */
    pinMode(7, INPUT);      /*  RED LED */
    pinMode(8, OUTPUT);     /*  Not used    */
    pinMode(9, OUTPUT);     /*  Not used    */
    pinMode(10, OUTPUT);    /*  Not used    */
    pinMode(11, OUTPUT);    /*  Not used    */
    pinMode(12, OUTPUT);    /*  Not used    */
    pinMode(13, OUTPUT);    /*  Not used    */
    pinMode(14, INPUT);     /*  A0, ADC0, for test sensor   */
    pinMode(15, INPUT);     /*  A1, ADC1, for measuring sensor  */
    pinMode(16, OUTPUT);    /*  Not used    */
    pinMode(17, OUTPUT);    /*  Not used    */
    pinMode(18, OUTPUT);    /*  SoftwareSerial RX(twisted)  */
    pinMode(19, INPUT);     /*  SoftwareSerial TX(twisted)  */
}

void loop(void)
{
    prepareSensor();
    delay(5000);    /*  5 seconds wait  */
    sensing();
    delay(5000);    /*  5 seconds wait  */
    testing();
    delay(10000);   /*  10 seconds wait */
}
