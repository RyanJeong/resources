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
     *  i, j        : 조건문에서 사용하는 제어 변수
     *  buf         : 디버깅 메세지를 만들기 위한 버퍼
     */
    static char *func = SENSOR_PREPARESENSOR1;
    size_t      i, j;
    char        buf[SIZE_BUF];

    putsForDebug(__SENSOR__, func, MSG_START);
    for (i = 0; i < PREPARE_TIME; ++i) {
        for (j = 1; j < SIZE_BACKUP; ++j) {
            *(backup + SIZE_BACKUP - j) = *(backup + SIZE_BACKUP - (j + 1));
        }
        ENABLE_ADC;
        *(backup)   = (float) analogRead(ACTUAL_SENSOR);
        DISABLE_ADC;
        sprintf(buf, "Sensor value: %f", *(backup));
        putsForDebug(__SENSOR__, func, buf);
        delay(1000);
    }
    index       = 0;
    threshold   = *(backup);
    pastValue   = *(backup + 1);
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
     *  i, j        : 조건문에서 사용하는 제어 변수
     */
    size_t      i, j;

    for (i = 0; i < PREPARE_TIME; ++i) {
        for (j = 1; j < SIZE_BACKUP; ++j) {
            *(backup + SIZE_BACKUP - j) = *(backup + SIZE_BACKUP - (j + 1));
        }
        ENABLE_ADC;
        *(backup)   = (float) analogRead(ACTUAL_SENSOR);
        DISABLE_ADC;
        delay(1000);
    }
    index       = 0;
    threshold   = *(backup);
    pastValue   = *(backup + 1);
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
    temp        = (float) analogRead(ACTUAL_SENSOR);
    DISABLE_ADC;
    filtered    = (SMOKE_ALPHA * temp) + (SMOKE_BETA * pastValue);  /*  노이즈 제거부 */
    if (ABS(filtered - threshold) > NOISE_THRESHOLD) {  /*  noise 입력 제거 */
        *(backup)   = threshold;
    } else {    /*  smoke voltage 평균  */
        *(backup)   = filtered;
        threshold   += ((filtered - *(backup + SIZE_BACKUP - 1)) / SIZE_BACKUP);
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
        averageValue    = (size_t) (temp / (float) SIZE_DATA);
        thingState      = SEND_A_AVERAGE_DATA;
    }
    if ((filtered - threshold) > SMOKE_DETECTION_VOLTAGE) {
        (countFireDetected >= LAPSE) ? (isWarning = true) : ++countFireDetected;
    } else {
        if (countFireDetected) {
            --countFireDetected;
        }
    }
    sprintf(buf, "Index: %u, sensor value: %f, count of fire detection: %u", index, filtered, countFireDetected);
    putsForDebug(__SENSOR__, func, buf);
    if (isWarning) {
        if (countFireDetected) {
            /*  화재 시 현재 측정 값을 전송하는 것이 우선   */
            if (thingState == SEND_A_AVERAGE_DATA) {
                index = (index == 0) ? (SIZE_DATA - 1) : (index - 1);
            }
            thingState = NOTIFY_FIRE_DETECTED;  
            currValue   = (size_t) *(data + index);
        } else {
            isWarning   = false;
        }
        putsForDebug(__SENSOR__, func, MSG_FIRE);
    }

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
    temp        = (float) analogRead(ACTUAL_SENSOR);
    DISABLE_ADC;
    filtered    = (SMOKE_ALPHA * temp) + (SMOKE_BETA * pastValue);  /*  노이즈 제거부 */
    if (ABS(filtered - threshold) > NOISE_THRESHOLD) {  /*  noise 입력 제거 */
        *(backup)   = threshold;
    } else {    /*  smoke voltage 평균  */
        *(backup)   = filtered;
        threshold   += ((filtered - *(backup + SIZE_BACKUP - 1)) / SIZE_BACKUP);
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
        averageValue    = (size_t) (temp / (float) SIZE_DATA);
        thingState      = SEND_A_AVERAGE_DATA;
    }
    if ((filtered - threshold) > SMOKE_DETECTION_VOLTAGE) {
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
            currValue   = (size_t) *(data + index);
        } else {
            isWarning   = false;
        }
    }

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
     *  sensor      : 센서로부터 얻은 값
     *
     *  currValue, currTestValue 자리에 0 또는 1 값을 넣어 4가지 형태로 구분하는 역할로 사용
     *  실패 시: FAILED_SMOKE, 00, 01, 10, 11
     *  성공 시: SUCCESS, xx(don't care)
     */
    static char *func = SENSOR_TESTING1;
    float       sensor;

    putsForDebug(__SENSOR__, func, MSG_START);
    testResult = FAILED_SMOKE;
    ENABLE_ADC;
    delay(500);                     /*  500 (ms), to stabilize the thing   */
    digitalWrite(SHUTTER, LOW);     /*  shutter off */
    digitalWrite(LED, LOW);         /*  LED off */
    sensor = (float) analogRead(ACTUAL_SENSOR);
    if (sensor > TEST_OFF_THRESHOLD) {  /*  ... | 0 | 0 | ...   */
        putsForDebug(__SENSOR__, func, MSG_SMOKE_TEST_FAILED1);

        return;
    };

    delay(500);                     /*  500 (ms), to stabilize the thing   */
    digitalWrite(SHUTTER, LOW);     /*  shutter off */
    digitalWrite(LED, HIGH);        /*  LED on */
    sensor = (float) analogRead(ACTUAL_SENSOR);
    if (sensor > TEST_OFF_THRESHOLD) {  /*  ... | 0 | 1 | ...   */
        digitalWrite(LED, LOW);     /*  LED off */
        putsForDebug(__SENSOR__, func, MSG_SMOKE_TEST_FAILED2);

        return;
    };

    delay(500);                     /*  500 (ms), to stabilize the thing   */
    digitalWrite(SHUTTER, HIGH);    /*  shutter on */
    digitalWrite(LED, LOW);         /*  LED off */
    sensor = (float) analogRead(ACTUAL_SENSOR);
    if (sensor > TEST_ON_THRESHOLD) {  /*  ... | 1 | 0 | ...   */
        digitalWrite(SHUTTER, LOW); /*  shutter off */
        putsForDebug(__SENSOR__, func, MSG_SMOKE_TEST_FAILED3);

        return;
    };

    delay(500);                     /*  500 (ms), to stabilize the thing   */
    digitalWrite(SHUTTER, HIGH);    /*  shutter on */
    digitalWrite(LED, HIGH);        /*  LED on */
    sensor = (float) analogRead(ACTUAL_SENSOR);
    if (sensor < TEST_ON_THRESHOLD) {  /*  ... | 1 | 1 | ...   */
        digitalWrite(SHUTTER, LOW); /*  shutter off */
        digitalWrite(LED, LOW);     /*  LED off */
        putsForDebug(__SENSOR__, func, MSG_SMOKE_TEST_FAILED4);

        return;
    };
    DISABLE_ADC;
    digitalWrite(SHUTTER, LOW); /*  shutter off */
    digitalWrite(LED, LOW);     /*  LED off */
    testResult = SUCCESS;
    putsForDebug(__SENSOR__, func, MSG_TEST_SUCCESS);

    return;
}

#else
void testing2(void)
{
    /*
     *  sensor      : 센서로부터 얻은 값
     */
    float       sensor;

    testResult = FAILED_SMOKE;
    ENABLE_ADC;
    delay(500);                     /*  500 (ms), to stabilize the thing   */
    digitalWrite(SHUTTER, LOW);     /*  shutter off */
    digitalWrite(LED, LOW);         /*  LED off */
    sensor = (float) analogRead(ACTUAL_SENSOR);
    if (sensor > TEST_OFF_THRESHOLD) {  /*  ... | 0 | 0 | ...   */

        return;
    };

    delay(500);                     /*  500 (ms), to stabilize the thing   */
    digitalWrite(SHUTTER, LOW);     /*  shutter off */
    digitalWrite(LED, HIGH);        /*  LED on */
    sensor = (float) analogRead(ACTUAL_SENSOR);
    if (sensor > TEST_OFF_THRESHOLD) {  /*  ... | 0 | 1 | ...   */
        digitalWrite(LED, LOW);     /*  LED off */

        return;
    };

    delay(500);                     /*  500 (ms), to stabilize the thing   */
    digitalWrite(SHUTTER, HIGH);    /*  shutter on */
    digitalWrite(LED, LOW);         /*  LED off */
    sensor = (float) analogRead(ACTUAL_SENSOR);
    if (sensor > TEST_ON_THRESHOLD) {  /*  ... | 1 | 0 | ...   */
        digitalWrite(SHUTTER, LOW); /*  shutter off */

        return;
    };

    delay(500);                     /*  500 (ms), to stabilize the thing   */
    digitalWrite(SHUTTER, HIGH);    /*  shutter on */
    digitalWrite(LED, HIGH);        /*  LED on */
    sensor = (float) analogRead(ACTUAL_SENSOR);
    if (sensor < TEST_ON_THRESHOLD) {  /*  ... | 1 | 1 | ...   */
        digitalWrite(SHUTTER, LOW); /*  shutter off */
        digitalWrite(LED, LOW);     /*  LED off */

        return;
    };
    DISABLE_ADC;
    digitalWrite(SHUTTER, LOW); /*  shutter off */
    digitalWrite(LED, LOW);     /*  LED off */
    testResult = SUCCESS;

    return;
}
#endif
