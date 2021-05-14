#ifndef __SERVER_H__
#define __SERVER_H__

#define A_SECOND    1
#define A_MINUTE    60

#define PORT 5000    /* the port client will be connecting to */
#define HOST "kem.ourplan.kr"

#define THREAD_SEND             "SEND"
#define THREAD_RECV             "RECV"

#define __SERVER__                              "[server.c]"
#define SERVER_GETMACADDRFORPACKET              "[getMACAddrForPacket(void)] "
#define SERVER_GETSERIALIDFORPACKET             "[getSerialIDForPacket(uint8_t *)] "
#define SERVER_GETAVERAGEDATAFORPACKET          "[getAverageDataForPacket(uint8_t *, uint8_t)] "
#define SERVER_GETTESTRESULTSFORPACKET          "[getTestResultsForPacket(uint8_t *, uint8_t, uint8_t, uint8_t)] "
#define SERVER_GETFIREDETECTIONDATAFORPACKET    "[getAverageDataForPacket(uint8_t *, uint8_t)] "
#define SERVER_INITSOCKET                       "[initSocket(void)] "
#define SERVER_CLOSESOCKET                      "[closeSocket(void)] "
#define SERVER_SETUPTHREAD                      "[setupThread(void)] "
#define SERVER_SENDPACKET                       "[sendPacket(void)] "
#define SERVER_RECVFROMSERVER                   "[recvFromServer(void *)] "
#define SERVER_SENDTOSERVER                     "[sendToServer(void *)] "
#define SERVER_DESTROYTHREAD                    "[destroyThread(void)] "

#define MSG_ERROR_GET_MAC           "Failed to get MAC address."
#define MSG_ERROR_HOST              "Check your host name."
#define MSG_ERROR_SOCKET            "socket() error."
#define MSG_ERROR_CONNECT           "connect() error."
#define MSG_ERROR_SEND              "send() error."
#define MSG_ERROR_RECV              "recv() error."
#define MSG_ERROR_SETUPTHREAD       "Failed to create 'recvFromServer' thread."
#define MSG_COMPLETE_SETUPTHREAD    "Successfully created 'recvFromServer' thread."
#define MSG_ERROR_DESTROYTHREAD     "Failed to close 'recvFromServer' thread."
#define MSG_COMPLETE_DESTROYTHREAD  "Successfully closed 'recvFromServer' thread."
#define MSG_THREAD_START            "Thread start!"

#define THREAD_SERVER           0
#define THREAD_APP              1

#define CMD_PACKET_ADD_A_GATEWAY                                0x00
#define CMD_PACKET_ADD_A_THING                                  0x10
#define CMD_PACKET_SEND_A_DATA                                  0x20
#define CMD_PACKET_SEND_TEST_RESULT                             0x30
#define CMD_PACKET_SEND_HEAT_TEST_RESULT1                       0x32
#define CMD_PACKET_SEND_HEAT_TEST_RESULT2                       0x33
#define CMD_PACKET_SEND_SMOKE_TEST_RESULT1                      0x34
#define CMD_PACKET_SEND_SMOKE_TEST_RESULT2                      0x35
#define CMD_PACKET_SEND_SMOKE_TEST_RESULT3                      0x36
#define CMD_PACKET_SEND_SMOKE_TEST_RESULT4                      0x37
#define CMD_PACKET_SEND_REQUESTED_TEST_RESULT                   0x40
#define CMD_PACKET_SEND_REQUESTED_HEAT_TEST_RESULT1             0x42
#define CMD_PACKET_SEND_REQUESTED_HEAT_TEST_RESULT2             0x43
#define CMD_PACKET_SEND_REQUESTED_SMOKE_TEST_RESULT1            0x44
#define CMD_PACKET_SEND_REQUESTED_SMOKE_TEST_RESULT2            0x45
#define CMD_PACKET_SEND_REQUESTED_SMOKE_TEST_RESULT3            0x46
#define CMD_PACKET_SEND_REQUESTED_SMOKE_TEST_RESULT4            0x47
#define CMD_PACKET_SEND_INIT_TEST_RESULT                        0x50
#define CMD_PACKET_SEND_INIT_HEAT_TEST_RESULT1                  0x52
#define CMD_PACKET_SEND_INIT_HEAT_TEST_RESULT2                  0x53
#define CMD_PACKET_SEND_INIT_SMOKE_TEST_RESULT1                 0x54
#define CMD_PACKET_SEND_INIT_SMOKE_TEST_RESULT2                 0x55
#define CMD_PACKET_SEND_INIT_SMOKE_TEST_RESULT3                 0x56
#define CMD_PACKET_SEND_INIT_SMOKE_TEST_RESULT4                 0x57
#define CMD_PACKET_ALERT_A_FIRE_DETECTED                        0x60

#define CMD_PACKET_ADD_A_GATEWAY_RESPONSE                       0x80
#define CMD_PACKET_ADD_A_THING_RESPONSE                         0x90
#define CMD_PACKET_SEND_A_DATA_RESPONSE                         0xA0
#define CMD_PACKET_SEND_TEST_RESULT_RESPONSE                    0xB0
#define CMD_PACKET_SEND_HEAT_TEST_RESULT1_RESPONSE              0xB2
#define CMD_PACKET_SEND_HEAT_TEST_RESULT2_RESPONSE              0xB3
#define CMD_PACKET_SEND_SMOKE_TEST_RESULT1_RESPONSE             0xB4
#define CMD_PACKET_SEND_SMOKE_TEST_RESULT2_RESPONSE             0xB5
#define CMD_PACKET_SEND_SMOKE_TEST_RESULT3_RESPONSE             0xB6
#define CMD_PACKET_SEND_SMOKE_TEST_RESULT4_RESPONSE             0xB7
#define CMD_PACKET_SEND_REQUESTED_TEST_RESULT_RESPONSE          0xC0
#define CMD_PACKET_SEND_REQUESTED_HEAT_TEST_RESULT1_RESPONSE    0xC2
#define CMD_PACKET_SEND_REQUESTED_HEAT_TEST_RESULT2_RESPONSE    0xC3
#define CMD_PACKET_SEND_REQUESTED_SMOKE_TEST_RESULT1_RESPONSE   0xC4
#define CMD_PACKET_SEND_REQUESTED_SMOKE_TEST_RESULT2_RESPONSE   0xC5
#define CMD_PACKET_SEND_REQUESTED_SMOKE_TEST_RESULT3_RESPONSE   0xC6
#define CMD_PACKET_SEND_REQUESTED_SMOKE_TEST_RESULT4_RESPONSE   0xC7
#define CMD_PACKET_SEND_INIT_TEST_RESULT_RESPONSE               0xD0
#define CMD_PACKET_SEND_INIT_HEAT_TEST_RESULT1_RESPONSE         0xD2
#define CMD_PACKET_SEND_INIT_HEAT_TEST_RESULT2_RESPONSE         0xD3
#define CMD_PACKET_SEND_INIT_SMOKE_TEST_RESULT1_RESPONSE        0xD4
#define CMD_PACKET_SEND_INIT_SMOKE_TEST_RESULT2_RESPONSE        0xD5
#define CMD_PACKET_SEND_INIT_SMOKE_TEST_RESULT3_RESPONSE        0xD6
#define CMD_PACKET_SEND_INIT_SMOKE_TEST_RESULT4_RESPONSE        0xD7
#define CMD_PACKET_ALERT_A_FIRE_DETECTED_RESPONSE               0xE0

#define PACKET_SIZE_WITHOUT_DATAS   18  /*  1 + 6(12) + 1 + 9(18) + 0 + 1   */
#define PACKET_SIZE_WITH_A_DATA     19  /*  1 + 6(12) + 1 + 9(18) + 1 + 1   */
#define PACKET_SIZE_WITH_TWO_DATAS  20  /*  1 + 6(12) + 1 + 9(18) + 2 + 1   */

/*
 * size_t getMACAddrForPacket(void);
 *  --------------------
 *  Get MAC address of the gateway and init. the packet which used to communication between the gateway and the server
 *  Regist the gateway's information to the server
 */
size_t getMACAddrForPacket(void);

/*
 *  void getSerialIDForPacket(uint8_t *);
 *  --------------------
 *  Get serial ID of the thing and generate a packet for registers the thing to the server
 */
void getSerialIDForPacket(uint8_t *);

/*
 *  void getAverageDataForPacket(uint8_t);
 *  --------------------
 *  Get a data transmitted from a thing and generate a packet
 */
void getAverageDataForPacket(uint8_t *, uint8_t);

/*
 *  void getTestResultsForPacket(uint8_t, uint8_t,  uint8_t);
 *  --------------------
 *  Get datas transmitted from a thing and generate a packet
 */
void getTestResultsForPacket(uint8_t *, uint8_t, uint8_t,  uint8_t);

/*
 *  void getFireDetectionDataForPacket(uint8_t);
 *  --------------------
 *  Get fire detection data and generate a packet for announce fire detection
 */
void getFireDetectionDataForPacket(uint8_t *, uint8_t);

/*
 *  size_t initSocket(void);
 *  --------------------
 *  init. socket and connect to the server.
 */
size_t initSocket(void);

/*
 *  void closeSocket(void);
 *  --------------------
 *  close socket and disconnect between the gateway and the server
 */
void closeSocket(void);

/*
 * size_t sendPacket(void);
 *  --------------------
 *  Server에 패킷을 전송하는 함수
 *  -----------------------------
 *
 *  Length  | MAC   |   CMD.    |   Serial-ID   |   Data    |   CRC
 *
 *  1) Length   : MAC, CMD., Serial-ID, Data, CRC의 총 길이(1 byte)
 *  2) MAC.     : Gateway의 MAC Address(12 bytes)
 *  3) CMD.     : Packet의 명령(1 byte)
 *      3-1) From gateway to server
 *  +-------------------------------+-----------------------+
 *  |   Request                     |   Response(| 0x80)    |
 *  +-------------------------------+-----------------------+
 *  | 0x00(Add a gateway)           |   0x80                |
 *  | 0x10(Add a thing)             |   0x90                |
 *  | 0x20(Send a data)             |   0xA0                |
 *  | 0x3n(Send a result of test)   |   0xBn                |
 *  | 0x40(Alert a fire detected)   |   0xC0                |
 *  +-------------------------------+-----------------------+
 *      3-2) From server to gateway
 *  +-------------------------------+-----------------------+
 *  |   Request                     |   Response(| 0x80)    |
 *  +-------------------------------+-----------------------+
 *  | 0x50(Request a test)          |   0xD0                |
 *  | 0x60(Turn on a LED)           |   0xE0                |
 *  +-------------------------------+-----------------------+
 *  4) Serial ID    : ATmega328P 내부에 기록되어 있는 고유 번호(18 bytes)
 *  5) Data         : Packet에 포함되는 값(n bytes)
 *      5-1) From gateway to server
 *  +-------------------------------+-------------------+-----------------------+-------------------+
 *  |   Request                     |   Request Data    |   Response(| 0x80)    |   Response Data   |
 *  +-------------------------------+-------------------+-----------------------+-------------------+
 *  | 0x00(Add a gateway)           |   0 bytes         |   0x80                |   0 bytes         |
 *  | 0x10(Add a thing)             |   0 bytes         |   0x90                |   0 bytes         |
 *  | 0x20(Send a data)             |   2 byte          |   0xA0                |   0 bytes         |
 *  | 0x3n(Send a result of test)   |   1 byte          |   0xBn                |   0 bytes         |
 *  | 0x40(Alert a fire detected)   |   1 byte          |   0xC0                |   0 bytes         |
 *  +-------------------------------+-------------------+-----------------------+-------------------+
 *      5-2) From server to gateway
 *  +-------------------------------+-------------------+-----------------------+-------------------+
 *  |   Request                     |   Request Data    |   Response(| 0x80)    |   Response Data   |
 *  +-------------------------------+-------------------+-----------------------+-------------------+
 *  | 0x50(Request a test)          |   0 bytes         |   0xD0                |   0 bytes         |
 *  | 0x60(Turn on a LED)           |   0 bytes         |   0xE0                |   0 bytes         |
 *  +-------------------------------+-------------------+-----------------------+-------------------+
 *  6) CRC: Packet 오류 검사(1 byte)
 */
size_t sendPacket(void);

/*
 *  size_t setupThread(void);
 *  --------------------
 *  pThread : Server로부터 전송되는 메세지를 실시간 감지하기 위한 thread
 */
size_t setupThread(void);

/*
 *  recvFromServer(void)
 *  --------------------
 *  Server와 통신하는 thread    
 *  1. Test 요청 시간과 그 요청의 결과 저장
 *  2. Thing으로부터 데이터 측정 결과가 전송된 경우 Server에 저장
 *  3. 화재 알람 시 그 데이터와 시간 저장
 */
void *recvFromServer(void *);

/*
 * void *sendToServer(void *);
 * --------------------
 *  Server와 통신하는 thread    
 */
void *sendToServer(void *);

/*
 *  size_t destroyThread(void);
 *  --------------------
 *  thread를 종료한다.
 */
size_t destroyThread(void);
#endif
