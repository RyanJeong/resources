#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

/*
 *  WDTCSR = (N);
 *       0:  16 ms
 *       1:  32 ms
 *       2:  64 ms
 *       3: 125 ms
 *       4: 250 ms
 *       5: 500 ms
 *       6:   1 s
 *       7:   2 s
 *      32:   4 s
 *      33:   8 s
 */
#define MILSEC_125      3
#define MILSEC_500      5
#define SEC_1           6
#define SEC_4           32
#define SEC_8           33
#define SEC_60          (-1)

#define ENABLE_WATCHDOG         \
do {                            \
    WDTCSR |= (1 << 6);         \
} while (0)

/*
 *  Do not use these macros
#define DISABLE_WATCHDOG        \
do {                            \
    WDTCSR &= ~(1 << 6);        \
} while (0)

#define WATCHDOG_TIMER_RESET        \
do {                                \
    __asm__ __volatile__("wdr");    \
} while (0)
*/

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

/*
 *  void sleep(uint8_t);
 *  : a function that perform sleep routine
 */
void sleep(uint8_t);

/*
 *  void enableSleep(void);
 *  --------------------
 *  : A function that prepare to use the watchdog and sleep function
 *  --------------------
 *  Perform only once
 *  use for wake-up mcu
 */
void enableSleep(void);
#endif
