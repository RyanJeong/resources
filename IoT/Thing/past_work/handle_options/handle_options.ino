uint32_t    timer;
#define HEAT_MODE

#define ONE_DAY 864
#define __UTIL_H__

#define __UTIL__            "[util.ino]"
#define UTIL_CHECKBATTERY   "[checkBattery()] "

#define READ_BATTERY_VOLTAGE    (0.048828125 * analogRead(VCC_CHECK))  /*  10.0 * 5.0 / 1024.0 */
#define VOLTAGE_THRESHOLD       4.5

#define SIZE_ID     9
#define SIZE_BUF    64
#define SIZE        64  

#define RELAY_ON    digitalWrite(RELAY, HIGH)
#define RELAY_OFF   digitalWrite(RELAY, LOW)

#define RESERVE_TIME            4   /* if normal state, four breaks and one sensing */

#define ENABLE_ADC          \
do {                        \
    ADCSRA  |= (1 << ADEN); \
    delay(1);               \
} while (0)                 \

#define DISABLE_ADC         \
do {                        \
    ADCSRA  = 0;            \
} while (0)                 \

#define MSG_LINE                "--------------------------------"
#define MSG_START               "Start!"
#define MSG_RF_OFF              "RF module off."
#define MSG_GATEWAY_ERROR       "Occurred error from the gateway."
#define MSG_GATEWAY_OK          "Nomally received from the gateway."
#define MSG_NOT_RECEIVED        "Don't received the frame."
#define MSG_RECEIVED            "Received the frame."
#define MSG_CRC_ERROR           "CRC is not correct."
#define MSG_CRC_OK              "CRC is correct."
#define MSG_INVALID_CMD         "Command invalid"
#define MSG_INVALID_ID          "ID invalid"
#define MSG_INVALID_CHANNEL     "Channel invalid"
#define MSG_FIRE                "Fire detected!!!"
#define MSG_TEST_SUCCESS        "All tests passed"
#define MSG_CRC_TEST_FAILED     "Failed to CRC Test"
#define MSG_LED_ON              "Turn on the LED"
#define MSG_SMOKE_TEST_FAILED1  "Failed test with the shutter close and the LED turned off."
#define MSG_SMOKE_TEST_FAILED2  "Failed test with the shutter close and the LED turned on."
#define MSG_SMOKE_TEST_FAILED3  "Failed test with the shutter open and the LED turned off."
#define MSG_SMOKE_TEST_FAILED4  "Failed test with the shutter open and the LED turned on."
#define MSG_SENSOR_OVERHEATING  "Sensor overheating"
#define MSG_LOW_BATTERY_VOLTAGE "Low battery voltage."

#define __SENSOR__              "[sensor.ino]"
#define SENSOR_PREPARESENSOR    "[prepareSensor()] "
#define SENSOR_TESTING          "[testing()] "
#define SENSOR_SENSING          "[sensing()] "

#define ABS(x)              (x * ((x > 0) - (x < 0)))

#define STATE_NORMAL            00    
#define STATE_PREPARE           01
#define STATE_FULLED_BUFFER     02
#define STATE_CHECK_BATTERY     03
#define STATE_RECEIVED_TEST     04
#define STATE_TESTING_PASSED    05
#define STATE_TESTING_FAILED    06
#define STATE_FIRE              07

#define STATUS0         A2  /*  LED 1   */
#define RELAY           A3  /*  RELAY   */
#define STATUS1         A4  /*  LED 2   */
#define BUZZER          A5  /*  warning sound   */
#define VCC_CHECK       A6  /*  analog voltage in   */


#define MILSEC_16       0
#define MILSEC_64       2
#define MILSEC_125      3
#define MILSEC_500      5
#define SEC_1           6
#define SEC_4           33

#define ENABLE_WATCHDOG         \
do {                            \
    WDTCSR |= (1 << 6);         \
} while (0)

#define ENABLE_SLEEP                            \
do {                                            \
    SMCR |= (1 << 2);   /*  power down mode */  \
    SMCR |= 1;          /*  enable sleep  */    \
} while (0)

#define SLEEP                                   \
do {                                            \
    MCUCR   |= (3 << 5);                        \
    MCUCR   = (MCUCR & ~(1 << 5)) | (1 << 6);   \
    __asm__  __volatile__("sleep");             \
} while (0)

#define SIZE_TEMP               6

#define SIZE_DATA               30
#define TESTING_SENSOR          15
#define ACTUAL_SENSOR           14

#define HEATING                 2
#define HEAT_ALPHA              (0.7)
#define HEAT_BETA               (1.0 - HEAT_ALPHA)
#define Q1                      (0.00000029476)
#define Q2                      (-0.00045725)
#define Q3                      (0.3354)
#define Q4                      (-50.2612)
#define CURVE(n)                ((Q1 * n * n * n) + (Q2 * n * n) + (Q3 * n) + Q4)
#define LAPSE                   2
#ifdef TEST_MODE
 #define PREPARE_TIME           5
#else
 #define PREPARE_TIME           60
#endif
#define THRESHOLD               60
#define TEST_NORMAL_THRESHOLD   3.0
#define TEST_HEATING_THRESHOLD  45.0
#define TEST_HEATING_DIFF       0.5
#define INIT_SENSOR_TEMPERATURE 25

/*  extern uint8_t  commResult, thingState; */
uint8_t  commResult, thingState;

static float    data[SIZE_DATA];
static uint8_t  index, countFireDetected, isWarning, heatThreshold;
static float    pastValue;
static uint8_t  reserve;

uint8_t         testResult, averageValue, currValue, currTestValue;

void setup()
{
    Serial.begin(9600);
    enableSleep();
    prepareSensor();
    /*  0, 1: default serial pin    */
    pinMode(2, OUTPUT);     /*  HEAT_R  */
    pinMode(3, INPUT);      /*  Set Channel, bit 0  */
    pinMode(4, INPUT);      /*  Set Channel, bit 1  */
    pinMode(5, INPUT);      /*  Set ID, bit 0   */
    pinMode(6, INPUT);      /*  Set ID, bit 1   */
    pinMode(7, INPUT);      /*  Set ID, bit 2   */
    pinMode(8, INPUT);      /*  Set ID, bit 3   */
    pinMode(9, INPUT);      /*  Set ID, bit 4   */
    pinMode(10, INPUT);     /*  Set ID, bit 5   */
    pinMode(11, OUTPUT);    /*  Not used    */
    pinMode(12, OUTPUT);    /*  Not used    */
    pinMode(13, OUTPUT);    /*  Not used    */
    pinMode(14, INPUT);     /*  A0, ADC0, for test sensor   */
    pinMode(15, INPUT);     /*  A1, ADC1, for measuring sensor  */
    pinMode(16, OUTPUT);    /*  LED1 for debugging  */
    pinMode(17, OUTPUT);    /*  RELAY   */
    pinMode(18, OUTPUT);    /*  LED2 for debugging  */
    pinMode(19, OUTPUT);    /*  Buzzer  */
    pinMode(20, INPUT);     /*  Analog voltage in   */
    digitalWrite(HEATING, LOW);   
    digitalWrite(STATUS0, LOW);  
    digitalWrite(STATUS1, LOW);  
    digitalWrite(BUZZER, LOW);  
    digitalWrite(RELAY, LOW);  
}

void testBuzzerAndLED()
{
    Serial.println("Normal");
    Serial.flush();
    handleBuzzerAndLED(STATE_NORMAL);
    sleep(SEC_1);
    Serial.println("Prepare");
    Serial.flush();
    handleBuzzerAndLED(STATE_PREPARE);
    sleep(SEC_1);
    Serial.println("Fulled buffer");
    Serial.flush();
    handleBuzzerAndLED(STATE_FULLED_BUFFER);
    sleep(SEC_1);
    Serial.println("Check battery");
    Serial.flush();
    handleBuzzerAndLED(STATE_CHECK_BATTERY);
    sleep(SEC_1);
    Serial.println("Received test");
    Serial.flush();
    handleBuzzerAndLED(STATE_RECEIVED_TEST);
    sleep(SEC_1);
    Serial.println("Testing passed");
    Serial.flush();
    handleBuzzerAndLED(STATE_TESTING_PASSED);
    sleep(SEC_1);
    Serial.println("Testing failed");
    Serial.flush();
    handleBuzzerAndLED(STATE_TESTING_FAILED);
    sleep(SEC_1);
    Serial.println("Fire");
    Serial.flush();
    handleBuzzerAndLED(STATE_FIRE);
    sleep(SEC_1);
}

void testBatteryRoutine()
{
    timer = (timer == ONE_DAY) ? checkBattery(), 0 : (timer + 1);
    Serial.print("Timer: ");
    Serial.println(timer);
    Serial.flush();
}

void testSensingAndTesting()
{
    //sensing();
    testing();
}

void loop()
{
//    testBuzzerAndLED();
//    testBatteryRoutine();
    testSensingAndTesting();
}

void handleBuzzerAndLED(uint8_t op)
{
digitalWrite(STATUS1, (op & 01) ? HIGH : LOW);
op >>= 1;
digitalWrite(STATUS0, (op & 01) ? HIGH : LOW);
op >>= 1;
digitalWrite(BUZZER, (op & 01) ? HIGH : LOW);
/*  TODO: Try delay() to sleep()    */
/*    delay(500);   */
sleep(MILSEC_125);
digitalWrite(STATUS1, LOW);
digitalWrite(STATUS0, LOW);
digitalWrite(BUZZER, LOW);

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

} 

void prepareSensor(void)
{
#ifdef HEAT_MODE
    /*
     *  func        : 디버깅 메세지를 작성하는 데 사용되는 식별 변수
     *  i           : 조건문에서 사용하는 제어 변수
     *  temp        : analogRead 결과를 임시 저장하는 변수
     *  filtered    : 센서로부터 얻은 값에 필터링을 거친 값
     *  buf         : 디버깅 메세지를 만들기 위한 버퍼
     */
    static char *func = SENSOR_PREPARESENSOR;
    uint8_t     i;
    float       temp, filtered;
    char        buf[SIZE_BUF];

    putsForDebug(__SENSOR__, func, MSG_START);
    pastValue   = INIT_SENSOR_TEMPERATURE;
    for (i = 0; i < 5; ++i) {
        handleBuzzerAndLED(STATE_PREPARE);
        ENABLE_ADC;
        analogRead(ACTUAL_SENSOR);
        DISABLE_ADC;
        sleep(SEC_1);
    }
#else
/*  TODO: smoke */
#endif

    return;
}

void sensing(void)
{
#ifdef HEAT_MODE
    /*
     *  func        : 디버깅 메세지를 작성하는 데 사용되는 식별 변수
     *  i           : 조건문에서 사용하는 제어 변수
     *  temp        : 임시 값 저장 변수
     *  filtered    : 센서로부터 얻은 값에 필터링을 거친 값
     *  buf         : 디버깅 메세지를 만들기 위한 버퍼
     *  strTemp     : 실수값에 대한 디버깅 메세지를 만들기 위한 버퍼
     */
    static char *func = SENSOR_SENSING;
    uint8_t     i;
    float       temp, filtered;
    char        buf[SIZE_BUF], strTemp[SIZE_TEMP];

    sleep(SEC_1);
    /*  불이 났거나, 감지를 해야할 시기일 경우  */
    reserve = RESERVE_TIME;
    if (isWarning || (reserve == RESERVE_TIME)) {
        putsForDebug(__SENSOR__, func, MSG_START);
        ENABLE_ADC;
        temp            = (float) analogRead(ACTUAL_SENSOR);
        DISABLE_ADC;
        /*  노이즈 제거부 */
        filtered        = (HEAT_ALPHA * CURVE(temp)) + (HEAT_BETA * pastValue);  
        data[index++]   = filtered;
        if (index == SIZE_DATA) {
            handleBuzzerAndLED(STATE_FULLED_BUFFER);
            index   = 0;
            temp    = (float) 0.0;
            for (i = 0; i < SIZE_DATA; ++i) {
                temp += *(data + i);
            }
            temp            /= (float) SIZE_DATA;
            /*  Frame에서의 데이터는 1 byte여야 함  */
            averageValue    = (uint8_t) temp;
//          thingState      = SEND_A_AVERAGE_DATA;
        }
        heatThreshold   = (countFireDetected) ? (THRESHOLD - 1) : THRESHOLD;
        if (filtered > heatThreshold) {
            (countFireDetected >= LAPSE) ? (isWarning = true) : ++countFireDetected;
        } else {
            if (countFireDetected) {
                --countFireDetected;
            }
        }
        dtostrf(filtered, 4, 2, strTemp);
        sprintf(buf, "Index: %u, filtered value: %s degree, count of fire detection: %u", index, strTemp, countFireDetected);
        putsForDebug(__SENSOR__, func, buf);
        if (isWarning) {
            if (countFireDetected) {
                /*  화재일 경우 thingState에는 화재 정보를 최우선으로 담아야 함 */
//              thingState = NOTIFY_FIRE_DETECTED;  
                handleBuzzerAndLED(STATE_FIRE);
                RELAY_ON;
            } else {
                isWarning   = false;
                RELAY_OFF;
            }
            putsForDebug(__SENSOR__, func, MSG_FIRE);
        }
        pastValue   = filtered;
        reserve     = 0;
    } else {
        ++reserve;
    }
#else
/*  TODO: smoke */
#endif

    return;
}

/*  TODO: 식 중간에 CURVE로 전부 바꿔야 함  */
void testing(void)
{
#ifdef HEAT_MODE
    /*
     *  func        : 디버깅 메세지를 작성하는 데 사용되는 식별 변수
     *  i           : 조건문에서 사용하는 제어 변수
     *  testValue1  : 테스트 센서로부터 얻은 값, Heating 전
     *  testValue2  : 테스트 센서로부터 얻은 값, 1차 Heating 직후
     *  testValue3  : 테스트 센서로부터 얻은 값, 2차 Heating 직후
     *  value       : 주 감지 센서로부터 얻은 값
     *  buf         : 디버깅 메세지를 만들기 위한 버퍼
     *  strTemp1    : 실수값에 대한 디버깅 메세지를 만들기 위한 버퍼1
     *  strTemp2    : 실수값에 대한 디버깅 메세지를 만들기 위한 버퍼2
     */
    static char *func = SENSOR_TESTING;
    uint8_t     i;
    float       testValue1, testValue2, testValue3, value;
    char        buf[SIZE_BUF], strTemp1[SIZE_TEMP], strTemp2[SIZE_TEMP];

    sleep(SEC_1);
    putsForDebug(__SENSOR__, func, MSG_START);
    /* if failed at test1, testResult = 0 + 2   */
    testResult  = 0; 
    ENABLE_ADC;
    testValue1      = CURVE((float) analogRead(TESTING_SENSOR));
    value           = CURVE((float) analogRead(ACTUAL_SENSOR));
    DISABLE_ADC;
    currTestValue   = (uint8_t) testValue1;
    currValue       = (uint8_t) value;
    dtostrf(testValue1, 4, 2, strTemp1);
    dtostrf(value, 4, 2, strTemp2);
    sleep(MILSEC_125);
    if (ABS((testValue1 - value)) > TEST_NORMAL_THRESHOLD) { /*  2도 이상 차이가 나면 에러 */
        //testResult      |= HEAT_ERROR;
        sprintf(buf, "Test 1 failed. Heating value: %s degree, value: %s degree", strTemp1, strTemp2);
        putsForDebug(__SENSOR__, func, buf);
        handleBuzzerAndLED(STATE_TESTING_FAILED);

        return;
    }
    sprintf(buf, "Test 1 passed. Heating value: %s degree, value: %s degree", strTemp1, strTemp2);
    putsForDebug(__SENSOR__, func, buf);
    sleep(SEC_1);   /*  sleep 1 second, to stabilize the thing   */
    if (testValue1 < TEST_HEATING_THRESHOLD) {
        digitalWrite(HEATING, HIGH);
        sleep(SEC_4);
        ENABLE_ADC;
        testValue2  = CURVE((float) analogRead(TESTING_SENSOR));
        DISABLE_ADC;
        sleep(SEC_4);
        ENABLE_ADC;
        testValue3  = CURVE((float) analogRead(TESTING_SENSOR));
        value       = CURVE((float) analogRead(ACTUAL_SENSOR));
        DISABLE_ADC;
        digitalWrite(HEATING, LOW);
        currTestValue   = (uint8_t) testValue3;
        currValue       = (uint8_t) value;
        dtostrf(testValue3, 4, 2, strTemp1);
        dtostrf(value, 4, 2, strTemp2);
        if ((ABS((testValue2 - testValue1)) < TEST_HEATING_DIFF) || (ABS((testValue3 - testValue2)) < TEST_HEATING_DIFF) || (ABS((testValue3 - value)) < TEST_HEATING_DIFF)) {
            /* if failed at test2, testResult = 1 + 2   */
            //testResult      |= (HEAT_ERROR + 1);
            sprintf(buf, "Test 2 failed. Heating value: %s degree, value: %s degree", strTemp1, strTemp2);
            putsForDebug(__SENSOR__, func, buf);
            handleBuzzerAndLED(STATE_TESTING_FAILED);
            
            return;
        }
        sleepOneMinute();
    } else {
        sprintf(buf, "Sensor overheating. Heating value: %s degree, value: %s degree", strTemp1, strTemp2);
    }
    /*  Testing passed. testResult = 0 at line 158  */
    sprintf(buf, "Test 2 passed. Heating value: %s degree, value: %s degree", strTemp1, strTemp2);
    putsForDebug(__SENSOR__, func, buf);
    putsForDebug(__SENSOR__, func, MSG_TEST_SUCCESS);
    handleBuzzerAndLED(STATE_TESTING_PASSED);
#else
/*  TODO: smoke */
#endif

    return;
}

void putsForDebug(const char *file, const char *func, char *msg)
{
    Serial.print(file);
    Serial.print(func);
    Serial.println(msg);
    Serial.flush();

    return;
}

void checkBattery()
{
    static char *func       = UTIL_CHECKBATTERY;
    char        buf[SIZE_BUF], strTemp[SIZE_BUF];
    float       voltage;
    
    voltage = READ_BATTERY_VOLTAGE;
    dtostrf(voltage, 4, 2, strTemp);
    sprintf(buf, "Voltage: %s V", strTemp);
    putsForDebug(__UTIL__, func, buf);
    if (voltage < VOLTAGE_THRESHOLD) {
        putsForDebug(__UTIL__, func, MSG_LOW_BATTERY_VOLTAGE);
    }
    handleBuzzerAndLED(STATE_CHECK_BATTERY);

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
