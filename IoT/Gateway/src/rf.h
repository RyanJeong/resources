#ifndef __RF_H__
#define __RF_H__

/*
 *  --------------------
 *  Data frame structure
 *  0xAB | 0x01 | Length | ID | CMD | (Option |) CRC
 *  --------------------------
 *  Length              : 전송할 데이터 개수, From ID to CRC
 *  ID                  : Thing의 ID 
 *  CMD(Thing aspect)   : 
 *       1) Send test result(주기에 따라서)     : 0x00, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
 *          (1-1)   0x00: (공통) 통과
 *          (1-2)   0x02: (열감지) 1단계 실패
 *          (1-3)   0x03: (열감지) 2단계 실패
 *          (1-4)   0x04: (연기) 1단계 실패
 *          (1-5)   0x05: (연기) 2단계 실패
 *          (1-6)   0x06: (연기) 3단계 실패
 *          (1-7)   0x07: (연기) 4단계 실패
 *       2) Send requested test result          : 0x10, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17
 *          (2-1)   0x10: (공통) 통과
 *          (2-2)   0x12: (열감지) 1단계 실패
 *          (2-3)   0x13: (열감지) 2단계 실패
 *          (2-4)   0x14: (연기) 1단계 실패
 *          (2-5)   0x15: (연기) 2단계 실패
 *          (2-6)   0x16: (연기) 3단계 실패
 *          (2-7)   0x17: (연기) 4단계 실패
 *       3) Send init. test result              : 0x20, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27
 *          (3-1)   0x20: (공통) 통과
 *          (3-2)   0x22: (열감지) 1단계 실패
 *          (3-3)   0x23: (열감지) 2단계 실패
 *          (3-4)   0x24: (연기) 1단계 실패
 *          (3-5)   0x25: (연기) 2단계 실패
 *          (3-6)   0x26: (연기) 3단계 실패
 *          (3-7)   0x27: (연기) 4단계 실패
 *       4) Notify fire detected                : 0x30
 *       5) Check command                       : 0x40
 *          - Error일 경우, thing에서 보낸 값에 or 연산을 한 값을 반환함
 *          - Gateway로부터 여러 유형의 값을 받더라도, error값은 모두 동일함
 *       6) Send a average data                 : 0x50
 *       7) Request connection                  : 0x60
 *          -   Serial-ID의 0-2 bits를 전송
 *       8) Request re-connection               : 0x67
 *          -   계속 통신이 안 되는 상태이므로 강제로 재연결 과정을 수행하도록 함.
 *          -   이 명령은 다른 명령들보다 우선 순위가 가장 높음.
 *       9) Send battery voltage                : 0x70
 *  Option              : Frame에 포함될 데이터 필드
 *  CRC                 : Frame의 유효성 검사에 사용되는 필드
 *  
 *  [아래 내용은 Thing이 주체임]
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
 *
 *  Raspberry Pi -> Arduino
 *  +-----------------------+---------------+-----------------------+---------------+
 *  | Request re-connection | 0x67          | 0xE7                  | -             |
 *  +-----------------------+---------------+-----------------------+---------------+
 *  | Send battery voltage  | 0x77          | 0xF7                  | -             |
 *  +-----------------------+---------------+-----------------------+---------------+
 */

#define DEV_TARGET          "/dev/ttyS0"
/*	Raspberry Pi 3: #define DEV_TARGET          "/dev/ttyAMA0"	*/
#define BAUDRATE            B9600
#define __RF__              "[rf.c]"
#define RF_SETUPUART        "[setupUART(void)] "
#define RF_CLOSEUART        "[closeUART(void)] "
#define RF_SETUPRF          "[setupRF(void)] "
#define RF_RECVANDSENDMSG   "[recvAndSendMsg(void)] "
#define RF_RECVANDSENDMSG1  "[recvAndSendMsg1(void)] "
#define RF_RECVANDSENDMSG2  "[recvAndSendMsg2(void)] "
#define RF_RECVANDSENDMSG3  "[recvAndSendMsg3(void)] "
#define RF_WRITESERVER      "[writeServer(uint8_t)] "
#define RF_PUSHAPP          "[pushApp(uint8_t)] "

#define MSG_ERROR_SETUPUART         "Error. Unable to open UART. Ensure it is not in use by another application."
#define MSG_COMPLETE_SETUPUART      "Setup UART has been completed."
#define MSG_ERROR_CLOSEUART         "Failed to close file stream."
#define MSG_COMPLETE_CLOSEUART      "Close UART has been completed."
#define MSG_ERROR_SETUPRF1          "Failed to set channel."
#define MSG_ERROR_SETUPRF2          "Failed to set TX power."
#define MSG_ERROR_SETUPRF3          "Failed to set data-rate."
#define MSG_COMPLETE_SETUPRF        "Setup RF has been completed."
#define MSG_ERROR_RECVANDSENDMSG1   "UART TX error"
#define MSG_ERROR_RECVANDSENDMSG2   "UART RX error"
#define MSG_INFO_FIRE               "A fire has been detected."
#define MSG_INFO_CHECK_TEST         "Check if the test should be performed."
#define MSG_INFO_WAIT_TEST          "It's not time to test yet."
#define MSG_INFO_START_TEST         "The test is going to begin soon."
#define MSG_INFO_FINISH_TEST_00     "[NORMAL] Test finished successfully."
#define MSG_INFO_FINISH_TEST_02     "[NORMAL] Heat sensor test 1 failed."
#define MSG_INFO_FINISH_TEST_03     "[NORMAL] Heat sensor test 2 failed."
#define MSG_INFO_FINISH_TEST_04     "[NORMAL] Smoke sensor test 1 failed."
#define MSG_INFO_FINISH_TEST_05     "[NORMAL] Smoke sensor test 2 failed."
#define MSG_INFO_FINISH_TEST_06     "[NORMAL] Smoke sensor test 3 failed."
#define MSG_INFO_FINISH_TEST_07     "[NORMAL] Smoke sensor test 4 failed."
#define MSG_INFO_FINISH_TEST_10     "[USER] Test finished successfully."
#define MSG_INFO_FINISH_TEST_12     "[USER] Heat sensor test 1 failed."
#define MSG_INFO_FINISH_TEST_13     "[USER] Heat sensor test 2 failed."
#define MSG_INFO_FINISH_TEST_14     "[USER] Smoke sensor test 1 failed."
#define MSG_INFO_FINISH_TEST_15     "[USER] Smoke sensor test 2 failed."
#define MSG_INFO_FINISH_TEST_16     "[USER] Smoke sensor test 3 failed."
#define MSG_INFO_FINISH_TEST_17     "[USER] Smoke sensor test 4 failed."
#define MSG_INFO_FINISH_TEST_20     "[INIT] Test finished successfully."
#define MSG_INFO_FINISH_TEST_22     "[INIT] Heat sensor test 1 failed."
#define MSG_INFO_FINISH_TEST_23     "[INIT] Heat sensor test 2 failed."
#define MSG_INFO_FINISH_TEST_24     "[INIT] Smoke sensor test 1 failed."
#define MSG_INFO_FINISH_TEST_25     "[INIT] Smoke sensor test 2 failed."
#define MSG_INFO_FINISH_TEST_26     "[INIT] Smoke sensor test 3 failed."
#define MSG_INFO_FINISH_TEST_27     "[INIT] Smoke sensor test 4 failed."
#define MSG_INFO_DATA               "Received a data from the thing."
#define MSG_CONNECTION              "Request connection from the thing."
#define MSG_RE_CONNECTION           "Request re-connection from the thing."
#define MSG_ERROR_CHECKCMD          "Occurred error. Can not recognize the command"
#define MSG_CMD_ERROR               "Command error"

#define MSG_RF_BYTES        "bytes read."
#define MSG_RF_RX_INDEX     "RX index"
#define MSG_RF_TX_INDEX     "TX index"

#define PTX_AMEND_OFFSET    2

#define RESPONSE_OK             0x80
#define RESPONSE_ERROR          0x88

#define CMD_WRITE_TEST_RESULT   4000
#define CMD_WRITE_FIRE_DETECTED 4001
#define CMD_WRITE_A_DATA        4002

#define CMD_PUSH_TEST_RESULT    5000
#define CMD_PUSH_FIRE_DETECTED  5001

#define CMD_SEND_TEST_RESULT                    0x00
#define CMD_SEND_HEAT_TEST_RESULT1              0x02
#define CMD_SEND_HEAT_TEST_RESULT2              0x03
#define CMD_SEND_SMOKE_TEST_RESULT1             0x04
#define CMD_SEND_SMOKE_TEST_RESULT2             0x05
#define CMD_SEND_SMOKE_TEST_RESULT3             0x06
#define CMD_SEND_SMOKE_TEST_RESULT4             0x07
#define CMD_SEND_REQUESTED_TEST_RESULT          0x10
#define CMD_SEND_HEAT_REQUESTED_TEST_RESULT1    0x12
#define CMD_SEND_HEAT_REQUESTED_TEST_RESULT2    0x13
#define CMD_SEND_SMOKE_REQUESTED_TEST_RESULT1   0x14
#define CMD_SEND_SMOKE_REQUESTED_TEST_RESULT2   0x15
#define CMD_SEND_SMOKE_REQUESTED_TEST_RESULT3   0x16
#define CMD_SEND_SMOKE_REQUESTED_TEST_RESULT4   0x17
#define CMD_SEND_INIT_TEST_RESULT               0x20
#define CMD_SEND_INIT_HEAT_TEST_RESULT1         0x22
#define CMD_SEND_INIT_HEAT_TEST_RESULT2         0x23
#define CMD_SEND_INIT_SMOKE_TEST_RESULT1        0x24
#define CMD_SEND_INIT_SMOKE_TEST_RESULT2        0x25
#define CMD_SEND_INIT_SMOKE_TEST_RESULT3        0x26
#define CMD_SEND_INIT_SMOKE_TEST_RESULT4        0x27
#define CMD_NOTIFY_FIRE_DETECTED                0x30
#define CMD_CHECK_COMMAND                       0x40
#define CMD_SEND_A_AVERAGE_DATA                 0x50
#define CMD_REQUEST_CONNECTION                  0x60
#define CMD_REQUEST_CONNECTION1                 0x61
#define CMD_REQUEST_CONNECTION2                 0x62
#define REQUEST_RE_CONNECTION                   0x67
#define REQUEST_RE_CONNECTION_RESPONSE          0xE7
#define CMD_SEND_BATTERY_VOLTAGE                0x70

/*
 *  setupUART(void)
 *  --------------------
 *  UART 통신에 필요한 기본 세팅을 수행합니다.
 *  셋팅 내용: 9,600 bps, 8 bit data, no parity, 1 stop bit, non-blocking mode 
 */
size_t setupUART(void);

/*
 *  closeUART(void)
 *  --------------------
 *  UART 통신에 사용한 FileStream을 닫습니다. 
 */
size_t closeUART(void);

/*  
 *  setupRF(void)
 *  --------------------------
 *  RF Module을 사용하기 위한 설정을 수행하는 함수.
 *  1. Set Channel      : {0xAB, 0x04, 0x01, 0x00}; 
 *  2. Set Tx Power     : {0xAB, 0x05, 0x0A};
 *  3. Set Data Rate    : {0xAB, 0x06, 0x02};
 */
size_t setupRF(void);

/*  
 *  recvAndSendMsg(void)
 *  --------------------------
 *  Receive message from the thing and send message to the thing
 *  Receive message before Send message
 *  !! If the sending or receiving result is an error, we need to add a method to deal with it !!
 *
 */
void recvAndSendMsg(void);

/*  
 *  recvAndSendMsg1(void)
 *  --------------------------
 *  Receive message from the thing and send message to the thing when mode is alpha
 */
void recvAndSendMsg1(void);

/*  
 *  recvAndSendMsg2(void)
 *  --------------------------
 *  Receive message from the thing and send message to the thing when mode is beta
 */
void recvAndSendMsg2(void);

/*  
 *  recvAndSendMsg3(void)
 *  --------------------------
 *  Receive message from the thing and send message to the thing when mode is release
 */
void recvAndSendMsg3(void);

#endif
