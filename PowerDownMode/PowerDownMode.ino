
/*
 *  Arduino(ATmega328P) Power Down Mode Example
 *  Wake Up: INT0, Watchdog
 */
void setup() {
  /*
   *  Save Power by writing all Digital IO LOW 
   *  - note that pins just need to be tied one way or another, do not damage devices!
   */
  for (int i = 0; i < 20; i++) {
    pinMode(i, OUTPUT);
  }
  /*
   *  interrupt for waking up
   *  in Arduino UNO, INT0 -> Pin 2, INT1 -> Pin 3
   *  make a circuit at pin 2 which call INT0 by transition
   */
  attachInterrupt(0, digitalInterrupt, FALLING);
  /*
   *  SETUP WATCHDOG TIMER
   *  ------
   *  WDTCSR = (24);     : change enable and WDE - also resets
   *  WDTCSR = (33);     : prescalers only - get rid of the WDE and WDCE bit
   *      WDP3 - WDP2 - WPD1 - WDP0 - time
   *      0      0      0      0      16 ms
   *      0      0      0      1      32 ms
   *      0      0      1      0      64 ms
   *      0      0      1      1      0.125 s
   *      0      1      0      0      0.25 s
   *      0      1      0      1      0.5 s
   *      0      1      1      0      1.0 s
   *      0      1      1      1      2.0 s
   *      1      0      0      0      4.0 s
   *      1      0      0      1      8.0 s
   *      
   *  WDTCSR |= (1<<6);  : enable interrupt mode
   */
  WDTCSR  = (24);
  WDTCSR  = (33);
  WDTCSR  |= (1<<6);
  /*  
   *   Disable ADC 
   *  - don't forget to flip back after waking up if using ADC in your application ADCSRA |= (1 << 7);
   */
  ADCSRA &= ~(1 << 7);
  
  /*  
   *  ENABLE SLEEP 
   *  - this enables the sleep mode
   *  ------
   *  SMCR |= (1 << 2); : power down mode
   *  SMCR |= 1;        : enable sleep
   */
  SMCR |= (1 << 2);
  SMCR |= 1;
}

void loop() {

  digitalWrite(LED_PIN, HIGH);
  delay(1000);
  digitalWrite(LED_PIN, LOW);

  /*
   *  gives ~16 sec.
   *  Watchdog(8 sec.) x 2
   */
  for (int i = 0; i < 2; ++i) {
    /*
     *  BOD DISABLE 
     *  - this must be called right before the __asm__ sleep instruction
     *  ------
     *  MCUCR |= (3 << 5);                      : set both BODS and BODSE at the same time
     *  MCUCR = (MCUCR & ~(1 << 5)) | (1 << 6); : then set the BODS bit and clear the BODSE bit at the same time
     *  __asm__  __volatile__("sleep");         : in line assembler to go to sleep
     */
    MCUCR |= (3 << 5);
    MCUCR = (MCUCR & ~(1 << 5)) | (1 << 6);
    __asm__  __volatile__("sleep");
  }
}

/*
 *  needed for the digital input interrupt
 */
void digitalInterrupt() 
{
  
}

/*
 *  Needed for the watch dog timer
 *  This is called after a watch dog timer timeout
 *  - this is the interrupt function called after waking up
 */
ISR(WDT_vect) 
{
 
}
