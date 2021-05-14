#include "kem.h"
#include "util.h"
#include "rf.h"

#ifdef SOFTWARESERIAL_MODE
#include <SoftwareSerial.h> /*  for RF Module RX-TX */
SoftwareSerial RF(A5, A4);  /*  rx, tx, ADC4 - RX, ADC5 - TX와 크로스 케이블되어야 함   */
#endif

/*
 *  commResult(kem.ino)         : Gateway와 통신 결과를 저장하는 변수
 *  thingState(kem.ino)         : 현재 thing의 상태를 저장하는 변수
 *  thingID(init.ino)           : thing의 ID를 저장하는 변수(0~63)
 *  thingChannel(init.ino)      : thing의 Channel을 저장하는 변수(0~3)
 *  testResult(sensor.ino)      : 감지기 test 수행 결과를 저장하는 변수
 *  averageValue(sensor.ino)    : buffer에 저장된 값이 가득 찰 경우, buffer의 평균값을 저장하는 변수
 *  currValue(sensor.ino)       : 화재가 감지되었을 경우, 감지기에 측정된 온도를 저장하는 변수
 *  currTestValue(sensor.ino)   : 테스트 수행 중에 테스트 감지기에 측정된 온도를 저장하는 변수
 *  serialId[SIZE_ID](util.ino) : Thing의 고유 번호가 저장되어 있는 배열
 */
extern uint8_t  commResult, thingState;
extern uint8_t  thingID, thingChannel;
extern uint8_t  testResult, averageValue, currValue, currTestValue;
extern uint8_t  serialId[SIZE_ID];

void setupRF(void)
{
    uint8_t     i;
    char        cmdSetChannel[4]    = {0xAB, 0x04, 0x01};
    char        cmdSetTxPower[3]    = {0xAB, 0x05, 0x0A};
    char        cmdSetDataRate[3]   = {0xAB, 0x06, 0x02};
    char        cmdOff[2]           = {0xAB, 0x02};
    
    RF.begin(9600);
    RF.flush();
    *(cmdSetChannel + SET_CHANNEL_OFFSET) = thingChannel;   /*  set channel */
    for (i = 0; i < 4; i++) {
        RF.print(*(cmdSetChannel + i));
    }
    for (i = 0; i < 3; i++) {
        RF.print(*(cmdSetTxPower + i));
    }
    for (i = 0; i < 3; i++) {
        RF.print(*(cmdSetDataRate + i));
    }
    for (i = 0; i < 2; i++) {
        RF.print(*(cmdOff + i));
    }

    return;
}

void sendMsg(void)
{
#ifdef TEST_COMM
    sendMsgA();
#elif defined COMM
    sendMsgB();
#elif defined TEST_SERIAL_COMM
    sendMsgC();
#elif defined SERIAL_COMM
    sendMsgE();
#endif

    return OK;
}

#ifdef TEST_COMM
void sendMsgA(void)
{
    char        txTestData1[6]  = {0xAB, 0x01, 0x03, 0xEE, 0xE0, 0xEA};
    char        txTestData2[13] = {0xAB, 0x01, 0x0A, 0xEE, 0xE0, 0xEE, 0xAA, 0xAB, 0x00, 0x08, 0x07, 0x09, 0xEA};
    uint8_t     i;

    for (i = 0; i < 13; ++i) {
        RF.print(*(txTestData2 + i));
    }   
    delay(1200);
    /*  delay(175); : 175 (ms)에서도 동작하나, 보다 안정적으로 동작할 수 있도록 200 (ms) 간격으로 수행하도록 작성   */
    /*
    for (i = 0; i < 6; ++i) {
        RF.print(txTestData1[i]);
    }   
    delay(200);
    */

    return;
}

#elif defined COMM
void sendMsgB(void)
{
    static char *func       = RF_SENDMSGB;
    char        tx[SIZE]    = {0xAB, 0x01};
    char        *pTx;
    uint8_t     len, data, crc, i;
    char        buf[SIZE_BUF];

    putsForDebug(__RF__, func, MSG_START);
    pTx = (tx + PTX_AMEND_OFFSET);
    len = 0;
    sprintf(buf, "Thing state: %u", thingState);
    putsForDebug(__RF__, func, buf);
    switch (thingState) {
    case (REQUEST_CONNECTION + 0): 
    case (REQUEST_CONNECTION + 1): 
    case (REQUEST_CONNECTION + 2): 
        *pTx++  = LENGTH_WITH_SERIALID;
        *pTx++  = thingID;
        *pTx++  = CMD_REQUEST_CONNECTION + (thingState & 03);
        if (!(thingState & 03)) {     /*  thingState == 0101 1000 */
            *pTx++  = *(serialId + 0);
            *pTx++  = *(serialId + 1);
            *pTx++  = *(serialId + 2);
        } else if (thingState & 01) {   /*  thingState == 0101 1001 */
            *pTx++  = *(serialId + 3);
            *pTx++  = *(serialId + 4);
            *pTx++  = *(serialId + 5);
        } else {                        /*  thingState == 0101 1010 */
            *pTx++  = *(serialId + 6);
            *pTx++  = *(serialId + 7);
            *pTx++  = *(serialId + 8);
            initCRC();
        }
        break;
    case NOTIFY_FIRE_DETECTED:
        *pTx++  = LENGTH_WITH_DATA;
        *pTx++  = thingID;
        *pTx++  = CMD_NOTIFY_FIRE_DETECTED;
        *pTx++  = currValue;
        break;
    case INIT_TEST_COMMAND:
        *pTx++  = LENGTH_WITH_TEST_RESULT;
        *pTx++  = thingID;
        if (testResult) {
#ifdef HEAT_MODE
            *pTx++ = CMD_SEND_INIT_TEST_RESULT + (testResult & 01); /*  00, 01   */
#else
            *pTx++ = CMD_SEND_INIT_TEST_RESULT + (testResult & 03); /*  00, 01, 10, 11  */
#endif
        } else {
            *pTx++ = CMD_SEND_INIT_TEST_RESULT;
        }
        *pTx++  = currValue;
        *pTx++  = currTestValue;
        break;
    case (RECEIVED_TEST_COMMAND + 0):   /*  by period   */
        *pTx++  = LENGTH_WITH_TEST_RESULT;
        *pTx++  = thingID;
        if (testResult) {
#ifdef HEAT_MODE
            *pTx++ = CMD_SEND_TEST_RESULT + (testResult & 01); /*  00, 01   */
#else
            *pTx++ = CMD_SEND_TEST_RESULT + (testResult & 03); /*  00, 01, 10, 11  */
#endif
        } else {
            *pTx++ = CMD_SEND_TEST_RESULT;
        }
        *pTx++  = currValue;
        *pTx++  = currTestValue;
        break;
    case (RECEIVED_TEST_COMMAND + 1):   /*  by user */
        *pTx++  = LENGTH_WITH_TEST_RESULT;
        *pTx++  = thingID;
        if (testResult) {
#ifdef HEAT_MODE
            *pTx++ = CMD_SEND_REQUESTED_TEST_RESULT + (testResult & 01); /*  00, 01   */
#else
            *pTx++ = CMD_SEND_REQUESTED_TEST_RESULT + (testResult & 03); /*  00, 01, 10, 11  */
#endif
        } else {
            *pTx++ = CMD_SEND_REQUESTED_TEST_RESULT;
        }
        *pTx++  = currValue;
        *pTx++  = currTestValue;
        break;
    case SEND_A_AVERAGE_DATA:
        *pTx++  = LENGTH_WITH_DATA;
        *pTx++  = thingID;
        *pTx++  = CMD_SEND_A_AVERAGE_DATA;
        *pTx++  = averageValue;
        break;
    case NORMAL:
        *pTx++  = LENGTH;
        *pTx++  = thingID;
        *pTx++  = CMD_CHECK_COMMAND;
        break;
    default:
        break;
    }
    len     = *(tx + PTX_AMEND_OFFSET);
    *pTx    = updateCRC((tx + PTX_AMEND_OFFSET), len);
    len     += TX_LENGTH_AMEND_OFFSET;  /*  total length of tx  */
    for (i = 0; i < len; ++i) {
        RF.print(*(tx + i));
        sprintf(buf, "Index %u: %x", i, *(tx + i));
        putsForDebug(__RF__, func, buf);
    }   

    return;
}

#elif defined TEST_SERIAL_COMM
void sendMsgC(void)
{
    uint8_t i;
    char    txTestData[6]    = {0xAB, 0x01, 0x03, 0xEE, 0xE0, 0xEA};

    for (i = 0; i < 6; ++i) {
        Serial.print(*(txTestData + i));
    }   

    return;
}

#else
//TODO: need to implementation
void sendMsgD(void)
{

    return;
}
#endif

void recvMsg(void)
{
#ifdef TEST_COMM
    recvMsgA();
#elif defined COMM
    recvMsgB();
#elif defined TEST_SERIAL_COMM
    recvMsgC();
#elif defined SERIAL_COMM
    recvMsgD();
#endif

    return OK;
}

#ifdef TEST_COMM
void recvMsgA(void)
{
    char        buf[SIZE_BUF];
    uint8_t     len;

    for (len = 0; RF.available(); ++len) {
        sprintf(buf, "Index %u: %x", len, *(buf + len) = RF.read());
        putsForDebug(__RF__, func, buf);
    }

    return;
}

#elif defined COMM
void recvMsgB(void)
{       
    /*  
     *  process
     *  1. Set pRx pointer
     *  2. get message from the module buffer
     *  3. ID check
     *      3-1. ID is mine: go to the next process
     *      3-2. ID is not mine: erase this frame and go to the 1.
     *  4. RF module off
     */
    static char *func       = RF_RECVMSGB;
    char        cmdOff[2]   = {0xAB, 0x02};
    char        cmdOn[2]    = {0xAB, 0x03};
    char        buf[SIZE_BUF];
    char        rx[SIZE];
    char        *pRx;
    uint8_t     id, len, cmd, i, addrFrom, addrTo;

    putsForDebug(__RF__, func, MSG_START);
    pRx = rx;
    for (len = 0; RF.available(); ++len) {
        *(rx + len) = RF.read();
        sprintf(buf, "Index %u: %x", len, *(rx + len));
        putsForDebug(__RF__, func, buf);
    }
    sprintf(buf, "%u bytes received!", len);
    putsForDebug(__RF__, func, buf);
    for (i = 0; i < RF_MODULE_OFF_SIZE; ++i) {
        RF.print(*(cmdOff + i));
    }
    putsForDebug(__RF__, func, MSG_RF_OFF);
    if (!len) {
        putsForDebug(__RF__, func, MSG_NOT_RECEIVED);
        commResult = ERROR;
    } else {
        while (1) {
            /*  RF module buffer에 들어 있던 데이터의 길이 내에서 해당 ID의 frame을 못 찾을 경우    */
            if ((pRx - rx) > len) {
                putsForDebug(__RF__, func, MSG_NOT_RECEIVED);
                commResult = ERROR;
                break;
            }
            id  = *(pRx + RX_ID);
            if (id == thingID) {    /*  check this frame is mine    */
                putsForDebug(__RF__, func, MSG_RECEIVED);
                break;  
            } else {
                addrFrom    = pRx;
                pRx         += RECV_LEN_FROM_RASP;
                addrTo      = pRx;
                sprintf(buf, "Pass this frame, from %x to %x", addrFrom, addrTo);
                putsForDebug(__RF__, func, buf);
            }
        }
        len = *pRx;         /*  update len from buffer length to frame length   */
        cmd = *(pRx + RX_CMD);
        cmd &= PREPROCESS;  /*  need to preprocess  */
        if (cmd == CMD_REQUEST_RE_CONNECTION) {
            setCRC();       /*  set CRC value 0xFF for re-connection  */
            thingState  = REQUEST_CONNECTION;
            commResult  = INIT;
        } else {
            /*  가독성 향상 위해, if (checkCRC(pRx, len)) { 아래 문장으로 대체함    */ 
            if (checkCRC(pRx, len) == ERROR) {  /*  check crc is correct    */
                putsForDebug(__RF__, func, MSG_CRC_ERROR);
                commResult  = ERROR;
            } else {
                /*
                 *  이 부분에 Bit operator를 사용하여 길이나 값을 맞추었었는데, 원리대로라면 마스킹 없이 값 비교가 되어야 한다.
                 *  지금은 마스킹 연산을 모두 제외하고, 순수 값 비교 연산만을 넣었는데, 아마도 오류가 날 것 같다.
                 *  통신이 원활이 되지 않는다면 이 부분 문제일 가능성이 크므로, 이러한 코멘트를 남긴다. 
                 *  오류가 발생할 경우 값이 어떻게 나오는지 찍어보고, 값을 마스킹으로 바꾸거나, 비교 연산을 수정해야 한다.  
                 *  만약, 오류가 발생하지 않는다면 rf.h 파일에 정의한 마스킹 관련 매크로를 모두 삭제해주기 바란다. 
                 *  디버깅 값
                 *      Serial.println(cmd);                    : 65488
                 *      Serial.println(cmd & CHECK_ERROR);      : 128
                 *      Serial.println(cmd & CHECK_RESPONSE);   : 208
                 *  값을 사용하기 전에, 전처리가 필요함!
                 */ 
                putsForDebug(__RF__, func, MSG_CRC_OK);
                if (cmd & CHECK_ERROR) {    /*  check cmd is correct    */
                    putsForDebug(__RF__, func, MSG_GATEWAY_ERROR);
                    commResult  = ERROR;
                } else {
                    putsForDebug(__RF__, func, MSG_GATEWAY_OK);
                    sprintf(buf, "Cmd from the frame: %x", cmd);
                    putsForDebug(__RF__, func, buf);
                    switch (cmd & PREPROCESS) {
                    case (CMD_REQUEST_CONNECTION + 0):
                        ++thingState;   /*  thingState = (REQUEST_CONNECTION + 1);  */
                        commResult  = INIT;
                        break;
                    case (CMD_REQUEST_CONNECTION + 1):
                        ++thingState;   /*  thingState = (REQUEST_CONNECTION + 2);  */
                        commResult  = INIT;
                        break;
                    case (CMD_REQUEST_CONNECTION + 2):
                        thingState  = INIT_TEST_COMMAND;   /*  Test after initializing the thing   */
                        commResult  = OK;
                        break;
                    case CMD_NOTIFY_FIRE_DETECTED:
                    case (CMD_SEND_INIT_TEST_RESULT + 0):
                    case (CMD_SEND_INIT_TEST_RESULT + 2):
                    case (CMD_SEND_INIT_TEST_RESULT + 3):
                    case (CMD_SEND_INIT_TEST_RESULT + 4):
                    case (CMD_SEND_INIT_TEST_RESULT + 5):
                    case (CMD_SEND_INIT_TEST_RESULT + 6):
                    case (CMD_SEND_INIT_TEST_RESULT + 7):
                    case (CMD_SEND_TEST_RESULT + 0):
                    case (CMD_SEND_TEST_RESULT + 2):
                    case (CMD_SEND_TEST_RESULT + 3):
                    case (CMD_SEND_TEST_RESULT + 4):
                    case (CMD_SEND_TEST_RESULT + 5):
                    case (CMD_SEND_TEST_RESULT + 6):
                    case (CMD_SEND_REQUESTED_TEST_RESULT + 0):
                    case (CMD_SEND_REQUESTED_TEST_RESULT + 2):
                    case (CMD_SEND_REQUESTED_TEST_RESULT + 3):
                    case (CMD_SEND_REQUESTED_TEST_RESULT + 4):
                    case (CMD_SEND_REQUESTED_TEST_RESULT + 5):
                    case (CMD_SEND_REQUESTED_TEST_RESULT + 6):
                    case CMD_SEND_A_AVERAGE_DATA:
                        thingState  = NORMAL;
                        commResult  = OK;
                        break;
                    case CMD_CHECK_COMMAND:
                        //  TODO: need to implementation
                        //  test by period, test requested by user, led, buzzer, led + buzzer
                        //  test 만 thingState에 기록, LED와 BUZZER는 단순 동작
                        // thingState  = (cmd & 01) ? RECEIVED_TEST_COMMAND : NORMAL;
                        commResult  = OK;
                        break;
                    default:
                        putsForDebug(__RF__, func, MSG_INVALID_CMD);
                        commResult = ERROR;
                        break;
                    }
                }
            }
        }
    }
    putsForDebug(__RF__, func, MSG_LINE);

    return;
}

#elif defined TEST_SERIAL_COMM
void recvMsgC(void)
{
    uint8_t i;

    for (i = 0; Serial.available(); ++i) {
        ;
    }
    while (i--) {
        digitalWrite(LED, HIGH);
        delay(500);
        digitalWrite(LED, LOW);
        delay(500);
    }

    return;
}
#else
//TODO: need to implementation
void recvMsgD(void)
{

    return;
}
#endif
