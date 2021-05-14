
/*
 * Debugging ver.
 * 소스코드는 의도적으로 보수적으로 작성
 *  - 문제 발생을 최소화 하고 안정적으로 운용하기 위함
 * 소스코드 완성 후 Serial.println 모두 삭제 및 Assembley code로 변환
 *  - Clock 낭비 제거 및 낭비 전력 보호 목적
 *  
 * A0: Test mode sensor
 * A1: Fire detect sensor
 * 
 * RF Module
 *  RX – A4 , TX – A5
 *  
 * ADC enable, disable 위치 잘 고려하여 넣을 것(위치에 따라 ADC 운용 시간이 차이나고, 소비 전력이 누적됨)
 *  해야할 것(190528)
 *    1. ok
 *    2. ok
 *  
 *    3. 라즈베리파이 UART 통신 예제
 *    4. 라즈베리파이와 아두이노 통신 여부 확인
 *    5. Foo 함수 모두 수정 
 *    6. processSensing -> Foo 함수 수정
 *    7. processTest -> Foo 함수 수정 
 *    8. 앱과 통신
 *    9. 상태에 따른 LED 출력 제어
 *    
 *    
 */

#include "kem.h"  /*  프로그램 실행에 필요한 라이브러리, 함수, 상수, 변수 등 정의 */

inline void rfModuleSetup(void);  /*  prepare to use the RF Module */
inline void timer1Setup(void);    /*  prepare to use the Timer1 */
inline void watchdogSetup(void);  /*  prepare to use the watchdog */

inline void processReset(void);    /*  화재 비감지 루틴 */
inline void processSensing(void);  /*  센싱 루틴 */
inline void processTest(void);     /*  테스트 루틴  */

/*  
 *  1. if fire detected, than send datas
 *  2. if test signal received, than send test response(success, failed) 
 *  3. if ping signal received, than send ping msg
 *  send success, return true
 */
inline boolean sendMsg(void);  

/*  
 *  1. Ping msg
 *  2. Test signal
 */
inline boolean recvMsg(void);

/*
 * setup 과정은 한 번만 수행됨
 * rfModuleSetup()  : RF Module을 사용할 수 있도록 설정 후, disable
 * timer1Setup()    : Timer1을 사용할 수 있도록 설정 후, disable
 * watchdogSetup()  : Watchdog을 사용할 수 있도록 설정 후, enable
 */
void setup() 
{
  if (!isSetup) {
    cli();  /*  disable all interrupts  */
    
    ADCSRA &= ~(1 << 7);  /*  전력 소모 아끼기 위해 ADC 끈 상태로 시작  */
    /*
     *  디버깅 모드 선택
     *    true  : Serial.print 활성화
     *    false : Serial.print 비활성화
     */
    isDebugging   = true;
    
    Serial.begin(9600);
    D_PRINTF_NEW_LINE(isDebugging, Serial, "%s", "setup()");
    
    /*
     *  Save Power by writing all Digital IO LOW
     *  - note that pins just need to be tied one way or another, do not damage devices!
     */
    for (i = 0; i < 20; i++) {
      pinMode(i, OUTPUT);
    }
    pastValue     = 20;   /*  전원 인가 시 감지기 초기 가정 온도  */
    countNumReset = INIT; /*  (watchdog period * 30) 초 동안 화재 비감지 시간 */
    isSetup       = true; /*  Watchdog reset 후 초기화 루틴 무시  */
    rfModuleSetup();      /*  RF Setup and disable RF     */
    timer1Setup();        /*  Timer setup and disable timer1  */
    watchdogSetup();      /*  Watchdog setup and enable watchdog  */

    sei();  /*  enable all interrupts */
  }

  return;
}

void loop() 
{
  if (countNumReset) {  /*  화재 비감지부 */
    ADCSRA |= (1 << 7); /*  ADC On      */
    processReset();
  } else {
    D_PRINTF_NEW_LINE(isDebugging, Serial, "%s", "Ready to run RF routine");

    ENABLE_RF();     /*  Enable RF Module  */
    if (isSend) {
      sendMsg();
      isSend = false;
    }
    isRecv = true;
    TIMSK1 |= (1 << OCIE1A);  /*  Enable timer compare interrupt 1. At timer1, enable ADC, and disable timer 1 and RF  */
    recvMsg();                /*  Receive message from RF Module  */

    D_PRINTF_NEW_LINE(isDebugging, Serial, "%s", "Ready to sensing and test routine");
    if (!isTest) {      /*  센싱부 */
      processSensing();
    } else {            /*  테스트부 */
      processTest();
    }

  }
  /*
   *  Disable ADC
   *  - don't forget to flip back after waking up if using ADC in your application ADCSRA |= (1 << 7);
   */
  ADCSRA &= ~(1 << 7);
  DISABLE_RF();  /*  Disable RF Module */
 
  /*  DISABLE BOD - this must be calledu right before the __asm__ sleep instruction
   * MCUCR |= (3 << 5);                      /*  set both BODS and BODSE at the same time
   * MCUCR = (MCUCR & ~(1 << 5)) | (1 << 6); /*  then set the BODS bit and clear the BODSE bit at the same time  */
  BOD_DISABLE();
  __asm__  __volatile__("sleep");         /*  in line assembler to go to sleep  */
}

/*
 *  Watchdog Timer(1 second period)
 *  Needed for the watch dog timer.
 *  This is called after a watch dog timer timeout
 *  - this is the interrupt function called after waking up
 */
ISR(WDT_vect)
{
  D_PRINTF_NEW_LINE(isDebugging, Serial, "%s", "ISR(WDT_vect)");
    
  /*  
   *  Enable BOD
   *  - The BODS bit is automatically cleared after three clock cycles.
   *    Writing this bit to one turns off the BOD in relevant sleep modes, while a zero in this bit keeps
   *    BOD active. Default setting keeps BOD active, i.e. BODS set to zero.
   */  
}

/*  Timer Compare Interrupt 1 Service Routine(24 milliseconds) */
ISR(TIMER1_COMPA_vect)
{
  D_PRINTF_NEW_LINE(isDebugging, Serial, "%s", "Timer1");

  isRecv = false;
  TIMSK1 &= ~(1 << OCIE1A);   /*  Disable timer compare interrupt 1  */
  DISABLE_RF();
  ADCSRA |= (1 << 7);
}
