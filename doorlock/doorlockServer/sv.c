#include <netinet/in.h>				/* constants and structures needed for internet domain address.	*/
#include <arm-linux-gnueabihf/sys/socket.h>
#include <arm-linux-gnueabihf/sys/types.h>	/* include ssize_t definition	*/
#include <unistd.h>				/* use the close function	*/
#include <stdio.h>
#include <string.h>				/* use the memset function	*/
#include <stdlib.h>				/* use the exit function	*/
#include <wiringPi.h>
#include <pthread.h>				/* use the Thread		*/

#define LED		29
#define	PORT		6000
#define BUF_LEN		256	
#define KEY01		8
#define KEY02		7
#define KEY03		0	
#define KEY04		3	
#define CONNECT		13
#define SENSE		21	
#define READY		23
#define OPEN		25
#define TRIG		12
#define	ECHO		14
#define MOTOR01		9
#define	MOTOR02		2
#define	LSWITCH		22
#define	BUTTON		24
#define	HIGH		1
#define	LOW		0
#define	TRUE		1
#define	FALSE		0
#define KEY		4
#define	COUNT		6
#define	DELAY		500
#define	MOTOR_DELAY	400
#define	INTRO_DELAY	2000
#define MSG_CONNECT	"C\r\n"
#define MSG_SENSE	"S\r\n"
#define MSG_OPEN	"O\r\n"
#define MSG_CLOSE	"CS\r\n"
#define WAIT		1
#define PATTERN		2
#define	AGREEMENT	3
#define	SETTING		4

void init();
void allOn();
void allOff();
double ultrasonicSensor();
void doorlockOpen();
void doorlockClose();
void *readMsg(void *);

struct	sockaddr_in	serverAddress, clientAddress;
int			handleServerSocket, handleClientSocket;
int			clientAddressSize, messageSize, socketOption;
int			isSocket = TRUE, isConnect = FALSE, threadState;
char			buffer[BUF_LEN], door, sensorCount = 0;
pthread_t		thread;

int main(int argc, char *argv[])
{
	int	i, j, k;
	int	button, status, lsCount;
	
	init();
	while (TRUE) {
		printf("While start!\n");

		allOn();
		if ((handleClientSocket = accept(handleServerSocket, (struct sockaddr *) &clientAddress, &clientAddressSize)) < 0) {
			printf("accept() Error!\n");
			return (-1);
		}
		printf("HandleClientSocket : %d\n", handleClientSocket);

		isSocket = TRUE;
		allOff();
		if (isConnect) {
			printf("isConnect\n");
			isConnect = FALSE;
		} else {
			printf("!isConnect\n");

			digitalWrite(CONNECT, HIGH);
			write(handleClientSocket, MSG_CONNECT, sizeof(MSG_CONNECT) - 1);


			delay(2000);

			button = FALSE;
			door = FALSE;
			sensorCount = 0;
			lsCount = 0;
			status = WAIT;
		}
		printf("Thread start!\n");
		threadState = pthread_create(&thread, NULL, readMsg, NULL);

		printf("isSocket%d\n", isSocket);

		while (isSocket) {	
/*
			if (digitalRead(BUTTON) == LOW) {
				if (door) {
					doorlockOpen();
					if (status != AGREEMENT) {
						button = TRUE;
						status = AGREEMENT;
					}
				} else {
					doorlockClose();
					if (status != WAIT) {
						button = TRUE;
						status = WAIT;
					}
				}
			} else if (digitalRead(LSWITCH) == HIGH) {
				lsCount++;
				delay(DELAY);
				if ((lsCount == COUNT) && !door) {
					lsCount = 0;
					doorlockClose();
					if (status != WAIT) {
						button = TRUE;
						status = WAIT;
					}
				}
			} else {
				lsCount = 0;
			}
*/
			switch (status) {
				case WAIT:
/*
					if (button) {
						button = FALSE;
						write(handleClientSocket, MSG_CONNECT, sizeof(MSG_CONNECT) - 1);
					}
					ultrasonicSensor();
					if (sensorCount > COUNT) {
						write(handleClientSocket, MSG_SENSE, sizeof(MSG_SENSE) - 1);
						sensorCount = 0;
						status = PATTERN;
					}
*/
					break;
				case PATTERN:
					break;
				case AGREEMENT:
					if (button) {
						button = FALSE;
						write(handleClientSocket, MSG_OPEN, sizeof(MSG_OPEN) - 1);
					}		
					break;
				case SETTING:
					break;
				default:
					break;
			}
		}
		printf("close clientSocket\n");

		close(handleClientSocket);
	}
	close(handleServerSocket);
	
	return 0;
}

void init()
{
	if (wiringPiSetup() == (-1)) {

		exit(-1);
	}
	pinMode(KEY01, OUTPUT);
	pinMode(KEY02, OUTPUT);
	pinMode(KEY03, OUTPUT);
	pinMode(KEY04, OUTPUT);
	pinMode(CONNECT, OUTPUT);
	pinMode(SENSE, OUTPUT);
	pinMode(READY, OUTPUT);
	pinMode(OPEN, OUTPUT);
	pinMode(MOTOR01, OUTPUT);
	pinMode(MOTOR02, OUTPUT);
	pinMode(LSWITCH, INPUT);
	pinMode(BUTTON, INPUT);
	pinMode(TRIG, OUTPUT);
	pinMode(ECHO, INPUT);
	digitalWrite(MOTOR01, LOW);
	digitalWrite(MOTOR02, LOW);
	if ((handleServerSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("socket() Error!\n");
		
		exit(-1);
	}
	memset(&serverAddress, 0x00, sizeof(serverAddress));	/* initializing the server socket to null		*/
	serverAddress.sin_family	= AF_INET;		/* set a address family(AF_INET : IPv4 Protocol)	*/
	serverAddress.sin_addr.s_addr	= htonl(INADDR_ANY);	/* convert long integer data to network byte order	*/
	serverAddress.sin_port		= htons(PORT);		/* convert short integer data to network byte order	*/
	socketOption = 1;
	setsockopt(handleServerSocket, SOL_SOCKET, SO_REUSEADDR, &socketOption, sizeof(socketOption));	/* fix bind() error */
	if (bind(handleServerSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
		printf("bind() Error!\n");

		exit(-1);
	}
	if (listen(handleServerSocket, 5) < 0) {
		printf("listen() Error!\n");

		exit(-1);
	}
	clientAddressSize = sizeof(clientAddress);

	return;
}

void allOn()
{
	digitalWrite(KEY01, HIGH);
	digitalWrite(KEY02, HIGH);
	digitalWrite(KEY03, HIGH); 
	digitalWrite(KEY04, HIGH); 
	digitalWrite(CONNECT, HIGH);
	digitalWrite(SENSE, HIGH);
	digitalWrite(READY, HIGH);
	digitalWrite(OPEN, HIGH);
	
	return;
}

void allOff()
{
	digitalWrite(KEY01, LOW);
	digitalWrite(KEY02, LOW);
	digitalWrite(KEY03, LOW);
	digitalWrite(KEY04, LOW);
	digitalWrite(CONNECT, LOW);
	digitalWrite(SENSE, LOW);
	digitalWrite(READY, LOW);
	digitalWrite(OPEN, LOW);
	
	return;
}

double ultrasonicSensor()
{
	int startTime, endTime, size;
	double distance;
	
	digitalWrite(TRIG, LOW);
	delay(DELAY);
	digitalWrite(TRIG, HIGH);
	delayMicroseconds(10);
	digitalWrite(TRIG, LOW);
	while (digitalRead(ECHO) == LOW) {
		;
	}
	startTime = micros();
	while (digitalRead(ECHO) == HIGH) {
		;
	}
	endTime = micros();

	distance = (endTime - startTime) / (29.0 * 2.0);
	if (distance <= 30.0) {
		sensorCount++;
	}
	size = sprintf(buffer, "%lf\r\n", distance);
	write(handleClientSocket, buffer, size - 1);
}

void doorlockOpen()
{
	digitalWrite(MOTOR01, HIGH);
	digitalWrite(MOTOR02, LOW);
	delay(MOTOR_DELAY);
	digitalWrite(MOTOR01, LOW);
	digitalWrite(MOTOR02, LOW);
	door = FALSE;

	return;
}

void doorlockClose()
{
	digitalWrite(MOTOR01, LOW);
	digitalWrite(MOTOR02, HIGH);
	delay(MOTOR_DELAY);
	digitalWrite(MOTOR01, LOW);
	digitalWrite(MOTOR02, LOW);
	door = TRUE;

	return;
}

void *readMsg(void *parameter)
{
	while (TRUE) {

		messageSize = read(handleClientSocket, buffer, sizeof(buffer));

		printf("message, handle : %d %d\n", messageSize, handleClientSocket);
		printf("buffer, sizeof : %s %d\n", buffer, sizeof(buffer));

		if (messageSize < 0) {
			printf("read() Error!\n");
			
			exit(-1);
		} else {
			buffer[(messageSize > (BUF_LEN - 1)) ? (BUF_LEN - 1) : messageSize] = '\0';
			printf("read : %s\n", buffer);
			if (!strcmp(buffer, "continue")) {
				isConnect = TRUE;
				isSocket = FALSE;
				return;
			}
		}
	}
}
