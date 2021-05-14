#ifndef __INIT_H__
#define __INIT_H__

#define __INIT__                    "[init.ino]"
#define INIT_SETID                  "[setID()] "
#define INIT_SETCHANNEL             "[setChannel()] "
#define INIT_IDANDCHANNELISINVALID1 "[IDAndChannelIsInvalid1()] "
#define MAX_ID          64
#define MAX_CHANNEL     4

/*
 * uint8_t initThing(void);
 *  --------------------
 *   1. set pin mode
 *   2. set channel
 *   3. set ID
 *   4. setup RF module
 *   5. get serial-id
 *   6. clear interrupt for settings
 *   7. set watchdog and enable it
 *   8. enable sleep
 *   9. set interrupt
 *  10. set 'thingState'
 *  11. set 'cmdResult'
 *  12. check id and channel if they set invalid value
 *  13. check CRC is running correctly
 *  --------------------
 *  thingState
 *  
 *  NORMAL;
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
 * : Set pin mode 
 */
void setPinMode(void);

/*
 * void setPinMode1(void);
 *  --------------------
 *  Set pin mode for SoftwareSerial mode
 *  --------------------
 *  pinMode(0, INPUT);      //  RX
 *  pinMode(1, OUTPUT);     //  TX
 *  pinMode(2, OUTPUT);     //  HEAT_R
 *  pinMode(3, OUTPUT);     //  Not used
 *  pinMode(4, OUTPUT);     //  Not used
 *  pinMode(5, INPUT);      //  GREEN LED
 *  pinMode(6, INPUT);      //  YELLOW LED
 *  pinMode(7, INPUT);      //  RED LED
 *  pinMode(8, OUTPUT);     //  Not used
 *  pinMode(9, OUTPUT);     //  Not used
 *  pinMode(10, OUTPUT);    //  Not used
 *  pinMode(11, OUTPUT);    //  Not used
 *  pinMode(12, OUTPUT);    //  Not used
 *  pinMode(13, OUTPUT);    //  Not used
 *  pinMode(14, INPUT);     //  A0, ADC0, for test sensor
 *  pinMode(15, INPUT);     //  A1, ADC1, for measuring sensor
 *  pinMode(14, OUTPUT);    //  Not used
 *  pinMode(15, OUTPUT);    //  Not used
 *  pinMode(18, OUTPUT);    //  SoftwareSerial TX(twisted)
 *  pinMode(19, INPUT);     //  SoftwareSerial RX(twisted)
 */
void setPinMode1(void);

/*
 *  setPinMode2(void)
 *  --------------------
 *  Set pin mode for the serial mode
 *  --------------------
 *  pinMode(0, INPUT);      //  RX
 *  pinMode(1, OUTPUT);     //  TX
 *  pinMode(2, OUTPUT);     //  HEAT_R
 *  pinMode(3, INPUT);      //  Set Channel, bit 0
 *  pinMode(4, INPUT);      //  Set Channel, bit 1
 *  pinMode(5, INPUT);      //  Set ID, bit 0
 *  pinMode(6, INPUT);      //  Set ID, bit 1
 *  pinMode(7, INPUT);      //  Set ID, bit 2
 *  pinMode(8, INPUT);      //  Set ID, bit 3
 *  pinMode(9, INPUT);      //  Set ID, bit 4
 *  pinMode(10, INPUT);     //  Set ID, bit 5
 *  pinMode(11, OUTPUT);    //  Not used
 *  pinMode(12, OUTPUT);    //  Not used
 *  pinMode(13, OUTPUT);    //  Not used
 *  pinMode(14, INPUT);     //  A0, ADC0, for test sensor
 *  pinMode(15, INPUT);     //  A1, ADC1, for measuring sensor
 *  pinMode(16, OUTPUT);    //  LED for debugging
 *  pinMode(17, INPUT);     //  Set ID, bit 1
 *  pinMode(18, OUTPUT);    //  Not used
 *  pinMode(19, OUTPUT);    //  Not used
 */
void setPinMode2(void);

/*
 *  void setID(void);
 *  : Set ID using the button
 *  : Range: 0 ~ 63
 */
void setID(void);

/*
 * void setIDA(void);
 *  --------------------
 *  Set ID using the button when defined TEEST_COMMUNICATION
 */
void setIDA(void);

/*
 * void setID1(void);
 *  --------------------
 *  Set ID using the button when defined SOFTWARESERIAL_MODE
 */
void setID1(void);

/*
 * void setID2(void);
 *  --------------------
 *  Set ID using the button when not defined SOFTWARESERIAL_MODE
 *  +--+--+--+--+--+--+
 *  | 5| 4| 3| 2| 1| 0|
 *  +--+--+--+--+--+--+
 *  |10| 9| 8| 7| 6| 5|
 *  +--+--+--+--+--+--+
 */
void setID2(void);

/*
 *  void setChannel(void);
 *  : Set channel using the button
 *  : Range: 0 ~ 3
 */
void setChannel(void);

/*
 * void setChannelA(void);
 *  --------------------
 *  Set channel using the button when defined TEEST_COMMUNICATION
 */
void setChannelA(void);

/*
 *  setChannel1(void)
 *  --------------------
 *  Set channel using the button when defined SOFTWARESERIAL_MODE
 */
void setChannel1(void);

/*
 *  setChannel2(void)
 *  --------------------
 *  Set channel using the button when not defined SOFTWARESERIAL_MODE
 *  +--+--+
 *  | 1| 0|
 *  +--+--+
 *  | 4| 3|
 *  +--+--+
 */
void setChannel2(void);

/*
 * uint8_t IDAndChannelIsInvalid(void);
 *  : check ID and channel is valid value
 */
uint8_t IDAndChannelIsInvalid(void);

/*
 * uint8_t IDAndChannelIsInvalid1(void);
 *  : check ID and channel is valid value when defined SOFTWARESERIAL_MODE
 */
uint8_t IDAndChannelIsInvalid1(void);

/*
 * uint8_t IDAndChannelIsInvalid2(void);
 *  : check ID and channel is valid value when not defined SOFTWARESERIAL_MODE
 */
uint8_t IDAndChannelIsInvalid2(void);

#endif
