#include "kem.h"	//	header for kem programs

#define BUF_SIZE	64;

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
			while (rxLength) {
				printf("%x\n", *pRx++);
				rxLength--;
			}
			puts("");
			pTx = temp;
			*pTx++ = 0xEE;
			*pTx++ = 0x23;
			write(uart0_filestream, txBuffer, 5);
		}
	}	
	close(uart0_filestream);
	
	return 0;
}
