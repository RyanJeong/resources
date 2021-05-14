#include "heat.h"
#define __UTIL__ "[util.ino]"
#define VOLTAGE_THRESHOLD       5.0

uint8_t     commResult, thingState;

static float    data[SIZE_DATA];
static uint8_t  index, countFireDetected, isWarning, heatThreshold;
static float    pastValue;
static uint8_t  reserve;

uint8_t         testResult, averageValue, currValue, currTestValue;

static char     tempDbg[SIZE_BUF];
static char     tempStr[SIZE_TEMP];

void setup()
{
    initThing();
/*
    digitalWrite(STATUS1, HIGH);
    digitalWrite(STATUS0, HIGH);
    exit(0);
*/
}

/*  TODO: 10번 값 입력 잘 되는가 확인   */
void loop()
{
    int i;

    prepareSensor();
    for (i = 0; i < 10; ++i) {
        sensing();
        sleep(SEC_1);
    }
    checkBattery();
    testing();
/*
    checkBattery();
    delay(5000);
*/
}

void checkBattery(void)
{
    static char *func = "[void checkBattery(void)]";
    char        tempStr[SIZE_BUF];
    float       voltage;
    
    voltage = (0.04882812 * getValue(VCC_CHECK));    /*   10.0 * 5.0 / 1024.0 */
    dtostrf(voltage, 4, 2, tempStr);
    sprintf(tempDbg, "Voltage: %s V", tempStr);
    putsForDebug(__UTIL__, func, tempDbg);
    if (voltage < VOLTAGE_THRESHOLD) {
        sprintf(tempDbg, "%x", MSG_LOW_BATTERY_VOLTAGE);
        putsForDebug(__UTIL__, func, MSG_LOW_BATTERY_VOLTAGE);
    }
    handleBuzzerAndLED(STATE_CHECK_BATTERY);

    return;
}

void heatingTest(void)
{
/*  #define SEC_8 33    */
    digitalWrite(HEATING, HIGH);
    Serial.println("HEATING ON");
    Serial.flush();
    Serial.println((getValue(TESTING_SENSOR)), DEC);
    Serial.flush();
    sleep(SEC_4);
    Serial.println((getValue(TESTING_SENSOR)), DEC);
    Serial.flush();
    sleep(SEC_4);
    Serial.println((getValue(TESTING_SENSOR)), DEC);
    Serial.flush();
    digitalWrite(HEATING, LOW);
    Serial.println("HEATING OFF");
    Serial.flush();
    Serial.println((getValue(TESTING_SENSOR)), DEC);
    Serial.flush();
    sleep(SEC_4);
    Serial.println((getValue(TESTING_SENSOR)), DEC);
    Serial.flush();
    sleep(SEC_8);
    Serial.println((getValue(TESTING_SENSOR)), DEC);
    Serial.flush();

    return;
}

float getValue(uint8_t pin)
{
    int         i;
    float       temp;

    temp = 0;
    ENABLE_ADC;
    for (i = 0; i < 10; ++i) {
        temp += analogRead(pin);
    }
    DISABLE_ADC;

    return (temp / 10.0);
}

void sensing(void)
{
    /*
     *  func        : 디버깅 메세지를 작성하는 데 사용되는 식별 변수
     *  i           : 조건문에서 사용하는 제어 변수
     *  temp        : 임시 값 저장 변수
     *  filtered    : 센서로부터 얻은 값에 필터링을 거친 값
     *  tempStr     : 실수값에 대한 디버깅 메세지를 만들기 위한 버퍼
     */
    static char *func = "[void sensing(void)]";
    uint8_t     i;
    float       temp, filtered;
    char        tempStr[SIZE_BUF];

    /*  불이 났거나, 감지를 해야할 시기일 경우  */
#ifdef TEST_SENSOR
    reserve = RESERVE_TIME;
#endif
    if (isWarning || (reserve == RESERVE_TIME)) {
        temp = getValue(ACTUAL_SENSOR);
        /*  노이즈 제거부 */
        data[index++] = (filtered = (HEAT_ALPHA * CURVE(temp)) + (HEAT_BETA * pastValue));
        if (index == SIZE_DATA) {
            handleBuzzerAndLED(STATE_FULLED_BUFFER);
            index   = 0;
            temp    = (float) 0.0;
            for (i = 0; i < SIZE_DATA; ++i) {
                temp += data[i];
            }
            /*  Frame에서의 데이터는 1 byte여야 함  */
            averageValue    = (uint8_t) (temp / (float) SIZE_DATA);
            thingState      = SEND_A_AVERAGE_DATA;
        }
        heatThreshold   = (countFireDetected) ? (THRESHOLD - 1) : THRESHOLD;
        if (filtered > heatThreshold) {
            (countFireDetected >= LAPSE) ? (isWarning = true) : ++countFireDetected;
        } else {
            if (countFireDetected) {
                --countFireDetected;
            }
        }
        dtostrf(filtered, 4, 2, tempStr);
        sprintf(tempDbg, "Index: %u, filtered value: %s degree, count of fire detection: %u", index, tempStr, countFireDetected);
        putsForDebug(__SENSOR__, func, tempDbg);
        if (isWarning) {
            if (countFireDetected) {
                /*  화재일 경우 thingState에는 화재 정보를 최우선으로 담아야 함 */
                thingState = NOTIFY_FIRE_DETECTED;  
                handleBuzzerAndLED(STATE_FIRE);
                RELAY_ON;
                sprintf(tempDbg, "%x", MSG_FIRE);
                putsForDebug(__SENSOR__, func, tempDbg);
            } else {
                isWarning   = false;
                RELAY_OFF;
            }
        }
        pastValue   = filtered;
        reserve     = 0;
    } else {
        ++reserve;
    }

    return;
}

void testing(void)
{
    /*
     *  func        : 디버깅 메세지를 작성하는 데 사용되는 식별 변수
     *  i           : 조건문에서 사용하는 제어 변수
     *  testValue1  : 테스트 센서로부터 얻은 값, Heating 전
     *  testValue2  : 테스트 센서로부터 얻은 값, 1차 Heating 직후
     *  testValue3  : 테스트 센서로부터 얻은 값, 2차 Heating 직후
     *  value       : 주 감지 센서로부터 얻은 값
     *  tempStr1    : 실수값에 대한 디버깅 메세지를 만들기 위한 버퍼1
     *  tempStr2    : 실수값에 대한 디버깅 메세지를 만들기 위한 버퍼2
     */
    static char *func = "[void testing(void)]";
    uint8_t     i;
    float       testValue1, testValue2, testValue3, value;
    char        tempStr1[SIZE_BUF];
    char        tempStr2[SIZE_BUF];

    handleBuzzerAndLED(STATE_RECEIVED_TEST);
    testResult  = 0; 
    testValue1      = CURVE(getValue(TESTING_SENSOR));
    value           = CURVE(getValue(ACTUAL_SENSOR));
    currTestValue   = (uint8_t) testValue1;
    currValue       = (uint8_t) value;
    dtostrf(testValue1, 4, 2, tempStr1);
    dtostrf(value, 4, 2, tempStr2);
    /*  2도 이상 차이가 나면 에러 */
    if (ABS((testValue1 - value)) > TEST_NORMAL_THRESHOLD) { 
        testResult      |= HEAT_ERROR;
        sprintf(tempDbg, "Test 1 failed. Heating value: %s, value: %s", tempStr1, tempStr2);
        putsForDebug(__SENSOR__, func, tempDbg);
        handleBuzzerAndLED(STATE_TESTING_FAILED);

        return;
    }
    sprintf(tempDbg, "Test 1 passed. Heating value: %s, value: %s", tempStr1, tempStr2);
    putsForDebug(__SENSOR__, func, tempDbg);
    /*  sleep 1 second, to stabilize the thing   */
    sleep(SEC_1);   
    ++testResult;
    if (testValue1 < TEST_HEATING_THRESHOLD) {
        digitalWrite(HEATING, HIGH);
        sleep(SEC_4);
        testValue2      = CURVE(getValue(TESTING_SENSOR));
        sleep(SEC_4);
        testValue3      = CURVE(getValue(TESTING_SENSOR));
        value           = CURVE(getValue(ACTUAL_SENSOR));
        digitalWrite(HEATING, LOW);
        currTestValue   = (uint8_t) testValue3;
        currValue       = (uint8_t) value;
        dtostrf(testValue3, 4, 2, tempStr1);
        dtostrf(value, 4, 2, tempStr2);
        /*  Cooling after heating   */
        sleep(SEC_60);
        if ((ABS((testValue2 - testValue1)) < TEST_HEATING_DIFF) 
            || (ABS((testValue3 - testValue2)) < TEST_HEATING_DIFF) 
            || (ABS((testValue3 - value)) < TEST_HEATING_DIFF)) {
            testResult      |= HEAT_ERROR;
            sprintf(tempDbg, "Test 2 failed. Heating value: %s, value: %s", tempStr1, tempStr2);
            putsForDebug(__SENSOR__, func, tempDbg);
            handleBuzzerAndLED(STATE_TESTING_FAILED);
            
            return;
        }
    } else {
        sprintf(tempDbg, "Sensor overheating. Heating value: %s, value: %s", tempStr1, tempStr2);
        putsForDebug(__SENSOR__, func, tempDbg);
    }
    /*  testResult 값을 0으로 만들어 성공했음을 알림    */
    --testResult;
    sprintf(tempDbg, "Test 2 passed. Heating value: %s, value: %s", tempStr1, tempStr2);
    putsForDebug(__SENSOR__, func, tempDbg);
    handleBuzzerAndLED(STATE_TESTING_PASSED);

    return;
}

/*
 *  1. ADC ON   -> READ -> delay(1000);
 *  2. ADC OFF  -> READ -> delay(1000);
 *  3. ADC ON   -> READ -> sleep(SEC_1);
 *  4. ADC OFF  -> READ -> sleep(SEC_1);
 */
void test1()
{
    static char *func = "[void test1()]";

/*
    ENABLE_ADC;
    sprintf(tempDbg, "1. ADC ON -> READ -> delay(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    delay(10);
    sprintf(tempDbg, "1. ADC ON -> READ -> delay(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    delay(10);
    sprintf(tempDbg, "1. ADC ON -> READ -> delay(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    delay(10);
    sprintf(tempDbg, "1. ADC ON -> READ -> delay(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    delay(10);
    sprintf(tempDbg, "1. ADC ON -> READ -> delay(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    delay(1000);
    
    DISABLE_ADC;
    sprintf(tempDbg, "2. ADC OFF -> READ -> delay(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    delay(10);
    sprintf(tempDbg, "2. ADC OFF -> READ -> delay(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    delay(10);
    sprintf(tempDbg, "2. ADC OFF -> READ -> delay(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    delay(10);
    sprintf(tempDbg, "2. ADC OFF -> READ -> delay(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    delay(10);
    sprintf(tempDbg, "2. ADC OFF -> READ -> delay(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    delay(1000);
*/
    
    ENABLE_ADC;
    sprintf(tempDbg, "3. ADC ON -> READ -> sleep(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    delay(10);
    sprintf(tempDbg, "3. ADC ON -> READ -> sleep(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    sleep(SEC_1);
    
    DISABLE_ADC;
    sprintf(tempDbg, "4. ADC OFF -> READ -> sleep(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    delay(10);
    sprintf(tempDbg, "4. ADC OFF -> READ -> sleep(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    sleep(SEC_1);

    ENABLE_ADC;
    sprintf(tempDbg, "3. ADC ON -> READ -> sleep(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    delay(10);
    sprintf(tempDbg, "3. ADC ON -> READ -> sleep(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    sleep(SEC_1);
    
    DISABLE_ADC;
    sprintf(tempDbg, "4. ADC OFF -> READ -> sleep(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    delay(10);
    sprintf(tempDbg, "4. ADC OFF -> READ -> sleep(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    sleep(SEC_1);

    return;    
}

/*
 *  1. ADC ON   -> READ with filtering -> delay(1000);
 *  2. ADC OFF  -> READ with filtering -> delay(1000);
 *  3. ADC ON   -> READ with filtering -> sleep(SEC_1);
 *  4. ADC OFF  -> READ with filtering -> sleep(SEC_1);
 */
void test1A()
{
    static char *func = "[void test1()]";

    ENABLE_ADC;
    sprintf(tempDbg, "1. ADC ON -> READ -> delay(1000), Sensing: %u", (int) CURVE((float) analogRead(ACTUAL_SENSOR)));
    putsForDebug(__SENSOR__, func, tempDbg);
    delay(1000);
    
    DISABLE_ADC;
    sprintf(tempDbg, "2. ADC OFF -> READ -> delay(1000), Sensing: %u", (int) CURVE((float) analogRead(ACTUAL_SENSOR)));
    putsForDebug(__SENSOR__, func, tempDbg);
    delay(1000);
    
    ENABLE_ADC;
    sprintf(tempDbg, "3. ADC ON -> READ -> sleep(1000), Sensing: %u", (int) CURVE((float)analogRead(ACTUAL_SENSOR)));
    putsForDebug(__SENSOR__, func, tempDbg);
    sleep(SEC_1);
    
    DISABLE_ADC;
    sprintf(tempDbg, "4. ADC OFF -> READ -> sleep(1000), Sensing: %u", (int) CURVE((float)analogRead(ACTUAL_SENSOR)));
    putsForDebug(__SENSOR__, func, tempDbg);
    sleep(SEC_1);

    return;
}

void putsForDebug(const char *file, const char *func, const char *msg)
{
#ifndef ALALALA
    DISABLE_RF;
    Serial.print('\n');
    Serial.print('\r');
    Serial.print(file);
    Serial.print(func);
    Serial.println(msg);
    Serial.flush();
#endif

    return;
}

void sleep(int i)
{
/*  #define SEC_60          (-1);   */
    if (i == SEC_60) {
        /*  change enable and WDE - also resets */
        WDTCSR  = (24);
        /*  prescalers only - get rid of the WDE and WDCE bit */
        WDTCSR  = (33);      
        /*  enable watchdog timer   */
        ENABLE_WATCHDOG;    
        /*  8 sec * 7 times    */
        SLEEP;
        SLEEP;
        SLEEP;
        SLEEP;
        SLEEP;
        SLEEP;
        SLEEP;
    } else {
        /*  change enable and WDE - also resets */
        WDTCSR  = (24);
        /*  prescalers only - get rid of the WDE and WDCE bit */
        WDTCSR  = (i);      
        /*  enable watchdog timer   */
        ENABLE_WATCHDOG;    
        SLEEP;
    }

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
    /*  
     *  Watchdog Timer
     *  Enable BOD
     *      The BODS bit is automatically cleared after three clock cycles.
     *      Writing this bit to one turns off the BOD in relevant sleep modes, while a zero in this bit keeps
     *      BOD active. Default setting keeps BOD active, i.e. BODS set to zero.
     */  
}

void sleepOneMinute(void)
{
    /*  change enable and WDE - also resets */
    WDTCSR  = (24);
    /*  prescalers only - get rid of the WDE and WDCE bit */
    WDTCSR  = (33);      
    /*  enable watchdog timer   */
    ENABLE_WATCHDOG;    
    /*  8 sec * 7 times    */
    SLEEP;
    SLEEP;
    SLEEP;
    SLEEP;
    SLEEP;
    SLEEP;
    SLEEP;

    return;
}

void handleBuzzerAndLED(uint8_t op)
{
    digitalWrite(STATUS1, (op & 01) ? HIGH : LOW);
    op >>= 1;
    digitalWrite(STATUS0, (op & 01) ? HIGH : LOW);
    op >>= 1;
    digitalWrite(BUZZER, (op & 01) ? HIGH : LOW);
    sleep(MILSEC_125);
    digitalWrite(STATUS1, LOW);
    digitalWrite(STATUS0, LOW);
    digitalWrite(BUZZER, LOW);

    return;
}

void initThing(void)
{
    Serial.begin(9600);

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

    enableSleep();

    return;
}

void prepareSensor(void)
{
    /*
     *  i           : 조건문에서 사용하는 제어 변수
     *  temp        : analogRead 결과를 임시 저장하는 변수
     *  filtered    : 센서로부터 얻은 값에 필터링을 거친 값
     */
    uint8_t     i;
    float       temp, filtered;

    pastValue   = INIT_SENSOR_TEMPERATURE;
    for (i = 0; i < PREPARE_TIME; ++i) {
        handleBuzzerAndLED(STATE_PREPARE);
        ENABLE_ADC;
        analogRead(ACTUAL_SENSOR);
        DISABLE_ADC;
        sleep(SEC_1);
    }

    return;
}

