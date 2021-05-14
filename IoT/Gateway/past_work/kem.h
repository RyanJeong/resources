/*
        설정 사항

        Raspberry Pi는 기본적으로 UART를 콘솔이 사용하도록 설정되어 있다.
        하지만 Arduino 등의 보드를 UART를 통해 연결해 제어할  경우 제어 프로그램이 UART를 사용하려면 콘솔이 UART를 잡>고 있지 않도록 설정해 줄 필요가 있다.
        SD카드를 PC에 연결하고 오픈해보면 cmdline.txt 파일이 있을 것이다. 기본적으로 수정을 하지 않았다면 파일의 내용>은 다음과 같을 것이다.

        dwc_otg.lpm_enable=0 console=ttyAMA0,115200 console=tty1 root=/dev/mmcblk0p2 rootfstype=ext4 elevator=deadline rootwait

        여기서 "ttyAMA0"에 관련된 모든 파라미터를 삭제해주면 된다.
        즉 위의 경우 'console=ttyAMA0,115200' 부분을 삭제한 후 파일을 저장해 주면 된다.
        또한 /etc/inittab 파일도 수정해 줘야 한다.
        에디터로 파일을 열고 'ttyAMA0' 를 검색하면 시리얼 포트를 지정하는 라인을 찾을 수 있다.
        해당 라인의 맨 앞에 '#'을 추가해 주고 저장해 주면 된다.
        이제 리부팅 하고 나면 UART핀은 다른 프로세스가 사용하거나, 또는 그 핀을 일반 GPIO 용도로 사용할 수 있게 된다.
*/

#include <termios.h>    //      Used for UART, control terminal
#include <stdio.h>
#include <unistd.h>     //      Used for UART
#include <fcntl.h>      //      Used for UART
#include <sys/signal.h>
#include <sys/types.h>

#define BAUDRATE        B9600           // 9600 baudrate
#define DEVICE          "/dev/ttyAMA0"  //      시리얼 포트 정의
#define SIZE       	64

#define TRUE 1
#define FALSE 0

int uart0_filestream; 

unsigned char offModule[2] = {0xAB, 0x02};
unsigned char onModule[2] = {0xAB, 0x03};
unsigned char setChannel[4] = {0xAB, 0x04, 0x01, 0x00};
unsigned char setTxPower[3] = {0xAB, 0x05, 0x0A};
unsigned char setDataRate[3] = {0xAB, 0x06, 0x02};
struct termios  options;

static unsigned char const crc8_table[] = {
    0x00, 0x31, 0x62, 0x53, 0xc4, 0xf5, 0xa6, 0x97, 
    0xb9, 0x88, 0xdb, 0xea, 0x7d, 0x4c, 0x1f, 0x2e, 
    0x43, 0x72, 0x21, 0x10, 0x87, 0xb6, 0xe5, 0xd4, 
    0xfa, 0xcb, 0x98, 0xa9, 0x3e, 0x0f, 0x5c, 0x6d, 
    0x86, 0xb7, 0xe4, 0xd5, 0x42, 0x73, 0x20, 0x11, 
    0x3f, 0x0e, 0x5d, 0x6c, 0xfb, 0xca, 0x99, 0xa8, 
    0xc5, 0xf4, 0xa7, 0x96, 0x01, 0x30, 0x63, 0x52, 
    0x7c, 0x4d, 0x1e, 0x2f, 0xb8, 0x89, 0xda, 0xeb, 
    0x3d, 0x0c, 0x5f, 0x6e, 0xf9, 0xc8, 0x9b, 0xaa, 
    0x84, 0xb5, 0xe6, 0xd7, 0x40, 0x71, 0x22, 0x13, 
    0x7e, 0x4f, 0x1c, 0x2d, 0xba, 0x8b, 0xd8, 0xe9, 
    0xc7, 0xf6, 0xa5, 0x94, 0x03, 0x32, 0x61, 0x50, 
    0xbb, 0x8a, 0xd9, 0xe8, 0x7f, 0x4e, 0x1d, 0x2c,
    0x02, 0x33, 0x60, 0x51, 0xc6, 0xf7, 0xa4, 0x95, 
    0xf8, 0xc9, 0x9a, 0xab, 0x3c, 0x0d, 0x5e, 0x6f, 
    0x41, 0x70, 0x23, 0x12, 0x85, 0xb4, 0xe7, 0xd6, 
    0x7a, 0x4b, 0x18, 0x29, 0xbe, 0x8f, 0xdc, 0xed, 
    0xc3, 0xf2, 0xa1, 0x90, 0x07, 0x36, 0x65, 0x54, 
    0x39, 0x08, 0x5b, 0x6a, 0xfd, 0xcc, 0x9f, 0xae, 
    0x80, 0xb1, 0xe2, 0xd3, 0x44, 0x75, 0x26, 0x17, 
    0xfc, 0xcd, 0x9e, 0xaf, 0x38, 0x09, 0x5a, 0x6b, 
    0x45, 0x74, 0x27, 0x16, 0x81, 0xb0, 0xe3, 0xd2, 
    0xbf, 0x8e, 0xdd, 0xec, 0x7b, 0x4a, 0x19, 0x28, 
    0x06, 0x37, 0x64, 0x55, 0xc2, 0xf3, 0xa0, 0x91, 
    0x47, 0x76, 0x25, 0x14, 0x83, 0xb2, 0xe1, 0xd0, 
    0xfe, 0xcf, 0x9c, 0xad, 0x3a, 0x0b, 0x58, 0x69,
    0x04, 0x35, 0x66, 0x57, 0xc0, 0xf1, 0xa2, 0x93, 
    0xbd, 0x8c, 0xdf, 0xee, 0x79, 0x48, 0x1b, 0x2a, 
    0xc1, 0xf0, 0xa3, 0x92, 0x05, 0x34, 0x67, 0x56, 
    0x78, 0x49, 0x1a, 0x2b, 0xbc, 0x8d, 0xde, 0xef, 
    0x82, 0xb3, 0xe0, 0xd1, 0x46, 0x77, 0x24, 0x15, 
    0x3b, 0x0a, 0x59, 0x68, 0xff, 0xce, 0x9d, 0xac};

int init(void)
{
        // -------------------------
        // ----- SETUP USART 0 -----
        // -------------------------
        // At bootup, pins 8 and 10 are already set to UART0_TXD, UART0_RXD 
        // (ie the alt0 function) respectively

        uart0_filestream = (-1);
        // OPEN THE UART
        // The flags (defined in fcntl.h):
        // Access modes (use 1 of these):
        //   O_RDONLY   - Open for reading only.
        //   O_RDWR     - Open for reading and writing.
        //   O_WRONLY   - Open for writing only.
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
        // c_cflag &= ~CSTOPB;  -       set 1 stop bit(default)
        // c_cflag |= CSTOPB;   -       set 2 stop bit

        tcgetattr(uart0_filestream, &options);
        options.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;      //      Set baud rate
        options.c_iflag = IGNPAR;
        options.c_oflag = 0;
        options.c_lflag = 0;
        tcflush(uart0_filestream, TCIFLUSH);                    //      flush buffer
        tcsetattr(uart0_filestream, TCSANOW, &options);         //      9,600 bps, 8 bit data, no parity, 1 stop bit, non-blocking mode
        write(uart0_filestream, setChannel, 4);
        write(uart0_filestream, setTxPower, 3);
        write(uart0_filestream, setDataRate, 3);

	return 0;
}

