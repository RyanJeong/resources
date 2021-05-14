#ifndef __KEM_H__
#define __KEM_H__

#define HEAT_MODE

/* 
 *  DO NOT REMOVE A BELOW SENTENCE
 *
 *  0x00    TEST_SENSOR with Heat Sensor        : Alpha ver. Sensor test routine
 *  0x01    TEST_SENSOR with Smoke Sensor       : Alpha ver. Sensor test routine
 *  0x02    TEST_RF with Heat Sensor            : Alpha ver. RF Communication test routine
 *  0x03    TEST_RF with Smoke Sensor           : Alpha ver. RF Communication test routine
 *  0x04    RELEASE_CANDIDATE with Heat Sensor  : Release candidate            
 *  0x05    RELEASE_CANDIDATE with Smoke Sensor : Release candidate            
 *  0x06    RELEASE with Heat Sensor            : The program routine with release version
 *  0x07    RELEASE with Smoke Sensor           : The program routine with release version
 */

/*
 *  프로그램 구분 1. TEST A, 2. TEST B
 */

#define KEM_THING_MODE 0x02

#if (KEM_THING_MODE == 0x00)
 #define TEST_SENSOR
 #define ON_DEBUG_MSG
 #define HEAT_MODE
#elif (KEM_THING_MODE == 0x01)
 #define TEST_SENSOR
 #define ON_DEBUG_MSG
 #define SMOKE_MODE
#elif (KEM_THING_MODE == 0x02)
 #define TEST_RF
 #define ON_DEBUG_MSG
 #define HEAT_MODE
#elif (KEM_THING_MODE == 0x03)
 #define TEST_RF
 #define ON_DEBUG_MSG
 #define SMOKE_MODE
#elif (KEM_THING_MODE == 0x04)
 #define RELEASE_CANDIDATE
 #define ON_DEBUG_MSG
 #define HEAT_MODE
#elif (KEM_THING_MODE == 0x05)
 #define RELEASE_CANDIDATE
 #define ON_DEBUG_MSG
 #define SMOKE_MODE
#elif (KEM_THING_MODE == 0x06)
 #define RELEASE
 #define OFF_DEBUG_MSG
 #define HEAT_MODE
#elif (KEM_THING_MODE == 0x07)
 #define RELEASE
 #define OFF_DEBUG_MSG
 #define SMOKE_MODE
#else
/*  Destroy */
 #undef KEM_THING_MODE
#endif

/*  Types of return */
#define OK              0
#define NO              1

/*  Thing states    */
#define NORMAL          0
#define INIT            1
#define ERROR           2

/*  Alias of pins   */
#define STATUS0         A2  /*  LED 1   */
#define RELAY           A3  /*  RELAY   */
#define STATUS1         A4  /*  LED 2   */
#define BUZZER          A5  /*  warning sound   */
#define VCC_CHECK       A6  /*  analog voltage in   */

/*  Debug message   */
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
/*
#define MSG_LINE                "--------------------------------"
#define MSG_START               "Start"
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
#define MSG_TEST_PASSED         "All tests passed"
#define MSG_CRC_TEST_FAILED     "Failed to CRC Test"
#define MSG_LED_ON              "Turn on the LED"
#define MSG_SENSOR_OVERHEATING  "Sensor overheating"
#define MSG_LOW_BATTERY_VOLTAGE "Low battery voltage."
#define MSG_START               "0x00"
#define MSG_RF_OFF              "0x01"
#define MSG_GATEWAY_ERROR       "0x02"
#define MSG_GATEWAY_OK          "0x03"
#define MSG_NOT_RECEIVED        "0x04"
#define MSG_RECEIVED            "0x05"
#define MSG_CRC_ERROR           "0x06"
#define MSG_CRC_OK              "0x07"
#define MSG_INVALID_CMD         "0x08"
#define MSG_INVALID_ID          "0x09"
#define MSG_INVALID_CHANNEL     "0x0A"
#define MSG_FIRE                "0x0B"
#define MSG_TEST_PASSED         "0x0C"
#define MSG_CRC_TEST_FAILED     "0x0D"
#define MSG_LED_ON              "0x0E"
#define MSG_SENSOR_OVERHEATING  "0x0F"
#define MSG_LOW_BATTERY_VOLTAGE "0x10"
*/

/*  Apparently, the chip Atmega328p have a hidden serial number with 9 bytes, and others AVR Microcontroller maybe too  */
#define SIZE_ID     9
#define SIZE_BUF    96
#define SIZE        64  

#endif
