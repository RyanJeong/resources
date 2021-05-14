#ifndef __KEM_H__
#define __KEM_H__

/*
 *  TEST_COMMUNICATION      :   Basic RF communication
 *  TEST_COMM_FUNCTIONAL    :   Basic RF communication and running according to flow defined in the program
 *  COMM_FUNCTIONAL         :   RF Communication with CRC checker and running according to flow defined in the program
 *  TEST_SERIAL_COMM        :   Basic RF communication without SoftwareSerial
 *  SERIAL_COMM             :   RF Communication with CRC checker and running according to flow defined in the program without SoftwareSerial(final step)
 *
 *  Naming rules
 *  *A(): TEST_COMMUNICATION
 *  *B(): TEST_COMM_FUNCTIONAL
 *  *C(): COMM_FUNCTIONAL
 *  *D(): TEST_SERIAL_COMM
 *  *E(): SERIAL_COMM
 *  -----------------------
 *  *1(): SOFTWARESERIAL_MODE
 *  *2(): !SOFTWARESERIAL_MODE
 */

//#define   TEST_COMMUNICATION
//#define   TEST_COMM_FUNCTIONAL
#define COMM_FUNCTIONAL
//#define   TEST_SERIAL_COMM
//#define   SERIAL_COMM

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
#define LED             16

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
#define MSG_TEST1_FAILED        "Failed to Test1"
#define MSG_TEST2_FAILED        "Failed to Test2"
#define MSG_TEST_SUCCESS        "All tests passed"
#define MSG_CRC_TEST_FAILED     "Failed to CRC Test"
#define MSG_LED_ON              "Turn on the LED"

/*  Apparently, the chip Atmega328p have a hidden serial number with 9 bytes, and others AVR Microcontroller maybe too  */
#define SIZE_ID     9
#define SIZE_BUF    64
#define SIZE        64  

#endif
