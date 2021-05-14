#include "kem.h"
#include "util.h"
#include "rf.h"

/*
 *  commResult(kem.ino)         : Gateway와 통신 결과를 저장하는 변수
 *  thingState(kem.ino)         : 현재 thing의 상태를 저장하는 변수
 *  thingID(init.ino)           : thing의 ID를 저장하는 변수(0~63)
 *  thingChannel(init.ino)      : thing의 Channel을 저장하는 변수(0~3)
 *  testResult(sensor.ino)      : 감지기 test 수행 결과를 저장하는 변수
 *  averageValue(sensor.ino)    : buffer에 저장된 값이 가득 찰 경우, buffer의 평균값을 저장하는 변수
 *  currValue(sensor.ino)       : 테스트 수행 중에 감지기에 측정된 온도를 저장하는 변수
 *  currTestValue(sensor.ino)   : 테스트 수행 중에 테스트 감지기에 측정된 온도를 저장하는 변수
 *  serialId[SIZE_ID](util.ino) : Thing의 고유 번호가 저장되어 있는 배열
 *  tempDbg[SIZE_BUF](kem.ino)  : Debug message를 담는 배열
 *
 *  tx[SIZE]                    : 전송할 frame
 *  rx[SIZE]                    : 전송받은 frame
 *  recvLen                     : 전송받은 frame의 총 길이
 */
extern uint8_t  commResult, thingState;
extern uint8_t  thingID, thingChannel;
extern uint8_t  testResult, averageValue, currValue, currTestValue;
extern uint8_t  serialId[SIZE_ID];
extern char     tempDbg[SIZE_BUF];

static char tx[SIZE] = {0xAB, 0x01};
static char rx[SIZE];
static int  recvLen;

void setupRF(void)
{
    Serial.begin(9600);

    /*  set channel */
    Serial.print((char) 0xAB);
    Serial.print((char) 0x04);
    Serial.print((char) 0x01);
    Serial.print((char) thingChannel);
    delay(DELAY_CONF);

    /*  set tx power    */
    Serial.print((char) 0xAB);
    Serial.print((char) 0x05);
    Serial.print((char) 0x0A);
    delay(DELAY_CONF);

    /*  set data rate   */
    Serial.print((char) 0xAB);
    Serial.print((char) 0x06);
    Serial.print((char) 0x02);
    delay(DELAY_CONF);

    return;
}

void communicate(void)
{
    prepareFrame();
    sendAndRecv();
    parseFrame();
    sleep(MILSEC_500);
    
    return;
}

void testCommunication(void)
{
    int         i;
    char        TEST_DATA[] = {0xAB, 0x01, 0x06, 0xEE, 0xEE, 0xEF, 0xEF, 0x00, 0xEF};
    static char *func       = "[void testCommunication(void)]";

    ENABLE_RF;
    for (i = 0; i < 9; ++i) {
        Serial.print(TEST_DATA[i]);
    }
    DISABLE_RF;
    delay(DELAY_COMM);
    for (i = 0; Serial.available(); ++i) {
        rx[i] = Serial.read();
    }
    sprintf(tempDbg, "%u bytes received", i);
    putsForDebug(__RF__, func, tempDbg);
/*
    sprintf(tempDbg, 
        "Frame: %x %x %x %x %x %x %x %x %x",
        TEST_DATA[0],
        TEST_DATA[1],
        TEST_DATA[2],
        TEST_DATA[3],
        TEST_DATA[4],
        TEST_DATA[5],
        TEST_DATA[6],
        TEST_DATA[7],
        TEST_DATA[8]);
    putsForDebug(__RF__, func, tempDbg);
    sprintf(tempDbg,
        "Received frame: %x %x %x %x", 
        rx[0], rx[1], rx[2], rx[3]);
    putsForDebug(__RF__, func, tempDbg);
*/
    sleep(MILSEC_500);

    return;
}

void prepareFrame(void)
{
    int         index;

    index       = FRAME_START_OFFSET;
    switch (thingState) {
    case (REQUEST_CONNECTION + 0): 
    case (REQUEST_CONNECTION + 1): 
    case (REQUEST_CONNECTION + 2): 
        tx[index++] = LENGTH_WITH_SERIALID;
        tx[index++] = thingID;
        tx[index++] = CMD_REQUEST_CONNECTION + (thingState & 03);
        /*  thingState == 0101 1000 */
        if (!(thingState & 03)) {     
            tx[index++] = serialId[0];
            tx[index++] = serialId[1];
            tx[index++] = serialId[2];
        /*  thingState == 0101 1001 */
        } else if (thingState & 01) {   
            tx[index++] = serialId[3];
            tx[index++] = serialId[4];
            tx[index++] = serialId[5];
        /*  thingState == 0101 1010 */
        } else {                        
            tx[index++] = serialId[6];
            tx[index++] = serialId[7];
            tx[index++] = serialId[8];
            initCRC();
        }
        break;
    case NOTIFY_FIRE_DETECTED:
        tx[index++] = LENGTH_WITH_DATA;
        tx[index++] = thingID;
        tx[index++] = CMD_NOTIFY_FIRE_DETECTED;
        /*  
         *  averageValue 값은 평소엔 0, 버퍼에 값이 꽉 찼을 경우에만 0이 아닌 값으로 바뀐다.
         *  따라서, 값이 0일 경우엔 불이 났다는 정보만 처리하고,
         *  0이 아닐 경우엔 불이 났다는 정보와 동시에 DB에 해당 값을 저장해야 한다. 
         */
        tx[index++]     = averageValue;
        averageValue    = 0;
        break;
    /*  by init */
    case INIT_TEST_COMMAND:
        tx[index++] = LENGTH_WITH_TEST_RESULT;
        tx[index++] = thingID;
        /*  성공했을 경우 testResult: 0, 그 외: 2, 3, 4, 5, 6, 7  */
        if (testResult) {
#ifdef HEAT_MODE
            /*  00, 01   */
            tx[index++] = CMD_SEND_INIT_TEST_RESULT + (testResult & 01);
#else
            /*  00, 01, 10, 11  */
            tx[index++] = CMD_SEND_INIT_TEST_RESULT + (testResult & 03);
#endif
        } else {
            tx[index++] = CMD_SEND_INIT_TEST_RESULT;
        }
        tx[index++] = currValue;
        tx[index++] = currTestValue;
        break;
    /*  by period   */
    case (RECEIVED_TEST_COMMAND + 0):   
        tx[index++] = LENGTH_WITH_TEST_RESULT;
        tx[index++] = thingID;
        /*  성공했을 경우 testResult: 0, 그 외: 2, 3, 4, 5, 6, 7  */
        if (testResult) {
#ifdef HEAT_MODE
            /*  00, 01   */
            tx[index++] = CMD_SEND_TEST_RESULT + (testResult & 01);
#else
            /*  00, 01, 10, 11  */
            tx[index++] = CMD_SEND_TEST_RESULT + (testResult & 03);
#endif
        } else {
            tx[index++] = CMD_SEND_TEST_RESULT;
        }
        tx[index++] = currValue;
        tx[index++] = currTestValue;
        break;
    /*  by user */
    case (RECEIVED_TEST_COMMAND + 1):   
        tx[index++] = LENGTH_WITH_TEST_RESULT;
        tx[index++] = thingID;
        /*  성공했을 경우 testResult: 0, 그 외: 2, 3, 4, 5, 6, 7  */
        if (testResult) {
#ifdef HEAT_MODE
            /*  00, 01   */
            tx[index++] = CMD_SEND_REQUESTED_TEST_RESULT + (testResult & 01);
#else
            /*  00, 01, 10, 11  */
            tx[index++] = CMD_SEND_REQUESTED_TEST_RESULT + (testResult & 03);
#endif
        } else {
            tx[index++] = CMD_SEND_REQUESTED_TEST_RESULT;
        }
        tx[index++] = currValue;
        tx[index++] = currTestValue;
        break;
    case SEND_A_AVERAGE_DATA:
        tx[index++] = LENGTH_WITH_DATA;
        tx[index++] = thingID;
        tx[index++] = CMD_SEND_A_AVERAGE_DATA;
        tx[index++] = averageValue;
        averageValue = 0;
        break;
    /*  TODO: need to implementation    */
    /*
    case SEND_BATTERY_VOLTAGE:
        break;
    */
    case NORMAL:
        tx[index++] = LENGTH;
        tx[index++] = thingID;
        tx[index++] = CMD_CHECK_COMMAND;
        break;
    default:
        break;
    }
    tx[index]   = (char) updateCRC((tx + FRAME_START_OFFSET), tx[FRAME_START_OFFSET]);

    return;
}

void sendAndRecv(void)
{
    int len, i;

    /*  0xAB | 0x01 | LEN | n(LEN)  */
    len = FRAME_START_OFFSET + 1 + tx[FRAME_START_OFFSET];
    ENABLE_RF;
    for (i = 0; i < len; ++i) {
        Serial.print(tx[i]);
    }
    DISABLE_RF;
    delay(DELAY_COMM);
    for (i = 0; Serial.available(); ++i) {
        rx[i] = Serial.read();
    }
    recvLen = i;

    return;
}

void parseFrame(void)
{       
    static char *func       = "[void parseFrame(void)]";
    int         i, index;
    uint8_t     id, cmd, addrFrom, addrTo;

    index = 0;
    sprintf(tempDbg, "%u bytes received", recvLen);
    putsForDebug(__RF__, func, tempDbg);
    if (!recvLen) {
        sprintf(tempDbg, "%x", MSG_NOT_RECEIVED);
        putsForDebug(__RF__, func, tempDbg);
        commResult = ERROR;
    } else {
        while (1) {
            /*  RF module buffer에 들어 있던 데이터의 길이 내에서 해당 ID의 frame을 못 찾을 경우    */
            if (index > recvLen) {
                sprintf(tempDbg, "%x", MSG_NOT_RECEIVED);
                putsForDebug(__RF__, func, tempDbg);
                commResult = ERROR;
                break;
            }
            id = rx[index + RX_ID];
            /*  check this frame is mine    */
            if (id == thingID) {    
                sprintf(tempDbg, "%x", MSG_RECEIVED);
                putsForDebug(__RF__, func, tempDbg);
                break;  
            } else {
                addrFrom    = index;
                index       += RECV_LEN_FROM_RASP;
                addrTo      = index;
                sprintf(tempDbg, "Pass this frame, from %u to %u", addrFrom, addrTo);
                putsForDebug(__RF__, func, tempDbg);
            }
        }
        /*  update len from buffer length to frame length   */
        cmd = rx[index + RX_CMD];
        /*  need to preprocess  */
        cmd &= PREPROCESS;  
        if (cmd == CMD_REQUEST_RE_CONNECTION) {
            /*  set CRC value 0xFF for re-connection  */
            setCRC();       
            thingState  = REQUEST_CONNECTION;
            commResult  = INIT;
        } else {
            /*  checkCRC()에서 오류가 발생했을 경우 */
            if (checkCRC((rx + index), RECV_LEN_FROM_RASP)) {
                sprintf(tempDbg, "%x", MSG_CRC_ERROR);
                putsForDebug(__RF__, func, tempDbg);
                commResult  = ERROR;
            /*  checkCRC()가 통과했을 경우  */
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
                sprintf(tempDbg, "%x", MSG_CRC_OK);
                putsForDebug(__RF__, func, tempDbg);
                /*  check cmd is correct    */
                if (cmd & CHECK_ERROR) {    
                    sprintf(tempDbg, "%x", MSG_GATEWAY_ERROR);
                    putsForDebug(__RF__, func, tempDbg);
                    commResult  = ERROR;
                } else {
                    sprintf(tempDbg, "%x", MSG_GATEWAY_OK);
                    putsForDebug(__RF__, func, tempDbg);
                    sprintf(tempDbg, "Cmd from the frame: %x", cmd);
                    putsForDebug(__RF__, func, tempDbg);
                    switch (cmd & PREPROCESS) {
                    case (CMD_REQUEST_CONNECTION + 0):
                        /*  thingState = (REQUEST_CONNECTION + 1);  */
                        ++thingState;   
                        commResult  = INIT;
                        break;
                    case (CMD_REQUEST_CONNECTION + 1):
                        /*  thingState = (REQUEST_CONNECTION + 2);  */
                        ++thingState;   
                        commResult  = INIT;
                        break;
                    case (CMD_REQUEST_CONNECTION + 2):
                        /*  Test after initializing the thing   */
                        thingState  = INIT_TEST_COMMAND;   
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
                    case (CMD_SEND_TEST_RESULT + 7):
                    case (CMD_SEND_REQUESTED_TEST_RESULT + 0):
                    case (CMD_SEND_REQUESTED_TEST_RESULT + 2):
                    case (CMD_SEND_REQUESTED_TEST_RESULT + 3):
                    case (CMD_SEND_REQUESTED_TEST_RESULT + 4):
                    case (CMD_SEND_REQUESTED_TEST_RESULT + 5):
                    case (CMD_SEND_REQUESTED_TEST_RESULT + 6):
                    case (CMD_SEND_REQUESTED_TEST_RESULT + 7):
                    case CMD_SEND_A_AVERAGE_DATA:
                        thingState  = NORMAL;
                        commResult  = OK;
                        break;
                    case CMD_CHECK_COMMAND:
                        //  TODO: need to implementation
                        commResult  = OK;
                        break;
                    default:
                        sprintf(tempDbg, "%x", MSG_INVALID_CMD);
                        putsForDebug(__RF__, func, tempDbg);
                        commResult = ERROR;
                        break;
                    }
                }
            }
        }
    }

    return;
}
