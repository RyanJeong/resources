#ifndef __UTIL_H__
#define __UTIL_H__

#define __UTIL__            "[util.ino]"
#define UTIL_GETSERIALID1   "[getSerialId1()] "

/*
 *  getSerialId()
 *  --------------------
 *   get serial ID from ATmega328P
 */
void getSerialId();

/*
 *  getSerialId1()
 *  --------------------
 *   get serial ID from ATmega328P when use SoftwareSerial library
 */
void getSerialId1();

/*
 *  getSerialId2();
 *  --------------------
 *   get serial ID from ATmega328P when not use SoftwareSerial library
 */
void getSerialId2();

/*
 * void turnOnLED(void)
 * ----------------
 * : Turn on the LED when user requests it.
 */

void turnOnLED(void);
/*
 * void putsForDebug(const char *, const char *, char *);
 * --------------------
 *  puts debug message on the serial monitor
 */
void putsForDebug(const char *, const char *, char *);

#endif

