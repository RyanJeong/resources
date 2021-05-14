#include "kem.h"
#include "sensor.h"
#include "interrupt.h"

/*
 *  commResult(kem.ino)         : Gateway와 통신 결과를 저장하는 변수
 *  thingState(kem.ino)         : 현재 thing의 상태를 저장하는 변수
 *  tempDbg[SIZE_BUF](kem.ino)  : Debug message를 담는 배열
 *
 *  data[SIZE_DATA]     : 감지기로부터 측정한 값을 저장하는 buffer
 *  index               : 감지기로부터 측정한 값을 저장하는 buffer의 index
 *  countFireDetected   : 화재 감지 시 지속 시간을 측정하는 카운터
 *  isWarning           : 화재의 발생 여부를 저장하는 변수
 *  heatThreshold       : 화재 감지에 사용하는 열 감지 기준점(threshold)
 *  pastValue           : 감지기 동작 초기 센서 가정 온도
 *  reserve             : 평상시엔 RESERVE_TIME check, 화재시엔 사용되지 않음
 * 
 *  testResult          : 감지기 test 수행 결과를 저장하는 변수
 *  averageValue        : buffer에 저장된 값이 가득 찰 경우, buffer의 평균값을 저장하는 변수
 *  currValue           : 테스트 수행 중에 감지기에 측정된 온도를 저장하는 변수
 *  currTestValue       : 테스트 수행 중에 테스트 감지기에 측정된 온도를 저장하는 변수
 */
extern uint8_t  commResult, thingState;
extern char     tempDbg[SIZE_BUF];

static float    data[SIZE_DATA];
static uint8_t  index, countFireDetected, isWarning, heatThreshold;
static float    pastValue;
static uint8_t  reserve;

uint8_t         testResult, averageValue, currValue, currTestValue;

void prepareSensor(void)
{
#ifdef HEAT_MODE
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
#else
/*  TODO: smoke */
#endif

    return;
}

void testSensor(void)
{
    int i;

    prepareSensor();
    for (i = 0; i < 5; ++i) {
        /*  5초에 한 번 센싱하는 것이 아닌 1초에 한 번 센싱 */
        reserve = RESERVE_TIME;
        sensing();
        delay(1000);
    }
    testing();

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
#ifdef HEAT_MODE
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
    if (isWarning || (reserve == RESERVE_TIME)) {
        temp = getValue(ACTUAL_SENSOR);
        /*  노이즈 제거부 */
        data[index++] = (filtered = (HEAT_ALPHA * CURVE(temp)) + (HEAT_BETA * pastValue));
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
        sprintf(tempDbg, "Test 1 failed. Heating value: %s degree, value: %s degree", tempStr1, tempStr2);
        putsForDebug(__SENSOR__, func, tempDbg);
        handleBuzzerAndLED(STATE_TESTING_FAILED);

        return;
    }
    sprintf(tempDbg, "Test 1 passed. Heating value: %s degree, value: %s degree", tempStr1, tempStr2);
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
            sprintf(tempDbg, "Test 2 failed. Heating value: %s degree, value: %s degree", tempStr1, tempStr2);
            putsForDebug(__SENSOR__, func, tempDbg);
            handleBuzzerAndLED(STATE_TESTING_FAILED);
            
            return;
        }
    } else {
        sprintf(tempDbg, "Sensor overheating. Heating value: %s degree, value: %s degree", tempStr1, tempStr2);
        putsForDebug(__SENSOR__, func, tempDbg);
    }
    /*  testResult 값을 0으로 만들어 성공했음을 알림    */
    --testResult;
    sprintf(tempDbg, "Test 2 passed. Heating value: %s degree, value: %s degree", tempStr1, tempStr2);
    putsForDebug(__SENSOR__, func, tempDbg);
    handleBuzzerAndLED(STATE_TESTING_PASSED);
#else
/*  TODO: smoke */
#endif

    return;
}
