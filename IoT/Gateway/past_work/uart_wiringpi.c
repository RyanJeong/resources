/*
wiringPi에서 지원하는 함수는 다음과 같다.

더욱 자세히 알고싶다면 http://wiringpi.com/reference/serial-library/를 참고하면 된다.

int serialOpen(char *device, int baud) : 시리얼오픈 함수는 시리얼 포트를 열고 초기화를 하며, 속도(baud rate)를 설정하는 함수이다. 앞의 인자값은 "/dev/ttyAMA0"로 오픈할 시리얼 포트를 지정하며, 두번째 인자는 baud의 통신 속도를 지정하면 된다. 리턴값으로는 에러가 있을 경우 -1을 넘겨준다.
int serialGetchar(int fd) : 이 함수는 시리얼 포트에서 한 바이트를 받아서 리턴해주는 함수이다.  fd는 시리얼 포트를 오픈할때 리턴 된 파일 디스크립터이다.
void serialClose(int fd) : open했던 시리얼 포트를 닫는다
void serialPutchar(inf fd, unsigned char c) : 이 함수는 한바이트만큼 전송한다.
void serialPuts(int fd, char *s) : 입출력 함수중 하나로 쉽게 설명해서 fgets로 저장한 문자열을 fd로 전송하게 된다.
void serialPrintf(int fd, char *message, ...)
void serialFlush(int fd) : 수신된 모든 데이터를 삭제한다.
char *fgets(char *string, sizeof(*string), File *stream) : 이 함수는 텍스트 파일에 저장된 문자를 줄 단위로 읽어들어와 반환하는 함수로서 첫번째 인자는 입력 받은 문자열을 저장할 포인터를 뜻하고, 두번째 인자는 입력 받을 문자의 수를 설정하는 것이며, 세번째 인자는 형성한 스트림의 이름을 뜻한다.
이 함수는 읽어온 문자열에 대한 포인터를 반환하며 파일의 끝에 도달하거나 오류가 발생할 경우 NULL을 반환한다.

컴파일 및 실행
- gcc uart.c -o uart -lwiringPi
- sudo ./uart
*/
#include <stdio.h>
#include <errno.h>
#include <wiringPi.h>

int main()
{
	int fd, i, c;
	int temp;
	if (wiringPiSetup() == -1) {
		printf("Please install the library wiringPi\n");

		return 1;
	}
	
	if ((fd = serialOpen("/dev/ttyAMA0", 9600)) < 0) {
		fprintf(stderr, "Unable to open serial device: %s\n", strerror(errno));

		return 1;
	}
	printf("%s\n", "Raspberry Pi UART Test");
	serialPutchar(fd, 0xAB);
	serialPutchar(fd, 0x04);
	serialPutchar(fd, 0x01);
	serialPutchar(fd, 0x00);
		
	serialPutchar(fd, 0xAB);
	serialPutchar(fd, 0x05);
	serialPutchar(fd, 0x0A);
	
	serialPutchar(fd, 0xAB);
	serialPutchar(fd, 0x06);
	serialPutchar(fd, 0x02);

	serialFlush(fd);

	serialPutchar(fd, 0xAB);
	serialPutchar(fd, 0x02);
	while (1) {
		temp = 	serialDataAvail(fd);
		if (temp) {
	//		printf("check\n");
			printf("%d, ", serialGetchar(fd));	
		}
		temp = 0;
		delay(500);
	}
	/*
	while (1) {
		while (serialDataAvail(fd)) {
		printf("%d, ", serialGetchar(fd));	
		printf("%d, ", serialGetchar(fd));	
		printf("%d, ", serialGetchar(fd));	
		printf("%d, ", serialGetchar(fd));	
		}
	}
	while (1) {
		printf("check\n");
		printf("%d, ", serialGetchar(fd));	
		printf("%d, ", serialGetchar(fd));	
		printf("%d, ", serialGetchar(fd));	
		printf("%d, ", serialGetchar(fd));	
		puts("");
		delay(1000);
	}
	while (1) {
		serialPutchar(fd, 0xAB);
		serialPutchar(fd, 0x01);
		serialPutchar(fd, 0x03);
		serialPutchar(fd, 0x11);
		serialPutchar(fd, 0x22);
		serialPutchar(fd, 0x33);
		printf("end\n");

		delay(500);
	}
	*/
	serialClose(fd);

	return 0;
}

