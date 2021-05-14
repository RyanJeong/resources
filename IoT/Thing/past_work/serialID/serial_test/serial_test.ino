/*
 * [RF Module OFF] Command
 */
#define DISABLE_RF                \
do {                              \
  Serial.print((char) 0xAB);   \
  Serial.print((char) 0x02);   \
} while (0)                       \

/*
 * [RF Module ON] Command
 */
#define ENABLE_RF                 \
do {                              \   
  Serial.print((char) 0xAB);   \
  Serial.print((char) 0x03);   \
} while (0)                       \

void sendMsg(void);
void recvMsg(void);

char  nn;
char  P_TX_DATA[6] = {0xAB, 0x01, 0x03, 0xEE, 0xEF, 0xEA};
char  OFF_COM[2]   = {0xAB, 0x02};
char  ON_COM[2]    = {0xAB, 0x03};
char  CH_SET[4]    = {0xAB, 0x04, 0x01, 0x00};
char  TX_PWR[3]    = {0xAB, 0x05, 0x0A};
char  DATA_RATE[3] = {0xAB, 0x06, 0x02};
char  RSSI_READ[2] = {0xAB, 0x07};
char  TX_TEST[3]   = {0xAB, 0x08, 0x03};
char  BER_TEST[2]  = {0xAB, 0x09};
char  CONF_READ[2] = {0xAB, 0x0A};
char  VERSION[2]   = {0xAB, 0x0B};

void setup() {
	Serial.begin(9600);
	Serial.flush();
	for (nn = 0; nn < 4; nn++) {
		Serial.print(CH_SET[nn]);
	}
	for (nn = 0; nn < 3; nn++) {
		Serial.print(TX_PWR[nn]);
	}
	for (nn = 0; nn < 3; nn++) {
		Serial.print(DATA_RATE[nn]);
	}
	pinMode(5, OUTPUT);
	pinMode(6, OUTPUT);
	pinMode(7, OUTPUT);
	delay(3000);
}

void loop() {
	digitalWrite(5, HIGH);
	sendMsg();
	digitalWrite(5, LOW);
}

void sendMsg(void)
{
	digitalWrite(6, HIGH);
	for (nn = 0; nn < 6; nn++) {
		Serial.print(P_TX_DATA[nn]);
	}
	digitalWrite(6, LOW);
	delay(500);  
}

void recvMsg(void)
{
	delay(500);  
}
