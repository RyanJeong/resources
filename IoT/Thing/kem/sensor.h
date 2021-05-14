#ifndef __SENSOR_H__
#define __SENSOR_H__
#define __SENSOR__  "[sensor.ino]"

#define ABS(x)      (x * ((x > 0) - (x < 0)))

/*
 *  SIZE_DATA                       30                  :   buffer에 담을 데이터 갯수
 *  TESTING_SENSOR                  14                  :   테스트 센서 번호
 *  ACTUAL_SENSOR                   15                  :   감지 센서 번호
 *
 *  HEAT    -------------------------
 *	HEATING                 2
 *	HEAT_ALPHA              (0.7)
 *	HEAT_BETA               (1.0 - HEAT_ALPHA)
 *	Q1                      (0.00000029476)
 *	Q2                      (-0.00045725)
 *	Q3                      (0.3354)
 *	Q4                      (-50.2612)
 *	CURVE(n)                ((Q1 * n * n * n) + (Q2 * n * n) + (Q3 * n) + Q4)
 *	LAPSE                   2   : 화재 지속 판정 유지
 *	PREPARE_TIME            60  : 센서 초기화 시간(60초)
 *	THRESHOLD               60  : 화재 판정 온도, 이 값보다 크다면 화재
 *	TEST_NORMAL_THRESHOLD   3.0 : 평소 두 센서의 온도 차이 기준(이 값 이상 차이나면 이상값)
 *	TEST_HEATING_THRESHOLD  45.0: 테스트 시 과열 판단 온도, 이 값보다 크면, 센서가 잘 작동된다고 간주하고 테스트 종료
 *  TEST_HEATING_DIFF       0.5 : 열 가할 때 최소 온도 상승값, 이 값보다 작으면 센서 고장으로 판단
 *	INIT_SENSOR_TEMPERATURE 25  : 초기 상태 온도
 *  SMOKE   -------------------------
 *
 */


#ifndef RELEASE
 #define SIZE_DATA              3
#else
 #define SIZE_DATA              30
#endif
#define ACTUAL_SENSOR           14
#define TESTING_SENSOR          15

#define HEATING                 2
#define HEAT_ALPHA              (0.7)
#define HEAT_BETA               (1.0 - HEAT_ALPHA)
#define Q1                      (0.00000029476)
#define Q2                      (-0.00045725)
#define Q3                      (0.3354)
#define Q4                      (-50.2612)
#define CURVE(n)                ((Q1 * n * n * n) + (Q2 * n * n) + (Q3 * n) + Q4)
#define LAPSE                   2
#ifdef TEST_SENSOR
 #define PREPARE_TIME           5
 #define THRESHOLD              35
#elif defined TEST_RF
 #define PREPARE_TIME           1
 #define THRESHOLD              60
#elif (defined BETA) || (defined RELEASE_CANDIDATE)
 #define PREPARE_TIME           3
 #define THRESHOLD              60
#else
 #define PREPARE_TIME           60
 #define THRESHOLD              60
#endif
#define TEST_NORMAL_THRESHOLD   3.0
#define TEST_HEATING_THRESHOLD  45.0
#define TEST_HEATING_DIFF       0.5
#define INIT_SENSOR_TEMPERATURE 25

/*
#define SHUTTER                 2
#define SMOKE_ALPHA             (0.5)
#define SMOKE_BETA              (1 - SMOKE_ALPHA)
#define SMOKE_DETECTION_VOLTAGE 0.025
#define SIZE_BACKUP             30
#define NOISE_THRESHOLD         0.02
#define TEST_ON_THRESHOLD       1.0
#define TEST_OFF_THRESHOLD      0.2
*/

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
 *  void testSensor(void)
 *  --------------------
 *  테스트 단계에서 수행하는 함수
 */
void testSensor(void);

/*
 *  float getValue(uint8_t);
 *  --------------------
 *  값의 noise를 줄이기 위해 analogRead() 10번에 대한 평균값을 반환하는 함수
 */
float getValue(uint8_t);

/*
 *  sensing(void)
 *  --------------------
 *  주기적으로 센서로부터 값을 읽어오는 함수
 *  모든 동작을 마친 후 1초 sleep
 */
void sensing(void);

/*
 *  testing(void);
 *  --------------------
 *  Raspberry Pi로부터 테스트 명령을 수신받아 수행하는 함수
 *  모든 동작을 마친 후 1초 sleep
 */
void testing(void);
#endif
