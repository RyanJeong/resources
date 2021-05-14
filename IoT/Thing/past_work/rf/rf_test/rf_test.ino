#include <SoftwareSerial.h>
//SoftwareSerial RF_Serial(A4,A5);  // rx, tx
SoftwareSerial RF_Serial(A5,A4);  // rx, tx

void test(void);

char nn;
char P_TX_DATA[6] = {0xAB, 0x01, 0x03, 0xEE, 0xEF, 0xEA};
char OFF_COM[2]   = {0xAB, 0x02};
char ON_COM[2]    = {0xAB, 0x03};
char CH_SET[4]    = {0xAB, 0x04, 0x01, 0x00};
char TX_PWR[3]    = {0xAB, 0x05, 0x0A};
char DATA_RATE[3] = {0xAB, 0x06, 0x02};
char RSSI_READ[2] = {0xAB, 0x07};
char TX_TEST[3]   = {0xAB, 0x08, 0x03};
char BER_TEST[2]  = {0xAB, 0x09};
char CONF_READ[2] = {0xAB, 0x0A};
char VERSION[2]   = {0xAB, 0x0B};

int serial_buffer[20];               //통신을 할때 buffer배열에 전송받은 데이터 입력
int bufferIndex;

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
  *  [Version] Command
  *  Header[1](0xAB) + OP Code[1](0x0B)
  *    Response data:
  *      0xAB0B53312E3030(단말기 버전, S1.00)
  */
  
void setup() {
  Serial.begin(9600);
  RF_Serial.begin(9600);
  
  for(nn = 0; nn < 4; nn++) {                            // *EEPROM에 저장되므로 자주 변경 시 메모리 수명에 영향을 줌
    RF_Serial.print(CH_SET[nn]);
  }
  
  for(nn = 0; nn < 3; nn++) {                            // *EEPROM에 저장되므로 자주 변경 시 메모리 수명에 영향을 줌
    RF_Serial.print(TX_PWR[nn]);
  }
  
  for(nn = 0; nn < 3; nn++) {                            // Data rate를 0x02 : 1.2kbps 로 설정하였으나 1.2kbps는 Default 설정이 되어있음
    RF_Serial.print(DATA_RATE[nn]);
  }
  
  delay(5000);
}

void loop() {
  Serial.println("OFF_COM start");
  delay(500);
  for(nn = 0; nn < 2; nn++) {                           
    RF_Serial.print(OFF_COM[nn]);
  }
  Serial.println("OFF_COM end");
  delay(1500);
  test();
  Serial.println("ON_COM start");
  delay(500);
  for(nn = 0; nn < 2; nn++) {                           
    RF_Serial.print(ON_COM[nn]);
  }
  Serial.println("ON_COM end");
  delay(1500);
  test();
}

void test(void)
{
  Serial.println("RSSI_READ start");
  delay(500);
  for(nn = 0; nn < 2; nn++) {                           
    RF_Serial.print(RSSI_READ[nn]);
  }
  Serial.println("RSSI_READ end");
  delay(1500);
  for (bufferIndex = 0; RF_Serial.available(); ++bufferIndex) {
    serial_buffer[bufferIndex]  = RF_Serial.read();   // 시리얼 통신으로 버퍼배열에 데이터 수신                      
    Serial.print(serial_buffer[bufferIndex],HEX);  
    Serial.print("  ");  
  }  
  Serial.println("");
  Serial.println("BER_TEST start");
  delay(500);   
  for(nn = 0; nn < 2; nn++) {                           
    RF_Serial.print(BER_TEST[nn]);
  }
  Serial.println("BER_TEST end");
  delay(1500);
  for (bufferIndex = 0; RF_Serial.available(); ++bufferIndex) {
    serial_buffer[bufferIndex]  = RF_Serial.read();   // 시리얼 통신으로 버퍼배열에 데이터 수신                      
    Serial.print(serial_buffer[bufferIndex],HEX);  
    Serial.print("  ");  
  }   
  Serial.println("");
  Serial.println("CONF_READ start");
  delay(500);
  for(nn = 0; nn < 2; nn++) {                           
    RF_Serial.print(CONF_READ[nn]);
  }
  Serial.println("CONF_READ end");
  delay(1500);
  for (bufferIndex = 0; RF_Serial.available(); ++bufferIndex) {
    serial_buffer[bufferIndex]  = RF_Serial.read();   // 시리얼 통신으로 버퍼배열에 데이터 수신                      
    Serial.print(serial_buffer[bufferIndex],HEX);  
    Serial.print("  ");  
  }   
  Serial.println("");
  Serial.println("VERSION start");
  delay(500);
  for(nn = 0; nn < 2; nn++) {                           
    RF_Serial.print(VERSION[nn]);
  }
  Serial.println("VERSION end");
  delay(1500);
  for (bufferIndex = 0; RF_Serial.available(); ++bufferIndex) {
    serial_buffer[bufferIndex]  = RF_Serial.read();   // 시리얼 통신으로 버퍼배열에 데이터 수신                      
    Serial.print(serial_buffer[bufferIndex],HEX);  
    Serial.print("  ");  
  }   
  Serial.println("");
  Serial.println("End of test routine");
  delay(2000);
  return;
}
