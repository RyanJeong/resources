int ccc;
#define SIZE_BUF    96
#define MSG_START   0
#define MSG_FINISH  1
#define MSG_RECEIVED    2
#define __CRC__         "[crc.ino]"
char     tempDbg[SIZE_BUF];

char buff[16];
#define DISABLE_RF              \
do {                            \
    Serial.print((char) 0xAB);  \
    Serial.print((char) 0x02);  \
    delay(1);                   \
} while (0)

#define DISABLE_RF_1            \
do {                            \
    Serial.print((char) 0xAB);  \
    Serial.print((char) 0x02);  \
} while (0)

#define ENABLE_RF               \
do {                            \
    Serial.print((char) 0xAB);  \
    Serial.print((char) 0x03);  \
} while (0)

#define ENABLE_RF_1             \
do {                            \
    Serial.print((char) 0xAB);  \
    Serial.print((char) 0x03);  \
    sleep(MILSEC_16);           \
} while (0)

#define MILSEC_16       0
#define MILSEC_125      3
#define MILSEC_500      5
#define SEC_1           6
#define SEC_4           33

#define ENABLE_WATCHDOG         \
do {                            \
    WDTCSR |= (1 << 6);         \
} while (0)

#define DISABLE_WATCHDOG        \
do {                            \
    WDTCSR &= ~(1 << 6);        \
} while (0)

#define WATCHDOG_TIMER_RESET        \
do {                                \
    __asm__ __volatile__("wdr");    \
} while (0)

/*  ENABLE SLEEP - this enables the sleep mode  */
#define ENABLE_SLEEP                            \
do {                                            \
    SMCR |= (1 << 2);   /*  power down mode */  \
    SMCR |= 1;          /*  enable sleep  */    \
} while (0)

/*  
 *  SLEEP
 *  1. Set both BODS and BODSE at the same time
 *  2. Then set the BODS bit and clear the BODSE bit at the same time
 *  3. Call sleep operation
 */
#define SLEEP                                   \
do {                                            \
    MCUCR   |= (3 << 5);                        \
    MCUCR   = (MCUCR & ~(1 << 5)) | (1 << 6);   \
    __asm__  __volatile__("sleep");             \
} while (0)

char nn;
char P_TX_DATA[6] = {0xAB, 0x01, 0x03, 0x82, 0x44, 0x28};
char TX_DATA[]    = {0xAB, 0x01, 0x06, 0x82, 0x44, 0x28, 0x06, 0x04, 0x02};
char OFF_COM[2]   = {0xAB, 0x02};
char ON_COM[2]    = {0xAB, 0x03};
char CH_SET[4]    = {0xAB, 0x04, 0x01, 0x03};
char TX_PWR[3]    = {0xAB, 0x05, 0x0A};
char DATA_RATE[3] = {0xAB, 0x06, 0x02};
char RSSI_READ[2] = {0xAB, 0x07};
char TX_TEST[3]   = {0xAB, 0x08, 0x03};
char BER_TEST[2]  = {0xAB, 0x09};
char CONF_READ[2] = {0xAB, 0x0A};
char VERSION[2]   = {0xAB, 0x0B};

int serial_buffer[20];               //통신을 할때 buffer배열에 전송받은 데이터 입력
int bufferIndex;

#define SET_CHANNEL_0           \
do {                            \
    Serial.print((char) 0xAB);  \
    Serial.print((char) 0x04);  \
    Serial.print((char) 0x01);  \
    Serial.print((char) 0x00);  \
    delay(1 << 7);              \
} while (0)

#define SET_CHANNEL_1           \
do {                            \
    Serial.print((char) 0xAB);  \
    Serial.print((char) 0x04);  \
    Serial.print((char) 0x01);  \
    Serial.print((char) 0x01);  \
    delay(1 << 7);              \
} while (0)

#define SET_TX_PWR_0            \
do {                            \
    Serial.print((char) 0xAB);  \
    Serial.print((char) 0x05);  \
    Serial.print((char) 0x00);  \
    delay(1 << 7);              \
} while (0)

#define SET_TX_PWR_1            \
do {                            \
    Serial.print((char) 0xAB);  \
    Serial.print((char) 0x05);  \
    Serial.print((char) 0x01);  \
    delay(1 << 7);              \
} while (0)

#define SET_DATA_RATE_0         \
do {                            \
    Serial.print((char) 0xAB);  \
    Serial.print((char) 0x06);  \
    Serial.print((char) 0x00);  \
    delay(1 << 7);              \
} while (0)

#define SET_DATA_RATE_1         \
do {                            \
    Serial.print((char) 0xAB);  \
    Serial.print((char) 0x06);  \
    Serial.print((char) 0x01);  \
    delay(1 << 7);              \
} while (0)

#define READ_CONF               \
do {                            \
    Serial.print((char) 0xAB);  \
    Serial.print((char) 0x0A);  \
} while (0)
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
    /*    EEPROM에 저장되므로 자주 변경 시 메모리 수명에 영향을 줌    */
    for(nn = 0; nn < 4; nn++) {
        Serial.print(CH_SET[nn]);
    }
    delay(1 << 7);
    /*    EEPROM에 저장되므로 자주 변경 시 메모리 수명에 영향을 줌    */
    for(nn = 0; nn < 3; nn++) {                            
        Serial.print(TX_PWR[nn]);
    }
    delay(1 << 7);
    /* Data rate를 0x02 : 1.2kbps 로 설정하였으나 1.2kbps는 Default 설정이 되어있음   */
    for(nn = 0; nn < 3; nn++) {                            
        Serial.print(DATA_RATE[nn]);
    }
    delay(1 << 7);
    enableSleep();
  
    delay(2000);
}

/*
    Serial.println("CASE 1. OFF -> SEND -> OFF -> RECV");
    Serial.flush();
    off();
    send(1);
    off();
    recv();

    Serial.println("CASE 1. OFF -> SEND -> ON -> RECV");
    Serial.flush();
    off();
    send(2);
    on();
    recv();

    Serial.println("CASE 1. ON -> SEND -> OFF -> RECV");
    Serial.flush();
    on();
    send(3);
    off();
    recv();

    Serial.println("CASE 1. ON -> SEND -> ON -> RECV");
    Serial.flush();
    on();
    send(4);
    on();
    recv();
*/
/*  Feb 28, 2020 
 *  데이터시트 상에 OFF -> RX 상태로의 전이 시, 즉 on 명령 시 필요 시간은 1(ms)만 언급되어 있음
 *  off 상태에서 delay()를 줄 경우, 다음 동작 안 함
 *  off 명령은 delay()를 주지 말고, on 명령 시에만 delay()를 주어야 함
 *  on 명령에서 delay(1) 주었을 때 동작하는 것 확인
 *  delay() -> sleep 대체 시 작동할 것인가? 
 *  test 결과,ㅡ delay() 함수 대체 시 동작하지 않음
 *
 *  결론
 *  1. 통신 시 ON -> SEND -> OFF, ON -> RECV -> OFF가 각각 한 사이클임
 *  2. Off 시에는 delay()를 사용하면 안 되고, On에서만 사용할 것
 *  3. delay() 함수를 사용하고, sleep 함수는 사용하지 말 것(동작하지 않음)

    on();
    send(5);
    off();
    on();
    recv();
    off();
*/
/*
 *  Feb 28, 2020
 *
 *  현재, 통신 시 delay()를 사용하고 있는데, 이를 sleep으로 대체할 수 있는가?
 *  send 시 delay(), recv시 sleep() 사용 가능
 *  send 후 최대한 delay()를 짧게, sleep을 길게 가져가야 최대한 전력을 아낄 수 있음
    ENABLE_RF;
    send(5);
    DISABLE_RF;
    ENABLE_RF;
    recv();
    DISABLE_RF;
 */

/*
 *  1. ENABLE -> SEND -> DISABLE, ENABLE -> RECV -> DISABLE
 *  2. DISABLE 시엔 delay 사용 금지
 *  3. ENABLE 시엔 delay 사용(delay(1))
 *  4. send 시 delay(1 << 9)
 *  5. recv 시 sleep(MILSEC_500)
 *  아래 코드가 최종 코드임
 *
 *  ENABLE_RF;
 *  send(ccc);
 *  DISABLE_RF;
 *  ENABLE_RF;
 *  recv1();
 *  DISABLE_RF;
 * 
 *  ccc++;
 */


/*
 *  각 설정을 바꾸는 데에 delay가 얼마나 필요한가?
 *  현재 기판에서 사용하는 설정은 그룹 설정, 주파수 대역 설정, 송신 세기 설정 이렇게 세 가지임
 *  테스트를 통해 delay()의 필요 여부와, 혹여 필요하다면 얼마마큼의 delay()가 필요한가 확인하고자 함
 *  각 설정 마다, delay(1 << 7) 정도의 시간이 필요함
 */

/*
    ENABLE_RF;
    SET_CHANNEL_0;
    SET_TX_PWR_0;
    SET_DATA_RATE_0;
    READ_CONF;
    delay(1 << 9);
    recv_hex();

    SET_CHANNEL_1;
    SET_TX_PWR_1;
    SET_DATA_RATE_1;
    READ_CONF;
    delay(1 << 9);
    recv_hex();
    DISABLE_RF;
*/

/*  
    Feb 28, 2020 마지막 테스트 코드
    ENABLE_RF;
    send2();
    DISABLE_RF;
    ENABLE_RF;
    recv3();
    DISABLE_RF;
*/
/*
    static const char *func = "[void loop()] ";

    sprintf(tempDbg, "%x", MSG_START);
    putsForDebug(__CRC__, func, tempDbg);
    기존에 검증되었던 코드에 putsForDebug 추가할 경우 어떻게 되는가?
    ENABLE_RF;
    send2();
    DISABLE_RF;
    ENABLE_RF;
    recv3();
    DISABLE_RF;
    static const char *func = "[void loop()] ";

    sprintf(tempDbg, "%x", MSG_START);
    putsForDebug(__CRC__, func, tempDbg);
*/
    /*  crc ... many times print serial msg */

/*  쪼개자!!    */
    //ENABLE_RF;
    //DISABLE_RF;
void loop() {
/*
    DISABLE_RF;
    send(1);
    DISABLE_RF;
    delay(1 << 9);
    recv();

    DISABLE_RF;
    send(2);
    ENABLE_RF;
    delay(1 << 9);
    recv();

    ENABLE_RF;
    send(3);
    DISABLE_RF;
    delay(1 << 9);
    recv();

    ENABLE_RF;
    send(4);
    ENABLE_RF;
    delay(1 << 9);
    recv();
    ENABLE_RF;
    DISABLE_RF;
*/

/*
    dummy();
    prepareFrame();
    dummy();
    sendAndRecv();
    dummy();
    parseFrame();
    dummy();
    sleep(MILSEC_500);
*/
/*
    dummyStr();
    prepareFrame();
    dummyStr();
    sendAndRecv();
    dummyStr();
    parseFrame();
    dummyStr();
    sleep(MILSEC_500);
*/
    dummyCombined();
    prepareFrame();
    dummyCombined();
    sendAndRecv();
    dummyCombined();
    parseFrame();
    dummyCombined();
    sleep(MILSEC_500);
}

void dummy()
{
    static const char *func = "[void dummy()] ";

    sprintf(tempDbg, "%x", MSG_START);
    putsForDebug(__CRC__, func, tempDbg);

    return;
}

void dummyCombined()
{
    static const char *func = "[void dummyCombined()]";

    sprintf(tempDbg, "%x", MSG_FINISH);
    putsForDebug(__CRC__, func, tempDbg);
    sprintf(tempDbg, "%x, string test", MSG_FINISH);
    putsForDebug(__CRC__, func, tempDbg);

    return;
}

void dummyStr()
{
    static const char *func = "[void dummyStr()] ";

    sprintf(tempDbg, "%x, string test", MSG_RECEIVED);
    putsForDebug(__CRC__, func, tempDbg);

    return;
}

void prepareFrame()
{
    TX_DATA[0] = 0xAB;
    TX_DATA[1] = 0x01;
    TX_DATA[2] = 0x06;
    TX_DATA[3] = ccc;
    TX_DATA[4] = 0xEE;
    TX_DATA[5] = 0xEE;
    TX_DATA[6] = 0xEE;
    TX_DATA[7] = 0xEE;
    TX_DATA[8] = 0xEE;
    ++ccc;
    buff[0] = 0;
    buff[1] = 0;
    buff[2] = 0;
    buff[3] = 0;
    buff[4] = 0;
    buff[5] = 0;
    buff[6] = 0;
    buff[7] = 0;

    return;
}

void sendAndRecv()
{
    int i, j;

/*  
    가정

    off 상태에서 0xAB, 0x01 만나면 상태 변이가 이루어 짐. 
    그러면, 전송 후 off 시키고, read 후 send 시 전송이 될까?

    README.md 정리 1 참고
*/
    ENABLE_RF;
    for (i = 0; i < 9; ++i) {
        Serial.print(TX_DATA[i]);
    }
    DISABLE_RF;
    delay(1 << 9);
/*
    for (i = 0; i < 500; ++i) {
        DISABLE_RF;
    }
*/
    for (j = 0; Serial.available(); ++j) {
        buff[j] = Serial.read();
    }

    return;
}

void parseFrame()
{
    int i;
    
   // for (i = 0; i < 8; ++i) { 프레임 두게 전송 결과를 볼 경우
    for (i = 0; i < 4; ++i) {
        Serial.print(buff[i], DEC);
        Serial.print(" ");
    }
    Serial.println("");
    Serial.flush();

    return;
}

void enableRF()
{
    Serial.print((char) 0xAB); 
    Serial.print((char) 0x03);
    delay(1);

    return;
}

void disableRF()
{
    Serial.print((char) 0xAB); 
    Serial.print((char) 0x02);

    return;
}

//ENABLE_RF;
//DISABLE_RF;
void send2()
{
    int i;

    for (i = 0; i < 9; ++i) {
        Serial.print(TX_DATA[i]);
    } 
    delay(1 << 9);

    return;
}

void recv3(void)
{
    int i, j;

    i = 0; 
    while (Serial.available()) {
        buff[i++] = Serial.read();
    }
    j = 0;
    while (j < i) {
        Serial.print(buff[j++], HEX);
        Serial.print(" ");
    }
    Serial.println("");
    Serial.flush();
    sleep(MILSEC_500);

    return;
}

void putsForDebug(const char *file, const char *func, const char *msg)
{
#ifndef ALALALA
    DISABLE_RF;
    Serial.print('\n');
    Serial.print('\r');
    Serial.print(file);
    Serial.print(func);
    Serial.println(msg);
    Serial.flush();
#endif

    return;
}

void off(void)
{
    for(nn = 0; nn < 2; nn++) {                           
        Serial.print(OFF_COM[nn]);
    }
    delay(1);

    return;
}

void on(void)
{
    for(nn = 0; nn < 2; nn++) {                           
        Serial.print(ON_COM[nn]);
    }

    return;
}

void send(char i)
{
    Serial.print((char) 0xAB);
    Serial.print((char) 0x01);
    Serial.print((char) 0x03);
    Serial.print(i);
    Serial.print((char) 0x33);
    Serial.print((char) 0x33);
    //delay(1 << 9);

    return;
}

void recv_hex(void)
{
    delay(500);
    while (Serial.available()) {
        Serial.print(Serial.read(), HEX);
        Serial.print(" ");
    }
    Serial.println("");
    delay(500);

    return;
}

void recv(void)
{
    while (Serial.available()) {
        Serial.print(Serial.read());
        Serial.print(" ");
    }
    Serial.println("");
    delay(500);

    return;
}

void send1(char i)
{
    Serial.print((char) 0xAB);
    Serial.print((char) 0x01);
    Serial.print((char) 0x03);
    Serial.print(i);
    Serial.print((char) 0x33);
    Serial.print((char) 0x33);
    sleep(MILSEC_500);

    return;
}

void recv1(void)
{
    while (Serial.available()) {
        Serial.print(Serial.read());
        Serial.print(" ");
    }
    Serial.println("");
    Serial.flush();
    sleep(MILSEC_500);

    return;
}

void recv2(void)
{
    while (Serial.available()) {
        Serial.print(Serial.read());
        Serial.print(" ");
    }
    Serial.println("");
    Serial.flush();
    delay(1 << 4);
    sleep(SEC_1);

    return;
}

void send3()
{
    int i;
    static const char *func = "[void send3()] ";

    Serial.print((char) 0xAB);
    Serial.print((char) 0x01);
    Serial.print((char) 0x06);
    Serial.print((char) ccc);
    Serial.print((char) 0xEE);
    Serial.print((char) 0xEE);
    Serial.print((char) 0xEE);
    Serial.print((char) 0xEE);
    Serial.print((char) 0xEE);
    Serial.flush();
    ++ccc;

    sprintf(tempDbg, "%x", MSG_START);
    putsForDebug(__CRC__, func, tempDbg);
    delay(500);
    sprintf(tempDbg, "%x", MSG_FINISH);
    putsForDebug(__CRC__, func, tempDbg);


    return;
}

void recv4(void)
{
    int i, j;
    static const char *func = "[void send3()] ";

    i = 0; 

    sprintf(tempDbg, "%x", MSG_FINISH);
    putsForDebug(__CRC__, func, tempDbg);
    while (Serial.available()) {
        buff[i++] = Serial.read();
    }
    j = 0;
    sprintf(tempDbg, "%x", MSG_FINISH);
    putsForDebug(__CRC__, func, tempDbg);
    while (j < i) {
        Serial.print(buff[j++], HEX);
        Serial.print(" ");
    }
    Serial.println("");
    Serial.flush();
    sprintf(tempDbg, "%x", MSG_FINISH);
    putsForDebug(__CRC__, func, tempDbg);
    //sleep(MILSEC_500);

    return;
}
/*
*/

void test(void)
{
  Serial.println("OFF_COM start");
  delay(500);
  for(nn = 0; nn < 2; nn++) {                           
    Serial.print(OFF_COM[nn]);
  }
  Serial.println("OFF_COM end");
  delay(1500);
  Serial.println("RSSI_READ start");
  delay(500);
  for(nn = 0; nn < 2; nn++) {                           
    Serial.print(RSSI_READ[nn]);
  }
  Serial.println("RSSI_READ end");
  delay(1500);
  for (bufferIndex = 0; Serial.available(); ++bufferIndex) {
    serial_buffer[bufferIndex]  = Serial.read();   // 시리얼 통신으로 버퍼배열에 데이터 수신                      
    Serial.print(serial_buffer[bufferIndex],HEX);  
    Serial.print("  ");  
  }  
  Serial.println("");
  Serial.println("BER_TEST start");
  delay(500);   
  for(nn = 0; nn < 2; nn++) {                           
    Serial.print(BER_TEST[nn]);
  }
  Serial.println("BER_TEST end");
  delay(1500);
  for (bufferIndex = 0; Serial.available(); ++bufferIndex) {
    serial_buffer[bufferIndex]  = Serial.read();   // 시리얼 통신으로 버퍼배열에 데이터 수신                      
    Serial.print(serial_buffer[bufferIndex],HEX);  
    Serial.print("  ");  
  }   
  Serial.println("");
  Serial.println("CONF_READ start");
  delay(500);
  for(nn = 0; nn < 2; nn++) {                           
    Serial.print(CONF_READ[nn]);
  }
  Serial.println("CONF_READ end");
  delay(1500);
  for (bufferIndex = 0; Serial.available(); ++bufferIndex) {
    serial_buffer[bufferIndex]  = Serial.read();   // 시리얼 통신으로 버퍼배열에 데이터 수신                      
    Serial.print(serial_buffer[bufferIndex],HEX);  
    Serial.print("  ");  
  }   
  Serial.println("");
  Serial.println("VERSION start");
  delay(500);
  for(nn = 0; nn < 2; nn++) {                           
    Serial.print(VERSION[nn]);
  }
  Serial.println("VERSION end");
  delay(1500);
  for (bufferIndex = 0; Serial.available(); ++bufferIndex) {
    serial_buffer[bufferIndex]  = Serial.read();   // 시리얼 통신으로 버퍼배열에 데이터 수신                      
    Serial.print(serial_buffer[bufferIndex],HEX);  
    Serial.print("  ");  
  }   
  Serial.println("");
  Serial.println("End of test routine");
  delay(2000);

  return;
}

void sleep(int i)
{
    /*  change enable and WDE - also resets */
    WDTCSR  = (24);
    /*  prescalers only - get rid of the WDE and WDCE bit */
    WDTCSR  = (i);      
    /*  enable watchdog timer   */
    ENABLE_WATCHDOG;    
    SLEEP;

    return;
}

void sleepOneMinute(void)
{
    /*  change enable and WDE - also resets */
    WDTCSR  = (24);
    /*  prescalers only - get rid of the WDE and WDCE bit */
    WDTCSR  = (33);      
    /*  enable watchdog timer   */
    ENABLE_WATCHDOG;    
    /*  16 sec * 4 times    */
    SLEEP;
    SLEEP;
    SLEEP;
    SLEEP;

    return;
}

void enableSleep(void)
{
    /*  clear interrupts    */
    cli();
    /*  enable sleep    */
    ENABLE_SLEEP;
    /*  set interrupts  */
    sei();

    return;
}

ISR(WDT_vect)
{
    /*  
     *  Watchdog Timer
     *  Enable BOD
     *      The BODS bit is automatically cleared after three clock cycles.
     *      Writing this bit to one turns off the BOD in relevant sleep modes, while a zero in this bit keeps
     *      BOD active. Default setting keeps BOD active, i.e. BODS set to zero.
     */  
}

