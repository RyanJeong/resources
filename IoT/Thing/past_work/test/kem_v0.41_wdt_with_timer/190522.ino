#define FET         1000        /*  FET제어(heat) */
#define NORMAL      0           /*  현재 감지기 상태: 보통 */
#define WARNING     1           /*  현재 감지기 상태: 경고 */
#define DATA_LEN    300         /*  감지기 내에 저장할 데이터 갯수 */
#define LED_NORMAL  5           /*  현재 감지기 상태: 보통, 초록 LED */
#define LED_WARNING 6           /*  현재 감지기 상태: 경고, 노랑 LED */
#define LED_ALERT   7           /*  현재 감지기 상태: 알림, 빨 LED */
#define Q1          (0.0001233) /*  y = (Q1 * x^2) + (Q2 * x) + Q3  */
#define Q2          (-0.0389)   /*  y = (Q1 * x^2) + (Q2 * x) + Q3  */
#define Q3          (29.1755)   /*  y = (Q1 * x^2) + (Q2 * x) + Q3  */
#define ALPHA       (0.9)       /*  Noise filter 계수 */
#define LAPSE       4           /*  화재 지속 판전 유보 시간  */
#define RESET       30          /*  초기 전원 인가 시 화재 비감지 시간, 30초 */
#define TEST_TIME   60          /*  테스트 수행 시간, 60초  */
#define THRESHOLD   60          /*  Fire alarm temp.[C] */

int   normal[DATA_LEN];   /*  평상시 값 저장 배열, 0으로 초기화  */
int   test[DATA_LEN];     /*  테스트 값 저장 배열, 0으로 초기화  */
int   index;              /*  값 저장에 사용하는 인덱스, 0으로 초기화  */
int   countNumDetect;     /*  화재 감지 지속 시간 측정 카운터, 0으로 초기화  */
int   countNumReset;      /*  감지기 전원 인가 또는 감지기 Reset 시 초기 시작 카운터, 0으로 초기화  */
int   isTest;             /*  Test 수행 여부, false로 초기화  */
int   isFire;             /*  화재 발생 여부, false로 초기화  */
int   isInit;             /*  감지기 전원 인가 또는 감지기 Reset 여부, false로 초기화 */
float initValueNormal;    /*  전원 인가 시 normal 초기 가정 온도 */
float initValueTest;      /*  전원 인가 시 test 초기 가정 온도 */

int i;

void setup() {
  if (!isInit) {
    Serial.begin(9600);
    /*
    *  Save Power by writing all Digital IO LOW
    *  - note that pins just need to be tied one way or another, do not damage devices!
    */
    for (i = 0; i < 20; i++) {
      pinMode(i, OUTPUT);
    }
    initValueNormal = 20;
    initValueTest   = 20;
    
    cli();            /*  Clear interrupts  */
    
    /*
     *  SETUP WATCHDOG TIMER
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
    WDTCSR = (24);    /*  change enable and WDE - also resets */
    WDTCSR = (6);     /*  prescalers only - get rid of the WDE and WDCE bit */
    
    /*  WDTCSR |= (1<<6); /*  enable interrupt mode */
    
    /*  ENABLE SLEEP - this enables the sleep mode  */
    SMCR |= (1 << 2); /*  power down mode */
    SMCR |= 1;        /*  enable sleep  */
    
    TCCR1A  = 0;              /*  Reset TCCR1A */
    TCCR1B  = 0;              /*  Reset TCCR1B */
    TCNT1   = 0;              /*  Reset Timer1 Counter  */
    OCR1A   = 62499;          /*  Compare Match Register 16MHz/256/2Hz  */
    TCCR1A |= (1 << COM1A1);  /*  Clear OC1A/OC1B on Compare Match, Set OC1A/OC1B at BOTTOM (non-inverting mode) */
    TCCR1A |= (1 << WGM10);   /*  Fast Pulse Width Modulation (PWM) */
    TCCR1A |= (1 << WGM11);   /*  Fast Pulse Width Modulation (PWM) */
    TCCR1B |= (1 << WGM12);   /*  Fast Pulse Width Modulation (PWM) */
    TCCR1B |= (1 << WGM13);   /*  Fast Pulse Width Modulation (PWM) */
    TCCR1B |= (1 << CS12);    /*  256 prescaler */
    
    TIMSK1 |= (1 << OCIE1A);  /*  Enable timer compare interrupt 1  */
    
    TCCR2A  = 0;              /*  Reset TCCR2A */
    TCCR2A  = 0;              /*  Reset TCCR2A */
    TCNT2   = 0;              /*  Reset Timer2 Counter  */
    OCR2A   = 249;            /*  Compare Match Register 16MHz/1024/2Hz */
    TCCR2A |= (1 << COM2A1);  /*  Clear OC0A/OC2A on Compare Match, Set OC0A/OC2A at BOTTOM (non-inverting mode)  */
    TCCR2A |= (1 << WGM20);   /*  Fast Pulse Width Modulation (PWM) */
    TCCR2A |= (1 << WGM21);   /*  Fast Pulse Width Modulation (PWM) */
    TCCR2B |= (1 << WGM22);   /*  Fast Pulse Width Modulation (PWM) */
    TCCR2B |= (1 << CS21);    /*  64 prescaler  */
    TCCR2B |= (1 << CS20);    /*  64 prescaler  */
    
    TIMSK2 |= (1 << OCIE2A);  /*  Enable timer compare interrupt 2  */
    
    sei();            /*  Set interrupts  */
    isInit = true;
  }
}

void loop() {
  if (!isTest) {

  } else {
    /*
     *  Disable ADC
     *  - don't forget to flip back after waking up if using ADC in your application ADCSRA |= (1 << 7);
     */
    ADCSRA &= ~(1 << 7);

    /*
     *  Disable RF Module
     *  Foo();
     */

    /*  DISABLE BOD - this must be calledu right before the __asm__ sleep instruction  */
    MCUCR |= (3 << 5);                      /*  set both BODS and BODSE at the same time  */
    MCUCR = (MCUCR & ~(1 << 5)) | (1 << 6); /*  then set the BODS bit and clear the BODSE bit at the same time  */
    __asm__  __volatile__("sleep");         /*  in line assembler to go to sleep  */
  }
}

/*
 *  Watchdog Timer(1 second)
 *  Needed for the watch dog timer.
 *  This is called after a watch dog timer timeout
 *  - this is the interrupt function called after waking up
 *  - The BODS bit is automatically cleared after three clock cycles.
 */
ISR(WDT_vect){
  Serial.println("Watchdog");
  Serial.flush();
  /*  Enable ADC  */
  ADCSRA |= (1 << 7);

  /*
   *  Disable RF Module
   *  Foo();
   */

  /*
   *  Enable BOD
   *  - The BODS bit is automatically cleared after three clock cycles.
   */

  /*
   *  Check Recieve Buffer if recieve test operation
   *  isTest = Foo();
   */
   
  isTest  = false;
  TIMSK1  |= (1 << OCIE1A);  /*  Enable timer compare interrupt 1  */
  TIMSK2  |= (1 << OCIE2A);  /*  Enable timer compare interrupt 2  */
  WDTCSR  &= ~(1<<6);
}

/*  Timer Compare Interrupt 2 Service Routine(1 millisecond) */
ISR(TIMER2_COMPA_vect)
{
  Serial.println("Timer2");
  Serial.flush();
}

/*  Timer Compare Interrupt 1 Service Routine(1 second) */
ISR(TIMER1_COMPA_vect)
{
  Serial.println("Timer1");
  Serial.flush();
  isTest  = true;
  TIMSK1  &= ~(1 << OCIE1A);  /*  Enable timer compare interrupt 1  */
  TIMSK2  &= ~(1 << OCIE2A);  /*  Enable timer compare interrupt 2  */
  WDTCSR  |= (1<<6); /*  enable interrupt mode */
}
