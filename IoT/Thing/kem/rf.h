#ifndef __RF_H__
#define __RF_H__

#define DELAY_CONF      (1 << 7)
#define DELAY_COMM      (1 << 9)

#define DISABLE_RF              \
do {                            \
    Serial.print((char) 0xAB);  \
    Serial.print((char) 0x02);  \
    delay(1);                   \
} while (0)

#define ENABLE_RF               \
do {                            \
    Serial.print((char) 0xAB);  \
    Serial.print((char) 0x03);  \
} while (0)

/*
 *  --------------------
 *  Data frame structure
 *  0xAB | 0x01 | Length | ID | CMD | (Option |) CRC
 *  --------------------------
 *  Length              : 전송할 데이터 개수, From ID to CRC
 *  ID                  : Thing의 ID 
 *  CMD(Thing aspect)   : 
 *       1) Send test result(주기에 따라서)  : 0x00, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
 *          (1-1)   0x00: (공통) 통과
 *          (1-2)   0x02: (열감지) 1단계 실패
 *          (1-3)   0x03: (열감지) 2단계 실패
 *          (1-4)   0x04: (연기) 1단계 실패
 *          (1-5)   0x05: (연기) 2단계 실패
 *          (1-6)   0x06: (연기) 3단계 실패
 *          (1-7)   0x07: (연기) 4단계 실패
 *       2) Send requested test result       : 0x10, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17
 *          (2-1)   0x10: (공통) 통과
 *          (2-2)   0x12: (열감지) 1단계 실패
 *          (2-3)   0x13: (열감지) 2단계 실패
 *          (2-4)   0x14: (연기) 1단계 실패
 *          (2-5)   0x15: (연기) 2단계 실패
 *          (2-6)   0x16: (연기) 3단계 실패
 *          (2-7)   0x17: (연기) 4단계 실패
 *       3) Send init. test result           : 0x20, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27
 *          (3-1)   0x20: (공통) 통과
 *          (3-2)   0x22: (열감지) 1단계 실패
 *          (3-3)   0x23: (열감지) 2단계 실패
 *          (3-4)   0x24: (연기) 1단계 실패
 *          (3-5)   0x25: (연기) 2단계 실패
 *          (3-6)   0x26: (연기) 3단계 실패
 *          (3-7)   0x27: (연기) 4단계 실패
 *       4) Notify fire detected             : 0x30
 *       5) Check command                    : 0x40
 *          - Error일 경우, thing에서 보낸 값에 or 연산을 한 값을 반환함
 *          - Gateway로부터 여러 유형의 값을 받더라도, error값은 모두 동일함
 *       6) Send a average data              : 0x50
 *       7) Request connection               : 0x60
 *          -   Serial-ID의 0-2 bits를 전송
 *       8) Request re-connection            : 0x67
 *          -   계속 통신이 안 되는 상태이므로 강제로 재연결 과정을 수행하도록 함.
 *          -   이 명령은 다른 명령들보다 우선 순위가 가장 높음.
 *       9) Send battery voltage             : 0x70
 *  Option              : Frame에 포함될 데이터 필드
 *  CRC                 : Frame의 유효성 검사에 사용되는 필드
 *  
 *  Arduino -> Raspberry Pi 
 *  +-----------------------+---------------+-----------------------+---------------+
 *  | Label                 | Command       | Response(| 0x80)      | Error(| 0x88) |
 *  +-----------------------+---------------+-----------------------+---------------+
 *  | Send test result      | 0x00(passed)  | 0x80                  | 0x88          |
 *  | - 0x02, 0x03: heat    | 0x02(error 1) | 0x82                  | 0x8A          |
 *  |                       | 0x03(error 2) | 0x83                  | 0x8B          |
 *  | - 0x04, 0x05,         | 0x04(error 1) | 0x84                  | 0x8C          |
 *  |   0x06, 0x07: smoke   | 0x05(error 2) | 0x85                  | 0x8D          |
 *  |                       | 0x06(error 3) | 0x86                  | 0x8E          |
 *  |                       | 0x07(error 4) | 0x87                  | 0x8F          |
 *  +-----------------------+---------------+-----------------------+---------------+
 *  | Send req. test result | 0x10(passed)  | 0x90                  | 0x98          |
 *  | - 0x12, 0x13: heat    | 0x12(error 1) | 0x92                  | 0x9A          |
 *  |                       | 0x13(error 2) | 0x93                  | 0x9B          |
 *  | - 0x14, 0x15,         | 0x14(error 1) | 0x94                  | 0x9C          |
 *  |   0x16, 0x17: smoke   | 0x15(error 2) | 0x95                  | 0x9D          |
 *  |                       | 0x16(error 3) | 0x96                  | 0x9E          |
 *  |                       | 0x17(error 4) | 0x97                  | 0x9F          |
 *  +-----------------------+---------------+-----------------------+---------------+
 *  | Send init. test result| 0x20(passed)  | 0xA0                  | 0xA8          |
 *  | - 0x22, 0x23: heat    | 0x22(error 1) | 0xA2                  | 0xAA          |
 *  |                       | 0x23(error 2) | 0xA3                  | 0xAB          |
 *  | - 0x24, 0x25,         | 0x24(error 1) | 0xA4                  | 0xA4          |
 *  |   0x26, 0x27: smoke   | 0x25(error 2) | 0xA5                  | 0xA5          |
 *  |                       | 0x26(error 3) | 0xA6                  | 0xA6          |
 *  |                       | 0x27(error 4) | 0xA7                  | 0xA7          |
 *  +-----------------------+---------------+-----------------------+---------------+
 *  | Notify fire detected  | 0x30          | 0xB0                  | 0xB8          |
 *  +-----------------------+---------------+-----------------------+---------------+
 *  | Check command         | 0x40          | 0xC0(Not received yet)| 0xC8          |
 *  |                       | 0x40          | 0xC2(Received)        | 0xC8          |
 *  |                       | 0x40          | 0xC3(Received by user)| 0xC8          |
 *  |                       | 0x40          | 0xC5(LED)             | 0xC8          |
 *  |                       | 0x40          | 0xC6(Buzzer)          | 0xC8          |
 *  |                       | 0x40          | 0xC7(LED + Buzzer)    | 0xC8          |
 *  +-----------------------+---------------+-----------------------+---------------+
 *  | Send a average data   | 0x50          | 0xD0                  | 0xD8          |
 *  +-----------------------+---------------+-----------------------+---------------+
 *  | Request connection    | 0x60          | 0xE0                  | 0xE8          |
 *  +-----------------------+---------------+-----------------------+---------------+
 *  | Send battery voltage  | 0x70          | 0xF0                  | 0xF8          |
 *  +-----------------------+---------------+-----------------------+---------------+
 * 
 *  Raspberry Pi -> Arduino
 *  +-----------------------+---------------+-----------------------+---------------+
 *  | Request re-connection | 0x67          | 0xE7                  | -             |
 *  +-----------------------+---------------+-----------------------+---------------+
 */

#define __RF__      "[rf.ino]"

#define FRAME_START_OFFSET      2
#define RF_MODULE_OFF_SIZE      2   

#define LENGTH                  3
#define LENGTH_WITH_TEST_RESULT 5
#define LENGTH_WITH_SERIALID    6  /*  3 + 3   */
#define LENGTH_WITH_DATA        4

#define SET_CHANNEL_OFFSET      3
#define RECV_LEN_FROM_RASP      4

#define TX_LENGTH_AMEND_OFFSET  3

/*
 *  ex) Send req. test result; occurred error on heat mode at test 2
 *      CMD_SEND_REQUESTED_TEST_RESULT | HEAT_ERROR | (var)
 *  error 1: var = 0
 *  error 2: var = 1
 *  error 3: var = 2 
 *  error 4: var = 3 
 */
#define CMD_SEND_TEST_RESULT                0x00
#define CMD_SEND_REQUESTED_TEST_RESULT      0x10
#define CMD_SEND_INIT_TEST_RESULT           0x20
#define HEAT_ERROR                          0x02
#define SMOKE_ERROR                         0x04

#define CMD_NOTIFY_FIRE_DETECTED            0x30

/*
 *  ex) Received test request by user
 *  (RESPONSE CMD) | RECEIVED_TEST_REQUEST + 1
 *  ex) Received handling LED and buzzer
 *  if ((RESPONSE CMD) | RECEIVED_HANDLE_ACCESSORIES + 1)
 *  if ((RESPONSE CMD) | RECEIVED_HANDLE_ACCESSORIES + 2)
 */
#define CMD_CHECK_COMMAND                   0x40
#define RECEIVED_TEST_REQUEST               0x02
#define RECEIVED_HANDLE_ACCESSORIES         0x04

#define CMD_SEND_A_AVERAGE_DATA             0x50

/*
 *  CMD_REQUEST_CONNECTION
 *  CMD_REQUEST_CONNECTION + 1
 *  CMD_REQUEST_CONNECTION + 2
 */
#define CMD_REQUEST_CONNECTION              0x60
#define CMD_REQUEST_RE_CONNECTION           0x67

#define CMD_SEND_BATTERY_VOLTAGE            0x70

/*  
 * thing의 상태가 CMD_CHECK_COMMAND일 경우 
 */

#define NORMAL                  0x08
#define SEND_TEST_RESULT        0x18
#define NOTIFY_FIRE_DETECTED    0x28
#define INIT_TEST_COMMAND       0x38
#define SEND_A_AVERAGE_DATA     0x48
#define REQUEST_CONNECTION      0x58
#define RECEIVED_TEST_COMMAND   0x68
#define FAILED_TEST             0x78
#define CHECK_BATTERY_VOLTAGE   0x88

#define PREPROCESS      0x77
#define CHECK_ERROR     0x08

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
 *  void communicate(void); 
 *  --------------------
 *  communicate between the thing and the gateway
 *  --------------------
 *  1. prepare a frame
 *  2. send the frame to the gateway
 *  3. receive the frame's response from the gateway
 *  4. parse received frame
 */
void communicate(void); 

/*
 *  void prepareFrame(void);
 *  --------------------
 *  prepare a frame which will send to the gateway
 */
void prepareFrame(void);

/*
 *  void testCommunication(void);
 *  --------------------
 *  Communication between the thing and the gateway test routine
 */
void testCommunication(void);

/*
 *  void sendAndRecv(void);
   --------------------
 *  1. send the frame to the gateway
 *  2. receive the frame from the gateway
 */
void sendAndRecv(void);

/*
 *  void parseFrame(void);
 *  --------------------
 *  parse a frame which received from the gateway
 */
void parseFrame(void);
#endif
