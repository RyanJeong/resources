#include <avr/boot.h>
#define ID_SIZE 9
uint8_t id[ID_SIZE];
void setup() {
	Serial.begin(9600);
	Serial.flush();
}

void loop() {
	delay(1000);
	connectToGateway();
}

void connectToGateway()
{
	size_t i;

	for (i = 0; i < ID_SIZE; ++i) {
		id[i] = boot_signature_byte_get(0x0E + i + (((ID_SIZE == 9) && (i > 5)) ? 1 : 0));
		if (id[i] < 0x10) {
			Serial.print("0");
		}
		Serial.print(id[i], HEX);
		Serial.print(" ");
	}
	Serial.println("");

}
