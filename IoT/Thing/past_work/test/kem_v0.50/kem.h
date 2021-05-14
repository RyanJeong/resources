#include "rf.h"   /*  defined functions related with rf communications  */
#include "crc.h"  /*  related with CRC codes... */                  

#define ABS(x)      (x * ((x > 0) - (x < 0)))
#define HEAT_TEST   2           /*  TR 열 발생 핀 2번  */
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
#define WDT_RESET()  __asm__ __volatile__("wdr")
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
 *  현재 디버깅 모드인가?
 *  isDebugging : true
 *    Serial.print 활성화
 *  isDebugging : false
 *    Serial.print 비활성화
 *  테스트 모두 마친 후, 제품화 단계에서 이 변수를 false로 입력
 */
int   isDebugging;    

int   index;          /*  값 저장에 사용하는 인덱스, 0으로 초기화  */
int   countNumDetect; /*  화재 감지 지속 시간 측정 카운터, 0으로 초기화  */
int   countNumReset;  /*  감지기 전원 인가 또는 감지기 Reset 시 초기 시작 카운터, 0으로 초기화  */
int   isTest;         /*  테스트 수행 여부, false로 초기화 */
int   isWarning;      /*  화재 경고 여부, false로 초기화  */
float pastValue;  /*  전원 인가 시 감지기 초기 가정 온도 */
int   threshold;  /*  화재 감지에 사용되는 threshold, (THRESHOLD - 1) | THRESHOLD  */
int   testCond;   /*  for condition branch var. */
float temp;       /*  a value which get from ADC  */
float filtered1;  /*  filtered temp value */
float filtered2;  /*  filtered temp value */

/*
 * setID(void)
 * get ID from DIP switch
 */
inline void setID(void)
{
  txID = 0x00;  /*  will change this routine..  */

  return;
}

/*
 * setAllPinMode(void)
 * set all pin mode for low power
 */
inline void setAllPinMode(void)
{
  pinMode(0, OUTPUT); /*  Don't care  */
  pinMode(1, OUTPUT); /*  Don't care  */
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(14, INPUT);   /*  A0  */
  pinMode(15, INPUT);   /*  A1  */
  pinMode(16, OUTPUT);
  pinMode(17, OUTPUT);
  pinMode(18, OUTPUT);  /*  A4, SoftwareSerial Tx */
  pinMode(19, INPUT);   /*  A5, SoftwareSerial Rx */

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
  OCR1A   = 249;            /*  Compare Match Register 16MHz/1024/2Hz  */
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
 *    WDTCSR = (N);
 *       0:  16 ms
 *       1:  32 ms
 *       2:  64 ms
 *       3: 125 ms
 *       4: 250 ms
 *       5: 500 ms
 *       6:   1 s
 *       7:   2 s
 *      32:   4 s
 *      33:   8 s
 */
inline void watchdogSetup(void)
{
  /*  SETUP WATCHDOG TIMER
   *  ------
   *  WDTCSR = (24);      : change enable and WDE - also resets
   *  WDTCSR = (6);       : prescalers only - get rid of the WDE and WDCE bit
   *  WDF | WDIE | WDP3 | WDCE | WDE | WDP2 | WDP1 | WDP0
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
  WDTCSR = (4);       /*  prescalers only - get rid of the WDE and WDCE bit */
  
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

/*  
 *  화재 비감지 루틴
 *  이 루틴에서는 정해진 시간동안 RF 신호를 무시하며  
 *  센서 값 측정 및 값 저장 동작만을 수행한다. 
 */
inline void processReset(void)
{
  digitalWrite(LED_NORMAL, HIGH);   /*  화재 비감지 시간 동안은 초록, 노란색, 적색 LED 모두 ON  */
  digitalWrite(LED_WARNING, HIGH);  /*  화재 비감지 시간 동안은 초록, 노란색, 적색 LED 모두 ON  */        
  digitalWrite(LED_ALERT, HIGH);    /*  화재 비감지 시간 동안은 초록, 노란색, 적색 LED 모두 ON  */
  D_PRINTF_NEW_LINE(isDebugging, Serial, "%d", --countNumReset);

  ADCSRA |= (1 << 7); /*  ADC On      */
  
  temp      = (float) analogRead(A0);
  filtered1 = (Q1 * temp * temp) + (Q2 * temp) + Q3;
  temp      = (float) analogRead(A1);
  filtered2 = (Q1 * temp * temp) + (Q2 * temp) + Q3;
  /*
   *  Disable ADC
   *  - don't forget to flip back after waking up if using ADC in your application ADCSRA |= (1 << 7);
   */
  ADCSRA &= ~(1 << 7);
  txData[index++] = (int) filtered2;   /*  store data that from analogRead(A1)  */
  if (index == DATA_LEN) {
    index = 0;
  }
  /*
   * Due to some performance reasons %f is not included in the Arduino's implementation(use type cast)
   */
  D_PRINTF_NEW_LINE(isDebugging, Serial, "%s%d%c%s%d%c%s%d", "Test mode sensor: ", (int) filtered1, '\t', "Fire detect sensor: ", (int) filtered2, '\t', "Index: ", index);  
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
  ADCSRA |= (1 << 7); /*  ADC On      */
  temp      = (float) analogRead(A1);
  /*
   *  Disable ADC
   *  - don't forget to flip back after waking up if using ADC in your application ADCSRA |= (1 << 7);
   */
  ADCSRA &= ~(1 << 7);
  filtered2 = (Q1 * temp * temp) + (Q2 * temp) + Q3;
  txData[index++] = (int) filtered2;   /*  store data that from analogRead(A1)  */  
  if (index == DATA_LEN) {
    sendDatas();
    index = 0;
  }
  /*
   * Due to some performance reasons %f is not included in the Arduino's implementation(use type cast)
   */
  D_PRINTF_NEW_LINE(isDebugging, Serial, "%s%d%c%s%d", "Fire detect sensor: ", (int) filtered2, '\t', "Index: ", index);
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
  
  return;
}

/*  
 *  테스트 루틴  
 *  두 번의 테스트를 수행하며, 
 *  테스트 수행 도중 문제가 발생하면
 *  RF 통신을 사용하여 Gateway에 문제가 있음을 알린다. 
 */
inline void processTest(void)
{
  WDTCSR    &= ~(1 << 6); /*  Disable interrupt mode  */
  ADCSRA    |= (1 << 7); /*  ADC On      */
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
    txCMD = (unsigned char) 0x11;
  } else {
    D_PRINTF_NEW_LINE(isDebugging, Serial, "%s%c%s", "Test 1: Sensor OK", '\n', "Test 2 Start");
    for (testCond = HEATING; testCond; --testCond) {
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
    /*
     *  Disable ADC
     *  - don't forget to flip back after waking up if using ADC in your application ADCSRA |= (1 << 7);
     */
    ADCSRA &= ~(1 << 7);
    if (!testCond) {
      D_PRINTF_NEW_LINE(isDebugging, Serial, "%s", "Test 2: Sensor Error");
      txCMD = (unsigned char) 0x12;
    } else {
      D_PRINTF_NEW_LINE(isDebugging, Serial, "%s", "Test 2: Sensor OK");
      txCMD = (unsigned char) 0x10;
    }
  }
  isTest  = false;      /*  Test 종료 */
  WDTCSR  |= (1 << 6);  /*  enable interrupt mode */
  
  return;
}
