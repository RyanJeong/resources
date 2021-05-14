/*  각 패킷에 들어가는 값들은 함수를 통해 설정하며, 함수의 마지막 부분에 isReady 값을 enable 시킴으로써 통신   */

#include <pthread.h>
/*  headers for socket communication    */
#include <errno.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <net/if.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
/*  size_t  */
#include <sys/types.h>  
#include <sys/ioctl.h>
#include <unistd.h>
/*  end */
#include "rf.h"
#include "util.h"
#include "server.h"

/*
 *  msg[SIZE](process.c)        : debug message를 담는 변수
 *  sockfd(server.c)            : socket 통신에 사용하는 file descriptor
 *  host(server.c)              : host 정보를 갖는 구조체
 *  addr(server.c)              : 주소 정보를 갖는 구조체
 *  isConnect(server.c)         : 서버와의 접속 상태를 나타내는 변수, 1이 연결, 0이 연결 끊김
 *  pThreadNameRecv[](server.c) : recv 역할을 수행하는 스레드의 이름
 *  pThreadNameSend[](server.c) : send 역할을 수행하는 스레드의 이름
 *  pThreadRecv(server.c)       : recv 역할을 수행하는 스레드 정보를 갖는 변수
 *  pThreadSend(server.c)       : send 역할을 수행하는 스레드 정보를 갖는 변수
 *  pThreadIdRecv(server.c)     : recv 역할을 수행하는 스레드의 ID 번호
 *  pThreadIdSend(server.c)     : send 역할을 수행하는 스레드의 ID 번호
 *  threadStatus(server.c)      : pThread_join 동작을 수행할 때 status를 저장할 변수
 *  gatewayMACAddr(server.c)    : 해당 Gateway의 MAC 주소를 저장하는 변수
 *  packet(server.c)            : server에 전송할 packet을 담는 변수
 *  isReady(server.c)           : server로 전송할 packet의 준비가 다 되면 1로 설정, frame 전송 또는 아직 frame 준비가 안 되었다면 0
 */

/*  TODO: test 명령 수신 하면 각 ID 내부 자료구조에 값을 반영할 것  */
extern char                 msg[SIZE];
static int                  sockfd;
static struct hostent       *host;
static struct sockaddr_in   addr;
static size_t               isConnect   = 0;
static const char           pThreadNameRecv[]   = THREAD_RECV;
static const char           pThreadNameSend[]   = THREAD_SEND;
static pthread_t            pThreadRecv, pThreadSend;
static size_t               pThreadIdSend, pThreadIdRecv, threadStatus;
static char                 gatewayMACAddr[SIZE_STR_MAC];
static unsigned char        packet[SIZE];
static size_t               isReady;

int testTest;

size_t getMACAddrForPacket(void)
{
    static const char *func = SERVER_GETMACADDRFORPACKET;
    int     fd;
    struct  ifreq ifr;

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == (-1)) {
        putsForDebug(__SERVER__, func, MSG_ERROR_GET_MAC);

        return 1;
    }
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ - 1);
    ioctl(fd, SIOCGIFHWADDR, &ifr);
    close(fd);
    sprintf(gatewayMACAddr, "%02X%02X%02X%02X%02X%02X", 
        (unsigned char) (*(ifr.ifr_hwaddr.sa_data + 0)),
        (unsigned char) (*(ifr.ifr_hwaddr.sa_data + 1)),
        (unsigned char) (*(ifr.ifr_hwaddr.sa_data + 2)),
        (unsigned char) (*(ifr.ifr_hwaddr.sa_data + 3)),
        (unsigned char) (*(ifr.ifr_hwaddr.sa_data + 4)),
        (unsigned char) (*(ifr.ifr_hwaddr.sa_data + 5)));
    sprintf(msg, "Gateway's MAC address: %s", gatewayMACAddr);
    putsForDebug(__SERVER__, func, msg);
    /*  Set packet's MAC address field  */
    *(packet + 0) = PACKET_SIZE_WITHOUT_DATAS;
    memcpy((packet + 1), ifr.ifr_hwaddr.sa_data, SIZE_MAC);
    *(packet + 7) = CMD_PACKET_ADD_A_GATEWAY;
    memset((packet + 8), '\0', SIZE_SERIAL_ID);
    /*  TODO: CRC 구현 필요 */
    /*  이 경우만 예외로, 길이를 제외한 index 1번부터 7번까지 CRC를 계산하고, 다른 패킷들은 Serial ID를 포함하여 CRC 계산    */
    *(packet + 17) = (unsigned char) 0xFF;  
    isReady = 1;
     
    return 0;
}

/*  Packet에서 MAC address 부분은 입력이 되어 있으므로, 그대로 값을 두고 다른 값들만 변경   */
void getSerialIDForPacket(uint8_t *id)
{
    static const char *func = SERVER_GETSERIALIDFORPACKET;

    *(packet + 0) = PACKET_SIZE_WITHOUT_DATAS;
    /*  MAC address 부분은 한번 입력해 두고 값 변경하지 않음    */
    *(packet + 7) = CMD_PACKET_ADD_A_THING;
    memcpy((packet + 8), id, SIZE_SERIAL_ID);
    /*  TODO: CRC 구현 필요 */
    *(packet + 17) = (unsigned char) 0xFF;
    isReady = 1;

    return;
}

/*  Packet에서 MAC address 부분은 입력이 되어 있으므로, 그대로 값을 두고 다른 값들만 변경   */
void getAverageDataForPacket(uint8_t *id, uint8_t data)
{
    static const char *func = SERVER_GETAVERAGEDATAFORPACKET;

    *(packet + 0) = PACKET_SIZE_WITH_A_DATA;
    /*  MAC address 부분은 한번 입력해 두고 값 변경하지 않음    */
    *(packet + 7) = CMD_PACKET_SEND_A_DATA;
    memcpy((packet + 8), id, SIZE_SERIAL_ID);
    *(packet + 17) = data;
    /*  TODO: CRC 구현 필요 */
    *(packet + 18) = (unsigned char) 0xFF;
    isReady = 1;

    return;
}

/*  Packet에서 MAC address 부분은 입력이 되어 있으므로, 그대로 값을 두고 다른 값들만 변경   */
/*  value1: 실제 센서 값, value2: 테스트 센서 값    */
void getTestResultsForPacket(uint8_t *id, uint8_t cmd, uint8_t value1, uint8_t value2)
{
    static const char *func = SERVER_GETTESTRESULTSFORPACKET;

    *(packet + 0) = PACKET_SIZE_WITH_TWO_DATAS;
    /*  MAC address 부분은 한번 입력해 두고 값 변경하지 않음    */
    /*  TODO: 추가 해야 함  */
    switch (cmd) {
    case CMD_SEND_INIT_TEST_RESULT:
        *(packet + 7) = CMD_PACKET_SEND_INIT_TEST_RESULT;
        break;
    case CMD_SEND_INIT_HEAT_TEST_RESULT1:
        *(packet + 7) = CMD_PACKET_SEND_INIT_HEAT_TEST_RESULT1;
        break;
    case CMD_SEND_INIT_HEAT_TEST_RESULT2:
        *(packet + 7) = CMD_PACKET_SEND_INIT_HEAT_TEST_RESULT2;
        break;
    case CMD_SEND_INIT_SMOKE_TEST_RESULT1:
        *(packet + 7) = CMD_PACKET_SEND_INIT_SMOKE_TEST_RESULT1;
        break;
    case CMD_SEND_INIT_SMOKE_TEST_RESULT2:
        *(packet + 7) = CMD_PACKET_SEND_INIT_SMOKE_TEST_RESULT2;
        break;
    case CMD_SEND_INIT_SMOKE_TEST_RESULT3:
        *(packet + 7) = CMD_PACKET_SEND_INIT_SMOKE_TEST_RESULT3;
        break;
    case CMD_SEND_INIT_SMOKE_TEST_RESULT4:
        *(packet + 7) = CMD_PACKET_SEND_INIT_SMOKE_TEST_RESULT4;
        break;
    }
    memcpy((packet + 8), id, SIZE_SERIAL_ID);
    *(packet + 17) = value1;
    *(packet + 18) = value2;
    /*  TODO: CRC 구현 필요 */
    *(packet + 19) = (unsigned char) 0xFF;
    isReady = 1;

    return;
}

/*  Packet에서 MAC address 부분은 입력이 되어 있으므로, 그대로 값을 두고 다른 값들만 변경   */
void getFireDetectionDataForPacket(uint8_t *id, uint8_t data)
{
    static const char *func = SERVER_GETFIREDETECTIONDATAFORPACKET;

    *(packet + 0) = PACKET_SIZE_WITH_A_DATA;
    /*  MAC address 부분은 한번 입력해 두고 값 변경하지 않음    */
    *(packet + 7) = CMD_PACKET_ALERT_A_FIRE_DETECTED;
    memcpy((packet + 8), id, SIZE_SERIAL_ID);
    *(packet + 17) = data;
    /*  TODO: CRC 구현 필요 */
    *(packet + 18) = (unsigned char) 0xFF;
    isReady = 1;

    return;
}

size_t initSocket(void)
{
    static const char *func = SERVER_INITSOCKET;

    if ((host = gethostbyname(HOST)) == NULL) {
        putsForDebug(__SERVER__, func, MSG_ERROR_HOST);

        return 1;
    }
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        putsForDebug(__SERVER__, func, MSG_ERROR_HOST);

        return 1;
    }
    /* host byte order */
    addr.sin_family = AF_INET;      
    /* short, network byte order */
    addr.sin_port   = htons(PORT);  
    addr.sin_addr   = *((struct in_addr *) host->h_addr);
    /* zero the rest of the struct */
    bzero(&(addr.sin_zero), 8);     
    if (connect(sockfd, (struct sockaddr *) &addr, sizeof(struct sockaddr)) == -1) {
        putsForDebug(__SERVER__, func, MSG_ERROR_CONNECT);

        return 1;
    }
    isConnect = 1;

    return 0;
}

void closeSocket(void)
{
    close(sockfd);
    isConnect = 0;

    return;
}

size_t setupThread(void)
{
    static const char *func = SERVER_SETUPTHREAD;

    pThreadIdRecv   = pthread_create(&pThreadRecv, NULL, recvFromServer, (void *) pThreadNameRecv);
    pThreadIdSend   = pthread_create(&pThreadSend, NULL, sendToServer, (void *) pThreadNameSend);
    if ((pThreadIdRecv < 0) || (pThreadIdSend < 0)) {
        putsForDebug(__SERVER__, func, MSG_ERROR_SETUPTHREAD);

        return 1;
    } else {
        putsForDebug(__SERVER__, func, MSG_COMPLETE_SETUPTHREAD);

        return 0;
    }
}

size_t sendPacket(void)
{
    static const char *func = SERVER_SENDTOSERVER;
    int len;

    if (isConnect) {
        if (isReady) {
            if (send(sockfd, packet, *packet, 0) == -1) {
                putsForDebug(__SERVER__, func, MSG_ERROR_SEND);
            } else {
                isReady = 0;
            }
        }
    }

    return 0;
}

//  TODO: need to implementation
void *recvFromServer(void *arg)
{
    static const char   *func       = SERVER_RECVFROMSERVER;
    char *threadName;

    threadName = (char *) arg;
    putsForDebug(__SERVER__, func, MSG_THREAD_START);
    while (1) {
        /*  sleep(A_SECOND);    */
        scanf("%d", &testTest);
    }

    return NULL;
}

void *sendToServer(void *arg)
{
    static const char   *func       = SERVER_SENDTOSERVER;
    char   *threadName;
    size_t  len;

    putsForDebug(__SERVER__, func, MSG_THREAD_START);
    threadName = (char *) arg;
    while (1) {
        sendPacket();
        sleep(A_SECOND);
    }

    return NULL;
}

size_t destroyThread(void)
{
    static const char *func = SERVER_DESTROYTHREAD;

    if (pthread_join(pThreadRecv, (void **) &threadStatus)) {
        putsForDebug(__SERVER__, func, MSG_ERROR_DESTROYTHREAD);

        return 1;
    }
    pthread_join(pThreadSend, (void **) &threadStatus);
    if (pthread_join(pThreadSend, (void **) &threadStatus)) {
        putsForDebug(__SERVER__, func, MSG_ERROR_DESTROYTHREAD);

        return 1;

    }
    putsForDebug(__SERVER__, func, MSG_COMPLETE_DESTROYTHREAD);

    return 0;
}
