#include "kem.h"
#include "interrupt.h"

void sleep(int i)
{
    if (i == SEC_60) {
        /*  change enable and WDE - also resets */
        WDTCSR  = (24);
        /*  prescalers only - get rid of the WDE and WDCE bit */
        WDTCSR  = (33);      
        /*  enable watchdog timer   */
        ENABLE_WATCHDOG;    
        /*  8 sec * 7 times    */
        SLEEP;
        SLEEP;
        SLEEP;
        SLEEP;
        SLEEP;
        SLEEP;
        SLEEP;
    } else {
        /*  change enable and WDE - also resets */
        WDTCSR  = (24);
        /*  prescalers only - get rid of the WDE and WDCE bit */
        WDTCSR  = (i);      
        /*  enable watchdog timer   */
        ENABLE_WATCHDOG;    
        SLEEP;
    }

    return;
}

void enableSleep(void)
{
    /*  clear interrupts    */
    cli();
    /*  enable sleep    */
    ENABLE_SLEEP;
    /*  set interrupts  */
    sei();

    return;
}

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
