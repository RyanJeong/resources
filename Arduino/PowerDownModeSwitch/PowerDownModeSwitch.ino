int flag;

void setup() {
  Serial.begin(9600);
  /*
   *  Save Power by writing all Digital IO LOW 
   *  - note that pins just need to be tied one way or another, do not damage devices!
   */
  for (int i = 0; i < 20; i++) {
    pinMode(i, OUTPUT);
  }
  
  cli();  /*  Clear interrupts  */
  
  /*
   *  SETUP WATCHDOG TIMER
   *  ------
   *  WDF | WDIE | WDP3 | WDCE | WDE | WDP2 | WDP1 | WDP0
   *  WDTCSR = (24);     : change enable and WDE - also resets
   *  WDTCSR = (6);     : prescalers only - get rid of the WDE and WDCE bit
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
  
  /*  WDTCSR  |= (1<<6);  */

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

  TCCR1A  = 0;              /*  Reset TCCR1A */
  TCCR1B  = 0;              /*  Reset TCCR1B */
  TCNT1   = 0;              /*  Reset Timer1 Counter  */
  OCR1A   = 62499;          /*  Compare Match Register 16MHz/256/2Hz  */
  TCCR1A |= (1 << COM1A1);  /*  Clear OC1A/OC1B on Compare Match, Set OC1A/OC1B at BOTTOM (non-inverting mode) */
  TCCR1A |= (1 << WGM10);   /*  Fast Pulse Width Modulation (PWM) */
  TCCR1A |= (1 << WGM11);   /*  Fast Pulse Width Modulation (PWM) */
  TCCR1B |= (1 << WGM12);   /*  Fast Pulse Width Modulation (PWM) */
  TCCR1B |= (1 << WGM13);   /*  Fast Pulse Width Modulation (PWM) */
  TCCR1B |= (1 << CS12);    /*  256 prescaler */
  TIMSK1 |= (1 << OCIE1A);  /*  Enable timer compare interrupt 1  */

  TCCR2A  = 0;              /*  Reset TCCR2A */
  TCCR2A  = 0;              /*  Reset TCCR2A */
  TCNT2   = 0;              /*  Reset Timer2 Counter  */
  OCR2A   = 249;            /*  Compare Match Register 16MHz/1024/2Hz */
  TCCR2A |= (1 << COM2A1);  /*  Clear OC0A/OC2A on Compare Match, Set OC0A/OC2A at BOTTOM (non-inverting mode)  */
  TCCR2A |= (1 << WGM20);   /*  Fast Pulse Width Modulation (PWM) */
  TCCR2A |= (1 << WGM21);   /*  Fast Pulse Width Modulation (PWM) */
  TCCR2B |= (1 << WGM22);   /*  Fast Pulse Width Modulation (PWM) */
  TCCR2B |= (1 << CS21);    /*  64 prescaler  */
  TCCR2B |= (1 << CS20);    /*  64 prescaler  */
  TIMSK2 |= (1 << OCIE2A);  /*  Enable timer compare interrupt 2  */
  
  sei();  /*  Set interrupts  */
  
}

void loop() {
  if (!flag) {
    /*
     *  Out Power Down Mode
     */
     
  } else {
    /*
     *  In Power Down Mode
     */  
     
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
 *  Watchdog Timer(1 second)
 *  Needed for the watch dog timer.
 *  This is called after a watch dog timer timeout
 *  - this is the interrupt function called after waking up
 *  - The BODS bit is automatically cleared after three clock cycles.
 */
ISR(WDT_vect){
  Serial.println("Watchdog");
  Serial.flush();
  
  flag = false;
  TIMSK1 |= (1 << OCIE1A);  /*  Enable timer compare interrupt 1  */
  TIMSK2 |= (1 << OCIE2A);  /*  Enable timer compare interrupt 2  */
  WDTCSR &= ~(1<<6);        /*  Disable interrupt mode */
}

/*  Timer Compare Interrupt 2 Service Routine(1 millisecond) */
ISR(TIMER2_COMPA_vect)
{
  Serial.println("Timer2");
  Serial.flush();
}

/*  Timer Compare Interrupt 1 Service Routine(1 second) */
ISR(TIMER1_COMPA_vect)
{
  Serial.println("Timer1");
  Serial.flush();
  flag = true;
  TIMSK1 &= ~(1 << OCIE1A);   /*  Disable timer compare interrupt 1  */
  TIMSK2 &= ~(1 << OCIE2A);   /*  Disable timer compare interrupt 2  */
  WDTCSR |= (1<<6);           /*  Enable interrupt mode */
}
