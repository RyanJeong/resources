#ifndef __INIT_H__
#define __INIT_H__

#define __INIT__                    "[init.ino]"
#define MAX_ID                      64
#define MAX_CHANNEL                 4

/*  GPIO MAPPING DATA   */
#define CHANNEL_0       3
#define CHANNEL_1       4
#define ID_0            5
#define ID_1            6
#define ID_2            7
#define ID_3            8
#define ID_4            9
#define ID_5            10

/*
 * uint8_t initThing(void);
 *  --------------------
 *   1. setup RF module
 *   2. set pin mode
 *   3. set channel
 *   4. set ID
 *   5. get serial-id
 *   6. clear interrupt for settings
 *   7. enable sleep
 *   8. set interrupt
 *   9. check ID and channel if they set invalid value
 *  10. check CRC is running correctly
 *  11. check battery voltage
 *  12. prepare sensor to use
 *  13. set CRC value
 *  14. set 'thingState' and 'cmdResult'
 *  --------------------
 *  thingState
 *  --------------------
 *  NORMAL
 *  SEND_TEST_RESULT        
 *  NOTIFY_FIRE_DETECTED    
 *  RECEIVED_TEST_COMMAND   
 *  SEND_A_AVERAGE_DATA     
 *  REQUEST_CONNECTION      
 *  REQUEST_CONNECTION3     
 *  REQUEST_CONNECTION6     
 */
uint8_t initThing(void);

/*
 * void setPinMode(void);
 *  Set pin mode for the serial mode
 *  --------------------
 *  pinMode(0, INPUT);
 *  pinMode(1, OUTPUT);
 *  pinMode(2, OUTPUT);
 *  pinMode(3, INPUT);
 *  pinMode(4, INPUT);
 *  pinMode(5, INPUT);
 *  pinMode(6, INPUT);
 *  pinMode(7, INPUT);
 *  pinMode(8, INPUT);
 *  pinMode(9, INPUT);
 *  pinMode(10, INPUT);
 *  pinMode(11, OUTPUT);
 *  pinMode(12, OUTPUT);
 *  pinMode(13, OUTPUT);
 *  pinMode(14, INPUT);
 *  pinMode(15, INPUT);
 *  pinMode(16, OUTPUT);
 *  pinMode(17, OUTPUT);
 *  pinMode(18, OUTPUT);
 *  pinMode(19, OUTPUT);
 *  pinMode(20, INPUT);
 */
void setPinMode(void);

/*
 *  void setID(void);
 *  : Set ID using the button
 *  : Range: 0 ~ 63
 *  +--+--+--+--+--+--+
 *  | 5| 4| 3| 2| 1| 0|
 *  +--+--+--+--+--+--+
 *  |10| 9| 8| 7| 6| 5|
 *  +--+--+--+--+--+--+
 */
void setID(void);

/*
 *  void setChannel(void);
 *  : Set channel using the button
 *  : Range: 0 ~ 3
 *  +--+--+
 *  | 1| 0|
 *  +--+--+
 *  | 4| 3|
 *  +--+--+
 */
void setChannel(void);

/*
 *  uint8_t IDAndChannelIsInvalid(void);
 *  : check ID and channel is valid value
 */
uint8_t IDAndChannelIsInvalid(void);
#endif
