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
 *  smokeThreshold      : 화재 감지에 사용하는 연기 감지 기준점(threshold)
 *  heatThreshold       : 화재 감지에 사용하는 열 감지 기준점(threshold)
 *  pastValue           : 감지기 동작 초기 센서 가정 온도
 *  reserve             : 평상시엔 RESERVE_TIME check, 화재시엔 사용되지 않음
 * 
 *  testResult          : 감지기 test 수행 결과를 저장하는 변수
 *  averageValue        : buffer에 저장된 값이 가득 찰 경우, buffer의 평균값을 저장하는 변수
 *  currValue           : 화재가 감지되었을 경우, 감지기에 측정된 온도를 저장하는 변수
 *  currTestValue       : 테스트 수행 중에 테스트 감지기에 측정된 온도를 저장하는 변수
 */
extern uint8_t  commResult, thingState;

static float    data[SIZE_DATA], backup[SIZE_BACKUP], smokeThreshold;
static uint8_t  index, countFireDetected, isWarning, heatThreshold;
static float    pastValue;
static uint8_t  reserve;

uint8_t         testResult, averageValue, currValue, currTestValue;

void prepareSensor(void)
{
#ifdef SOFTWARESERIAL_MODE
    prepareSensor1();
#else
    prepareSensor2();
#endif

    return;
}

#ifdef HEAT_MODE
# ifdef SOFTWARESERIAL_MODE
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
    uint8_t     i;
    float       temp, filtered;
    char        buf[SIZE_BUF], strTemp[SIZE_TEMP];

    putsForDebug(__SENSOR__, func, MSG_START);
    pastValue = INIT_SENSOR_TEMPERATURE;
    for (i = 0; i < PREPARE_TIME; ++i) {
        ENABLE_ADC;
        temp        = (float) analogRead(ACTUAL_SENSOR);
        DISABLE_ADC;
        filtered    = CURVE(temp);
        /*  char *dtostrf(double val, signed char width, unsigned char prec, char *s    */
        dtostrf(filtered, 4, 2, strTemp);
        sprintf(buf, "Sensor value: %s", strTemp);
        putsForDebug(__SENSOR__, func, buf);
        pastValue   = (HEAT_ALPHA * filtered) + (HEAT_BETA * pastValue);    /*  노이즈 제거부 */
        delay(1000);
    }
    index = 0;

    return;
}

# else
void prepareSensor2(void)
{
    /*
     *  i           : 조건문에서 사용하는 제어 변수
     *  temp        : analogRead 결과를 임시 저장하는 변수
     *  filtered    : 센서로부터 얻은 값에 필터링을 거친 값
     */
    uint8_t     i;
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

    return;
}
# endif
#else
# ifdef SOFTWARESERIAL_MODE
void prepareSensor1(void)
{
    /*
     *  func        : 디버깅 메세지를 작성하는 데 사용되는 식별 변수
     *  i, j        : 조건문에서 사용하는 제어 변수
     *  buf         : 디버깅 메세지를 만들기 위한 버퍼
     */
    static char *func = SENSOR_PREPARESENSOR1;
    uint8_t     i, j;
    char        buf[SIZE_BUF], strTemp[SIZE_TEMP];

    putsForDebug(__SENSOR__, func, MSG_START);
    for (i = 0; i < PREPARE_TIME; ++i) {
        for (j = 1; j < SIZE_BACKUP; ++j) {
            *(backup + SIZE_BACKUP - j) = *(backup + SIZE_BACKUP - (j + 1));
        }
        ENABLE_ADC;
        *backup = (float) analogRead(ACTUAL_SENSOR);
        DISABLE_ADC;
        dtostrf(*backup, 4, 2, strTemp);
        sprintf(buf, "Sensor value: %s", strTemp);
        putsForDebug(__SENSOR__, func, buf);
        delay(1000);
    }
    index           = 0;
    smokeThreshold  = *(backup);
    pastValue       = *(backup + 1);

    return;
}

# else
void prepareSensor2(void)
{
    /*
     *  i, j        : 조건문에서 사용하는 제어 변수
     */
    uint8_t     i, j;

    for (i = 0; i < PREPARE_TIME; ++i) {
        for (j = 1; j < SIZE_BACKUP; ++j) {
            *(backup + SIZE_BACKUP - j) = *(backup + SIZE_BACKUP - (j + 1));
        }
        ENABLE_ADC;
        *backup = (float) analogRead(ACTUAL_SENSOR);
        DISABLE_ADC;
        delay(1000);
    }
    index           = 0;
    smokeThreshold  = *(backup);
    pastValue       = *(backup + 1);

    return;
}
# endif
#endif

void sensing(void)
{
    if ((reserve == RESERVE_TIME) || isWarning) {
#ifdef SOFTWARESERIAL_MODE
        sensing1();
#else
        sensing2();
#endif
        reserve = 0;
    } else {
        ++reserve;
    }

    return;
}

#ifdef HEAT_MODE
# ifdef SOFTWARESERIAL_MODE
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
    uint8_t     i;
    float       temp, filtered;
    char        buf[SIZE_BUF], strTemp[SIZE_TEMP];

    putsForDebug(__SENSOR__, func, MSG_START);
    ENABLE_ADC;
    temp            = (float) analogRead(ACTUAL_SENSOR);
    DISABLE_ADC;
    data[index++]   = temp;
    filtered        = (HEAT_ALPHA * CURVE(temp)) + (HEAT_BETA * pastValue);  /*  노이즈 제거부 */
    if (index == SIZE_DATA) {
        index   = 0;
        temp    = (float) 0.0;
        for (i = 0; i < SIZE_DATA; ++i) {
            temp += *(data + i);
        }
        temp            /= (float) SIZE_DATA;
        averageValue    = (uint8_t) CURVE(temp); /*  Frame에서의 데이터는 1 byte여야 함, 따라서 filtering을 거친 값(~255)을 frame에 포함시킴   */
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
    dtostrf(filtered, 4, 2, strTemp);
    sprintf(buf, "Sensor value: %s", strTemp);
    sprintf(buf, "Index: %u, filtered value: %s, count of fire detection: %u", index, strTemp, countFireDetected);
    putsForDebug(__SENSOR__, func, buf);
    if (isWarning) {
        if (countFireDetected) {
            /*  화재 시 현재 온도를 실시간으로 전송하는 것이 우선   */
            if (thingState == SEND_A_AVERAGE_DATA) {
                index = (index == 0) ? (SIZE_DATA - 1) : (index - 1);
            }
            thingState = NOTIFY_FIRE_DETECTED;  
            temp        = *(data + index);
            currValue   = (uint8_t) CURVE(temp);
        } else {
            isWarning   = false;
        }
        putsForDebug(__SENSOR__, func, MSG_FIRE);
    }
    pastValue   = filtered;

    return;
}

# else
void sensing2(void)
{
    /*
     *  i           : 조건문에서 사용하는 제어 변수
     *  temp        : 임시 값 저장 변수
     *  filtered    : 센서로부터 얻은 값에 필터링을 거친 값
     */
    uint8_t     i;
    float       temp, filtered;

    ENABLE_ADC;
    temp            = (float) analogRead(ACTUAL_SENSOR);
    DISABLE_ADC;
    data[index++]   = temp;
    filtered        = (HEAT_ALPHA * CURVE(temp)) + (HEAT_BETA * pastValue);  /*  노이즈 제거부 */
    if (index == SIZE_DATA) {
        index   = 0;
        temp    = (float) 0.0;
        for (i = 0; i < SIZE_DATA; ++i) {
            temp += *(data + i);
        }
        temp            /= (float) SIZE_DATA;
        averageValue    = (uint8_t) CURVE(temp); /*  Frame에서의 데이터는 1 byte여야 함, 따라서 filtering을 거친 값(~255)을 frame에 포함시킴   */
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
    if (isWarning) {
        if (countFireDetected) {
            /*  화재 시 현재 온도를 실시간으로 전송하는 것이 우선   */
            if (thingState == SEND_A_AVERAGE_DATA) {
                index = (index == 0) ? (SIZE_DATA - 1) : (index - 1);
            }
            thingState = NOTIFY_FIRE_DETECTED;  
            temp        = *(data + index);
            currValue   = (uint8_t) CURVE(temp);
        } else {
            isWarning   = false;
        }
    }
    pastValue   = filtered;

    return;
}
# endif
#else
# ifdef SOFTWARESERIAL_MODE
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
    uint8_t     i;
    float       temp, filtered;
    char        buf[SIZE_BUF], strTemp[SIZE_TEMP];

    putsForDebug(__SENSOR__, func, MSG_START);
    ENABLE_ADC;
    temp        = (float) analogRead(ACTUAL_SENSOR);
    DISABLE_ADC;
    filtered    = (SMOKE_ALPHA * temp) + (SMOKE_BETA * pastValue);  /*  노이즈 제거부 */
    if (ABS(filtered - smokeThreshold) > NOISE_THRESHOLD) {  /*  noise 입력 제거 */
        *(backup)   = smokeThreshold;
    } else {    /*  smoke voltage 평균  */
        *(backup)   = filtered;
        smokeThreshold   += ((filtered - *(backup + SIZE_BACKUP - 1)) / SIZE_BACKUP);
    }
    for (i = 1; i < SIZE_BACKUP; ++i) { /*  데이터 갱신 */
        *(backup + SIZE_BACKUP - i) = *(backup + SIZE_BACKUP - (i + 1));
    }
    pastValue       = filtered;
    data[index++]   = *(backup);    /*  noise 입력 방지 */
    /*  값이 매우 작을 경우, 추가 연산을 해 값을 보정할 계획 */
    /*  연기 감지에 사용되는 값도 buffer에 저장하고, 서버로 보내야 하는지 확인해야 함   */
    if (index == SIZE_DATA) {
        index   = 0;
        temp    = (float) 0.0;
        for (i = 0; i < SIZE_DATA; ++i) {
            temp += *(data + i);
        }
        averageValue    = (uint8_t) (temp / (float) SIZE_DATA);
        thingState      = SEND_A_AVERAGE_DATA;
    }
    if ((filtered - smokeThreshold) > SMOKE_DETECTION_VOLTAGE) {
        (countFireDetected >= LAPSE) ? (isWarning = true) : ++countFireDetected;
    } else {
        if (countFireDetected) {
            --countFireDetected;
        }
    }
    dtostrf(filtered, 4, 2, strTemp);
    sprintf(buf, "Index: %u, sensor value: %s, count of fire detection: %u", index, strTemp, countFireDetected);
    putsForDebug(__SENSOR__, func, buf);
    if (isWarning) {
        if (countFireDetected) {
            /*  화재 시 현재 측정 값을 전송하는 것이 우선   */
            if (thingState == SEND_A_AVERAGE_DATA) {
                index = (index == 0) ? (SIZE_DATA - 1) : (index - 1);
            }
            thingState = NOTIFY_FIRE_DETECTED;  
            currValue   = (uint8_t) *(data + index);
        } else {
            isWarning   = false;
        }
        putsForDebug(__SENSOR__, func, MSG_FIRE);
    }

    return;
}

# else
void sensing2(void)
{
    /*
     *  i           : 조건문에서 사용하는 제어 변수
     *  temp        : 임시 값 저장 변수
     *  filtered    : 센서로부터 얻은 값에 필터링을 거친 값
     */
    uint8_t     i;
    float       temp, filtered;

    ENABLE_ADC;
    temp        = (float) analogRead(ACTUAL_SENSOR);
    DISABLE_ADC;
    filtered    = (SMOKE_ALPHA * temp) + (SMOKE_BETA * pastValue);  /*  노이즈 제거부 */
    if (ABS(filtered - smokeThreshold) > NOISE_THRESHOLD) {  /*  noise 입력 제거 */
        *(backup)   = smokeThreshold;
    } else {    /*  smoke voltage 평균  */
        *(backup)   = filtered;
        smokeThreshold   += ((filtered - *(backup + SIZE_BACKUP - 1)) / SIZE_BACKUP);
    }
    for (i = 1; i < SIZE_BACKUP; ++i) { /*  데이터 갱신 */
        *(backup + SIZE_BACKUP - i) = *(backup + SIZE_BACKUP - (i + 1));
    }
    pastValue       = filtered;
    data[index++]   = *(backup);
    if (index == SIZE_DATA) {
        index   = 0;
        temp    = (float) 0.0;
        for (i = 0; i < SIZE_DATA; ++i) {
            temp += *(data + i);
        }
        averageValue    = (uint8_t) (temp / (float) SIZE_DATA);
        thingState      = SEND_A_AVERAGE_DATA;
    }
    if ((filtered - smokeThreshold) > SMOKE_DETECTION_VOLTAGE) {
        (countFireDetected >= LAPSE) ? (isWarning = true) : ++countFireDetected;
    } else {
        if (countFireDetected) {
            --countFireDetected;
        }
    }
    if (isWarning) {
        if (countFireDetected) {
            /*  화재 시 현재 측정 값을 전송하는 것이 우선   */
            if (thingState == SEND_A_AVERAGE_DATA) {
                index = (index == 0) ? (SIZE_DATA - 1) : (index - 1);
            }
            thingState = NOTIFY_FIRE_DETECTED;  
            currValue   = (uint8_t) *(data + index);
        } else {
            isWarning   = false;
        }
    }

    return;
}
# endif
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

#ifdef HEAT_MODE
# ifdef SOFTWARESERIAL_MODE
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
    uint8_t     i;
    float       testSensor, sensor;
    char        buf[SIZE_BUF], strTemp1[SIZE_TEMP], strTemp2[SIZE_TEMP];

    putsForDebug(__SENSOR__, func, MSG_START);
    testResult  = 0; /* if failed at test1, testResult = 0   */
    for (i = 0; i < TEST_NORMAL_TIME; ++i) {
        /*  이 부분에는 delay가 따로 필요 없는지?   */
        ENABLE_ADC;
        testSensor  = (float) analogRead(TESTING_SENSOR);
        sensor      = (float) analogRead(ACTUAL_SENSOR);
        DISABLE_ADC;
        delay(100); /*  100 (ms)    */
        if (ABS((testSensor - sensor)) > TEST_NORMAL_THRESHOLD) { /*  2도 이상 차이가 나면 에러 */
            testResult      |= HEAT_ERROR;
            currTestValue   = (uint8_t) CURVE(testSensor);
            currValue       = (uint8_t) CURVE(sensor);
            dtostrf(testSensor, 4, 2, strTemp1);
            dtostrf(sensor, 4, 2, strTemp2);
            sprintf(buf, "Test 1 failed. Test sensor: %s, Sensor: %s", strTemp1, strTemp2);
            putsForDebug(__SENSOR__, func, buf);

            return;
        }
    }
    dtostrf(testSensor, 4, 2, strTemp1);
    dtostrf(sensor, 4, 2, strTemp2);
    sprintf(buf, "Test 1 passed. Test sensor: %s, Sensor: %s", strTemp1, strTemp2);
    putsForDebug(__SENSOR__, func, buf);
    ++testResult;   /* if failed at test1, testResult = 1   */
    delay(1000);    /*  1000 (ms), to stabilize the thing   */
    for (i = 0; i < TEST_HEATING_TIME; ++i) {
        digitalWrite(HEATING, HIGH);
        delay(3000);    /*  3000 (ms)   */
        digitalWrite(HEATING, LOW);
        ENABLE_ADC;
        testSensor  = (float) analogRead(TESTING_SENSOR);
        sensor      = (float) analogRead(ACTUAL_SENSOR);
        DISABLE_ADC;
        /*  Cooling을 판단문 다음에 배치하면, 고장 판단 시 Cooling 과정을 거치지 못함   */
        /*  단순 열감지 센서 고장이라면, 곧바로 테스트 수행 시 발열 센서가 고장날 수 있음 */
        delay(60000);   /*  60000 (ms) in a minute, for cooling  */
        if (ABS((testSensor - sensor)) < TEST_HEATING_THRESHOLD) { /*  4도 이상 차이가 나지 않으면 에러 */
            testResult      |= HEAT_ERROR;
            currTestValue   = (uint8_t) CURVE(testSensor);
            currValue       = (uint8_t) CURVE(sensor);
            dtostrf(testSensor, 4, 2, strTemp1);
            dtostrf(sensor, 4, 2, strTemp2);
            sprintf(buf, "Test 2 failed. Test sensor: %s, Sensor: %s", strTemp1, strTemp2);
            putsForDebug(__SENSOR__, func, buf);

            return;
        }
    }
    dtostrf(testSensor, 4, 2, strTemp1);
    dtostrf(sensor, 4, 2, strTemp2);
    sprintf(buf, "Test 2 passed. Test sensor: %s, Sensor: %s", strTemp1, strTemp2);
    putsForDebug(__SENSOR__, func, buf);
    currTestValue   = (uint8_t) CURVE(testSensor);
    currValue       = (uint8_t) CURVE(sensor);
    putsForDebug(__SENSOR__, func, MSG_TEST_SUCCESS);

    return;
}

# else
void testing2(void)
{
    /*
     *  i           : 조건문에서 사용하는 제어 변수
     *  testSensor  : 테스트 센서로부터 얻은 값
     *  sensor      : 주 감지 센서로부터 얻은 값
     */
    uint8_t     i;
    float       testSensor, sensor;

    testResult  = 0; /* if failed at test1, testResult = 0   */
    for (i = 0; i < TEST_NORMAL_TIME; ++i) {
        /*  이 부분에는 delay가 따로 필요 없는지?   */
        ENABLE_ADC;
        testSensor  = (float) analogRead(TESTING_SENSOR);
        sensor      = (float) analogRead(ACTUAL_SENSOR);
        DISABLE_ADC;
        delay(100); /*  100 (ms)    */
        if (ABS((testSensor - sensor)) > TEST_NORMAL_THRESHOLD) { /*  2도 이상 차이가 나면 에러 */
            testResult      |= HEAT_ERROR;
            currTestValue   = (uint8_t) CURVE(testSensor);
            currValue       = (uint8_t) CURVE(sensor);

            return;
        }
    }
    ++testResult;   /* if failed at test1, testResult = 1   */
    delay(1000);    /*  1000 (ms), to stabilize the thing   */
    for (i = 0; i < TEST_HEATING_TIME; ++i) {
        digitalWrite(HEATING, HIGH);
        delay(3000);    /*  3000 (ms)   */
        digitalWrite(HEATING, LOW);
        ENABLE_ADC;
        testSensor  = (float) analogRead(TESTING_SENSOR);
        sensor      = (float) analogRead(ACTUAL_SENSOR);
        DISABLE_ADC;
        /*  Cooling을 판단문 다음에 배치하면, 고장 판단 시 Cooling 과정을 거치지 못함   */
        /*  단순 열감지 센서 고장이라면, 곧바로 테스트 수행 시 발열 센서가 고장날 수 있음 */
        delay(60000);   /*  60000 (ms) in a minute, for cooling  */
        if (ABS((testSensor - sensor)) < TEST_HEATING_THRESHOLD) { /*  4도 이상 차이가 나지 않으면 에러 */
            testResult      |= HEAT_ERROR;
            currTestValue   = (uint8_t) CURVE(testSensor);
            currValue       = (uint8_t) CURVE(sensor);

            return;
        }
    }
    currTestValue   = (uint8_t) CURVE(testSensor);
    currValue       = (uint8_t) CURVE(sensor);

    return;
}
# endif
#else
# ifdef SOFTWARESERIAL_MODE
void testing1(void)
{
    /*
     *  func        : 디버깅 메세지를 작성하는 데 사용되는 식별 변수
     *  sensor      : 센서로부터 얻은 값
     *
     *  currValue, currTestValue 자리에 0 또는 1 값을 넣어 4가지 형태로 구분하는 역할로 사용
     *  실패 시: FAILED_SMOKE, 00, 01, 10, 11
     *  성공 시: SUCCESS, xx(don't care)
     */
    static char *func = SENSOR_TESTING1;
    float       sensor;

    putsForDebug(__SENSOR__, func, MSG_START);
    testResult  = 0; /* if failed at test1, testResult = 0   */
    delay(500);                     /*  500 (ms), to stabilize the thing   */
    digitalWrite(SHUTTER, LOW);     /*  shutter off */
    digitalWrite(LED, LOW);         /*  LED off */
    ENABLE_ADC;
    sensor = (float) analogRead(ACTUAL_SENSOR);
    DISABLE_ADC;
    if (sensor > TEST_OFF_THRESHOLD) {  /*  ... | 0 | 0 | ...   */
        testResult      |= SMOKE_ERROR;
        putsForDebug(__SENSOR__, func, MSG_SMOKE_TEST_FAILED1);

        return;
    };

    ++testResult; /* if failed at test2, testResult = 1   */
    delay(500);                     /*  500 (ms), to stabilize the thing   */
    digitalWrite(SHUTTER, LOW);     /*  shutter off */
    digitalWrite(LED, HIGH);        /*  LED on */
    ENABLE_ADC;
    sensor = (float) analogRead(ACTUAL_SENSOR);
    DISABLE_ADC;
    if (sensor > TEST_OFF_THRESHOLD) {  /*  ... | 0 | 1 | ...   */
        digitalWrite(LED, LOW);     /*  LED off */
        testResult      |= SMOKE_ERROR;
        putsForDebug(__SENSOR__, func, MSG_SMOKE_TEST_FAILED2);

        return;
    };

    ++testResult; /* if failed at test3, testResult = 2   */
    delay(500);                     /*  500 (ms), to stabilize the thing   */
    digitalWrite(SHUTTER, HIGH);    /*  shutter on */
    digitalWrite(LED, LOW);         /*  LED off */
    ENABLE_ADC;
    sensor = (float) analogRead(ACTUAL_SENSOR);
    DISABLE_ADC;
    if (sensor > TEST_ON_THRESHOLD) {  /*  ... | 1 | 0 | ...   */
        digitalWrite(SHUTTER, LOW); /*  shutter off */
        testResult      |= SMOKE_ERROR;
        putsForDebug(__SENSOR__, func, MSG_SMOKE_TEST_FAILED3);

        return;
    };

    ++testResult; /* if failed at test4, testResult = 3   */
    delay(500);                     /*  500 (ms), to stabilize the thing   */
    digitalWrite(SHUTTER, HIGH);    /*  shutter on */
    digitalWrite(LED, HIGH);        /*  LED on */
    ENABLE_ADC;
    sensor = (float) analogRead(ACTUAL_SENSOR);
    DISABLE_ADC;
    if (sensor < TEST_ON_THRESHOLD) {  /*  ... | 1 | 1 | ...   */
        digitalWrite(SHUTTER, LOW); /*  shutter off */
        digitalWrite(LED, LOW);     /*  LED off */
        testResult      |= SMOKE_ERROR;
        putsForDebug(__SENSOR__, func, MSG_SMOKE_TEST_FAILED4);

        return;
    };
    digitalWrite(SHUTTER, LOW); /*  shutter off */
    digitalWrite(LED, LOW);     /*  LED off */
    putsForDebug(__SENSOR__, func, MSG_TEST_SUCCESS);

    return;
}

# else
void testing2(void)
{
    /*
     *  sensor      : 센서로부터 얻은 값
     */
    float       sensor;

    testResult  = 0; /* if failed at test1, testResult = 0   */
    delay(500);                     /*  500 (ms), to stabilize the thing   */
    digitalWrite(SHUTTER, LOW);     /*  shutter off */
    digitalWrite(LED, LOW);         /*  LED off */
    ENABLE_ADC;
    sensor = (float) analogRead(ACTUAL_SENSOR);
    DISABLE_ADC;
    if (sensor > TEST_OFF_THRESHOLD) {  /*  ... | 0 | 0 | ...   */
        testResult      |= SMOKE_ERROR;

        return;
    };

    ++testResult; /* if failed at test2, testResult = 1   */
    delay(500);                     /*  500 (ms), to stabilize the thing   */
    digitalWrite(SHUTTER, LOW);     /*  shutter off */
    digitalWrite(LED, HIGH);        /*  LED on */
    ENABLE_ADC;
    sensor = (float) analogRead(ACTUAL_SENSOR);
    DISABLE_ADC;
    if (sensor > TEST_OFF_THRESHOLD) {  /*  ... | 0 | 1 | ...   */
        digitalWrite(LED, LOW);     /*  LED off */
        testResult      |= SMOKE_ERROR;

        return;
    };

    ++testResult; /* if failed at test3, testResult = 2   */
    delay(500);                     /*  500 (ms), to stabilize the thing   */
    digitalWrite(SHUTTER, HIGH);    /*  shutter on */
    digitalWrite(LED, LOW);         /*  LED off */
    ENABLE_ADC;
    sensor = (float) analogRead(ACTUAL_SENSOR);
    DISABLE_ADC;
    if (sensor > TEST_ON_THRESHOLD) {  /*  ... | 1 | 0 | ...   */
        digitalWrite(SHUTTER, LOW); /*  shutter off */
        testResult      |= SMOKE_ERROR;

        return;
    };

    ++testResult; /* if failed at test4, testResult = 3   */
    delay(500);                     /*  500 (ms), to stabilize the thing   */
    digitalWrite(SHUTTER, HIGH);    /*  shutter on */
    digitalWrite(LED, HIGH);        /*  LED on */
    ENABLE_ADC;
    sensor = (float) analogRead(ACTUAL_SENSOR);
    DISABLE_ADC;
    if (sensor < TEST_ON_THRESHOLD) {  /*  ... | 1 | 1 | ...   */
        digitalWrite(SHUTTER, LOW); /*  shutter off */
        digitalWrite(LED, LOW);     /*  LED off */
        testResult      |= SMOKE_ERROR;

        return;
    };
    digitalWrite(SHUTTER, LOW); /*  shutter off */
    digitalWrite(LED, LOW);     /*  LED off */

    return;
}
# endif
#endif
