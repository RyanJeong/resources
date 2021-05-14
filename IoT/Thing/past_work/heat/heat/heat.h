
#define NORMAL                  0x08
#define SEND_TEST_RESULT        0x18
#define NOTIFY_FIRE_DETECTED    0x28
#define INIT_TEST_COMMAND       0x38
#define SEND_A_AVERAGE_DATA     0x48
#define REQUEST_CONNECTION      0x58
#define RECEIVED_TEST_COMMAND   0x68
#define FAILED_TEST             0x78
#define CHECK_BATTERY_VOLTAGE   0x88

#define STATE_NORMAL            00    
#define STATE_PREPARE           01
#define STATE_FULLED_BUFFER     02
#define STATE_CHECK_BATTERY     03
#define STATE_RECEIVED_TEST     04
#define STATE_TESTING_PASSED    05
#define STATE_TESTING_FAILED    06
#define STATE_FIRE              07

#define RELAY_ON    digitalWrite(RELAY, HIGH)
#define RELAY_OFF   digitalWrite(RELAY, LOW)

#define DISABLE_RF              \
do {                            \
    Serial.print((char) 0xAB);  \
    Serial.print((char) 0x02);  \
    delay(1);                   \
} while (0)

#define ENABLE_RF               \
do {                            \
    Serial.print((char) 0xAB);  \
    Serial.print((char) 0x03);  \
} while (0)
#define TEST_SENSOR
#define MILSEC_125      3
#define MILSEC_500      5
#define SEC_1           6
#define SEC_4           32
#define SEC_8           33
#define SEC_60          (-1)

#define SIZE_ID     9
#define SIZE_BUF    96
#define SIZE        64  

#define STATUS0         A2  /*  LED 1   */
#define RELAY           A3  /*  RELAY   */
#define STATUS1         A4  /*  LED 2   */
#define BUZZER          A5  /*  warning sound   */
#define VCC_CHECK       A6  /*  analog voltage in   */

#define ABS(x)      (x * ((x > 0) - (x < 0)))

#define SIZE_TEMP               6

#define SIZE_DATA              30
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
#define PREPARE_TIME            3
#define THRESHOLD               60
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


#define HEAT_ERROR                          0x02
#define SMOKE_ERROR                         0x04

#define MSG_START               0
#define MSG_RF_OFF              1
#define MSG_GATEWAY_ERROR       2
#define MSG_GATEWAY_OK          3
#define MSG_NOT_RECEIVED        4
#define MSG_RECEIVED            5
#define MSG_CRC_ERROR           6
#define MSG_CRC_OK              7
#define MSG_INVALID_CMD         8
#define MSG_INVALID_ID          9
#define MSG_INVALID_CHANNEL     10
#define MSG_FIRE                11
#define MSG_TEST_PASSED         12
#define MSG_CRC_TEST_FAILED     13
#define MSG_LED_ON              14
#define MSG_SENSOR_OVERHEATING  15
#define MSG_LOW_BATTERY_VOLTAGE 16
#define __SENSOR__  "[sensor.ino]"

#define ABS(x)      (x * ((x > 0) - (x < 0)))

#define ENABLE_WATCHDOG         \
do {                            \
    WDTCSR |= (1 << 6);         \
} while (0)

#define DISABLE_WATCHDOG        \
do {                            \
    WDTCSR &= ~(1 << 6);        \
} while (0)

#define WATCHDOG_TIMER_RESET        \
do {                                \
    __asm__ __volatile__("wdr");    \
} while (0)

/*  ENABLE SLEEP - this enables the sleep mode  */
#define ENABLE_SLEEP                            \
do {                                            \
    SMCR |= (1 << 2);   /*  power down mode */  \
    SMCR |= 1;          /*  enable sleep  */    \
} while (0)

/*  
 *  SLEEP
 *  1. Set both BODS and BODSE at the same time
 *  2. Then set the BODS bit and clear the BODSE bit at the same time
 *  3. Call sleep operation
 */
#define SLEEP                                   \
do {                                            \
    MCUCR   |= (3 << 5);                        \
    MCUCR   = (MCUCR & ~(1 << 5)) | (1 << 6);   \
    __asm__  __volatile__("sleep");             \
} while (0)

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

