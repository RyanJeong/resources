#ifndef __SENSOR_H__
#define __SENSOR_H__
#define __SENSOR__              "[sensor.ino]"
#define SENSOR_PREPARESENSOR1   "[prepareSensor1()] "
#define SENSOR_TESTING1         "[testing1()] "
#define SENSOR_SENSING1         "[sensing1()] "

#define ABS(x)              (x * ((x > 0) - (x < 0)))

/*
 *  SIZE_DATA                       30                  :   buffer에 담을 데이터 갯수
 *  TESTING_SENSOR                  14                  :   테스트 센서 번호
 *  ACTUAL_SENSOR                   15                  :   감지 센서 번호
 *
 *  HEAT    -------------------------
 *  HEATING                         2                   :   Test 시 열을 가할 트랜지스터 번호
 *  HEAT_ALPHA                      (0.9)               :   Noise filter 계수   
 *  HEAT_BETA                       (1 - HEAT_ALHPA)    :   Noise filter 계수   
 *  Q1                              (0.00000029476)     :   y = (Q1 * x^3) + (Q2 * x^2) + (Q3 * x) + Q4
 *  Q2                              (-0.00045725)       :   y = (Q1 * x^3) + (Q2 * x^2) + (Q3 * x) + Q4
 *  Q3                              (0.3354)            :   y = (Q1 * x^3) + (Q2 * x^2) + (Q3 * x) + Q4
 *  Q4                              (-50.2612)          :   y = (Q1 * x^3) + (Q2 * x^2) + (Q3 * x) + Q4
 *  CURVE(n)                        ((Q1 * n * n * n) + (Q2 * n * n) + (Q3 * n) + Q4)
 *  SAMPLE_RATE                     1                   :   초 당 샘플링 수
 *  LAPSE                           (SAMPLE_RATE * 2)   :   화재 지속 판별 유보 시간
 *  PREPARE_TIME                    (SAMPLE_RATE * 60)  :   초기 전원 인가 시 화재 비감지 시간, 30초
 *  TEST_NORMAL_TIME                10                  :   테스트 1단계 수행 횟수
 *  TEST_HEATING_TIME               2                   :   테스트 2단계 수행 횟수
 *  THRESHOLD                       60                  :   Fire alarm temp.[C]
 *  TEST_NORMAL_THRESHOLD           20                  :   This value represents 2 degrees at actual temperature
 *  TEST_HEATING_THRESHOLD          40                  :   This value represents 4 degrees at actual temperature
 *  INIT_SENSOR_TEMPERATURE         20                  :   초기 센서 온도 설정
 *  TEMP_DIFF                       5                   :   Testing 시 온도 차
 *
 *  SMOKE   -------------------------
 *  SHUTTER                         2                   :   Test 시 셔터를 사용할 번호
 *  SMOKE_ALPHA                     (0.5)               :   Noise filter 계수   
 *  SMOKE_BETA                      (1 - SMOKE_ALHPA)   :   Noise filter 계수   
 *  SMOKE_DETECTION_VOLTAGE         0.025               :   Smoke voltage difference detection
 *  SIZE_BACKUP                     30                  :   Size of backup data
 *  NOISE_THRESHOLD                 0.02                :   For average voltage reference
 *  TEST_ON_THRESHOLD               1.0                 :   Test, 1.0 voltage
 *  TEST_OFF_THRESHOLD              0.2                 :   Test, 0.2 voltage
 *  
 */
#define SIZE_TEMP               6

#define SIZE_DATA               30
#define TESTING_SENSOR          14
#define ACTUAL_SENSOR           15

#define HEATING                 2
#define HEAT_ALPHA              (0.9)
#define HEAT_BETA               (1 - HEAT_ALPHA)
#define Q1                      (0.00000029476)
#define Q2                      (-0.00045725)
#define Q3                      (0.3354)
#define Q4                      (-50.2612)
#define CURVE(n)                ((Q1 * n * n * n) + (Q2 * n * n) + (Q3 * n) + Q4)
#define SAMPLE_RATE             1
#define LAPSE                   (SAMPLE_RATE * 2)
#define PREPARE_TIME            (SAMPLE_RATE * 60)
#define TEST_NORMAL_TIME        10
#define TEST_HEATING_TIME       2
#define THRESHOLD               60
#define TEST_NORMAL_THRESHOLD   20
#define TEST_HEATING_THRESHOLD  40
#define INIT_SENSOR_TEMPERATURE 20
#define TEMP_DIFF               5

#define SHUTTER                 2
#define SMOKE_ALPHA             (0.5)
#define SMOKE_BETA              (1 - SMOKE_ALPHA)
#define SMOKE_DETECTION_VOLTAGE 0.025
#define SIZE_BACKUP             30
#define NOISE_THRESHOLD         0.02
#define TEST_ON_THRESHOLD       1.0
#define TEST_OFF_THRESHOLD      0.2

#define RESERVE_TIME            4   /* if normal state, four breaks and one sensing */

/*  On ATmega based boards, it takes about 100 microseconds to read an analog input, so the maximum reading rate is about 10,000 times a second.    */
#define ENABLE_ADC          \
do {                        \
    ADCSRA  |= (1 << ADEN); \
    delay(1);               \
} while (0)                 \

#define DISABLE_ADC         \
do {                        \
    ADCSRA  = 0;            \
} while (0)                 \

/*
 *  prepareSensor(void)
 *  --------------------
 *  동작하기 전 초기화 과정을 수행하는 함수
 */
void prepareSensor(void);

/*
 *  prepareSensor1(void)
 *  --------------------
 *  동작하기 전 초기화 과정을 수행하는 함수
 *  SOFTWARESERIAL_MODE가 정의되었을 때 사용되는 함수
 */
void prepareSensor1(void);

/*
 *  prepareSensor2(void)
 *  --------------------
 *  동작하기 전 초기화 과정을 수행하는 함수
 *  SOFTWARESERIAL_MODE가 정의되지 않았을 때 사용되는 함수
 */
void prepareSensor2(void);

/*
 *  sensing(void)
 *  --------------------
 *  주기적으로 센서로부터 값을 읽어오는 함수
 */
void sensing(void);

/*
 *  sensing1(void)
 *  --------------------
 *  주기적으로 센서로부터 값을 읽어오는 함수
 *  SOFTWARESERIAL_MODE가 정의되었을 때 사용되는 함수
 */
void sensing1(void);

/*
 *  sensing2(void)
 *  --------------------
 *  주기적으로 센서로부터 값을 읽어오는 함수
 *  SOFTWARESERIAL_MODE가 정의되지 않았을 때 사용되는 함수
 */
void sensing2(void);

/*
 *  testing(void);
 *  --------------------
 *  Raspberry Pi로부터 테스트 명령을 수신받아 수행하는 함수
 */
void testing(void);

/*
 *  testing1(void);
 *  --------------------
 *  Raspberry Pi로부터 테스트 명령을 수신받아 수행하는 함수
 *  SOFTWARESERIAL_MODE가 정의되었을 때 사용되는 함수
 */
void testing1(void);

/*
 *  testing2(void);
 *  --------------------
 *  Raspberry Pi로부터 테스트 명령을 수신받아 수행하는 함수
 *  SOFTWARESERIAL_MODE가 정의되지 않았을 때 사용되는 함수
 */
void testing2(void);
#endif
