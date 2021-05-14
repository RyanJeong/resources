#include "kem.h"	//	header for kem programs

int init(void);	//	init process for kem programs

unsigned char txBuffer[BUF_SIZE];
unsigned char rxBuffer[BUF_SIZE];

int  rxLength, txLength, len;

int main(void)
{
	int 		i;
	unsigned char	crc;
	unsigned char	*pRx, *pTx, *temp;

	char txID;
	char txCMD;

	txBuffer[0] = 0xAB;	/*	Trans CMD	*/
	txBuffer[1] = 0x01;	/*	Trans CMD	*/
	txBuffer[2] = 0x03;	/*	Length		*/

	len = 0;	/* length of frame	*/
	/*
	 *	init process for use rf module
	 *	if return 0: success, return 1: failed
	 */
	if (init()) {	
		return 1;
	}

	while(1) {
		rxLength = read(uart0_filestream, (void *) rxBuffer, (BUF_SIZE - 1));
		if (rxLength > 0) {
			pRx = rxBuffer;
			if (!len) {
				txID = *(rxBuffer + 1);
				txCMD = *(rxBuffer + 2);
				--rxLength;
				crc = crc8_table[0xFF ^ *pRx++];
				len = *rxBuffer;
			}
			while (rxLength) {
				crc = crc8_table[crc ^ *pRx++];
				--rxLength;
				--len;
			}
			if (!len) {
				if (!crc) {
					txCMD |= 0x80;
				} else {
					txCMD |= 0x8F;	
				}
				crc = crc8_table[0xFF ^ 0x03];
				crc = crc8_table[crc ^ txID];
				crc = crc8_table[crc ^ txCMD];
				txBuffer[3] = txID;
				txBuffer[4] = txCMD;
				txBuffer[5] = crc;
				printf("%02X%02X%02X%02X%02X%02X\n", txBuffer[0], txBuffer[1], txBuffer[2], txBuffer[3], txBuffer[4], txBuffer[5]);
				txLength = write(uart0_filestream, txBuffer, 6);
			}
		}
	}	
	close(uart0_filestream);
	
	return 0;
}
