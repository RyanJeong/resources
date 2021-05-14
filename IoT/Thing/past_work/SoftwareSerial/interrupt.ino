#include "kem.h"
#include "interrupt.h"

void clearInterrupt(void)
{
#ifndef SOFTWARESERIAL_MODE
    cli();
#endif

    return;
}

void setInterrupt(void)
{
#ifndef SOFTWARESERIAL_MODE
    sei();
#endif

    return;
}

void enableSleep(void)
{
#ifndef SOFTWARESERIAL_MODE
    ENABLE_SLEEP;
#endif

    return;
}

void sleep(void)
{
#ifndef SOFTWARESERIAL_MODE
    SLEEP;
#else
    delay(2000);
#endif

    return;
}

void setWatchdog(void)
{
#ifndef SOFTWARESERIAL_MODE
    /*  change enable and WDE - also resets */
    WDTCSR  = (24);
    /*  prescalers only - get rid of the WDE and WDCE bit */
    WDTCSR  = (7);      
    /*  enable watchdog timer   */
    ENABLE_WATCHDOG;    
#endif

    return;
}

/*  when the thing not use SoftwareSerial   */
#ifndef SOFTWARESERIAL_MODE
ISR(WDT_vect)
{
    /*  
     *  Watchdog Timer
     *  Enable BOD
     *      The BODS bit is automatically cleared after three clock cycles.
     *      Writing this bit to one turns off the BOD in relevant sleep modes, while a zero in this bit keeps
     *      BOD active. Default setting keeps BOD active, i.e. BODS set to zero.
     */  
}
#endif
