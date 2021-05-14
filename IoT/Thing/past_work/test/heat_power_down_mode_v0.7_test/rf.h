/*
 * Data frame structure
 * 0xAB | 0x01 | Length | ID | CMD | (Option) | CRC
 *  Length: 전송할 데이터 갯수, From ID to CRC
 *  ID: 전송하는 Thing의 아이디, 차후 구현 예정(DIP 스위치 사용하여 ID 구분 예정)
 *  CMD
 *    1) notifyFireDetected : 0x00 ~ 0x04, 0x05 | 0x80 ~ 0x84, 0x85 | 0x8F(F)
 *    2) sendTestResult     : 0x10 ~ 0x12       | 0x90 ~ 0x92       | 0x9F(F)
 *    3) receiveTestCommand : 0x20              | 0xA0(N), 0xA1(Y)  | 0xAF(F)
 *    4) sendDatas          : 0x30 ~ 0x34       | 0xB0 ~ 0xB4       | 0xBF(F)
 *  (Option): 전송할 데이터가 없으면 이 필드는 무시되며, 전송할 데이터가 있는 경우 이 필드에 데이터를 넣어 사용
 *  CRC: 8-bit CRC 사용값
 */

#include <SoftwareSerial.h>       /*  for RF Module RX-TX */

SoftwareSerial RF_Serial(A4, A5); /*  for RF Communication, RF_Serial(RX, TX) */

unsigned char txLength;  /*  frame length  */
unsigned char txID;      /*  frame ID  */
unsigned char txCMD;     /*  frame CMD */
unsigned char txCRC;     /*  frame CRC */
boolean isNotifyFireDetectFailed;   /*  notifyFireDetected 수행 실패할 경우 true. false로 초기화 */  
boolean isSendTestResultFailed;     /*  sendTestResult 수행 실패할 경우 true. false로 초기화 */  
boolean isReceiveTestCommandFailed; /*  receiveTestCommand 수행 실패할 경우 true. false로 초기화 */  
boolean isSendDatasFailed;          /*  sendDatas 수행 실패할 경우 true. false로 초기화 */  

/*
 * [RF Module OFF] Command
 */
#define DISABLE_RF()              \
do {                              \
  RF_Serial.print((char) 0xAB);   \
  RF_Serial.print((char) 0x02);   \
} while (0)                       \

/*
 * [RF Module ON] Command
 */
#define ENABLE_RF()               \
do {                              \   
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
  RF_Serial.print((char) 0x01);
   
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

inline boolean notifyFireDetected(void)
{

  return true;
}

inline boolean sendTestResult(void)
{

  return true;
}

inline boolean receiveTestCommand(void)
{

  return true;
}

inline boolean sendDatas(void)
{

  return true;
}
