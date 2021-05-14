#include <stdio.h>
#include <string.h>     /*  memcpy  */
#include <stdint.h>     /*  uint8_t */
#include <termios.h>    /*  Used for UART, control terminal */
#include <unistd.h>     /*  Used for UART   */
#include <fcntl.h>      /*  Used for UART   */
#include <sys/types.h>  /*  size_t  */

#include "util.h"       /*  THING   */
#include "rf.h"
#include "crc.h"
#include "server.h"

/*
 *  fileStreamUART(rf.c)        : UART 통신에 사용하는 file stream 변수
 *  msg[SIZE](process.c)        : 디버깅 메세지를 담는 변수  
 *  thing[SIZE_THING](util.c)   : 사물에 대한 정보를 담는 자료구조, serialID, time, CRC, test, LED 멤버를 포함
 *  frame(server.c)             : server에 전송할 frame을 담는 변수
 */
static int      fileStreamUART;     /*  need a negative value for debug */
extern THING    thing[SIZE_THING];
extern char     msg[SIZE];
extern char     frame[SIZE];

extern int testTest;

size_t setupUART(void)
{
    static const char *func = RF_SETUPUART;
    struct  termios options;

    /* Init. value for debug    */
    fileStreamUART = (-1);
    /* Open in non blocking read/write mode */
    fileStreamUART = open(DEV_TARGET, O_RDWR | O_NOCTTY | O_NDELAY);    
    /* ERROR - CAN'T OPEN SERIAL PORT   */
    if (fileStreamUART < 0) {                           
        putsForDebug(__RF__, func, MSG_ERROR_SETUPUART);

        return 1;
    }
    tcgetattr(fileStreamUART, &options);
    /*
     *  options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
     *  ------------------------------------------------
     *  CSIZE   - CS5, CS6, CS7, CS8
     *  CLOCAL  - Ignore modem status lines 
     *  CREAD   - Enable receiver
     */
    options.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    /*  Ignore characters with parity errors    */
    options.c_iflag = IGNPAR;               
    /*  output mode flags   */
    options.c_oflag = 0;                
    /*  local mode falgs    */
    options.c_lflag = 0;                    
    /*  flush buffer    */
    tcflush(fileStreamUART, TCIFLUSH);          
    /*  9,600 bps, 8 bit data, no parity, 1 stop bit, non-blocking mode     */
    tcsetattr(fileStreamUART, TCSANOW, &options);       
    putsForDebug(__RF__, func, MSG_COMPLETE_SETUPUART);

    return 0;
}

size_t closeUART()
{
    static const char *func = RF_CLOSEUART;

    if (close(fileStreamUART)) {
        putsForDebug(__RF__, func, MSG_ERROR_CLOSEUART);
        
        return 1;
    }
    putsForDebug(__RF__, func, MSG_COMPLETE_CLOSEUART);

    return 0;
}

size_t setupRF()
{
    static const char *func = RF_SETUPRF;
    char    setChannel[4]   = {0xAB, 0x04, 0x01, 0x03};
    char    setTxPower[3]   = {0xAB, 0x05, 0x0A};
    char    setDataRate[3]  = {0xAB, 0x06, 0x02};

    if (write(fileStreamUART, setChannel, 4) < 0) {
        putsForDebug(__RF__, func, MSG_ERROR_SETUPRF1);
    
        return 1;
    }
    if (write(fileStreamUART, setTxPower, 3) < 0) {
        putsForDebug(__RF__, func, MSG_ERROR_SETUPRF2);

        return 1;
    }
    if (write(fileStreamUART, setDataRate, 3) < 0) {
        putsForDebug(__RF__, func, MSG_ERROR_SETUPRF3);

        return 1;
    }
    putsForDebug(__RF__, func, MSG_COMPLETE_SETUPRF);

    return 0;
}

void recvAndSendMsg(void)
{
#if defined ALPHA
    recvAndSendMsg1();
#elif defined BETA
    recvAndSendMsg2();
#else
    recvAndSendMsg3();
#endif

    return;
}

#if defined ALPHA
void recvAndSendMsg1(void)
{
    static const char *func = RF_RECVANDSENDMSG1;
    char    rx[SIZE];
    char    txTest[13]  = {0xAB, 0x01, 0x02, 0x01, 0x03, 0x05, 0x03, 0x01, 0x0E, 0x0A, 0x0E, 0x0F, 0xAA};
    int     rxLength;   /*  use a value from negative to positive   */
    size_t  i;

    /*  Read up to 255 characters from the port if they are there   */
    rxLength = read(fileStreamUART, (void *) rx, SIZE); //Filestream, buffer to store in, number of bytes to read (max)
    if (rxLength < 0) {
        /*  An error occured (will occur if there are no bytes) */
    } else if (rxLength == 0) {
        /*  No data waiting */
    } else {
        sprintf(msg, "%d %s", rxLength, MSG_RF_BYTES);
        putsForDebug(__RF__, func, msg);
        for (i = 0; i < rxLength; ++i) {
            sprintf(msg, "%s %lu: %x", MSG_RF_RX_INDEX, i, *(rx + i));
            putsForDebug(__RF__, func, msg);
        }
        if (write(fileStreamUART, txTest, 13) > 0) {
            putsForDebug(__RF__, func, MSG_LINE);
        }
        sleep(1);
        if (write(fileStreamUART, txTest, 13) > 0) {
            putsForDebug(__RF__, func, MSG_LINE);
        }
    }

    return;
}

#elif defined BETA
void recvAndSendMsg2(void)
{
    /*
     *  pTx         : tx의 포인터
     *  pRx         : rx의 포인터
     *  tx          : 전송할 frame을 담는 배열
     *  rx          : 수신받은 frame을 담는 배열
     *  i           : 제어문 제어 변수
     *  len         : 수신 받은 frame의 총 길이
     *  id          : 수신 받은 frame의 id
     *  crc         : 수신 받은 frame의 crc
     *  data        : 수신 받은 frame의 data
     *  testData    : 수신 받은 frame의 test data(test 수행은 2 개의 데이터를 보내옴)
     *  cmd         : 수신 받은 frame의 명령
     *  temp        : frame에 데이터 포함 여부를 판단하기 위해 잠시 사용하는 변수
     */

    static const char *func = RF_RECVANDSENDMSG2;
    char    *pTx, *pRx;
    char    tx[SIZE]    = {0xAB, 0x01};
    char    rx[SIZE];
    size_t  i;
    uint8_t len, id, cmd, crc, data, testData, temp;
    int     rxLength;   

    if ((rxLength = read(fileStreamUART, (void *) rx, SIZE)) > 0) {
        sprintf(msg, "%d %s", rxLength, MSG_RF_BYTES);
        putsForDebug(__RF__, func, msg);
        pRx     = rx;
        len     = *pRx++;
        id      = *pRx++;
        cmd     = *pRx++;
        //  TODO: test 수행 결과를 서버에 전송하면, 각 자료구조의 테스트 변수를 0으로 초기화해주어야 함
        //  TODO: LED 점등은 마스킹을 수행하면서 동시에 각 자료구조의 LED 변수를 0으로 초기화
        //  TODO: 각 경우에 따라 패킷 생성기 만들어야 함
        switch (cmd) {
        //  TODO: 0, 2, 3, 4, 5, 6, 7 경우에 맞게 구분
		case CMD_SEND_TEST_RESULT:
            break;
		case CMD_SEND_HEAT_TEST_RESULT1:
            break;
		case CMD_SEND_HEAT_TEST_RESULT2:
            break;
		case CMD_SEND_SMOKE_TEST_RESULT1:
            break;
		case CMD_SEND_SMOKE_TEST_RESULT2:
            break;
		case CMD_SEND_SMOKE_TEST_RESULT3:
            break;
		case CMD_SEND_SMOKE_TEST_RESULT4:
            break;
		case CMD_SEND_REQUESTED_TEST_RESULT:
            break;
		case CMD_SEND_HEAT_REQUESTED_TEST_RESULT1:
            break;
		case CMD_SEND_HEAT_REQUESTED_TEST_RESULT2:
            break;
		case CMD_SEND_SMOKE_REQUESTED_TEST_RESULT1:
            break;
		case CMD_SEND_SMOKE_REQUESTED_TEST_RESULT2:
            break;
		case CMD_SEND_SMOKE_REQUESTED_TEST_RESULT3:
            break;
		case CMD_SEND_SMOKE_REQUESTED_TEST_RESULT4:
            break;
		case CMD_SEND_INIT_TEST_RESULT:
            putsForDebug(__RF__, func, MSG_INFO_FINISH_TEST_20);
            data        = *pRx++;
            testData    = *pRx;
            /*  TODO: remove after test
            getTestResultsForPacket(((*(thing + id)).serialID), cmd, data, testData);
            */
            cmd         |= (!checkCRC(rx, len)) ? RESPONSE_OK : RESPONSE_ERROR;
            break;
		case CMD_SEND_INIT_HEAT_TEST_RESULT1:
            putsForDebug(__RF__, func, MSG_INFO_FINISH_TEST_22);
            data        = *pRx++;
            testData    = *pRx;
            /*  TODO: remove after test
            getTestResultsForPacket(((*(thing + id)).serialID), cmd, data, testData);
            */
            cmd         |= (!checkCRC(rx, len)) ? RESPONSE_OK : RESPONSE_ERROR;
            break;
		case CMD_SEND_INIT_HEAT_TEST_RESULT2:
            putsForDebug(__RF__, func, MSG_INFO_FINISH_TEST_23);
            data        = *pRx++;
            testData    = *pRx;
            /*  TODO: remove after test
            getTestResultsForPacket(((*(thing + id)).serialID), cmd, data, testData);
            */
            cmd         |= (!checkCRC(rx, len)) ? RESPONSE_OK : RESPONSE_ERROR;
            break;
		case CMD_SEND_INIT_SMOKE_TEST_RESULT1:
            break;
		case CMD_SEND_INIT_SMOKE_TEST_RESULT2:
            break;
		case CMD_SEND_INIT_SMOKE_TEST_RESULT3:
            break;
		case CMD_SEND_INIT_SMOKE_TEST_RESULT4:
            break;
        case CMD_NOTIFY_FIRE_DETECTED:
            putsForDebug(__RF__, func, MSG_INFO_FIRE);
            data    = *pRx;
            /*  TODO: remove after test
            getFireDetectionDataForPacket(((*(thing + id)).serialID), data);
            */
            cmd     |= (!checkCRC(rx, len)) ? RESPONSE_OK : RESPONSE_ERROR;
            break;
        //  TODO: 구체화 해야 함    
        case CMD_CHECK_COMMAND:
            putsForDebug(__RF__, func, MSG_INFO_CHECK_TEST);
            //  TODO: received test operation from server, add routine 
            /*  cmdTest -> 각 자료구조 내부의 test 변수로 변화됨
            if (cmdTest) {
                putsForDebug(__RF__, func, MSG_INFO_START_TEST);
                ++(*command);
            } else {
                putsForDebug(__RF__, func, MSG_INFO_WAIT_TEST);
            }
            */
            if (testTest) {
                putsForDebug(__RF__, func, MSG_INFO_START_TEST);
                /*  period: 2, user: 3  */
                cmd += 3;
            } else {
                putsForDebug(__RF__, func, MSG_INFO_WAIT_TEST);
            }
            cmd     |= (!checkCRC(rx, len)) ? RESPONSE_OK : RESPONSE_ERROR;
            break;
        case CMD_SEND_A_AVERAGE_DATA:
            putsForDebug(__RF__, func, MSG_INFO_DATA);
            data    = *pRx;
            /*  TODO: remove after test
            getAverageDataForPacket(((*(thing + id)).serialID), data);
            */
            cmd     |= (!checkCRC(rx, len)) ? RESPONSE_OK : RESPONSE_ERROR;
            break;
        case CMD_REQUEST_CONNECTION:
            putsForDebug(__RF__, func, MSG_CONNECTION);
            (*(thing + id)).CRC = 0xFF;
            memcpy((*(thing + id)).serialID, pRx, SIZE_SERIAL_ID_3);
            cmd |= (!checkCRC(rx, len)) ? RESPONSE_OK : RESPONSE_ERROR;
            break;
        case CMD_REQUEST_CONNECTION1:
            putsForDebug(__RF__, func, MSG_CONNECTION);
            memcpy(((*(thing + id)).serialID + 3), pRx, SIZE_SERIAL_ID_3);
            cmd |= (!checkCRC(rx, len)) ? RESPONSE_OK : RESPONSE_ERROR;
            break;
        case CMD_REQUEST_CONNECTION2:
            putsForDebug(__RF__, func, MSG_CONNECTION);
            memcpy(((*(thing + id)).serialID + 6), pRx, SIZE_SERIAL_ID_3);
            sprintf((*(thing + id)).strSerialID, "%02X%02X%02X%02X%02X%02X%02X%02X%02X", 
                thing[id].serialID[0], 
                thing[id].serialID[1], 
                thing[id].serialID[2], 
                thing[id].serialID[3], 
                thing[id].serialID[4], 
                thing[id].serialID[5], 
                thing[id].serialID[6], 
                thing[id].serialID[7], 
                thing[id].serialID[8]);
            initCRC((thing + id));
            sprintf(msg, "Thing ID %u's Serial-ID: %s", id, (*(thing + id)).strSerialID);
            putsForDebug(__RF__, func, msg);
            /*  TODO: remove after test
            getSerialIDForPacket((*(thing + id)).serialID);
            */
            cmd |= (!checkCRC(rx, len)) ? RESPONSE_OK : RESPONSE_ERROR;
            break;
        default:
            putsForDebug(__RF__, func, MSG_ERROR_CHECKCMD);
            cmd |= RESPONSE_ERROR;
            break;
        }
        printf("%x\n", cmd);
        if ((cmd & RESPONSE_ERROR) == RESPONSE_ERROR) {
            putsForDebug(__RF__, func, MSG_CMD_ERROR);
            if ((getUnixTime() - (*(thing + id)).time) > SIZE_INTERVAL) {
                putsForDebug(__RF__, func, MSG_RE_CONNECTION);
                (*(thing + id)).CRC = 0xFF;
                cmd = REQUEST_RE_CONNECTION_RESPONSE;    /*  오랫동안 통신이 실패할 경우, 다시 연결  */
            }
        } else {
            (*(thing + id)).time = getUnixTime();
        }
        pTx     = (tx + PTX_AMEND_OFFSET);
        *pTx++  = SIZE_TX_LEN;
        *pTx++  = id;
        *pTx++  = cmd;
        *pTx++  = updateCRC(SIZE_TX_LEN, id, cmd);   /*  pTx++: 길이 보정(pTx - tx + 1) */
        if (write(fileStreamUART, tx, SIZE_TX) > 0) {
            putsForDebug(__RF__, func, MSG_LINE);
        }
    }

    return;
}

#else
//  TODO: recvAndSendMsg2 완벽히 구현한 후 최적화 하면서 옮기기
void recvAndSendMsg3(void)
{

    return;
}
#endif
