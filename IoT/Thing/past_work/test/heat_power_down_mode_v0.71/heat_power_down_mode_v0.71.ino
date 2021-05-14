
/*
 * 와치독 실행 후 setup으로 분기하는 것이 아닌 loop으로 분기함!
 *  isSetup 지우기
 * 
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

int serial_buffer[20];               //통신을 할때 buffer배열에 전송받은 데이터 입력
int bufferIndex = 0; 

inline void pinModeSetup(void);   /*  for use save power */
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

  pastValue     = 20;   /*  전원 인가 시 감지기 초기 가정 온도  */
  countNumReset = INIT; /*  (watchdog period * 30) 초 동안 화재 비감지 시간 */
  
  pinModeSetup();
  rfModuleSetup();  /*  RF Setup and disable RF     */
  
  cli();            /*  disable all interrupts  */
  timer1Setup();    /*  Timer setup and disable timer1  */
  watchdogSetup();  /*  Watchdog setup and enable watchdog  */
  sei();            /*  enable all interrupts */
  
  return;
}

void loop() 
{
  Serial.print(index);
 if (countNumReset) {  /*  화재 비감지부 */
    processReset();     /*  ADC ON -> routine -> ADC OFF  */
  } else {
    /*
    bufferIndex = 0;
    ENABLE_RF();
    while(RF_Serial.available()) {
      serial_buffer[bufferIndex]  = RF_Serial.read();   // 시리얼 통신으로 버퍼배열에 데이터 수신                      
      bufferIndex++;   
    }
    for (int i = 0; i < bufferIndex; ++i) {
      Serial.println(serial_buffer[i], HEX);
    }
    DISABLE_RF();
    */

  
    
    /*  구현해야 할 기능들
     *   1. fire notification
     *   2. test result transfer
     *   3. test command receive 
     *   4. data transfer
     */
    /*

    ENABLE_RF();     /*  Enable RF Module 
    if (isSend) {
      sendMsg();
      isSend = false;
    }
    isRecv = true;
    TIMSK1 |= (1 << OCIE1A);  /*  Enable timer compare interrupt 1. At timer1, enable ADC, and disable timer 1 and RF
    recvMsg();                /*  Receive message from RF Module
    */
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

}
