#include "kem.h"  /*  프로그램 실행에 필요한 라이브러리, 함수, 상수, 변수 등 정의 */

void setup() 
{
    Serial.begin(9600);
    isDebugging   = true;
    rfModuleSetup();      /*  RF Setup  */
    pastValue     = 20;   /*  전원 인가 시 감지기 초기 가정 온도  */
    countNumReset = INIT; /*  (watchdog period * 30) 초 동안 화재 비감지 시간 */
    cli();                /*  disable all interrupts  */
    watchdogSetup();      /*  Watchdog setup and enable watchdog  */
    sei();                /*  enable all interrupts */

    return;
}

void loop() 
{  
  if (countNumReset) {  /*  화재 비감지부 */
    processReset();     /*  ADC ON -> routine -> ADC OFF  */
  } else {
    processSensing();
  }
  
  BOD_DISABLE();
  __asm__  __volatile__("sleep");         /*  in line assembler to go to sleep  */
}

ISR(WDT_vect)
{
  
}
