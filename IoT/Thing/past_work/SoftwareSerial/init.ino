#include "kem.h"
#include "init.h"
#include "rf.h"         /*  for setupRF()   */
#include "util.h"       /*  for getSerialId()   */
#include "interrupt.h"  /*  for enableSleep(), enableWatchdog(), ...;   */
#include "sensor.h"     /*  for prepareSensor(), ...    */

/*
 *  thingID             : thing의 ID를 저장하는 변수(0~63)
 *  thingChannel        : thing의 Channel을 저장하는 변수(0~3)
 */
uint8_t thingID, thingChannel;

uint8_t initThing(void)
{
    setPinMode();
    setChannel();
    setID();
    setupRF();
    getSerialId();
    clearInterrupt();
    setWatchdog();
    enableSleep();
    setInterrupt();
    if (IDAndChannelIsInvalid()) {

        return ERROR;
    } else if (checkCRCIsInvalid()) {

        return ERROR;
    } else {
        prepareSensor();    
        setCRC();   /*  set CRC value 0xFF before start */
#ifdef COMM
        thingState  = REQUEST_CONNECTION;
        commResult  = INIT;
#else
        thingState  = NORMAL;
        commResult  = OK;
#endif

        return OK;
    }
}

void setPinMode(void)
{
#ifdef SOFTWARESERIAL_MODE
    setPinMode1();
#else
    setPinMode2();
#endif

    return;
}

#ifdef SOFTWARESERIAL_MODE
void setPinMode1(void)
{
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

    return;
}

#else
void setPinMode2(void)
{
    /*
     *  Serial default setting
     *  pinMode(0, INPUT);          RX
     *  pinMode(1, OUTPUT);         TX
     */

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
    pinMode(16, OUTPUT);    /*  LED for debugging   */
    pinMode(17, INPUT);     /*  Set ID, bit 1   */
    pinMode(18, OUTPUT);    /*  Not used    */
    pinMode(19, OUTPUT);    /*  Not used    */

    return;
}
#endif

void setID(void)
{
#ifdef TEST_COMM
    setIDA();
#elif defined SOFTWARESERIAL_MODE
    setID1();
#else
    setID2();
#endif

    return;
}

#ifdef TEST_COMM
void setIDA(void)
{
    static const char *func = INIT_SETID;
    char buf[SIZE_BUF];
    
    thingID = TEST_ID;
    sprintf(buf, "Test mode, Thing ID: %u", thingID);
    putsForDebug(__INIT__, func, buf);

    return;
}

#elif defined SOFTWARESERIAL_MODE
void setID1(void)
{
    static const char   *func = INIT_SETID;
    char                buf[SIZE_BUF];
    uint8_t             temp;
    
    temp    = digitalRead(ID_0);
    temp    <<= 1;
    temp    += digitalRead(ID_1);
    temp    <<= 1;
    temp    += digitalRead(ID_2);
    temp    <<= 1;
    temp    += digitalRead(ID_3);
    temp    <<= 1;
    temp    += digitalRead(ID_4);
    temp    <<= 1;
    temp    += digitalRead(ID_5);
    thingID = temp;
    sprintf(buf, "Thing ID: %u", thingID);
    putsForDebug(__INIT__, func, buf);
    
    return;
}

#else
void setID2(void)
{
    uint8_t temp;

    temp    = digitalRead(ID_0);
    temp    <<= 1;
    temp    += digitalRead(ID_1);
    temp    <<= 1;
    temp    += digitalRead(ID_2);
    temp    <<= 1;
    temp    += digitalRead(ID_3);
    temp    <<= 1;
    temp    += digitalRead(ID_4);
    temp    <<= 1;
    temp    += digitalRead(ID_5);
    thingID = temp;

    return;
}
#endif

void setChannel(void)
{
#ifdef TEST_COMM
    setChannelA();
#elif defined SOFTWARESERIAL_MODE
    setChannel1();
#else
    setChannel2();
#endif

    return;
}

#ifdef TEST_COMM
void setChannelA(void)
{
    static const char   *func = INIT_SETCHANNEL;
    char buf[SIZE_BUF];
    
    thingChannel = TEST_CHANNEL;
    sprintf(buf, "Test mode, Thing's channel: %u", thingChannel);
    putsForDebug(__INIT__, func, buf);

    return;
}

#elif defined SOFTWARESERIAL_MODE
void setChannel1(void)
{
    static const char   *func = INIT_SETCHANNEL;
    char                buf[SIZE_BUF];
    uint8_t             temp;
    
    temp            = digitalRead(CHANNEL_0);
    temp            <<= 1;
    temp            += digitalRead(CHANNEL_1);
    thingChannel    = temp;
    sprintf(buf, "Thing's channel: %u", thingChannel);
    putsForDebug(__INIT__, func, buf);

    return;
}

#else
void setChannel2(void)
{
    uint8_t temp;

    temp            = digitalRead(CHANNEL_0);
    temp            <<= 1;
    temp            += digitalRead(CHANNEL_1);
    thingChannel    = temp;

    return;
}
#endif

uint8_t IDAndChannelIsInvalid(void)
{
#ifdef SOFTWARESERIAL_MODE

    return IDAndChannelIsInvalid1();
#else

    return IDAndChannelIsInvalid2();
#endif
}

#ifdef SOFTWARESERIAL_MODE
uint8_t IDAndChannelIsInvalid1(void)
{
    static const char *func = INIT_IDANDCHANNELISINVALID1;

    putsForDebug(__INIT__, func, MSG_START);
    if ((thingID < 0) && (thingID >= MAX_ID)) {
        putsForDebug(__INIT__, func, MSG_INVALID_ID);

        return ERROR;
    }
    if ((thingChannel < 0) && (thingChannel >= MAX_CHANNEL)) {
        putsForDebug(__INIT__, func, MSG_INVALID_CHANNEL);

        return ERROR;
    }

    return OK;
}

#else
uint8_t IDAndChannelIsInvalid2(void)
{
    if ((thingID < 0) && (thingID >= MAX_ID)) {

        return ERROR;
    }
    if ((thingChannel < 0) && (thingChannel >= MAX_CHANNEL)) {

        return ERROR;
    }

    return OK;
}
#endif
