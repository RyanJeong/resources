#ifndef __CRC_H__
#define __CRC_H__

#define __CRC__                 "[crc.ino]"
#define CRC_INITCRC1            "[initCRC1()] "
#define CRC_CHECKCRC1           "[checkCRC1()] "
#define CRC_UPDATECRC1          "[updateCRC1()] "
#define CRC_CHECKCRCISINVALID1  "[checkCRCIsInvalid1()] "

/*
 * void setCRC(void);
 *  : a function that input the value 0xFF into the variable thingCRC
 */
void setCRC(void);

/*
 *  initCRC(char *, uint8_t)
 *  --------------------
 *  a function that initialization CRC value
 */
void initCRC(void);

/*
 *  initCRC1(char *, uint8_t)
 *  --------------------
 *  a function that initialization CRC value when defined SOFTWARESERIAL_MODE
 */
void initCRC1(void);

/*
 *  initCRC2(char *, uint8_t)
 *  --------------------
 *  a function that initialization CRC value when not defined SOFTWARESERIAL_MODE
 */
void initCRC2(void);

/*
 *  checkCRC(char *, uint8_t)
 *  --------------------
 *  a function that check CRC value
 */
uint8_t checkCRC(char *, uint8_t);

/*
 *  checkCRC1(char *, uint8_t)
 *  --------------------
 *  a function that check CRC value when defined SOFTWARESERIAL_MODE
 */
uint8_t checkCRC1(char *, uint8_t);

/*
 *  checkCRC2(char *, uint8_t)
 *  --------------------
 *  a function that check CRC value when not defined SOFTWARESERIAL_MODE
 */
uint8_t checkCRC2(char *, uint8_t);

/*
 *  updateCRC(char *, uint8_t)
 *  --------------------
 *  update crc value
 */
uint8_t updateCRC(char *, uint8_t);

/*
 *  updateCRC1(char *, uint8_t)
 *  --------------------
 *  a function that update CRC value when defined SOFTWARESERIAL_MODE
 */
uint8_t updateCRC1(char *, uint8_t);

/*
 *  updateCRC2(char *, uint8_t)
 *  --------------------
 *  a function that update CRC value when not defined SOFTWARESERIAL_MODE
 */
uint8_t updateCRC2(char *, uint8_t);

/*
 * uint8_t checkCRCIsInvalid(void);
 *  : a function that test crc routine
 */
uint8_t checkCRCIsInvalid(void);

/*
 * uint8_t checkCRCIsInvalid1(void);
 *  : a function that test crc routine when defined SOFTWARESERIAL_MODE
 */
uint8_t checkCRCIsInvalid1(void);

/*
 * uint8_t checkCRCIsInvalid2(void);
 *  : a function that test crc routine when not defined SOFTWARESERIAL_MODE
 */
uint8_t checkCRCIsInvalid2(void);
#endif
