/*
 * Data frame structure
 * 0xAB | 0x01 | Length | ID | CMD | (Option) | CRC
 *  Length: 전송할 데이터 갯수, From ID to CRC
 *  ID: 전송하는 Thing의 아이디, 차후 구현 예정(DIP 스위치 사용하여 ID 구분 예정)
 *  CMD
 *    1) notifyFireDetected : 0x00(init), 0x01(after)         | 0x80, 0x81        | 0x8F(F)
 *    2) sendTestResult     : 0x10, 0x01(test1), 0x02(test2)  | 0x90, 0x91, 0x92  | 0x9F(F)
 *    3) receiveTestCommand : 0x20                            | 0xA0(N), 0xA1(Y)  | 0xAF(F)
 *    4) sendDatas          : 0x30                            | 0xB0              | 0xBF(F)
 *  (Option): 전송할 데이터가 없으면 이 필드는 무시되며, 전송할 데이터가 있는 경우 이 필드에 데이터를 넣어 사용
 *  CRC: 8-bit CRC 사용값
 */

#include <SoftwareSerial.h>       /*  for RF Module RX-TX */

SoftwareSerial RF_Serial(A4, A5); /*  for RF Communication, RF_Serial(RX, TX) */

#define DATA_LEN    50  /*  감지기 내에 저장할 데이터 갯수 */
#define RX_BUF_LEN  6   /*  RF Module로부터 전송 받는 저장 배열의 길이 */

#define WAITING           0
#define FILE_ALARM        1
#define SEND_TEST_RESULT  2
#define RECV_TEST_CMD     3
#define SEND_DATAS        4
#define GREEN         5
#define YELLOW        6
#define RED           7


static unsigned char const crc8_table[] = {
    0x00, 0x31, 0x62, 0x53, 0xc4, 0xf5, 0xa6, 0x97, 
    0xb9, 0x88, 0xdb, 0xea, 0x7d, 0x4c, 0x1f, 0x2e, 
    0x43, 0x72, 0x21, 0x10, 0x87, 0xb6, 0xe5, 0xd4, 
    0xfa, 0xcb, 0x98, 0xa9, 0x3e, 0x0f, 0x5c, 0x6d, 
    0x86, 0xb7, 0xe4, 0xd5, 0x42, 0x73, 0x20, 0x11, 
    0x3f, 0x0e, 0x5d, 0x6c, 0xfb, 0xca, 0x99, 0xa8, 
    0xc5, 0xf4, 0xa7, 0x96, 0x01, 0x30, 0x63, 0x52, 
    0x7c, 0x4d, 0x1e, 0x2f, 0xb8, 0x89, 0xda, 0xeb, 
    0x3d, 0x0c, 0x5f, 0x6e, 0xf9, 0xc8, 0x9b, 0xaa, 
    0x84, 0xb5, 0xe6, 0xd7, 0x40, 0x71, 0x22, 0x13, 
    0x7e, 0x4f, 0x1c, 0x2d, 0xba, 0x8b, 0xd8, 0xe9, 
    0xc7, 0xf6, 0xa5, 0x94, 0x03, 0x32, 0x61, 0x50, 
    0xbb, 0x8a, 0xd9, 0xe8, 0x7f, 0x4e, 0x1d, 0x2c,
    0x02, 0x33, 0x60, 0x51, 0xc6, 0xf7, 0xa4, 0x95, 
    0xf8, 0xc9, 0x9a, 0xab, 0x3c, 0x0d, 0x5e, 0x6f, 
    0x41, 0x70, 0x23, 0x12, 0x85, 0xb4, 0xe7, 0xd6, 
    0x7a, 0x4b, 0x18, 0x29, 0xbe, 0x8f, 0xdc, 0xed, 
    0xc3, 0xf2, 0xa1, 0x90, 0x07, 0x36, 0x65, 0x54, 
    0x39, 0x08, 0x5b, 0x6a, 0xfd, 0xcc, 0x9f, 0xae, 
    0x80, 0xb1, 0xe2, 0xd3, 0x44, 0x75, 0x26, 0x17, 
    0xfc, 0xcd, 0x9e, 0xaf, 0x38, 0x09, 0x5a, 0x6b, 
    0x45, 0x74, 0x27, 0x16, 0x81, 0xb0, 0xe3, 0xd2, 
    0xbf, 0x8e, 0xdd, 0xec, 0x7b, 0x4a, 0x19, 0x28, 
    0x06, 0x37, 0x64, 0x55, 0xc2, 0xf3, 0xa0, 0x91, 
    0x47, 0x76, 0x25, 0x14, 0x83, 0xb2, 0xe1, 0xd0, 
    0xfe, 0xcf, 0x9c, 0xad, 0x3a, 0x0b, 0x58, 0x69,
    0x04, 0x35, 0x66, 0x57, 0xc0, 0xf1, 0xa2, 0x93, 
    0xbd, 0x8c, 0xdf, 0xee, 0x79, 0x48, 0x1b, 0x2a, 
    0xc1, 0xf0, 0xa3, 0x92, 0x05, 0x34, 0x67, 0x56, 
    0x78, 0x49, 0x1a, 0x2b, 0xbc, 0x8d, 0xde, 0xef, 
    0x82, 0xb3, 0xe0, 0xd1, 0x46, 0x77, 0x24, 0x15, 
    0x3b, 0x0a, 0x59, 0x68, 0xff, 0xce, 0x9d, 0xac};
    
boolean isNotifyFireDetectFailed;   /*  notifyFireDetected 수행 실패할 경우 true. false로 초기화 */  
boolean isSendTestResultFailed;     /*  sendTestResult 수행 실패할 경우 true. false로 초기화 */  
boolean isReceiveTestCommandFailed; /*  receiveTestCommand 수행 실패할 경우 true. false로 초기화 */  
boolean isSendDatasFailed;          /*  sendDatas 수행 실패할 경우 true. false로 초기화 */  
boolean crcError;                   /*  CRC 확인 결과 값에 오류가 있음, 재전송 요청, false로 초기화 */
int  recv;              /*  어느 동작을 수행중인지 저장, 초기값: WAITING*/

char  *pRx;     /*  rxBuf 포인터 */
char  *pTx;     /*  txData 포인터  */
char  txLength; /*  frame length  */
char  txID;     /*  frame ID  */
char  txCMD;    /*  frame CMD */
char  txCRC;    /*  frame CRC */
char  txData[DATA_LEN];  /*  센싱 값 저장 배열, 0으로 초기화  */
char  rxBuf[RX_BUF_LEN]; /*  RF Module로부터 전송되는 값 저장 배열 */

/*
 * [RF Module OFF] Command
 */
#define DISABLE_RF()              \
do {                              \
  delay(1);                       \
  RF_Serial.print((char) 0xAB);   \
  RF_Serial.print((char) 0x02);   \
} while (0)                       \

/*
 * [RF Module ON] Command
 */
#define ENABLE_RF()               \
do {                              \   
  delay(1);                       \
  RF_Serial.print((char) 0xAB);   \
  RF_Serial.print((char) 0x03);   \
} while (0)                       \

/*
 *  rfModuleSetup(void)
 *  Function that prepare to use the RF Module
 *  Perform only once
 *  !!Notice!!
 *   Arduino Serial에서는 타입에 따라 전송되는 형태가 다르므로
 *   character type 명시해주어야 정상적으로 작동됨..
 */
inline void rfModuleSetup(void)
{
  RF_Serial.begin(9600);

  /*
   * waiting for finish set up
   */
  while (!RF_Serial) {
    ;
  }
  /*  
   *  Pre-processing to use the RF Module
   *  Channel(Frequency) Setting -> Tx Power Level Setting -> Data Rate Setting
   *  
   *  [P_TX Data] Command
   *  Header[1](0xAB) + OP Code[1](0x01) + Length[1](0x01 ~ 0x3F) + Data[1 ~ 63](Length)
   *    ex) [TX]: 0xAB0103112233 -> [RX]: 0x03112233
   *  
   *  [Module OFF] Command
   *  Header[1](0xAB) + OP Code[1](0x02)
   *  
   *  [Module ON] Command
   *  Header[1](0xAB) + OP Code[1](0x03)
   *  
   *  [Channel(Frequency) Setting] Command
   *  - EEPROM에 저장되므로 자주 변경 시 메모리 수명에 영향을 줌
   *  Header[1](0xAB) + OP Code[1](0x04) + Channel Group[1] + Channel Number[1]
   *    Group   : 0x00 ~ 0x03 까지 설정 가능, 0x01 이상부터 447MHz
   *    Number  : 0x00 ~ 0x18 까지 25개의 채널 중 하나 선택 가능
   *    ----------------------------------------------------------------
   *    Ch No     Ch Gr = 0x00  Ch Gr = 0x01  Ch Gr = 0x02  Ch Gr = 0x03
   *    1(0x00)   424.7000      447.2625      447.6000      447.8625
   *    2(0x01)   424.7125      447.2750      447.6125      447.8750
   *    3(0x02)   424.7250      447.2875      447.6250      447.8875
   *    4(0x03    424.7375      447.3000      447.6375      447.9000
   *    5(0x04)   424.7500      447.3125      447.6500      447.9125
   *    6(0x05)   424.7625      447.3250      447.6625      447.9250
   *    7(0x06)   424.7750      447.3375      447.6750      447.9375
   *    8(0x07)   424.7875      447.3500      447.6875      447.9500
   *    9(0x08)   424.8000      447.3625      447.7000      447.9625
   *    10(0x09)  424.8125      447.3750      447.7125      447.9750
   *    11(0x0A)  424.8250      447.3875      447.7250      447.9875
   *    12(0x0B)  424.8375      447.4000      447.7375      -
   *    13(0x0C)  424.8500      447.4125      447.7500      -
   *    14(0x0D)  424.8625      447.4250      447.7625      -
   *    15(0x0E)  424.8750      447.4375      447.7750      -
   *    16(0x0F)  424.8875      447.4500      447.7875      -
   *    17(0x10)  424.9000      447.4625      447.8000      -
   *    18(0x11)  424.9125      447.4750      447.8125      -
   *    19(0x12)  424.9250      447.4875      447.8250      -
   *    20(0x13)  424.9375      447.5000      447.8375      -
   *    21(0x14)  424.9500      447.5125      447.8500      -
   *    22(0x15)  -             447.5250      -             -
   *    23(0x16)  -             447.5375      -             -
   *    24(0x17)  -             447.5500      -             -
   *    25(0x18)  -             447.5625      -             -
   *    ----------------------------------------------------------------
   *    
   *  [Tx Power Level Setting] Command
   *  - EEPROM에 저장되므로 자주 변경 시 메모리 수명에 영향을 줌
   *  Header[1](0xAB) + OP Code[1](0x05) + Power Level[1](0x00 ~ 0x0A, Default: 0x0A(10 dBm))
   *  
   *  [Data Rate Setting] Command
   *  - EEPROM에 저장되므로 자주 변경 시 메모리 수명에 영향을 줌
   *  Header[1](0xAB) + OP Code[1](0x06) + Data Rate[1](0x01 ~ 0x04, Default: 0x02)
   *    0x01: 0.6kbps
   *    0x02: 1.2kbps
   *    0x03: 2.4kbps
   *    0x04: 4.8kbps
   *  
   *  [RSSI Read] Command
   *  Header[1](0xAB) + OP Code[1](0x07)
   *    Response data:
   *      0xAB0705:  -5 dBm
   *      0xAB0750: -80 dBm
   *      
   *  [TX Test] Command
   *  Header[1](0xAB) + OP Code[1](0x08) + Mode[1](0x01 ~ 0x03)
   *    0x01: carrier test 시작
   *    0x02: 2 kHz modulation 시작
   *    0x03: test 종료 후 P_RX Mode로 전환
   *    
   *  [BER Test] Command
   *  Header[1](0xAB) + OP Code[1](0x09)
   *    Response data: 
   *      0xAB090100수
   *        BER 1,000개 중 1kHz 데이터 갯수
   *        
   *  
   *  [Configureation Read] Command
   *  Header[1](0xAB) + OP Code[1](0x0A)
   *    Response data:
   *      0xAB0A01020304
   *        0x01: Group
   *        0x02: Channel
   *        0x03: Data Rate
   *        0x04: TX Power Level
   *        
   *  [Versionj] Command
   *  Header[1](0xAB) + OP Code[1](0x0B)
   *    Response data:
   *      0xAB0B53312E3030(단말기 버전, S1.00)
   */

  /*
   * Channel and Group Setting
   */
  RF_Serial.print((char) 0xAB);
  RF_Serial.print((char) 0x04);
  RF_Serial.print((char) 0x01);
  RF_Serial.print((char) 0x00);
   
  /*
   *  Tx Power Level Setting
   *  10 dBm
   */
  RF_Serial.print((char) 0xAB);
  RF_Serial.print((char) 0x05);
  RF_Serial.print((char) 0x0A);
  
  /*
   *  Data Rate Setting
   *  1.2 kbps
   */
  RF_Serial.print((char) 0xAB);
  RF_Serial.print((char) 0x06);
  RF_Serial.print((char) 0x02);

  return;
}


/*
 * no parameter,
 * should be crc 0xFF
 */
char getCRC0(unsigned char len, unsigned char id, unsigned char CMD)
{
  unsigned char  crc;

  crc   = 0xFF;
  crc   = crc8_table[crc ^ len];
  crc   = crc8_table[crc ^ id];
  crc   = crc8_table[crc ^ CMD];
  
  return crc;
}

/*
 * 1 parameter,
 * should be crc 0xFF
 */ 
char getCRC1(unsigned char len, unsigned char id, unsigned char CMD, unsigned char param1)
{
  unsigned char  crc;

  crc   = 0xFF;
  crc   = crc8_table[crc ^ len];
  crc   = crc8_table[crc ^ id];
  crc   = crc8_table[crc ^ CMD];
  crc   = crc8_table[crc ^ param1];
  
  return crc;
}

/*
 * 2 parameters
 * should be crc 0xFF
 */
char getCRC2(unsigned char len, unsigned char id, unsigned char CMD, unsigned char param1, unsigned char param2)
{
  unsigned char  crc;

  crc   = 0xFF;
  crc   = crc8_table[crc ^ len];
  crc   = crc8_table[crc ^ id];
  crc   = crc8_table[crc ^ CMD];
  crc   = crc8_table[crc ^ param1];
  crc   = crc8_table[crc ^ param2];
  
  return crc;
}

/*
 * 60 parameters(get in array)
 * should be crc 0xFF
 */
char getCRC60(unsigned char len, unsigned char id, unsigned char cmd, unsigned char *data)
{
  unsigned char crc;
  int           i;

  crc = 0xFF;
  //Serial.println(crc, HEX);
  crc = crc8_table[crc ^ len];
  //Serial.println(crc, HEX);
  crc = crc8_table[crc ^ id];
  //Serial.println(crc, HEX);
  crc = crc8_table[crc ^ cmd];
  //Serial.println(crc, HEX);
  for (i = 0; i < DATA_LEN; ++i) {    
    crc = crc8_table[crc ^ *data++];
    //Serial.println(crc, HEX);
  }
  
  return crc;
}

inline boolean notifyFireDetected(void)
{
  recv      = FILE_ALARM;
  ENABLE_RF();
  WDTCSR &= ~(1 << 6);        /*  Disable interrupt mode */

  
  DISABLE_RF();
  WDTCSR |= (1 << 6);           /*  Enable interrupt mode */
  return true;
}

inline boolean sendTestResult(void)
{
  recv      = SEND_TEST_RESULT;
  ENABLE_RF();
  WDTCSR &= ~(1 << 6);        /*  Disable interrupt mode */
  
  DISABLE_RF();
  WDTCSR |= (1 << 6);           /*  Enable interrupt mode */
  return true;
}

inline boolean receiveTestCommand(void)
{
  recv      = RECV_TEST_CMD;
  ENABLE_RF();
  WDTCSR &= ~(1 << 6);        /*  Disable interrupt mode */
  
  DISABLE_RF();
  WDTCSR |= (1 << 6);           /*  Enable interrupt mode */
  return true;
}

inline boolean sendDatas(void)
{
  int i;

      int serial_buffer[20];               //통신을 할때 buffer배열에 전송받은 데이터 입력
      int bufferIndex = 0; 
      
  WDTCSR  &= ~(1 << 6);       /*  Disable interrupt mode */
  recv    = SEND_DATAS;

  digitalWrite(YELLOW, HIGH);  /*  전송 중  */        

  pTx       = txData;
  txLength  = 0x35;
  txCMD     = 0x30;
  txCRC     = getCRC60(txLength, txID, txCMD, txData);

  ENABLE_RF();
  RF_Serial.print((char) 0xAB);
  RF_Serial.print((char) 0x01);
  RF_Serial.print(txLength);
  RF_Serial.print(txID);
  RF_Serial.print(txCMD);
  for (i = 0; i < 50; ++i) {
    RF_Serial.print(*pTx++);
  }
  RF_Serial.print(txCRC);
  TIMSK1  |= (1 << OCIE1A);   /*  Enable timer compare interrupt 1  */
  TCNT1   = 0;                /*  reset Timer 1 counter register  */

  while (recv) {
    while(RF_Serial.available()) {
      serial_buffer[bufferIndex]  = RF_Serial.read();   // 시리얼 통신으로 버퍼배열에 데이터 수신                      
      Serial.print(serial_buffer[bufferIndex],HEX);  
      Serial.print("  ");  
      bufferIndex++;   
    } 
    Serial.println(bufferIndex);
    if (bufferIndex) {
      
      break;
    }
  }
  TIMSK1 &= ~(1 << OCIE1A);   /*  Disable timer compare interrupt 1  */
  DISABLE_RF();
  digitalWrite(YELLOW, LOW);    /*  전송 끝, 결과 수신 */
  if (!recv) {                  /*  데이터를 정해진 시간 내에 받지 못함  */
      digitalWrite(RED, HIGH);  /*  에러  */        
  } else {
    if (crcError) {             /*  데이터를 정해진 시간 내에 받았으나, CRC 에러 발생*/
      digitalWrite(RED, HIGH);  /*  에러  */
    } else {        /*  이상 없음*/
      digitalWrite(GREEN, HIGH);  /*  완료  */
    }
  }
  /*  receive something ...   */
  WDTCSR |= (1 << 6);         /*  Enable interrupt mode */


  
  return true;
}
