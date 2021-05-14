#include <SoftwareSerial.h>       /*  for RF Module RX-TX */

SoftwareSerial RF_Serial(A4, A5); /*  for RF Communication, RF_Serial(RX, TX) */

#define DATA_LEN    50  /*  감지기 내에 저장할 데이터 갯수 */
#define RX_BUF_LEN  6   /*  RF Module로부터 전송 받는 저장 배열의 길이 */

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

int state;  /*  현재 감지기 동작 상태(1. 화재 알림, 2. 테스트 결과 전송, 3. 테스트 명령 수신, 4. 데이터 전송)  */
char  *pTx;     /*  txData 포인터  */
char  txLength; /*  tx frame length  */
char  txID;     /*  tx frame ID  */
char  txCMD;    /*  tx frame CMD */
char  txCRC;    /*  tx frame CRC */
char  txData[DATA_LEN];  /*  센싱 값 저장 배열, 0으로 초기화  */

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

  while (!RF_Serial) {
    ;
  }
  RF_Serial.print((char) 0xAB);
  RF_Serial.print((char) 0x04);
  RF_Serial.print((char) 0x01);
  RF_Serial.print((char) 0x00);
   
  RF_Serial.print((char) 0xAB);
  RF_Serial.print((char) 0x05);
  RF_Serial.print((char) 0x0A);
  
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

inline boolean sendDatas(void)
{
  int i;

  WDTCSR  &= ~(1 << 6);       /*  Disable interrupt mode */
  digitalWrite(YELLOW, HIGH);  /*  전송 중  */        

  pTx       = txData;
  txLength  = 0x35;
  txCMD     = 0x30;
  txCRC     = getCRC60(txLength, txID, txCMD, txData);
  RF_Serial.print((char) 0xAB);
  RF_Serial.print((char) 0x01);
  RF_Serial.print((char) 0x35);
  RF_Serial.print((char) 0x00);
  RF_Serial.print((char) 0x30);
  for (i = 0; i < 50; ++i) {
    RF_Serial.print(*pTx++);
  }
  RF_Serial.print(txCRC);
  digitalWrite(GREEN, HIGH);    /*  전송 끝, CRC 에러 없음  */
  WDTCSR |= (1 << 6);         /*  Enable interrupt mode */
  
  return true;
}
