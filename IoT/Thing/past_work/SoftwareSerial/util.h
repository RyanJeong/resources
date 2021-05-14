#ifndef __UTIL_H__
#define __UTIL_H__

/*
 *  getSerialId()
 *  --------------------
 *   get serial ID from ATmega328P
 */
void getSerialId();

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

