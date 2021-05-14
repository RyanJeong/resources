#ifndef __CRC_H__
#define __CRC_H__

#define __CRC__ "[crc.ino]"

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
 *  checkCRC(char [], uint8_t)
 *  --------------------
 *  a function that check CRC value
 */
uint8_t checkCRC(char [], uint8_t);

/*
 *  uint8_t updateCRC(char [], uint8_t);
 *  --------------------
 *  update crc value
 */
uint8_t updateCRC(char [], uint8_t);

/*
 * uint8_t checkCRCIsInvalid(void);
 *  : a function that test crc routine
 */
uint8_t checkCRCIsInvalid(void);
#endif
