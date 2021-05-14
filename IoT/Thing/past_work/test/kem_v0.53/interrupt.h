#define ENABLE_WATCHDOG			\
do {					\
	WDTCSR |= (1 << 6);		\
} while (0)

#define DISABLE_WATCHDOG		\
do {					\
	WDTCSR &= ~(1 << 6);		\
} while (0)

#define ENABLE_TIMER1			\
do {					\
	TIMSK1 |= (1 << OCIE1A);	\
} while (0)

#define DISABLE_TIMER1			\
do {					\
	TIMSK1 &= ~(1 << OCIE1A);	\
} while (0)

#define WATCHDOT_TIMER_RESET		\
do {					\
	__asm__ __volatile__("wdr");	\
} while (0)

/*
 *  watchdogSetup(void)
 *  Function that prepare to use the watchdog
 *  Perform only once
 *  use for wake-up mcu
 *    WDTCSR = (N);
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
void watchdog_setup(void)
{
	/*  SETUP WATCHDOG TIMER
	*  ------
	*  WDTCSR = (24);      : change enable and WDE - also resets
	*  WDTCSR = (6);       : prescalers only - get rid of the WDE and WDCE bit
	*  WDF | WDIE | WDP3 | WDCE | WDE | WDP2 | WDP1 | WDP0
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
	WDTCSR	= (24);      /*  change enable and WDE - also resets */
	WDTCSR	= (7);       /*  prescalers only - get rid of the WDE and WDCE bit */

	return;
}

/*
 *  timer1Setup(void)
 *  Function that prepare to use the timer1
 *  Perform only once
 *  use for receiving rf msg
 *    124 :  8 ms
 *    249 : 16 ms
 *    374 : 24 ms
 */
inline void timer1Setup(void)
{
	TCCR1A	= 0;              /*  Reset TCCR1A */
	TCCR1B	= 0;              /*  Reset TCCR1B */
	TCNT1	= 0;              /*  Reset Timer1 Counter  */
	OCR1A	= 249;            /*  Compare Match Register 16MHz/1024/2Hz  */
	TCCR1A	|= (1 << COM1A1);  /*  Clear OC1A/OC1B on Compare Match, Set OC1A/OC1B at BOTTOM (non-inverting mode) */
	TCCR1A	|= (1 << WGM10);   /*  Fast Pulse Width Modulation (PWM) */
	TCCR1A	|= (1 << WGM11);   /*  Fast Pulse Width Modulation (PWM) */
	TCCR1B	|= (1 << WGM12);   /*  Fast Pulse Width Modulation (PWM) */
	TCCR1B	|= (1 << WGM13);   /*  Fast Pulse Width Modulation (PWM) */
	/*
	*  CSx2  CSx1  CSx0  선택 분주 클럭
	*  0     0     0     정지
	*  0     0     1     CK
	*  0     1     0     CK/8
	*  0     1     1     CK/64
	*  1     0     0     CK/256
	*  1     0     1     CK/1024
	*  1     1     0     외부 클럭(T1 또는 T2)의 하강 에지
	*  1     1     1     외부 클럭(T1 또는 T2)의 상승 에지 
	*/
	TCCR1B	|= (1 << CS12) | (1 << CS10);    /*  1024 prescaler */

	/*
	* TIMSK1 &= ~(1 << OCIE1A);  Disable timer compare interrupt 1
	* TIMSK1 |= (1 << OCIE1A);   Enable timer compare interrupt 1
	*/
	TIMSK1	&= ~(1 << OCIE1A);

	return;
}

/*
 *  Watchdog Timer(2 seconds period)
 *  Needed for the watch dog timer.
 *  This is called after a watch dog timer timeout
 *  - this is the interrupt function called after waking up
 */
ISR(WDT_vect)
{
  /*  
   *  Enable BOD
   *  - The BODS bit is automatically cleared after three clock cycles.
   *    Writing this bit to one turns off the BOD in relevant sleep modes, while a zero in this bit keeps
   *    BOD active. Default setting keeps BOD active, i.e. BODS set to zero.
   */  
}

/*  Timer Compare Interrupt 1 Service Routine(16 milliseconds) */
ISR(TIMER1_COMPA_vect)
{

}
