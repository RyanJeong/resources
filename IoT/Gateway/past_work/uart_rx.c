/*
	설정 사항

	Raspberry Pi는 기본적으로 UART를 콘솔이 사용하도록 설정되어 있다.
	하지만 Arduino 등의 보드를 UART를 통해 연결해 제어할  경우 제어 프로그램이 UART를 사용하려면 콘솔이 UART를 잡고 있지 않도록 설정해 줄 필요가 있다.
	SD카드를 PC에 연결하고 오픈해보면 cmdline.txt 파일이 있을 것이다. 기본적으로 수정을 하지 않았다면 파일의 내용은 다음과 같을 것이다.

	dwc_otg.lpm_enable=0 console=ttyAMA0,115200 console=tty1 root=/dev/mmcblk0p2 rootfstype=ext4 elevator=deadline rootwait

	여기서 "ttyAMA0"에 관련된 모든 파라미터를 삭제해주면 된다.
	즉 위의 경우 'console=ttyAMA0,115200' 부분을 삭제한 후 파일을 저장해 주면 된다.
	또한 /etc/inittab 파일도 수정해 줘야 한다.
	에디터로 파일을 열고 'ttyAMA0' 를 검색하면 시리얼 포트를 지정하는 라인을 찾을 수 있다.
	해당 라인의 맨 앞에 '#'을 추가해 주고 저장해 주면 된다.
	이제 리부팅 하고 나면 UART핀은 다른 프로세스가 사용하거나, 또는 그 핀을 일반 GPIO 용도로 사용할 수 있게 된다.
*/

#include <termios.h>	//	Used for UART, control terminal
#include <stdio.h>
#include <unistd.h>	//	Used for UART
#include <fcntl.h>	//	Used for UART
#include <sys/signal.h>	
#include <sys/types.h>

#define BAUDRATE	B9600		// 9600 baudrate
#define DEVICE		"/dev/ttyAMA0"	//	시리얼 포트 정의
#define BUF_SIZE	(1 << 9)

#define TRUE 1
#define FALSE 0

unsigned char offModule[2] = {0xAB, 0x02};
unsigned char onModule[2] = {0xAB, 0x03};
unsigned char setChannel[4] = {0xAB, 0x04, 0x01, 0x00};
unsigned char setTxPower[3] = {0xAB, 0x05, 0x0A};
unsigned char setDataRate[3] = {0xAB, 0x06, 0x02};
unsigned char cmdReadConfig[2] = {0xAB, 0x0A};
unsigned char cmdGetVersion[2] = {0xAB, 0x0B};
unsigned char txBuffer[BUF_SIZE];
unsigned char rxBuffer[BUF_SIZE];

int main(void)
{
	// -------------------------
	// ----- SETUP USART 0 -----
	// -------------------------
	// At bootup, pins 8 and 10 are already set to UART0_TXD, UART0_RXD 
	// (ie the alt0 function) respectively

	int		uart0_filestream;
	int		rxLength, txLength, i;
	unsigned char 	*pTxBuffer;
	struct termios	options;

	int temp;

	uart0_filestream = (-1);
	// OPEN THE UART
	// The flags (defined in fcntl.h):
	// Access modes (use 1 of these):
	//   O_RDONLY	- Open for reading only.
	//   O_RDWR	- Open for reading and writing.
	//   O_WRONLY	- Open for writing only.
	//
	// O_NDELAY / O_NONBLOCK (same function) - Enables nonblocking mode. 
	//    When set read requests on the file can return immediately with a failure status
	//    if there is no input immediately available (instead of blocking). 
	//    Likewise, write requests can also return immediately with a failure status 
	//    if the output can't be written immediately.
	//
	// O_NOCTTY - When set and path identifies a terminal device, 
	//    open() shall not cause the terminal device to become 
	//    the controlling terminal for the process.

	uart0_filestream = open(DEVICE, O_RDWR | O_NOCTTY | O_NDELAY); //Open in non blocking read/write mode
	if (uart0_filestream < 0) {     // ERROR - CAN'T OPEN SERIAL PORT
		printf("Error - Unable to open UART. Ensure it is not in use by another application\n");
		
		return 1;
	}

	// CONFIGURE THE UART
	// The flags (defined in /usr/include/termios.h 
	//     - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html):
	// Baud rate:- B1200, B2400, B4800, B9600, B19200, B38400, B57600, 
	//             B115200, B230400, B460800, B500000, B576000, B921600, 
	//             B1000000, B1152000, B1500000, B2000000, B2500000, B3000000, 
	//             B3500000, B4000000
	// CSIZE:- CS5, CS6, CS7, CS8
	// CLOCAL - Ignore modem status lines
	// CREAD - Enable receiver
	// IGNPAR = Ignore characters with parity errors
	// ICRNL - Map CR to NL on input 
	//         (Use for ASCII comms where you want to auto correct 
	//         end of line characters - don't use for bianry comms!)
	// PARENB - Parity enable
	// PARODD - Odd parity (else even)
	// c_cflag &= ~CSTOPB;	-	set 1 stop bit(default) 
	// c_cflag |= CSTOPB;	-	set 2 stop bit 

	tcgetattr(uart0_filestream, &options);
	options.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;	//	Set baud rate
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	tcflush(uart0_filestream, TCIFLUSH);			//	flush buffer
	tcsetattr(uart0_filestream, TCSANOW, &options);		//	9,600 bps, 8 bit data, no parity, 1 stop bit, non-blocking mode 
	write(uart0_filestream, onModule, 2);
	write(uart0_filestream, setChannel, 4);
	write(uart0_filestream, setTxPower, 3);
	write(uart0_filestream, setDataRate, 3);
	//	rx routine
	while(1) {
		rxLength = read(uart0_filestream, (void *) rxBuffer, (BUF_SIZE - 1));
		if (rxLength > 0) {
			printf("%i bytes read \n", rxLength);
			for (i = 0; i < rxLength; ++i) {
				printf("%x%c", *(rxBuffer + i), '\t');
			}
			puts("");
		}
	}	
	close(uart0_filestream);
	
	return 0;
}
