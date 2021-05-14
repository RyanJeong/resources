#include "rf.h"   /*  defined functions related with rf communications  */

inline void rfModuleSetup(void);  /*  prepare to use the RF Module */
inline void watchdogSetup(void);  /*  prepare to use the watchdog */

inline void processReset(void);    /*  화재 비감지 루틴 */
inline void processSensing(void);  /*  센싱 루틴 */
inline void processTest(void);     /*  테스트 루틴  */

inline boolean sendDatas(void);           /*  Buffer에 데이터가 꽉 찼을 때 Gateway로 데이터를 전송하는 함수 */  

inline char getCRC0(unsigned char, unsigned char, unsigned char);                                /*  get CRC with no paramete.         */
inline char getCRC1(unsigned char, unsigned char, unsigned char, unsigned char);                 /*  get CRC with one parameter.       */
inline char getCRC2(unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);  /*  get CRC with two Parameters.      */
inline char getCRC60(unsigned char, unsigned char, unsigned char, unsigned char*);               /*  get CRC with array of parameters  */

#define _PRINTF_BUFFER_LENGTH_  64  /*  buffer size for printf  */

#if 1
static char _pf_buffer_[_PRINTF_BUFFER_LENGTH_];
#else
extern char _pf_buffer_[_PRINTF_BUFFER_LENGTH_];
#endif

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

inline void watchdogSetup(void)
{

  WDTCSR = (24);      /*  change enable and WDE - also resets */
  WDTCSR = (6);       /*  prescalers only - get rid of the WDE and WDCE bit */
  SMCR |= (1 << 2);   /*  power down mode */
  SMCR |= 1;          /*  enable sleep  */
  WDTCSR |= (1 << 6); /*  enable interrupt mode */
  
  return;
}

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

inline void processSensing(void)
{
  int i;
  temp    = (float) analogRead(A1);
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
