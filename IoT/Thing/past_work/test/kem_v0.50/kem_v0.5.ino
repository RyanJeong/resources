/*
 *  KEM Ver. 0.5
 *  June 15, 2019
 *    A program without power save mode and rf module save routine
 *    
 *  Features
 *    화재 감지
 *    테스트 결과 반환
 *    테스트 명령 수신
 *    데이터 전송
 *    
 *  Power save mode 와 Watchdog, 그리고 RF Module save routine은 해당 Version에서 하나씩 추가할 예정.
 *    한 번에 여러 기능들을 넣고 구현하려다 보니 문제가 발생하여 디버깅이 필요함, 따라서 이 프로그램을 토대로 기능 하나씩 추가 구현할 것
 *    
 *    
 *  Pin map
 *    0: Input,   RX, FTDI(don't care)
 *    1: Output,  TX, FTDI(don't care)
 *   14: Input,   A0, Test sensor
 *   15: Input,   A1, Fire detect sensor
 *   18: Output,  A4, SoftwareSerial Tx(Will deprecated..)
 *   19: Input,   A5, SoftwareSerial Rx(Will deprecated..)
 *  그 외는 모두 Output Setting하여 저전력 구현
 *  
 *  LED
 *    Green   : LED_NORMAL,   RF 송신 성공(RF 명령어에 대한 반환값을 받으면 Yellow 소등, Green 점등)
 *    Yellow  : LED_WARNING,  RF 송신 처리중(RF 처리 루틴 시작하면 점등)
 *    Red     : LED_ALERT,    RF 송신 실패(정해진 시간 안에 반환값을 못 받으면 Yellow 소등, Red 점등)
 *    
 *  Ver. 0.6
 *    Ver. 0.5에서 SoftwareSerial -> 일반 Rx, Tx 핀 + LED로 디버깅
 *    
 *  Ver. 0.7
 *    Ver. 0.5에서 RF save routine 추가
 *    
 *  Ver. 0.8
 *    Ver. 0.7에서 Watchdog + Power save mode 추가
 *   
 *  Ver. 0.9
 *    Ver. 0.8에서 Ver. 0.6 기능 구현(Ver. 0.8에서 SoftwareSerial을 Deprecated)
 *    
 *  Ver. 1(Beta)
 *    Debug 및 Bug 수정 
 */

#include "kem.h"  /*  프로그램 실행에 필요한 라이브러리, 함수, 상수, 변수 등 정의 */

inline void rfModuleSetup(void);  /*  prepare to use the RF Module */
inline void timer1Setup(void);    /*  prepare to use the Timer1 */
inline void watchdogSetup(void);  /*  prepare to use the watchdog */

inline void processReset(void);    /*  화재 비감지 루틴 */
inline void processSensing(void);  /*  센싱 루틴 */
inline void processTest(void);     /*  테스트 루틴  */

inline boolean notifyFireDetected(void);  /*  화재 감지 시 Gateway로 알리는 함수   */
inline boolean sendTestResult(void);      /*  테스트 수행 결과를 Gateway로 전송하는 함수 */
inline boolean receiveTestCommand(void);  /*  테스트 수행 명령을 Gateway로부터 확인하는 함수 */
inline boolean sendDatas(void);           /*  Buffer에 데이터가 꽉 찼을 때 Gateway로 데이터를 전송하는 함수 */  
   
void setup() 
{
    ADCSRA &= ~(1 << 7);  /*  전력 소모 아끼기 위해 ADC 끈 상태로 시작  */
    Serial.begin(9600);
    D_PRINTF_NEW_LINE(isDebugging, Serial, "%s", "setup()");
    
    /*
     *  디버깅 모드 선택
     *    true  : Serial.print 활성화
     *    false : Serial.print 비활성화
     */
    isDebugging   = true;
    
    setID();              /*  this version, set 0x00  */
    setAllPinMode();      /*  for low power */
    pastValue     = 20;   /*  전원 인가 시 감지기 초기 가정 온도  */
    countNumReset = INIT; /*  (watchdog period * 30) 초 동안 화재 비감지 시간 */
    cli();                /*  disable all interrupts  */
    rfModuleSetup();      /*  RF Setup and disable RF     */
    timer1Setup();        /*  Timer setup and disable timer1  */
    watchdogSetup();      /*  Watchdog setup and enable watchdog  */
    sei();                /*  enable all interrupts */

    return;
}

void loop() 
{
  //ENABLE_RF();
  //DISABLE_RF();
 if (countNumReset) {  /*  화재 비감지부 */
    processReset();     /*  ADC ON -> routine -> ADC OFF  */
  } else {
    if (!isTest) {      /*  센싱부 */
      processSensing();
    } else {            /*  테스트부 */
      processTest();
    }

  }
  
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

/*  Timer Compare Interrupt 1 Service Routine(16 milliseconds) */
ISR(TIMER1_COMPA_vect)
{
  D_PRINTF_NEW_LINE(isDebugging, Serial, "%s", "Timer1");
}
