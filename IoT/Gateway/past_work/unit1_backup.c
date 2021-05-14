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

	pTx = txBuffer;
	*pTx++ = 0xAB;
	*pTx++ = 0x01;
	*pTx++ = 0x02;
	temp = pTx;

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
				pTx = temp;
			}
			/*
			while (rxLength) {
				crc = crc8_table[crc ^ *pRx];
				printf("%d %d %x %x\n", rxLength--, len--, *pRx++, crc);
			}
			*/
			if (!len) {
				printf("%s\n", "전송 준비");
				if (!crc) {
					txCMD |= 0x80;
				} else {
					txCMD |= 0x8F;	
				}
				*pTx++ = txID;
				*pTx++ = txCMD;	
				printf("%0x%0x%0x%0x%0x\n", txBuffer[0], txBuffer[1], txBuffer[2], txBuffer[3], txBuffer[4]);
				write(uart0_filestream, txBuffer, 5);
			}
		}
	}	
	close(uart0_filestream);
	
	return 0;
}
