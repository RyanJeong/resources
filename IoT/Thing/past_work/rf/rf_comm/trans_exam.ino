#include <SoftwareSerial.h>
#define STATE 1

/*
 * [RF Module OFF] Command
 */
#define DISABLE_RF                \
do {                              \
  RF_Serial.print((char) 0xAB);   \
  RF_Serial.print((char) 0x02);   \
} while (0)                       \

/*
 * [RF Module ON] Command
 */
#define ENABLE_RF                 \
do {                              \   
  RF_Serial.print((char) 0xAB);   \
  RF_Serial.print((char) 0x03);   \
} while (0)                       \

SoftwareSerial RF_Serial(A4,A5);  // rx, tx

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
  RF_Serial.begin(9600);

  /*
   *  Channel과 Tx Power Level은 처음에만 한번 설정해주고 나면 EEPROM에 저장되므로 따로 설정해주지 않아도 됨
   *  처음 설정하고나서 Channel과 Tx Power Level은 주석처리하여 프로그램해야함
   *  for (nn = 0; nn < 4; nn++) {
   *    RF_Serial.print(CH_SET[nn]);
   *  }
   *  for (nn = 0; nn < 3; nn++) {
   *    RF_Serial.print(TX_PWR[nn]);
   *  }
   *  for (nn = 0; nn < 3; nn++) {
   *    RF_Serial.print(DATA_RATE[nn]);
   *  }
   */
  delay(5000);
}


void loop() {
#if STATE
  Serial.println("ON");
  ENABLE_RF;
#else
  Serial.println("OFF");
  DISABLE_RF;
#endif
  Serial.println("Start after 3 sec.");
  delay(3000);
  RF_Serial.flush();
  while (1) {
    for (nn = 0; nn < 6; nn++) {
      RF_Serial.print(P_TX_DATA[nn]);
      Serial.print(P_TX_DATA[nn], HEX);
      Serial.print(" ");
    }
    Serial.println("");
    delay(1000);  
    Serial.println("Step");
  }
}
