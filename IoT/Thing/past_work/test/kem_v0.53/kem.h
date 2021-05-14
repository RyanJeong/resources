#include "rf.h"
#include "interrupt.h"
#include "crc.h"

#define ENABLE_ADC			\
do {					\
	ADCSRA |= (1 << 7);		\
} while (0)				

#define DISABLE_ADC			\
do {					\
	ADCSRA &= ~(1 << 7);		\
} while (0)				

/*  ENABLE SLEEP - this enables the sleep mode  */
#define ENABLE_SLEEP					\
do {							\
	SMCR |= (1 << 2);   /*  power down mode */	\
	SMCR |= 1;          /*  enable sleep  */	\
} while (0)

/*	
 * 	SLEEP
 * 	1. Set both BODS and BODSE at the same time
 * 	2. Then set the BODS bit and clear the BODSE bit at the same time
 * 	3. Call sleep operation
 */
#define SLEEP						\
do {							\
	MCUCR	|= (3 << 5); 				\
	MCUCR	= (MCUCR & ~(1 << 5)) | (1 << 6); 	\
	__asm__  __volatile__("sleep");			\
} while (0)

#define SLEEP1								\
do {									\
	unsigned char tempreg;						\
									\
	__asm__ __volatile__("in %[tempreg], %[mcucr]" "\n\t"		\
	               "ori %[tempreg], %[bods_bodse]" "\n\t"		\
	               "out %[mcucr], %[tempreg]" "\n\t"		\
	               "andi %[tempreg], %[not_bodse]" "\n\t"		\
	               "out %[mcucr], %[tempreg]"			\
	               : [tempreg] "=&d" (tempreg)			\
	               : [mcucr] "I" _SFR_IO_ADDR(MCUCR),		\
	                 [bods_bodse] "i" (_BV(BODS) | _BV(BODSE)),	\
	                 [not_bodse] "i" (~_BV(BODSE)));		\
	__asm__  __volatile__("sleep");					\
} while (0)

void pinmode_setup(void)
{
	/*
	*  Save Power by writing all Digital IO LOW
	*  - note that pins just need to be tied one way or another, do not damage devices!
	*/

	pinMode(0, INPUT);	//	RX
	pinMode(1, OUTPUT);	//	TX
	pinMode(2, OUTPUT);	//	HEAT_R
	pinMode(3, OUTPUT);	//	Not used
	pinMode(4, INPUT);	//	Set ID, bit 2
	pinMode(5, OUTPUT);	//	Status0
	pinMode(6, OUTPUT);	//	Status1, Will change to setting pins(set channel, bit 1) 
	pinMode(7, OUTPUT);	//	Status2, Will change to setting pins(set channel, bit 0)
	pinMode(8, INPUT);	//	Set ID, bit 3
	pinMode(9, INPUT);	//	Set ID, bit 4
	pinMode(10, INPUT);	//	Set ID, bit 5
	pinMode(11, OUTPUT);	//	Not used
	pinMode(12, OUTPUT);	//	Not used
	pinMode(13, OUTPUT);	//	Not used
	pinMode(14, INPUT);	//	A0, ADC0, for test sensor
	pinMode(15, INPUT);	//	A1, ADC1, for measuring sensor
	pinMode(16, INPUT);	//	Set ID, bit 0
	pinMode(17, INPUT);	//	Set ID, bit 1
	pinMode(18, OUTPUT);	//	SoftwareSerial TX(twisted)
#if SOFTWARE
	pinMode(19, INPUT);	//	SoftwareSerial RX(twisted)
#else
	pinMode(19, OUTPUT);	//	Not used
#endif

	return;
}
