/*
 * A0: Test mode sensor
 * A1: Fire detect sensor
 */
 
#define ABS(x)      (x * ((x > 0) - (x < 0)))
#define HEAT_TEST   2           /*  TR 열 발생 핀 2번  */
#define DATA_LEN    300         /*  감지기 내에 저장할 데이터 갯수 */
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
 *  functions to be changed
 */
void Foo_EnableRF(void);  /*  RF Module wake-up */
void Foo_DisableRF(void); /*  RF Module go to sleep */

/*  
 *  1. if fire detected, than send datas
 *  2. if test signal received, than send test response(success, failed) 
 *  3. if ping signal received, than send ping msg
 *  send success, return true
 */
boolean Foo_SendMsg(void);  

/*  
 *  1. Ping msg
 *  2. Test signal
 */
void Foo_RecvMsg(void);

int   data[DATA_LEN];     /*  센싱 값 저장 배열, 0으로 초기화  */
int   index;              /*  값 저장에 사용하는 인덱스, 0으로 초기화  */
int   countNumDetect;     /*  화재 감지 지속 시간 측정 카운터, 0으로 초기화  */
int   countNumReset;      /*  감지기 전원 인가 또는 감지기 Reset 시 초기 시작 카운터, 0으로 초기화  */
int   isReset;            /*  초기 전원 인가 후 화재 비감지 상태 여부, false로 초기화 */
int   isTest;             /*  테스트 수행 여부, false로 초기화 */
int   isWarning;          /*  화재 경고 여부, false로 초기화  */
int   isSetup;            /*  감지기 전원 인가 또는 감지기 Reset 여부, false로 초기화 */
float pastValue;          /*  전원 인가 시 감지기 초기 가정 온도 */
int   threshold;          /*  화재 감지에 사용되는 threshold, (THRESHOLD - 1) | THRESHOLD  */

int   i;          /*  for condition branch var. */
float temp;       /*  store a value which get from ADC  */
float filtered1;  /*  filtered temp value */
float filtered2;  /*  filtered temp value */

void setup() {
  if (!isSetup) {
    Serial.begin(9600);
    Serial.println("setup()");
    Serial.flush(); /* for print content completely */

    /*
     *  Save Power by writing all Digital IO LOW
     *  - note that pins just need to be tied one way or another, do not damage devices!
     */
    for (i = 0; i < 20; i++) {
      pinMode(i, OUTPUT);
    }
    pastValue = 20;
    
    cli();  /*  Clear interrupts  */
    
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
    WDTCSR |= (1 << 6); /*  enable interrupt mode */
    
    /*  ENABLE SLEEP - this enables the sleep mode  */
    SMCR |= (1 << 2); /*  power down mode */
    SMCR |= 1;        /*  enable sleep  */
    
    sei();  /*  Set interrupts  */
    
    countNumReset = INIT;             /*  (watchdog period * 30) 초 동안 화재 비감지 시간 */
    isSetup       = true;             /*  Watchdog reset 후 초기화 루틴 무시  */
    isReset       = true;             /*  화재 비감지 상태 돌입  */
  }
}

void loop() {
  if (countNumReset) {  /*  화재 비감지부 */
    digitalWrite(LED_NORMAL, HIGH);   /*  화재 비감지 시간 동안은 초록, 노란색, 적색 LED 모두 ON  */
    digitalWrite(LED_WARNING, HIGH);  /*  화재 비감지 시간 동안은 초록, 노란색, 적색 LED 모두 ON  */        
    digitalWrite(LED_ALERT, HIGH);    /*  화재 비감지 시간 동안은 초록, 노란색, 적색 LED 모두 ON  */
    Serial.println(countNumReset);
    temp      = (float) analogRead(A0);
    filtered1 = (Q1 * temp * temp) + (Q2 * temp) + Q3;
    temp      = (float) analogRead(A1);
    filtered2 = (Q1 * temp * temp) + (Q2 * temp) + Q3;
    Serial.print("Test mode sensor: ");
    Serial.print(filtered1);
    Serial.print('\t');
    Serial.print("Fire detect sensor: ");
    Serial.println(filtered2);
    digitalWrite(LED_NORMAL, LOW);  /*  화재 비감지 시간 종료, 초록, 노란색, 적색 LED 모두 OFF  */
    digitalWrite(LED_WARNING, LOW); /*  화재 비감지 시간 종료, 초록, 노란색, 적색 LED 모두 OFF  */       
    digitalWrite(LED_ALERT, LOW);   /*  화재 비감지 시간 종료, 초록, 노란색, 적색 LED 모두 OFF  */
    if (--countNumReset) {
      isReset = false;
    }
  } else if (!isTest) { /*  센싱부, filtered1: filtered, filtered2: not filtered */
    temp      = (float) analogRead(A1);
    filtered2 = (Q1 * temp * temp) + (Q2 * temp) + Q3;
    Serial.print("Fire detect sensor: ");
    Serial.println(filtered2);
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
    if (isWarning) {
      /*
       *  Foo_SendMsg()  /* fire detect signal and sensing datas
       */
      isWarning = (countNumDetect) ? Foo_SendMsg() : false;
    }
  } else {  /*  테스트부 */
    WDTCSR    &= ~(1 << 6);/*  Disable interrupt mode  */
    temp      = (float) analogRead(A0);
    filtered1 = (Q1 * temp * temp) + (Q2 * temp) + Q3;
    temp      = (float) analogRead(A1);
    filtered2 = (Q1 * temp * temp) + (Q2 * temp) + Q3;
    Serial.println("Test 1 Start");
    Serial.print("Test mode sensor: ");
    Serial.print(filtered1);
    Serial.print('\t');
    Serial.print("Fire detect sensor: ");
    Serial.println(filtered2);
    if (ABS((filtered1 - filtered2)) > 5) { /*  5도 이상 차이가 나면 에러 */
      Serial.println("Test 1: Sensor Error");
      /*
       *  Foo_SendMsg()  /* occurred error at test 1
       */
    } else {
      Serial.println("Test 1: Sensor OK");
      Serial.println("Test 2 Start");
      for (i = HEATING; i; --i) {
        digitalWrite(HEAT_TEST, HIGH);
        delay(110);
        digitalWrite(HEAT_TEST, LOW);
        delay(15);
      }
      temp      = (float) analogRead(A0);
      filtered1 = (Q1 * temp * temp) + (Q2 * temp) + Q3;
      temp      = (float) analogRead(A1);
      filtered2 = (Q1 * temp * temp) + (Q2 * temp) + Q3;
      Serial.print("Test mode sensor: ");
      Serial.print(filtered1);
      Serial.print('\t');
      Serial.print("Fire detect sensor: ");
      Serial.println(filtered2);
      if (ABS((filtered1 - filtered2)) > 5) { /*  5도 이상 차이가 나면 정상 */
        Serial.println("Test 2: Sensor OK");
        /*
         *  Foo_SendMsg()  /* Sensor OK
         */
      } else {
        Serial.println("Test 2: Sensor Error");
        /*
         *  Foo_SendMsg()  /* occurred error at test 2
         */
      }
    }
    isTest  = false;
    WDTCSR  |= (1 << 6); /*  enable interrupt mode */
  }
  Serial.flush();
  /*
   *  Disable ADC
   *  - don't forget to flip back after waking up if using ADC in your application ADCSRA |= (1 << 7);
   */
  ADCSRA &= ~(1 << 7);
  
  Foo_DisableRF();  /*  Disable RF Module */
  
  /*  DISABLE BOD - this must be calledu right before the __asm__ sleep instruction  */
  MCUCR |= (3 << 5);                      /*  set both BODS and BODSE at the same time  */
  MCUCR = (MCUCR & ~(1 << 5)) | (1 << 6); /*  then set the BODS bit and clear the BODSE bit at the same time  */
  __asm__  __volatile__("sleep");         /*  in line assembler to go to sleep  */
}

/*
 *  Watchdog Timer(1 second period)
 *  Needed for the watch dog timer.
 *  This is called after a watch dog timer timeout
 *  - this is the interrupt function called after waking up
 *  - The BODS bit is automatically cleared after three clock cycles.
 */
ISR(WDT_vect)
{
  Serial.println("ISR(WDT_vect)");
  Serial.flush();
    
  /*  
   *  Enable BOD
   *  - The BODS bit is automatically cleared after three clock cycles.
   */  

  Foo_EnableRF();     /*  Enable RF Module  */
  ADCSRA |= (1 << 7); /*  Enable ADC  */
  if (!isReset) {     /*  화재 비감지 상태가 끝나고 RF Msg 수신  */
    Foo_RecvMsg();      /*  Receive message from RF Module  */
  }
}

void Foo_EnableRF(void)
{

  /*  Do something ...  */
  
  return;
}

void Foo_DisableRF(void)
{

  /*  Do something ...  */
  
  return;
}

boolean Foo_SendMsg(void)
{

  /*  Do something ...  */
  
  return true;
}

void Foo_RecvMsg(void)
{
  
  /*  
   *   Do something ...
   * 
   *  if (ping) {
   *    Foo_SendMsg();  /*  send response
   *  } elif (test) {
   *      isTest = true;
   *  }     
   */
   
  return;
}
