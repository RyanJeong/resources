static size_t i;
void setup() {
	Serial.begin(9600);
	Serial.println("setup()");
	Serial.flush(); /* for print content completely */

	cli();  /*  Clear interrupts  */

	/*  SETUP WATCHDOG TIMER
	 *  ------
	 *  WDTCSR = (24);      : change enable and WDE - also resets
	 *  WDTCSR = (6);       : prescalers only - get rid of the WDE and WDCE bit
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
	WDTCSR = (24);      /*  change enable and WDE - also resets */
	WDTCSR = (7);       /*  prescalers only - get rid of the WDE and WDCE bit */
	WDTCSR |= (1 << 6); /*  enable interrupt mode */

	/*  ENABLE SLEEP - this enables the sleep mode  */
	SMCR |= (1 << 2); /*  power down mode */
	SMCR |= 1;        /*  enable sleep  */
	sei();  /*  Set interrupts  */
	pinMode(LED_BUILTIN, OUTPUT);
	i = 0;
	Serial.println("Setup() finish");
}

void loop() {
	Serial.println("loop()");
	delay(1000);
	digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)

	/*  DISABLE BOD - this must be calledu right before the __asm__ sleep instruction  */
	MCUCR |= (3 << 5);                      /*  set both BODS and BODSE at the same time  */
	MCUCR = (MCUCR & ~(1 << 5)) | (1 << 6); /*  then set the BODS bit and clear the BODSE bit at the same time  */
	__asm__  __volatile__("sleep");         /*  in line assembler to go to sleep  */
	Serial.println("loop() finish");
}

/*
 *  Watchdog Timer(1 second period)
 *  Needed for the watch dog timer.
 *  This is called after a watch dog timer timeout
 *  - this is the interrupt function called after waking up
 *  - The BODS bit is automatically cleared after three clock cycles.
 */
ISR(WDT_vect)
{
	Serial.println("ISR(WDT_vect)");
	digitalWrite(LED_BUILTIN, LOW);   // turn the LED off (LOW is the voltage level)

	++i;
	Serial.println(i);
	Serial.flush();
	/*  
	*  Enable BOD
	*  - The BODS bit is automatically cleared after three clock cycles.
	*/  
	Serial.println("ISR(WDT_vect) finish");
}
