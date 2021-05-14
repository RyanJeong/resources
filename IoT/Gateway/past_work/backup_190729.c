/*
	설정 사항

	Raspberry Pi는 기본적으로 콘솔이 UART를 사용하도록 설정되어 있다.
	하지만 Arduino 등의 보드를 UART를 통해 제어할 경우, 제어 프로그램이 UART를 사용하려면 콘솔이 UART를 잡고 있지 않도록 설정해 줄 필요가 있다.
	SD카드를 PC에 연결하고 오픈해보면 cmdline.txt 파일이 있을 것이다.(경로: /boot/cmdline.txt) 기본적으로 수정을 하지 않았다면 파일의 내용은 다음과 같을 것이다.

	dwc_otg.lpm_enable=0 console=ttyAMA0,115200 console=tty1 root=/dev/mmcblk0p2 rootfstype=ext4 elevator=deadline rootwait

	여기서 "ttyAMA0"에 관련된 모든 파라미터를 삭제해주면 된다.
	즉 위의 경우 'console=ttyAMA0,115200' 부분을 삭제한 후 파일을 저장해 주면 된다.
	또한 /etc/inittab 파일도 수정해 줘야 한다.
	에디터로 파일을 열고 'ttyAMA0' 를 검색하면 시리얼 포트를 지정하는 라인을 찾을 수 있다.
	해당 라인의 맨 앞에 '#'을 추가해 주고 저장해 주면 된다.
	이제 리부팅 하고 나면 UART핀은 다른 프로세스가 사용하거나, 또는 그 핀을 일반 GPIO 용도로 사용할 수 있게 된다.
	-------------------------------
	라즈베리안 Jessie 이상 버전에서는 inittab 을 사용하지 않고, systemd 를 사용한다. 따라서 systemd control 명령을 사용하여 비활성화를 해주도록 한다.
	$ sudo systemctl stop serial-getty@ttyAMA0.service
	$ sudo systemctl disable serial-getty@ttyAMA0.service

	Jessie 에서는 이와 별도로 UART 를 사용하겠다고 설정을 추가해주어야 한다.

	# sudo vi /boot/config.txt
	enable_uart=1

	Raspberry PI 3 에서는 UART 디바이스 정보가 바뀌어서 아래와 같이 설정해준다.
	$ sudo systemctl stop serial-getty@ttyS0.service 
	$ sudo systemctl disable serial-getty@ttyS0.service

	출처: https://pinocc.tistory.com/185 [땅뚱 창고]
*/

#include <termios.h>	//	Used for UART, control terminal
#include <stdio.h>
#include <unistd.h>	//	Used for UART
#include <fcntl.h>	//	Used for UART
#include <sys/signal.h>	
#include <sys/types.h>
#include <time.h>	//	for debug, print time

#define BAUDRATE	B9600		// 9600 baudrate
#define DEVICE		"/dev/ttyAMA0"	//	시리얼 포트 정의
#define SIZE		64
#define TRUE		1
#define FALSE		0

char offModule[2]	= {0xAB, 0x02};
char onModule[2]	= {0xAB, 0x03};
char setChannel[4]	= {0xAB, 0x04, 0x01, 0x00};
char setTxPower[3]	= {0xAB, 0x05, 0x0A};
char setDataRate[3]	= {0xAB, 0x06, 0x02};
char send_buffer[SIZE]	= {0xAB};
char recv_buffer[SIZE];

int main(void)
{
	// for debug
	time_t		t;
	struct tm	*tm;

	// -------------------------
	// ----- SETUP USART 0 -----
	// -------------------------
	// At bootup, pins 8 and 10 are already set to UART0_TXD, UART0_RXD 
	// (ie the alt0 function) respectively

	int		uart0_filestream;
	int		recv_length, send_length, i;
	unsigned char 	*p_send_buffer, *p_recv_buffer;
	struct termios	options;

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
	//write(uart0_filestream, setChannel, 4);
	//write(uart0_filestream, setTxPower, 3);
	//write(uart0_filestream, setDataRate, 3);
	while (1) {
		recv_length = read(uart0_filestream, (void *) recv_buffer, (SIZE - 1));
		if (recv_length > 0) {
			t		= time(NULL);
			tm		= localtime(&t);
			p_recv_buffer	= recv_buffer;
			p_send_buffer	= send_buffer;	
			printf("%d:%d:%d, ", tm->tm_hour, tm->tm_min, tm->tm_sec);
			while (recv_length--) {
				printf("%x ", *p_recv_buffer++);
			}
			puts("");
			*p_send_buffer++;	// already store 0xAB at index 0
			*p_send_buffer++ = (char) 0x01;
			*p_send_buffer++ = (char) 0x01;
			*p_send_buffer++ = (char) 0xFF;
			puts("");
			send_length = write(uart0_filestream, send_buffer, 6);
			if (send_length <= 0) {
				printf("TX Error\n");
			}
		}
	}
	close(uart0_filestream);
	
	return 0;
}
