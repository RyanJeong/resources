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

// PACKET MODE
// A3300A가 직접 엔코딩/디코딩 하기 때문에 사용자는 packet을 구성하는 요소들을 신경쓰지 않아도 됨  
//
//
// 
// A3300A Transmit code - (0x11), (0x22), (0x33) 3byte data 송신
// Buadrate - 9600, data bits - 8, stop bits - 1, parity - none  (3.3V TTL)   * 5V 사용 가능

  
#include <SoftwareSerial.h>
SoftwareSerial RF_Serial(A4,A5);  // rx, tx

      char nn;
      char P_TX_DATA[6];                  // 데이터 송신
      char OFF_COM[2];                    // Module 소비전류 최소화
      char ON_COM[2];                     // Module OFF상태에서 깨움
      char CH_SET[4];                     // frequency  Group, channel 선택 
      char TX_PWR[3];                     // 송신 파워 설정
      char DATA_RATE[3];                  // data 전송속도 설정
      char CONF_READ[2];                  // 구성요소 응답 명령
      
      
      int serial_buffer[20];               //통신을 할때 buffer배열에 전송받은 데이터 입력
      int bufferIndex = 0; 


void setup() {


      Serial.begin(9600);
      RF_Serial.begin(9600);
      
      P_TX_DATA[0]  = 0xAB;       // P_TX Mode로 전환후 길이에 따라 데이터 전송
      P_TX_DATA[1]  = 0x01;       // P_TX Mode - OP code
      P_TX_DATA[2]  = 0x03;       // Length 설정 - 0x01 ~ 0x3F(63) 1바이트씩
      P_TX_DATA[3]  = 0x11;       // 3Byte data인 0x112233 전송
      P_TX_DATA[4]  = 0x22;
      P_TX_DATA[5]  = 0x33;       // 이를 수신하는 modem은 UART로 Length와 Data 반환   ex) 수신데이타 : 0x03112233  3byte 수신 (0x11,0x22,0x33)
            
      OFF_COM[0]    = 0xAB;       // RF모듈의 소비전류를 최소화
      OFF_COM[1]    = 0x02;       // Module  OFF - OP code
      
      
      ON_COM[0]     = 0xAB;       // Module  OFF -> ON   RF WAKE
      ON_COM[1]     = 0x03;       // Module  ON - OP code
      
      
      CH_SET[0]     = 0xAB;       // channel setting                *EEPROM에 저장되므로 자주 변경 시 메모리 수명에 영향을 줌
      CH_SET[1]     = 0x04;       // channel setting - OP code
      CH_SET[2]     = 0x01;       // Group 설정 - 0x00 ~ 0x03까지 설정 가능 0x01 이상부터 447MHz
      CH_SET[3]     = 0x00;       // Channel 설정 - 0x00 ~ 0x18 까지 25개의 채널 선택 가능
      
      
      TX_PWR[0]     = 0xAB;       // Tx Power Level Setting         *EEPROM에 저장되므로 자주 변경 시 메모리 수명에 영향을 줌
      TX_PWR[1]     = 0x05;       // Tx Power Level Setting - OP code
      TX_PWR[2]     = 0x0A;       // 0x00 ~ 0x0A 까지 설정 가능       *Default : 0x0A(10dBm)
      
      
      DATA_RATE[0]  = 0xAB;       // data rate 
      DATA_RATE[1]  = 0x06;       // data rate - OP code
      DATA_RATE[2]  = 0x02;       // 0x01 : 0.6kbps , 0x02 : 1.2kbps , 0x03 : 2.4kbps , 0x04 : 4.8kbps    *Default : 1.2kbps 
      
      
      
      /* Configuration Read는 0xAB0A 명령어와 Group, Channel, Data Rate, Tx Power Level 순으로 설정되어있는 값 출력  */
      CONF_READ[0]  = 0xAB;       // Configuration Read
      CONF_READ[1]  = 0x0A;       // Configuration Read - OP code
      
      
      // for(nn=0;nn<2;nn++) {                            // Module Off 상태에서 사용  
      //   RF_Serial.print(ON_COM[nn]);    // number
      // }
      
      
      /*Channel과 Tx Power Level은 처음에만 한번 설정해주고 나면 EEPROM에 저장되므로 따로 설정해주지 않아도 됨*/    
      // 처음 설정하고나서 Channel과 Tx Power Level은 주석처리하여 프로그램해야함
              
       for(nn=0;nn<4;nn++) {                            // *EEPROM에 저장되므로 자주 변경 시 메모리 수명에 영향을 줌
         RF_Serial.print(CH_SET[nn]);    // number
       }
      
       for(nn=0;nn<3;nn++) {                          // *EEPROM에 저장되므로 자주 변경 시 메모리 수명에 영향을 줌
         RF_Serial.print(TX_PWR[nn]);    // number
       }
       
       for(nn=0;nn<3;nn++) {                            // Data rate를 0x02 : 1.2kbps 로 설정하였으나 1.2kbps는 Default 설정이 되어있음
         RF_Serial.print(DATA_RATE[nn]);    // number
       }
      
       
       
}   // setup


/*
7 63 0 ac
6 62 30 44
5 61 21 4e
4 60 21 95
3 59 21 7b
2 58 21 a5
1 57 21 be
8 56 21 17
7 55 21 63
6 54 21 5f
5 53 21 e7
4 52 21 e1
3 51 21 47
2 50 21 1d
1 49 21 b8
8 48 21 b1
7 47 21 39
6 46 21 fa
5 45 21 ee
4 44 21 69
3 43 21 84
2 42 21 9
1 41 21 3f
8 40 21 5c
7 39 21 b4
6 38 21 cc
5 37 21 8d
4 36 21 81
3 35 21 fc
2 34 21 48
1 33 21 33
8 32 21 21
7 31 21 0
6 30 21 b7
5 29 21 9f
4 28 21 a0
3 27 21 4b
2 26 21 60
1 25 21 c
8 24 21 ca
7 23 21 2b
6 22 21 db
5 21 21 59
4 20 21 41
3 19 21 bb
2 18 21 e2
1 17 21 14
8 16 21 30
7 15 21 72
6 14 21 2d
5 13 21 7d
4 12 21 3
3 11 21 e4
2 10 21 b2
1 9 21 6a
8 8 21 d7
7 7 21 24
6 6 21 f5
5 5 21 c0
4 4 21 f0
3 3 21 35
2 2 21 87
1 1 50 93

 */
void loop() {
  unsigned char crc;
  unsigned char a, b;


  a = 0xBE;
  b = 0xEF;
  

  crc = 0xff;
    crc = crc8_table[crc ^ 0x3f];
    Serial.println(crc, HEX);
  crc = crc8_table[crc ^ 0x00];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x30];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);
      crc = crc8_table[crc ^ 0x21];
    Serial.println(crc, HEX);

delay(10000);  
  /*
19:55:09.012 -> 47    3f
19:55:09.012 -> AA     0
19:55:09.012 -> E2    30
19:55:09.012 -> 14
19:55:09.012 -> 30
19:55:09.012 -> 72
19:55:09.012 -> 2D
19:55:09.045 -> 7D
19:55:09.045 -> 3
19:55:09.045 -> E4
19:55:09.045 -> B2
19:55:09.045 -> 6A
19:55:09.045 -> D7
19:55:09.045 -> 24
19:55:09.045 -> F5
19:55:09.078 -> C0
19:55:09.078 -> F0
19:55:09.078 -> 35
19:55:09.078 -> 87
19:55:09.078 -> 5A
19:55:09.078 -> 12
19:55:09.078 -> 96
19:55:09.078 -> 28
19:55:09.111 -> 88
19:55:09.111 -> 74
19:55:09.111 -> 8B
19:55:09.111 -> 27
19:55:09.111 -> A6
19:55:09.111 -> ED
19:55:09.111 -> 3A
19:55:09.111 -> A9
19:55:09.144 -> C3
19:55:09.144 -> A3
19:55:09.144 -> A3
19:55:09.144 -> 18
19:55:09.144 -> 4D
19:55:09.144 -> C6
19:55:09.144 -> 56
19:55:09.144 -> 6F
19:55:09.177 -> 22
19:55:09.177 -> 53
19:55:09.177 -> 9A
19:55:09.177 -> 55
19:55:09.177 -> 3C
19:55:09.177 -> F
19:55:09.177 -> 99
19:55:09.177 -> 6
19:55:09.177 -> 11
19:55:09.247 -> C5
19:55:09.247 -> 5
19:55:09.247 -> 42
19:55:09.247 -> E8
19:55:09.247 -> CF
19:55:09.247 -> DE
19:55:09.247 -> AC
19:55:09.247 -> 36
19:55:09.247 -> D4
19:55:09.247 -> 77
19:55:09.247 -> D8
19:55:09.247 -> A
19:55:09.247 -> 6C
19:55:09.247 -> 71
19:55:09.247 -> 7E
19:55:09.247 -> 50

   */
}   // loop
