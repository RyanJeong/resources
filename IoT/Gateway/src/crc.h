#ifndef __CRC_H__
#define __CRC_H__

#define __CRC__         "[crc.c]"
#define CRC_CHECKCRC    "[checkCRC(uint8_t *, size_t, uint8_t)] "
#define CRC_UPDATECRC   "[updateCRC(uint8_t, uint8_t, uint8_t, uint8_t)] "
#define CRC_INITCRC     "[initCRC(THING *)] "

#include "util.h"   /*  THING   */

/*
 *  size_t checkCRC(uint8_t *, size_t)
 *  --------------------
 *  a function that check CRC value 
 */
size_t checkCRC(uint8_t *, size_t);

/*
 *  uint8_t updateCRC(uint8_t, uint8_t, uint8_t);
 *  --------------------
 *  update crc value
 */
uint8_t updateCRC(uint8_t, uint8_t, uint8_t);

/*
 *  void initCRC(THING &);
 *  : init CRC value
 */
void initCRC(THING *);
#endif
