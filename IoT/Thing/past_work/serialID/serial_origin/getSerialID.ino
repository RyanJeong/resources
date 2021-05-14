#ifndef _ARDUINO_UNIQUE_ID_H_
#define _ARDUINO_UNIQUE_ID_H_

//#include <Arduino.h>

/*
#include <avr/boot.h>

#define UniqueIDsize 9

class ArduinoUniqueID
{
  public:
	ArduinoUniqueID();
	uint8_t id[UniqueIDsize];
};

#endif

ArduinoUniqueID::ArduinoUniqueID()
{
	for (size_t i = 0; i < UniqueIDsize; i++)
	{
		id[i] = boot_signature_byte_get(0x0E + i + (UniqueIDsize == 9 && i > 5 ? 1 : 0));
	}
}

ArduinoUniqueID _UniqueID;

void setup()
{
	Serial.begin(9600);
	pinMode(16, OUTPUT);
	for (int i = 0; i < UniqueIDsize; ++i) {
		if (_UniqueID.id[i] < 0x10) {
			Serial.print("0");
		}
		Serial.print(_UniqueID.id[i], HEX);
		Serial.print(" ");
	}
}

void loop()
{
	
digitalWrite(16, HIGH);
delay(2000);
digitalWrite(16, LOW);
while (1);
}

*/
#define UniqueIDsize 9
class ArduinoUniqueID
{
  public:
	ArduinoUniqueID();
	uint8_t id[UniqueIDsize];
};

#endif
void setup()
{

	
}

void loop()
{

	
}
