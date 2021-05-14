#ifndef __KEM_H__
#define __KEM_H__

/*
 *  TEST_COMM                           :   Basic RF communication
 *  TEST_COMM_FUNCTIONAL(DEPRECATED)    :   Basic RF communication and running according to flow defined in the program
 *  COMM                                :   RF Communication with CRC checker and running according to flow defined in the program
 *  TEST_SERIAL_COMM                    :   Basic RF communication without SoftwareSerial
 *  SERIAL_COMM                         :   RF Communication with CRC checker and running according to flow defined in the program without SoftwareSerial(final step)
 *
 *  Naming rules
 *  *A(): TEST_COMM
 *  *B(): COMM
 *  *C(): TEST_SERIAL_COMM
 *  *D(): SERIAL_COMM
 *  -----------------------
 *  *1(): SOFTWARESERIAL_MODE
 *  *2(): !SOFTWARESERIAL_MODE
 */

//#define   TEST_COMM
#define COMM
//#define   TEST_SERIAL_COMM
//#define   SERIAL_COMM

/*
 *  DO NOT REMOVE A BELOW SENTENCE
 *  
 *  #define HEAT_MODE:  A thing with heat sensor
 *  //define HEAT_MODE: A thing with smoke sensor
 */
#define HEAT_MODE

#ifndef TEST_SERIAL_COMM
# ifndef SERIAL_COMM
#  define SOFTWARESERIAL_MODE
# endif
#endif

#define TEST_ID         33
#define TEST_CHANNEL    0

/*  GPIO MAPPING DATA   */
#define CHANNEL_0       3
#define CHANNEL_1       4
#define ID_0            5
#define ID_1            6
#define ID_2            7
#define ID_3            8
#define ID_4            9
#define ID_5            10
#define STATUS0         A2  /*  LED 1   */
#define RELAY           A3  /*  RELAY   */
#define STATUS1         A4  /*  LED 2   */
#define BUZZER          A5  /*  warning sound   */
#define VCC_CHECK       A6  /*  analog voltage in   */

#define OK              0
#define NO              1
#define INIT            1
#define ERROR           1

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

/*  Apparently, the chip Atmega328p have a hidden serial number with 9 bytes, and others AVR Microcontroller maybe too  */
#define SIZE_ID     9
#define SIZE_BUF    64
#define SIZE        64  

#endif
