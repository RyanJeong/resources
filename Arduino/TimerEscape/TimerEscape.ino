/*
 * Timer n escape source code
 * Can be applied to error detection
 */

void setup() {
  Serial.begin(9600);
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
}


void loop() {
  Serial.println("Loop()");
  delay(500);
  TCNT1 = 0;    //  Timer 1 counter register, if reset this register, can avoid Timer1 continuously
}

/*  Timer Compare Interrupt 1 Service Routine(1 second) */
ISR(TIMER1_COMPA_vect)
{
  //  Never enter this routine...
  Serial.println("Timer1");
}
