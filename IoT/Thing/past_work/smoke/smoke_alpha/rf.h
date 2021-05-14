#ifndef __RF_H__
#define __RF_H__
#define __RF__      "[rf.ino]"
#define RF_SETUPRF1 "[setupRF1()] "
#define RF_SENDMSGA "[sendMsgA()] "
#define RF_SENDMSGB "[sendMsgB()] "
#define RF_SENDMSGC "[sendMsgC()] "
#define RF_RECVMSGA "[recvMsgA()] "
#define RF_RECVMSGB "[recvMsgB()] "
#define RF_RECVMSGC "[recvMsgC()] "

#define PTX_AMEND_OFFSET        2
#define RF_MODULE_OFF_SIZE      2   
#define LENGTH                  3
#define LENGTH_WITH_TEST_RESULT 5
#define LENGTH_WITH_SERIALID    6  /*  3 + 3   */
#define LENGTH_WITH_DATA        4
#define SET_CHANNEL_OFFSET      3
#define RECV_LEN_FROM_RASP      4

#define TX_LENGTH_AMEND_OFFSET  3

#define NORMAL                  2000

#define SEND_TEST_RESULT        3000    /*  use option field in frame   */
#define NOTIFY_FIRE_DETECTED    3010    /*  use option field in frame   */
#define RECEIVED_TEST_COMMAND   3020    /*  don't use option field in frame */
#define SEND_A_AVERAGE_DATA     3030    /*  use option field in frame   */
#define REQUEST_CONNECTION      3050    /*  use option field in frame   */
#define REQUEST_CONNECTION3     3051    /*  use option field in frame   */
#define REQUEST_CONNECTION6     3052    /*  use option field in frame   */

#define CMD_SEND_TEST_RESULT        0x00
#define CMD_SEND_TEST_RESULT1       0x01
#define CMD_SEND_TEST_RESULT2       0x02
#define CMD_NOTIFY_FIRE_DETECTED    0x10
#define CMD_RECEIVED_TEST_COMMAND   0x20
#define CMD_SEND_A_AVERAGE_DATA     0x30
#define CMD_REQUEST_CONNECTION      0x50    /*  serial ID index: 0-2    */
#define CMD_REQUEST_CONNECTION3     0x51    /*  serial ID index: 3-5    */
#define CMD_REQUEST_CONNECTION6     0x52    /*  serial ID index: 6-8    */

#define CMD_SEND_TEST_RESULT_RESPONSE       0x80
#define CMD_NOTIFY_FIRE_DETECTED_RESPONSE   0x90
#define CMD_RECEIVED_TEST_COMMAND_RESPONSE  0xA0
#define CMD_SEND_A_AVERAGE_DATA_RESPONSE    0xB0
#define CMD_REQUEST_CONNECTION_RESPONSE     0xD0
#define CMD_REQUEST_RE_CONNECTION_RESPONSE  0xF0    /*  thing의 상태가 CMD_RECEIVED_TEST_COMMAND일 경우 */
#define CMD_REQUEST_CONNECTION_RESPONSE     0xD0    /*  use option field in frame   */
#define CMD_REQUEST_CONNECTION3_RESPONSE    0xD1    /*  use option field in frame   */
#define CMD_REQUEST_CONNECTION6_RESPONSE    0xD2    /*  use option field in frame   */

#define PREPROCESS      0xFF
#define CHECK_ERROR     0x08
#define CHECK_LED       0x04
#define CHECK_RESPONSE  0xF3    /*  00, 01, 02, 03은 frame의 종류룰 구분하는 데 사용    */

#define RX_ID   1
#define RX_CMD  2
#define RX_CRC  3

/*
 *  setupRF(void)
 *  --------------------
 *  Setup RF Module(set channel, set tx power, set data rate)
 *  
 *  char txTestData[6]      = {0xAB, 0x01, 0x03, 0xEE, 0xEF, 0xEA};
 *  char cmdOff[2]          = {0xAB, 0x02};
 *  char cmdOn[2]           = {0xAB, 0x03};
 *  char cmdSetChannel[4]   = {0xAB, 0x04, 0x01, 0x00};
 *  char cmdSetTxPower[3]   = {0xAB, 0x05, 0x0A};
 *  char cmdSetDataRate[3]  = {0xAB, 0x06, 0x02};
 *  char cmdReadRSSI[2]     = {0xAB, 0x07};
 *  char cmdTestTx[3]       = {0xAB, 0x08, 0x03};
 *  char cmdTestBER[2]      = {0xAB, 0x09};
 *  char cmdReadConfig[2]   = {0xAB, 0x0A};
 *  char cmdGetVersion[2]   = {0xAB, 0x0B};
 *
 */
void setupRF(void);

/*
 *  setupRF1(void)
 *  --------------------
 *  Setup RF Module when defined SOFTWARESERIAL_MODE
 */
void setupRF1(void);

/*
 *  setupRF2(void)
 *  --------------------
 *  Setup RF Module when not defined SOFTWARESERIAL_MODE
 */
void setupRF2(void);

/*
 *  sendMsg(void)
 *  --------------------
 *  Send message to Raspberry Pi
 *  --------------------------
 *  Data frame structure
 *  0xAB | 0x01 | Length | ID | CMD | (Option |) CRC
 *  --------------------------
 *  Length  : 전송할 데이터 개수, From ID to CRC
 *  ID  : Thing의 ID 
 *  CMD(Thing aspect)   : 
 *      1) Send test result,        0x00, 0x01, 0x02
 *      2) Notify fire detected,    0x10
 *      3) Received test command,   0x20
 *      4) Send a average data,     0x30    
 *      5) Request connection,      0x50, 0x51, 0x52
 *      6) Request re-connection,   (0xF0), 계속 통신이 안 되는 상태이므로 강제로 재연결 과정을 수행하도록 함.
 *                                  이 명령은 다른 명령들보다 우선 순위가 가장 높음.
 *  
 *  | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
 *    1) 0번 bit: response, 명령을 성공적으로 수행하고 LED 점등을 요청하지 않음
 *    2) 0번 bit + 4-7번 bit: response, 명령 수행 도중 오류가 발생하였음
 *    3) 0번 bit + 5번 bit: response, 명령을 성공적으로 수행하고 LED 점등을 요청함
 *    4) 6-7번 bit: response 명령의 형태를 구분하는 데 사용하는 bit
 *  
 *  아래 내용은 Thing이 주체임
 *  1. Gateway에서 LED 점등 요청을 보내지 않을 경우
 *  +-----------------------+---------------+-----------------------+---------------+
 *  | Label                 | Command       | Response(| 0x80)      | Error(| 0x88) |
 *  +-----------------------+---------------+-----------------------+---------------+
 *  | Send test result      | 0x00          | 0x80                  | 0x88          |
 *  |                       | 0x01(error 1) | 0x81                  | 0x88          |
 *  |                       | 0x02(error 2) | 0x82                  | 0x88          |
 *  +-----------------------+---------------+-----------------------+---------------+
 *  | Notify fire detected  | 0x10          | 0x90                  | 0x98          |
 *  +-----------------------+---------------+-----------------------+---------------+
 *  | Received test command | 0x20          | 0xA0(Not received yet)| 0xA8          |
 *  |                       | 0x20          | 0xA1(Received)        | 0xA8          |
 *  +-----------------------+---------------+-----------------------+---------------+
 *  | Send a average data   | 0x30          | 0xB0                  | 0xB8          |
 *  +-----------------------+---------------+-----------------------+---------------+
 *  | Request connection    | 0x50          | 0xD0                  | 0xD8          |
 *  +-----------------------+---------------+-----------------------+---------------+
 *  | Request re-connection | -             | 0xF0                  | -             |
 *  +-----------------------+---------------+-----------------------+---------------+
 *
 *  2. Gateway에서 LED 점등 요청을 보낼 경우(다음 통신 전까지 LED를 점등함)
 *  +-----------------------+---------------+-----------------------+---------------+
 *  | Label                 | Command       | Response(| 0x84)      | Error(| 0x88) |
 *  +-----------------------+---------------+-----------------------+---------------+
 *  | Send test result      | 0x00          | 0x84                  | 0x88          |
 *  |                       | 0x01(error 1) | 0x85                  | 0x88          |
 *  |                       | 0x02(error 2) | 0x86                  | 0x88          |
 *  +-----------------------+---------------+-----------------------+---------------+
 *  | Notify fire detected  | 0x10          | 0x94                  | 0x98          |
 *  +-----------------------+---------------+-----------------------+---------------+
 *  | Received test command | 0x20          | 0xA4(Not received yet)| 0xA8          |
 *  |                       | 0x20          | 0xA5(Received)        | 0xA8          |
 *  +-----------------------+---------------+-----------------------+---------------+
 *  | Send a average data   | 0x30          | 0xB4                  | 0xB8          |
 *  +-----------------------+---------------+-----------------------+---------------+
 *  | Request connection    | 0x50          | 0xD4                  | 0xD8          |
 *  +-----------------------+---------------+-----------------------+---------------+
 *  | Request re-connection | -             | 0xF4                  | -             |
 *  +-----------------------+---------------+-----------------------+---------------+
 */
void sendMsg(void);

/*
 *  sendMsgA(void)
 *  --------------------
 *  Send message to Raspberry Pi when defined TEST_COMMUNICATION
 */
void sendMsgA(void);

/*
 *  sendMsgB(void)
 *  --------------------
 *  Send message to Raspberry Pi when defined TEST_COMM_FUNCTIONAL
 */
void sendMsgB(void);

/*
 *  sendMsgC(void)
 *  --------------------
 *  Send message to Raspberry Pi when defined COMM_FUNCTIONAL 
 */
void sendMsgC(void);

/*
 *  sendMsgD(void)
 *  --------------------
 *  Send message to Raspberry Pi when defined TEST_SERIAL_COMM 
 */
void sendMsgD(void);

/*
 *  sendMsgE(void)
 *  --------------------
 *  Send message to Raspberry Pi when defined SERIAL_COMM 
 */
void sendMsgE(void);

/*
 *  recvMsg(void)
 *  --------------------
 *  Recv. message from Raspberry Pi
 *  --------------------------
 *  1. Response(no error)
 *  2. Error(crack CRC)
 */
void recvMsg(void);

/*
 *  recvMsgA(void)
 *  --------------------
 *  Recv. message from Raspberry Pi when defined TEST_COMMUNICATION 
 */
void recvMsgA(void);

/*
 *  recvMsgB(void)
 *  --------------------
 *  Recv. message from Raspberry Pi when defined TEST_COMM_FUNCTIONAL 
 */
void recvMsgB(void);

/*
 *  recvMsgC(void)
 *  --------------------
 *  Recv. message from Raspberry Pi when defined COMM_FUNCTIONAL 
 */
void recvMsgC(void);

/*
 *  recvMsgD(void)
 *  --------------------
 *  Recv. message from Raspberry Pi when defined TEST_SERIAL_COMM 
 */
void recvMsgD(void);

/*
 *  recvMsgE(void)
 *  --------------------
 *  Recv. message from Raspberry Pi when defined SERIAL_COMM 
 */
void recvMsgE(void);

#endif
