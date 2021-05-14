/*
 *  KEM IoT Program using Arduino
 *  Munseong Jeong
 *  msjeong@daejin.ac.kr
 *  
 * ------------------------------------
 *	!!!NEED TO CODE MODULAR!!!
 * ------------------------------------
 *  Communication process
 *  1. Send MSG to GATEWAY
 *  2. Recv ACK from GATEWAY
 */
#define DEBUG		1	//	1: Use LED for DEBUG		| 0: Use LED for process monitoring
#define	SOFTWARE	0	//	1: SoftwareSerial, no WDT	| 0: Serial, with WDT

#include "kem.h"

void	pinmode_setup(void);
void	watchdog_setup(void);
void	RF_Setup(void);
void	set_ID(void);
void	set_channel(void);
void	send_msg(int);
int	recv_msg(int);

extern int	thing_status;

void setup() 
{
    	cli();  		//	disable all interrupts 
	pinmode_setup();	//	for work in low-power mode 
	Serial.begin(9600);	//	for use serial methods
#if SOFTWARE
	RF_Serial.begin(9600);	//	for use RF module
#endif
	set_ID();		//	for use to get ID from the dip-switch
	set_channel();		//	for use to get channel from the dip-switch
	RF_Setup();		//	MUST do RF_Setup() after set_ID() and set_channel();
#if SOFTWARE
	RF_Serial.flush();	//	Initialize buffer on RF module
#else
	Serial.flush();		//	Initialize buffer on RF module
#endif
#if !SOFTWARE
	watchdog_setup();
	ENABLE_WATCHDOG;	//	enable watchdog timer
    	ENABLE_SLEEP;		//	enable sleep mode
    	sei();			//	enable all interrupts
#endif
#if DEBUG
	digitalWrite(5, HIGH);
	delay(1000);
	digitalWrite(5, LOW);
	digitalWrite(6, HIGH);
	delay(1000);
	digitalWrite(6, LOW);
	digitalWrite(7, HIGH);
	delay(1000);
	digitalWrite(7, LOW);
#endif
}

void loop() 
{
	send_msg(CHECK_TEST_REQUEST);
	delay(250);
	recv_msg(CHECK_TEST_REQUEST);
	delay(250);
	//delayMicroseconds(1000);	//	Stabilization, need to optimization
	delay(500);
#if !SOFTWARE
	SLEEP;
#endif
}
