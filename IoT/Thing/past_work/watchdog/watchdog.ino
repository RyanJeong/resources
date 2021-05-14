void setup()
{
    Serial.begin(9600);
    /*  Save Power by writing all Digital IO LOW - note that pins just need to be tied one way or another, do not damage devices!   */
    for (int i = 0; i < 20; i++) {
        pinMode(i, OUTPUT);
    }

    /*  Disable ADC - don't forget to flip back after waking up if using ADC in your application ADCSRA |= (1 << 7);    */
    ADCSRA &= ~(1 << 7);

    /*  ENABLE SLEEP - this enables the sleep mode  */
    /*  1. power down mode */
    SMCR |= (1 << 2); 
    /*  2. enable sleep    */
    SMCR |= 1;
}

void loop()
{
    /*  change enable and WDE - also resets, Tick! */
    WDTCSR = (24);
    /*  
     *  Perform only once, Tick!
     *
     *  use for wake-up mcu
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
    WDTCSR = (5);
    /*  enable interrupt mode, Tick!   */
    WDTCSR |= (1 << 6);
    Serial.println("500 mil 1 time");
    Serial.flush();
    gotosleep(1);
    Serial.println("500 mil 2 times");
    Serial.flush();
    gotosleep(2);
    WDTCSR = (24);
    WDTCSR = 7;
    /*  enable interrupt mode   */
    WDTCSR |= (1 << 6);
    Serial.println("2 sec 1 time");
    Serial.flush();
    gotosleep(1);
    Serial.println("2 sec 2 times");
    Serial.flush();
    gotosleep(2);
}

void gotosleep(int i)
{
    while (i > 0) {
        /*  BOD DISABLE - this must be called right before the __asm__ sleep instruction    */
        /*  1. set both BODS and BODSE at the same time    */
        MCUCR |= (3 << 5);
        /*  2. then set the BODS bit and clear the BODSE bit at the same time  */
        MCUCR = (MCUCR & ~(1 << 5)) | (1 << 6);
        /*  3. in line assembler to go to sleep    */ 
        __asm__  __volatile__("sleep");
        --i;
    }

    return;
}

/*  watchdog interrupt  */
ISR(WDT_vect)
{
    /*  DON'T FORGET THIS!  Needed for the watch dog timer.  This is called after a watch dog timer timeout - this is the interrupt function called after waking up */
}
