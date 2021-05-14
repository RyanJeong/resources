#include <SoftwareSerial.h>       /*  for RF Module RX-TX */

SoftwareSerial RF_Serial(A4, A5); /*  for RF Communication, RF_Serial(RX, TX) */

#define ABS(x)      (x * ((x > 0) - (x < 0)))
#define HEAT_TEST   2           /*  TR 열 발생 핀 2번  */
#define DATA_LEN    300         /*  감지기 내에 저장할 데이터 갯수 */
#define BUF_LEN     16          /*  RF Module로부터 전송 받는 저장 배열의 길이 */
#define LED_NORMAL  5           /*  현재 감지기 상태: 보통, 초록 LED */
#define LED_WARNING 6           /*  현재 감지기 상태: 경고, 노랑 LED */
#define LED_ALERT   7           /*  현재 감지기 상태: 알림, 빨 LED */
#define Q1          (0.0001233) /*  y = (Q1 * x^2) + (Q2 * x) + Q3  */
#define Q2          (-0.0389)   /*  y = (Q1 * x^2) + (Q2 * x) + Q3  */
#define Q3          (29.1755)   /*  y = (Q1 * x^2) + (Q2 * x) + Q3  */
#define ALPHA       (0.9)       /*  Noise filter 계수 */
#define LAPSE       4           /*  화재 지속 판전 유보 시간  */
#define INIT        30          /*  초기 전원 인가시 화재 비감지 시간, 30초 */
#define HEATING     480         /*  트랜지스터 가열 시간 (0.125s * 480) */
#define THRESHOLD   60          /*  Fire alarm temp.[C] */
/*  
 *   __asm__ __volatile__ (asms : output : input : clobber);
 *    To disable BOD in relevant sleep modes,
 *    both BODS and BODSE must first be set to one. Then, to set the BODS bit, BODS must be set to one and BODSE must be
 *    set to zero within four clock cycles. 
 */
#define BOD_DISABLE()                                               \
do {                                                                \
  unsigned char tempreg;                                            \
  __asm__ __volatile__("in %[tempreg], %[mcucr]" "\n\t"             \
                       "ori %[tempreg], %[bods_bodse]" "\n\t"       \
                       "out %[mcucr], %[tempreg]" "\n\t"            \
                       "andi %[tempreg], %[not_bodse]" "\n\t"       \
                       "out %[mcucr], %[tempreg]"                   \
                       : [tempreg] "=&d" (tempreg)                  \
                       : [mcucr] "I" _SFR_IO_ADDR(MCUCR),           \
                         [bods_bodse] "i" (_BV(BODS) | _BV(BODSE)), \
                         [not_bodse] "i" (~_BV(BODSE)));            \
} while (0)

/*  
 *  implement printf for debugging  
 *  if (!isDebugging) {
 *    ignore
 *  } else {
 *    print to serial
 *  }
 *  
 *  
 *  /*  example: variadic_macros.cpp
 *  #include <stdio.h>
 *  #define EMPTY
 *  
 *  #define CHECK1(x, ...) if (!(x)) { printf(__VA_ARGS__); }
 *  #define CHECK2(x, ...) if ((x)) { printf(__VA_ARGS__); }
 *  #define CHECK3(...) { printf(__VA_ARGS__); }
 *  #define MACRO(s, ...) printf(s, __VA_ARGS__)
 *  
 *  int main() {
 *    CHECK1(0, "here %s %s %s", "are", "some", "varargs1(1)\n");
 *    CHECK1(1, "here %s %s %s", "are", "some", "varargs1(2)\n"); /*  won't print
 *  
 *    CHECK2(0, "here %s %s %s", "are", "some", "varargs2(3)\n");   /*  won't print
 *    CHECK2(1, "here %s %s %s", "are", "some", "varargs2(4)\n");
 *  
 *    /*  always invokes printf in the macro
 *    CHECK3("here %s %s %s", "are", "some", "varargs3(5)\n");
 *  
 *    MACRO("hello, world\n");
 *  
 *    MACRO("error\n", EMPTY);  /*  would cause error C2059, except VC++
 *                              /*  suppresses the trailing comma
 *  }
 */
#define _PRINTF_BUFFER_LENGTH_  64  /*  buffer size for printf  */

/*
 *  주석 혼용 방지위해 #if 1(#if 0) 사용 
 */
#if 1
static char _pf_buffer_[_PRINTF_BUFFER_LENGTH_];
#else
extern char _pf_buffer_[_PRINTF_BUFFER_LENGTH_];
#endif

/*
 * normal printf
 */
#define D_PRINTF(flag, _obj_, a, ...)                             \
do {                                                              \
  if (flag) {                                                     \
    snprintf(_pf_buffer_, sizeof(_pf_buffer_), a, ##__VA_ARGS__); \
    _obj_.print(_pf_buffer_);                                     \
    _obj_.flush();                                                \
  }                                                               \
} while (0)

/*
 * printf + new line
 */
#define D_PRINTF_NEW_LINE(flag, _obj_, a, ...)                          \
do {                                                                    \
  if (flag) {                                                           \
    snprintf(_pf_buffer_, sizeof(_pf_buffer_), a"\r\n", ##__VA_ARGS__); \
    _obj_.print(_pf_buffer_);                                           \
    _obj_.flush();                                                      \
  }                                                                     \
} while (0)

/*
 * [RF Module ON] Command
 */
#define ENABLE_RF()       \
do {                      \
  RF_Serial.print((char) 0xAB);  \
  RF_Serial.print((char) 0x03);  \
} while (0)               \

/*
 * [RF Module OFF] Command
 */
#define DISABLE_RF()      \
do {                      \
  RF_Serial.print((char) 0xAB);  \
  RF_Serial.print((char) 0x02);  \
} while (0)               \



int   data[DATA_LEN]; /*  센싱 값 저장 배열, 0으로 초기화  */
int   index;          /*  값 저장에 사용하는 인덱스, 0으로 초기화  */
int   countNumDetect; /*  화재 감지 지속 시간 측정 카운터, 0으로 초기화  */
int   countNumReset;  /*  감지기 전원 인가 또는 감지기 Reset 시 초기 시작 카운터, 0으로 초기화  */
int   isTest;         /*  테스트 수행 여부, false로 초기화 */
int   isWarning;      /*  화재 경고 여부, false로 초기화  */
int   isSetup;        /*  감지기 전원 인가 또는 감지기 Reset 여부, false로 초기화 */
int   isSend;         /*  전송할 메세지 유무 여부, false로 초기화 */
int   isRecv;         /*  RF 신호 전송받는 상태 여부, false로 초기화  */

int   isDebugging;    /*  현재 디버깅 모드인가?  */

float pastValue;  /*  전원 인가 시 감지기 초기 가정 온도 */
int   threshold;  /*  화재 감지에 사용되는 threshold, (THRESHOLD - 1) | THRESHOLD  */
int   i;          /*  for condition branch var. */
float temp;       /*  a value which get from ADC  */
float filtered1;  /*  filtered temp value */
float filtered2;  /*  filtered temp value */

byte  recv[BUF_LEN];  /*  RF Module로부터 전송 받은 데이터 저장 배열 */
int   recvIndex;      /*  RF Module로부터 전송 받은 데이터 길이, 처리 후 초기화 필수, 0으로 초기화 */

/*
 *  rfModuleSetup(void)
 *  Function that prepare to use the RF Module
 *  Perform only once
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
   *  Channel(Frequency) Setting
   *  447.2625 MHz
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

  DISABLE_RF(); /*  RF Module OFF */
  
  return;
}

/*
 *  timer1Setup(void)
 *  Function that prepare to use the timer1
 *  Perform only once
 *  use for receiving rf msg
 *    124 :  8 ms
 *    249 : 16 ms
 *    374 : 24 ms
 */
inline void timer1Setup(void)
{
  TCCR1A  = 0;              /*  Reset TCCR1A */
  TCCR1B  = 0;              /*  Reset TCCR1B */
  TCNT1   = 0;              /*  Reset Timer1 Counter  */
  OCR1A   = 374;            /*  Compare Match Register 16MHz/1024/2Hz  */
  TCCR1A |= (1 << COM1A1);  /*  Clear OC1A/OC1B on Compare Match, Set OC1A/OC1B at BOTTOM (non-inverting mode) */
  TCCR1A |= (1 << WGM10);   /*  Fast Pulse Width Modulation (PWM) */
  TCCR1A |= (1 << WGM11);   /*  Fast Pulse Width Modulation (PWM) */
  TCCR1B |= (1 << WGM12);   /*  Fast Pulse Width Modulation (PWM) */
  TCCR1B |= (1 << WGM13);   /*  Fast Pulse Width Modulation (PWM) */
  /*
   *  CSx2  CSx1  CSx0  선택 분주 클럭
   *  0     0     0     정지
   *  0     0     1     CK
   *  0     1     0     CK/8
   *  0     1     1     CK/64
   *  1     0     0     CK/256
   *  1     0     1     CK/1024
   *  1     1     0     외부 클럭(T1 또는 T2)의 하강 에지
   *  1     1     1     외부 클럭(T1 또는 T2)의 상승 에지 
   */
  TCCR1B |= (1 << CS12) | (1 << CS10);    /*  1024 prescaler */

  /*
   * TIMSK1 &= ~(1 << OCIE1A);  Disable timer compare interrupt 1
   * TIMSK1 |= (1 << OCIE1A);   Enable timer compare interrupt 1
   */
  TIMSK1 &= ~(1 << OCIE1A);
  
  return;
}

/*
 *  watchdogSetup(void)
 *  Function that prepare to use the watchdog
 *  Perform only once
 *  use for wake-up mcu
 */
inline void watchdogSetup(void)
{
  /*  SETUP WATCHDOG TIMER
   *  ------
   *  WDTCSR = (24);      : change enable and WDE - also resets
   *  WDTCSR = (6);       : prescalers only - get rid of the WDE and WDCE bit
   *      WDP3 - WDP2 - WPD1 - WDP0 - time
   *      0      0      0      0      16 ms
   *      0      0      0      1      32 ms
   *      0      0      1      0      64 ms
   *      0      0      1      1      0.125 s
   *      0      1      0      0      0.25 s
   *      0      1      0      1      0.5 s
   *      0      1      1      0      1.0 s
   *      0      1      1      1      2.0 s
   *      1      0      0      0      4.0 s
   *      1      0      0      1      8.0 s
   *      
   *  WDTCSR |= (1<<6);  : enable interrupt mode
   */
  WDTCSR = (24);      /*  change enable and WDE - also resets */
  WDTCSR = (6);       /*  prescalers only - get rid of the WDE and WDCE bit */
  
  /*  ENABLE SLEEP - this enables the sleep mode  */
  SMCR |= (1 << 2);   /*  power down mode */
  SMCR |= 1;          /*  enable sleep  */

  WDTCSR |= (1 << 6); /*  enable interrupt mode */
  
  return;
}

/*
 * Send message to the gateway(Raspberry Pi)
 *  1. if fire detected, than send datas
 *  2. if test signal received, than send test response(success, failed) 
 *  3. if ping signal received, than send ping msg
 *  send success, return true
 */
inline boolean sendMsg(void)
{

  /*  Do something ...  */
  
  return true;
}

/*
 * receive message from the gateway(Raspberry Pi)
 *  1. Ping msg
 *  2. Test signal
 */
inline boolean recvMsg(void)
{
  /*  
   *   Do something ...
   * 
   *  if (ping) {
   *    sendMsg();  /*  send response
   *  } elif (test) {
   *      isTest = true;
   *  }     
   */

   /*
  while (RF_Serial.available()) {
    recv[recvIndex++] = RF_Serial.read();
  }
  for (i = recvIndex; recvIndex; ) {
    D_PRINTF_NEW_LINE(isDebugging, Serial, "%x%c", recv[recvIndex--], '\t');
  }
  recvIndex = 0;  //recv 배열에 저장한 데이터 처리 완료 후 index 초기화
  */

  
  while (isRecv) {
    D_PRINTF_NEW_LINE(isDebugging, Serial, "%s", "Receiving...");
  }
  
  return true;
}

/*  화재 비감지 루틴 */
inline void processReset(void)
{
  digitalWrite(LED_NORMAL, HIGH);   /*  화재 비감지 시간 동안은 초록, 노란색, 적색 LED 모두 ON  */
  digitalWrite(LED_WARNING, HIGH);  /*  화재 비감지 시간 동안은 초록, 노란색, 적색 LED 모두 ON  */        
  digitalWrite(LED_ALERT, HIGH);    /*  화재 비감지 시간 동안은 초록, 노란색, 적색 LED 모두 ON  */
  D_PRINTF_NEW_LINE(isDebugging, Serial, "%d", --countNumReset);
  temp      = (float) analogRead(A0);
  filtered1 = (Q1 * temp * temp) + (Q2 * temp) + Q3;
  temp      = (float) analogRead(A1);
  filtered2 = (Q1 * temp * temp) + (Q2 * temp) + Q3;
  /*
   * Due to some performance reasons %f is not included in the Arduino's implementation(use type cast)
   */
  D_PRINTF_NEW_LINE(isDebugging, Serial, "%s%d%c%s%d", "Test mode sensor: ", (int) filtered1, '\t', "Fire detect sensor: ", (int) filtered2);  data[index++] = filtered2;
  if (index == DATA_LEN) {
    index = 0;
  }
  filtered1 = (ALPHA * pastValue) + ((1 - ALPHA) * filtered2);  /*  노이즈 제거부 */
  pastValue = filtered1;
  digitalWrite(LED_NORMAL, LOW);  /*  화재 비감지 시간 종료, 초록, 노란색, 적색 LED 모두 OFF  */
  digitalWrite(LED_WARNING, LOW); /*  화재 비감지 시간 종료, 초록, 노란색, 적색 LED 모두 OFF  */       
  digitalWrite(LED_ALERT, LOW);   /*  화재 비감지 시간 종료, 초록, 노란색, 적색 LED 모두 OFF  */

  return;
}

/*
 *  센싱 루틴 
 *  filtered1: filtered, filtered2: not filtered 
 */    
inline void processSensing(void)
{
  temp      = (float) analogRead(A1);
  filtered2 = (Q1 * temp * temp) + (Q2 * temp) + Q3;
  /*
   * Due to some performance reasons %f is not included in the Arduino's implementation(use type cast)
   */
  D_PRINTF_NEW_LINE(isDebugging, Serial, "%s%d", "Fire detect sensor: ", (int) filtered2);
  data[index++] = filtered2;
  if (index == DATA_LEN) {
    index = 0;
  }
  filtered1 = (ALPHA * pastValue) + ((1 - ALPHA) * filtered2);  /*  노이즈 제거부 */
  pastValue = filtered1;
  threshold = (countNumDetect) ? (THRESHOLD - 1) : THRESHOLD;
  if (filtered1 > threshold) {
    (countNumDetect >= LAPSE) ? (isWarning = true) : ++countNumDetect;
  } else {
    if (countNumDetect) {
      --countNumDetect;
    }
  }
  if (1) {
    /*
     *  sendMsg()  /* fire detect signal and sensing datas
     */
    isWarning = (countNumDetect) ? sendMsg() : false;
  }
  
  return;
}

/*  테스트 루틴  */
inline void processTest(void)
{
  WDTCSR    &= ~(1 << 6);/*  Disable interrupt mode  */
  temp      = (float) analogRead(A0);
  filtered1 = (Q1 * temp * temp) + (Q2 * temp) + Q3;
  temp      = (float) analogRead(A1);
  filtered2 = (Q1 * temp * temp) + (Q2 * temp) + Q3;
  /*
   * Due to some performance reasons %f is not included in the Arduino's implementation(use type cast)
   */
  D_PRINTF_NEW_LINE(isDebugging, Serial, "%s%c%s%d%c%s%d", "Test 1 Start", '\n', "Test mode sensor: ", (int) filtered1, '\t', "Fire detect sensor: ", (int) filtered2);
  if (ABS((filtered1 - filtered2)) > 5) { /*  5도 이상 차이가 나면 에러 */
    D_PRINTF_NEW_LINE(isDebugging, Serial, "%s", "Test 1: Sensor Error");
    /*
     *  sendMsg()  /* occurred error at test 1
     */
  } else {
    D_PRINTF_NEW_LINE(isDebugging, Serial, "%s%c%s", "Test 1: Sensor OK", '\n', "Test 2 Start");
    for (i = HEATING; i; --i) {
      digitalWrite(HEAT_TEST, HIGH);
      delay(110);
      digitalWrite(HEAT_TEST, LOW);
      delay(15);
      temp      = (float) analogRead(A0);
      filtered1 = (Q1 * temp * temp) + (Q2 * temp) + Q3;
      temp      = (float) analogRead(A1);
      filtered2 = (Q1 * temp * temp) + (Q2 * temp) + Q3;
      /*
       * Due to some performance reasons %f is not included in the Arduino's implementation(use type cast)
       */
      D_PRINTF_NEW_LINE(isDebugging, Serial, "%s%d%c%s%d", "Test mode sensor: ", (int) filtered1, '\t', "Fire detect sensor: ", (int) filtered2);
      if (ABS((filtered1 - filtered2)) > 5) { /*  5도 이상 차이가 나면 정상 */
        break;
      }
    }
    if (!i) {
      D_PRINTF_NEW_LINE(isDebugging, Serial, "%s", "Test 2: Sensor Error");
      /*
       *  sendMag()  /* occurred error at test 2
       */
    } else {
      D_PRINTF_NEW_LINE(isDebugging, Serial, "%s", "Test 2: Sensor OK");
      /*
       *  sendMsg()  /* Sensor OK
       */
    }
  }
  isTest  = false;      /*  Test 종료 */
  WDTCSR  |= (1 << 6);  /*  enable interrupt mode */
  
  return;
}
