#include "kem.h"
#include "init.h"
#include "rf.h"         /*  for setupRF()   */
#include "util.h"       /*  for getSerialId()   */
#include "interrupt.h"  /*  for enableSleep(), enableWatchdog(), ...;   */
#include "sensor.h"     /*  for prepareSensor(), ...    */

/*
 *  tempDbg[SIZE_BUF](kem.ino)  : Debug message를 담는 배열
 *
 *  thingID                     : thing의 ID를 저장하는 변수(0~63)
 *  thingChannel                : thing의 Channel을 저장하는 변수(0~3)
 */
extern uint8_t  serialId[SIZE_ID];

uint8_t         thingID, thingChannel;

uint8_t         initThing(void)
{
#ifndef KEM_THING_MODE
    SHUT_DOWN;
#elif defined TEST_RF
    setupRF();
    enableSleep();
#else
    setupRF();
    setPinMode();
    setChannel();
    setID();
    getSerialId();
    enableSleep();
    if (IDAndChannelIsInvalid()) {

        return ERROR;
    } else if (checkCRCIsInvalid()) {

        return ERROR;
    } else {
        checkBattery();
        prepareSensor();    
        setCRC();   /*  set CRC value 0xFF before start */
        thingState  = REQUEST_CONNECTION;
        commResult  = INIT;

        return OK;
    }
#endif
}

void setPinMode(void)
{
    /*  0, 1: default serial pin    */
    pinMode(2, OUTPUT);     /*  HEAT_R  */
    pinMode(3, INPUT);      /*  Set Channel, bit 0  */
    pinMode(4, INPUT);      /*  Set Channel, bit 1  */
    pinMode(5, INPUT);      /*  Set ID, bit 0   */
    pinMode(6, INPUT);      /*  Set ID, bit 1   */
    pinMode(7, INPUT);      /*  Set ID, bit 2   */
    pinMode(8, INPUT);      /*  Set ID, bit 3   */
    pinMode(9, INPUT);      /*  Set ID, bit 4   */
    pinMode(10, INPUT);     /*  Set ID, bit 5   */
    pinMode(11, OUTPUT);    /*  Not used    */
    pinMode(12, OUTPUT);    /*  Not used    */
    pinMode(13, OUTPUT);    /*  Not used    */
    pinMode(14, INPUT);     /*  A0, ADC0, for test sensor   */
    pinMode(15, INPUT);     /*  A1, ADC1, for measuring sensor  */
    pinMode(16, OUTPUT);    /*  LED1 for debugging  */
    pinMode(17, OUTPUT);    /*  RELAY   */
    pinMode(18, OUTPUT);    /*  LED2 for debugging  */
    pinMode(19, OUTPUT);    /*  Buzzer  */
    pinMode(20, INPUT);     /*  Analog voltage in   */
    digitalWrite(HEATING, LOW);   
    digitalWrite(STATUS0, LOW);  
    digitalWrite(STATUS1, LOW);  
    digitalWrite(BUZZER, LOW);  
    digitalWrite(RELAY, LOW);  

    return;
}

void setID(void)
{
    thingID    = digitalRead(ID_0);
    thingID    <<= 1;
    thingID    += digitalRead(ID_1);
    thingID    <<= 1;
    thingID    += digitalRead(ID_2);
    thingID    <<= 1;
    thingID    += digitalRead(ID_3);
    thingID    <<= 1;
    thingID    += digitalRead(ID_4);
    thingID    <<= 1;
    thingID    += digitalRead(ID_5);
    
    return;
}

void setChannel(void)
{
    thingChannel    = digitalRead(CHANNEL_0);
    thingChannel    <<= 1;
    thingChannel    += digitalRead(CHANNEL_1);

    return;
}

uint8_t IDAndChannelIsInvalid(void)
{
    static const char   *func = "[uint8_t IDAndChannelIsInvalid(void)]";
    
    sprintf(tempDbg, "ID: %u, Channel: %u", thingID, thingChannel);
    putsForDebug(__INIT__, func, tempDbg);
    if ((thingID < 0) && (thingID >= MAX_ID)) {
        sprintf(tempDbg, "%x", MSG_INVALID_ID);
        putsForDebug(__INIT__, func, tempDbg);

        return ERROR;
    }
    if ((thingChannel < 0) && (thingChannel >= MAX_CHANNEL)) {
        sprintf(tempDbg, "%x", MSG_INVALID_CHANNEL);
        putsForDebug(__INIT__, func, tempDbg);

        return ERROR;
    }

    return OK;
}
