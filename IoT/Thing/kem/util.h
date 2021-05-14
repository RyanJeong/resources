#ifndef __UTIL_H__
#define __UTIL_H__

#define __UTIL__ "[util.ino]"

#define VOLTAGE_THRESHOLD       5.0

#define STATE_NORMAL            00    
#define STATE_PREPARE           01
#define STATE_XXG               01
#define STATE_FULLED_BUFFER     02
#define STATE_XRX               02
#define STATE_CHECK_BATTERY     03
#define STATE_XRG               03
#define STATE_RECEIVED_TEST     04
#define STATE_BXX               04
#define STATE_TESTING_PASSED    05
#define STATE_BXG               05
#define STATE_TESTING_FAILED    06
#define STATE_BRX               06
#define STATE_FIRE              07
#define STATE_BRG               07

#define RELAY_ON    digitalWrite(RELAY, HIGH)
#define RELAY_OFF   digitalWrite(RELAY, LOW)

#define SHUT_DOWN                   \
do {                                \
    digitalWrite(STATUS1, HIGH);    \
    digitalWrite(STATUS0, HIGH);    \
    exit(0);                        \
} while (0)

/*
 *  void getSerialId(void);
 *  --------------------
 *   get serial ID from ATmega328P
 */
void getSerialId(void);

/*
 *  void checkBattery(void);
 *  --------------------
 *  Check the battery voltage
 */
void checkBattery(void);

/*
 *  void handleBuzzerAndLED(uint8_t);
 *  --------------------
 *  handle Buzzer and LEDs
 *  Buzzer  LED1    LED2    STATE
 *  0       0       0       Normal
 *  0       0       1       Prepare
 *  0       1       0       Fulled buffer
 *  0       1       1       Check battery
 *  1       0       0       Received test operation
 *  1       0       1       Testing passed
 *  1       1       0       Testing failed
 *  1       1       1       !!!Fire!!!
 */
void handleBuzzerAndLED(uint8_t);

/*
 *  [alt] void putsForDebug(const char *, const char *, const char *);
 *  void putsForDebug(const char *, const char *, int);
 *  --------------------
 *  puts debug message on the serial monitor
 *  IFDEF RELEASE, this function will be not working
 */
void putsForDebug(const char *, const char *, const char *);

#endif
