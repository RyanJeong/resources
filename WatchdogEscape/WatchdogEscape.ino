/*
 *  Arduino(ATmega328P) Escape Watchdog timer example
 */

 #define wdt_reset()  __asm__ __volatile__("wdr")
 
void setup() {
  Serial.begin(9600);
  
  /*
   *  SETUP WATCHDOG TIMER
   *  ------
   *  WDTCSR
   *  WDF | WDIE | WDP3 | WDCE | WDE | WDP2 | WDP1 | WDP0
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
  WDTCSR  = (6);
  
  WDTCSR  |= (1 << 6);
}

void loop() {
  if (true) {
    //  have to clear watchdog timer register
    wdt_reset();
    Serial.println("true routine");
  } else {    //  A serious problem, such as deadlock, has occurred.
    /*  looping ... */
  }
  delay(500);
}

//  Watchdog Timer
//  If the program has not a problem, do not allow access to this routine
//  If the program has a problem such as deadlock, then access this routine 
//  
ISR(WDT_vect) 
{
  Serial.println("Watchdog");
}
