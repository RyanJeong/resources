#include "kem.h"
#include "sensor.h"

/*
 *  commResult(kem.ino) : Gateway와 통신 결과를 저장하는 변수
 *  thingState(kem.ino) : 현재 thing의 상태를 저장하는 변수
 *
 *  data[SIZE_DATA]     : 감지기로부터 측정한 값을 저장하는 buffer
 *  backup[SIZE_BACKUP] : 연기 감지기에서 사용하는 값 보관 배열
 *  index               : 감지기로부터 측정한 값을 저장하는 buffer의 index
 *  countFireDetected   : 화재 감지 시 지속 시간을 측정하는 카운터
 *  isWarning           : 화재의 발생 여부를 저장하는 변수
 *  threshold           : 화재 감지에 사용하는 기준점(threshold)
 *  pastValue           : 감지기 동작 초기 센서 가정 온도
 * 
 *  testResult          : 감지기 test 수행 결과를 저장하는 변수
 *  averageValue        : buffer에 저장된 값이 가득 찰 경우, buffer의 평균값을 저장하는 변수
 *  currValue           : 화재가 감지되었을 경우, 감지기에 측정된 온도를 저장하는 변수
 *  currTestValue       : 테스트 수행 중에 테스트 감지기에 측정된 온도를 저장하는 변수
 */
extern size_t   commResult, thingState;
static float    data[SIZE_DATA], backup[SIZE_BACKUP];
static size_t   index, countFireDetected, isWarning, threshold;
static float    pastValue;
size_t          testResult, averageValue, currValue, currTestValue;

void prepareSensor(void)
{
#ifdef SOFTWARESERIAL_MODE
    prepareSensor1();
#else
    prepareSensor2();
#endif

    return;
}

#ifdef SOFTWARESERIAL_MODE
void prepareSensor1(void)
{
    /*
     *  func        : 디버깅 메세지를 작성하는 데 사용되는 식별 변수
     *  i           : 조건문에서 사용하는 제어 변수
     *  temp        : analogRead 결과를 임시 저장하는 변수
     *  filtered    : 센서로부터 얻은 값에 필터링을 거친 값
     *  buf         : 디버깅 메세지를 만들기 위한 버퍼
     */
    static char *func = SENSOR_PREPARESENSOR1;
    size_t      i;
    float       temp, filtered;
    char        buf[SIZE_BUF];

    putsForDebug(__SENSOR__, func, MSG_START);
    pastValue = INIT_SENSOR_TEMPERATURE;
    for (i = 0; i < PREPARE_TIME; ++i) {
        ENABLE_ADC;
        temp        = (float) analogRead(ACTUAL_SENSOR);
        DISABLE_ADC;
        filtered    = CURVE(temp);
        sprintf(buf, "Sensor value: %f", filtered);
        putsForDebug(__SENSOR__, func, buf);
        pastValue   = (HEAT_ALPHA * filtered) + (HEAT_BETA * pastValue);    /*  노이즈 제거부 */
        delay(1000);
    }
    index = 0;
#ifdef COMM_FUNCTIONAL
    thingState  = REQUEST_CONNECTION;
    commResult  = INIT;
#else
    thingState  = NORMAL;
    commResult  = OK;
#endif

    return;
}

#else
void prepareSensor2(void)
{
    /*
     *  i           : 조건문에서 사용하는 제어 변수
     *  temp        : analogRead 결과를 임시 저장하는 변수
     *  filtered    : 센서로부터 얻은 값에 필터링을 거친 값
     */
    size_t      i;
    float       temp, filtered;

    pastValue = INIT_SENSOR_TEMPERATURE;
    for (i = 0; i < PREPARE_TIME; ++i) {
        ENABLE_ADC;
        temp        = (float) analogRead(ACTUAL_SENSOR);
        DISABLE_ADC;
        filtered    = CURVE(temp);
        pastValue   = (HEAT_ALPHA * filtered) + (HEAT_BETA * pastValue);    /*  노이즈 제거부 */
        delay(1000);
    }
    index = 0;
#ifdef COMM_FUNCTIONAL
    thingState  = REQUEST_CONNECTION;
    commResult  = INIT;
#else
    thingState  = NORMAL;
    commResult  = OK;
#endif

    return;
}
#endif

void sensing(void)
{
#ifdef SOFTWARESERIAL_MODE
    sensing1();
#else
    sensing2();
#endif

    return;
}

#ifdef SOFTWARESERIAL_MODE
void sensing1(void)
{
    /*
     *  func        : 디버깅 메세지를 작성하는 데 사용되는 식별 변수
     *  i           : 조건문에서 사용하는 제어 변수
     *  temp        : 임시 값 저장 변수
     *  filtered    : 센서로부터 얻은 값에 필터링을 거친 값
     *  buf         : 디버깅 메세지를 만들기 위한 버퍼
     */
    static char *func = SENSOR_SENSING1;
    size_t      i;
    float       temp, filtered;
    char        buf[SIZE_BUF];

    putsForDebug(__SENSOR__, func, MSG_START);
    ENABLE_ADC;
    temp            = (float) analogRead(ACTUAL_SENSOR);
    DISABLE_ADC;
    data[index++]   = temp;
    filtered	    = (HEAT_ALPHA * CURVE(temp)) + (HEAT_BETA * pastValue);  /*  노이즈 제거부 */
    if (index == SIZE_DATA) {
        index   = 0;
        temp    = (float) 0.0;
        for (i = 0; i < SIZE_DATA; ++i) {
            temp += *(data + i);
        }
        temp            /= (float) SIZE_DATA;
        averageValue    = (size_t) CURVE(temp); /*  Frame에서의 데이터는 1 byte여야 함, 따라서 filtering을 거친 값(~255)을 frame에 포함시킴   */
        thingState      = SEND_A_AVERAGE_DATA;
    }
    threshold   = (countFireDetected) ? (THRESHOLD - 1) : THRESHOLD;
    if (filtered > threshold) {
        (countFireDetected >= LAPSE) ? (isWarning = true) : ++countFireDetected;
    } else {
        if (countFireDetected) {
            --countFireDetected;
        }
    }
    sprintf(buf, "Index: %u, filtered value: %f, count of fire detection: %u", index, filtered, countFireDetected);
    putsForDebug(__SENSOR__, func, buf);
    if (isWarning) {
        if (countFireDetected) {
            /*  화재 시 현재 온도를 실시간으로 전송하는 것이 우선   */
            if (thingState == SEND_A_AVERAGE_DATA) {
                index = (index == 0) ? (SIZE_DATA - 1) : (index - 1);
            }
            thingState = NOTIFY_FIRE_DETECTED;  
            temp        = *(data + index);
            currValue   = (size_t) CURVE(temp);
        } else {
            isWarning   = false;
        }
        putsForDebug(__SENSOR__, func, MSG_FIRE);
    }
	pastValue   = filtered;

    return;
}

#else
void sensing2(void)
{
    /*
     *  i           : 조건문에서 사용하는 제어 변수
     *  temp        : 임시 값 저장 변수
     *  filtered    : 센서로부터 얻은 값에 필터링을 거친 값
     */
    size_t      i;
    float       temp, filtered;

    ENABLE_ADC;
    temp            = (float) analogRead(ACTUAL_SENSOR);
    DISABLE_ADC;
    data[index++]   = temp;
    filtered	    = (HEAT_ALPHA * CURVE(temp)) + (HEAT_BETA * pastValue);  /*  노이즈 제거부 */
    if (index == SIZE_DATA) {
        index   = 0;
        temp    = (float) 0.0;
        for (i = 0; i < SIZE_DATA; ++i) {
            temp += *(data + i);
        }
        temp            /= (float) SIZE_DATA;
        averageValue    = (size_t) CURVE(temp); /*  Frame에서의 데이터는 1 byte여야 함, 따라서 filtering을 거친 값(~255)을 frame에 포함시킴   */
        thingState      = SEND_A_AVERAGE_DATA;
    }
    threshold   = (countFireDetected) ? (THRESHOLD - 1) : THRESHOLD;
    if (filtered > threshold) {
        (countFireDetected >= LAPSE) ? (isWarning = true) : ++countFireDetected;
    } else {
        if (countFireDetected) {
            --countFireDetected;
        }
    }
    if (isWarning) {
        if (countFireDetected) {
            /*  화재 시 현재 온도를 실시간으로 전송하는 것이 우선   */
            if (thingState == SEND_A_AVERAGE_DATA) {
                index = (index == 0) ? (SIZE_DATA - 1) : (index - 1);
            }
            thingState = NOTIFY_FIRE_DETECTED;  
            temp        = *(data + index);
            currValue   = (size_t) CURVE(temp);
        } else {
            isWarning   = false;
        }
    }
	pastValue   = filtered;

    return;
}
#endif

void testing(void)
{
#ifdef SOFTWARESERIAL_MODE
    testing1();
#else
    testing2();
#endif

    return;
}

#ifdef SOFTWARESERIAL_MODE
void testing1(void)
{
    /*
     *  func        : 디버깅 메세지를 작성하는 데 사용되는 식별 변수
     *  i           : 조건문에서 사용하는 제어 변수
     *  testSensor  : 테스트 센서로부터 얻은 값
     *  sensor      : 주 감지 센서로부터 얻은 값
     *  buf         : 디버깅 메세지를 만들기 위한 버퍼
     */
    static char *func = SENSOR_TESTING1;
    size_t      i;
    float       testSensor, sensor;
    char        buf[SIZE_BUF];

    putsForDebug(__SENSOR__, func, MSG_START);
    ENABLE_ADC;
    for (i = 0; i < TEST_NORMAL_TIME; ++i) {
        /*  이 부분에는 delay가 따로 필요 없는지?   */
        testSensor  = (float) analogRead(TESTING_SENSOR);
        sensor      = (float) analogRead(ACTUAL_SENSOR);
        delay(100); /*  100 (ms)    */
        if (ABS((testSensor - sensor)) > TEST_NORMAL_THRESHOLD) { /*  2도 이상 차이가 나면 에러 */
            testResult      = FAILED1;
            currTestValue   = (size_t) CURVE(testSensor);
            currValue       = (size_t) CURVE(sensor);
            putsForDebug(__SENSOR__, func, MSG_TEST1_FAILED);
            DISABLE_ADC;

            return;
        }
    }
    DISABLE_ADC;
    sprintf(buf, "Test 1 passed. Test sensor: %f, Sensor: %f", testSensor, sensor);
    putsForDebug(__SENSOR__, func, buf);
    delay(1000);    /*  1000 (ms), to stabilize the thing   */
    for (i = 0; i < TEST_HEATING_TIME; ++i) {
        ENABLE_ADC;
        digitalWrite(HEATING, HIGH);
        delay(3000);    /*  3000 (ms)   */
        digitalWrite(HEATING, LOW);
        DISABLE_ADC;
        testSensor  = (float) analogRead(TESTING_SENSOR);
        sensor      = (float) analogRead(ACTUAL_SENSOR);
        /*  Cooling을 판단문 다음에 배치하면, 고장 판단 시 Cooling 과정을 거치지 못함   */
        /*  단순 열감지 센서 고장이라면, 곧바로 테스트 수행 시 발열 센서가 고장날 수 있음 */
        delay(60000);   /*  60000 (ms) in a minute, for cooling  */
        if (ABS((testSensor - sensor)) < TEST_HEATING_THRESHOLD) { /*  4도 이상 차이가 나지 않으면 에러 */
            testResult      = FAILED2;
            currTestValue   = (size_t) CURVE(testSensor);
            currValue       = (size_t) CURVE(sensor);
            putsForDebug(__SENSOR__, func, MSG_TEST2_FAILED);

            return;
        }
    }
    sprintf(buf, "Test 2 passed. Test sensor: %f, Sensor: %f", testSensor, sensor);
    putsForDebug(__SENSOR__, func, buf);
    testResult      = SUCCESS;
    currTestValue   = (size_t) CURVE(testSensor);
    currValue       = (size_t) CURVE(sensor);
    putsForDebug(__SENSOR__, func, MSG_TEST_SUCCESS);

    return;
}

#else
void testing2(void)
{
    /*
     *  i           : 조건문에서 사용하는 제어 변수
     *  testSensor  : 테스트 센서로부터 얻은 값
     *  sensor      : 주 감지 센서로부터 얻은 값
     */
    size_t      i;
    float       testSensor, sensor;

    ENABLE_ADC;
    for (i = 0; i < TEST_NORMAL_TIME; ++i) {
        /*  이 부분에는 delay가 따로 필요 없는지?   */
        testSensor  = (float) analogRead(TESTING_SENSOR);
        sensor      = (float) analogRead(ACTUAL_SENSOR);
        delay(100); /*  100 (ms)    */
        if (ABS((testSensor - sensor)) > TEST_NORMAL_THRESHOLD) { /*  2도 이상 차이가 나면 에러 */
            testResult      = FAILED1;
            currTestValue   = (size_t) CURVE(testSensor);
            currValue       = (size_t) CURVE(sensor);
            DISABLE_ADC;

            return;
        }
    }
    DISABLE_ADC;
    delay(1000);    /*  1000 (ms), to stabilize the thing   */
    for (i = 0; i < TEST_HEATING_TIME; ++i) {
        ENABLE_ADC;
        digitalWrite(HEATING, HIGH);
        delay(3000);    /*  3000 (ms)   */
        digitalWrite(HEATING, LOW);
        DISABLE_ADC;
        testSensor  = (float) analogRead(TESTING_SENSOR);
        sensor      = (float) analogRead(ACTUAL_SENSOR);
        /*  Cooling을 판단문 다음에 배치하면, 고장 판단 시 Cooling 과정을 거치지 못함   */
        /*  단순 열감지 센서 고장이라면, 곧바로 테스트 수행 시 발열 센서가 고장날 수 있음 */
        delay(60000);   /*  60000 (ms) in a minute, for cooling  */
        if (ABS((testSensor - sensor)) < TEST_HEATING_THRESHOLD) { /*  4도 이상 차이가 나지 않으면 에러 */
            testResult      = FAILED2;
            currTestValue   = (size_t) CURVE(testSensor);
            currValue       = (size_t) CURVE(sensor);

            return;
        }
    }
    testResult      = SUCCESS;
    currTestValue   = (size_t) CURVE(testSensor);
    currValue       = (size_t) CURVE(sensor);

    return;
}
#endif
