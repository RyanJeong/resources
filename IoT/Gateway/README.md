# RaspberryPi_KEM<br>
* Gateway program using Raspberry Pi<br>
```Text
$ ssh -x pi@192.168.0.10 -p 22
```

## 주요 기능<br>
1. 화재 감지<br>
2. 테스트 결과 반환<br>
3. 테스트 명령 수신<br>
4. 데이터 전송<br>

## 주의사항<br>
1. ~~절때 각 객체들을 근접하게 배치하지 말 것.~~<br>
2. ~~주변에 전자파를 일으키는 물체와는 충분한 거리를 확보하여 운용할 것.~~<br>
3. ~~RF Module을 어떻게 배치하느냐에 따라 작동이 될 수도, 안 될 수도 있음.~~<br>
4. ~~RF Module을 똑바로 세워 사용할 경우 가장 잘 동작됨.~~<br>
*   상기 문제들은 보드들의 납땜 문제로 밝혀짐.<br>

*   **RF Module을 사용할 경우, thing에서는 메세지를 전송하고 500 (ms) 간격을 준 후 read 명령을 수행해야 하고, gateway에서는 각 frame을 처리하는 간격을 200 (ms) 주어야 함**<br>

*   RF Module을 사용하면서 명령을 한 번 전송하고 곧바로 전송하고자 할 때, 200 (ms) 정도의 간격을 주고 사용해야 함<br>
    *   현재 Arduino 환경에서 SoftwareSerial + Serial 환경에서 테스트 했을 때 175 (ms) 간격을 두고 사용했을 때 명령이 모두 전송되었음<br>
    *   실제 사용하는 환경(SoftwareSerial을 제거하고 Serial만 사용)에서는 간격을 조금 더 줄일 수 있음<br>
        *   이 부분은 테스트가 필요함<br>
    *   또한 Raspberry Pi에서 명령을 두 번 전송할 시에 필요한 간격이 다를 수 있음<br>
        *   이 부분은 테스트가 필요함<br>

*   RF Module을 실시간 운용(e.g. Gateway)할 경우, 8-byte 단위로 끊어 읽는 것을 확인함
    *   즉, Arduino에서 frame을 전송할 때 8-byte를 넘기지 않을 경우 frame이 분리될 경우는 없음<br>
    *   또한, Arduino에서 8-byte 이내의 frame을 전송할 경우 frame은 깨지지 않고 그대로 buffer에 저장됨<br>

*   수신 테스트 pseudo-code<br>
```C
    char a[13], b[6];
    
    send(a);
    delay(175);
    send(b);

    end;
```

*   단순히 수신 동작만 수행할 경우<br>
```Text
[2019-12-01 00:49:09][process.c][init(void)]              : 
[2019-12-01 00:49:09][process.c][init(void)]              : 
[2019-12-01 00:49:09][process.c][init(void)]              : 
[2019-12-01 00:49:09][process.c][init(void)]              : Start! Mode = alpha, Debug message at log.txt
[2019-12-01 00:49:09][rf.c][setupUART(void)]              : Setup UART has been completed.
[2019-12-01 00:49:09][rf.c][setupRF(void)]                : Setup RF has been completed.
[2019-12-01 00:49:09][server.c][setupThread(void)]        : Successfully created 'recvFromServer' thread
[2019-12-01 00:49:09][server.c][recvFromServer1(void)]    : Thread start!
[2019-12-01 00:49:09][process.c][init(void)]              : Init process has been completed.
[2019-12-01 00:49:09][process.c][process(void)]           : Start process().
[2019-12-01 00:49:11][rf.c][recvAndSendMsg1(void)]        : 8 bytes read.
[2019-12-01 00:49:11][rf.c][recvAndSendMsg1(void)]        : RX index 0: a
[2019-12-01 00:49:11][rf.c][recvAndSendMsg1(void)]        : RX index 1: ee
[2019-12-01 00:49:11][rf.c][recvAndSendMsg1(void)]        : RX index 2: e0
[2019-12-01 00:49:11][rf.c][recvAndSendMsg1(void)]        : RX index 3: ee
[2019-12-01 00:49:11][rf.c][recvAndSendMsg1(void)]        : RX index 4: aa
[2019-12-01 00:49:11][rf.c][recvAndSendMsg1(void)]        : RX index 5: ab
[2019-12-01 00:49:11][rf.c][recvAndSendMsg1(void)]        : RX index 6: 0
[2019-12-01 00:49:11][rf.c][recvAndSendMsg1(void)]        : RX index 7: 8
[2019-12-01 00:49:11][rf.c][recvAndSendMsg1(void)]        : 3 bytes read.
[2019-12-01 00:49:11][rf.c][recvAndSendMsg1(void)]        : RX index 0: 7
[2019-12-01 00:49:11][rf.c][recvAndSendMsg1(void)]        : RX index 1: 9
[2019-12-01 00:49:11][rf.c][recvAndSendMsg1(void)]        : RX index 2: ea
[2019-12-01 00:49:11][rf.c][recvAndSendMsg1(void)]        : 4 bytes read.
[2019-12-01 00:49:11][rf.c][recvAndSendMsg1(void)]        : RX index 0: 3
[2019-12-01 00:49:11][rf.c][recvAndSendMsg1(void)]        : RX index 1: ee
[2019-12-01 00:49:11][rf.c][recvAndSendMsg1(void)]        : RX index 2: e0
[2019-12-01 00:49:11][rf.c][recvAndSendMsg1(void)]        : RX index 3: ea
[2019-12-01 00:49:13][rf.c][recvAndSendMsg1(void)]        : 8 bytes read.
[2019-12-01 00:49:13][rf.c][recvAndSendMsg1(void)]        : RX index 0: a
[2019-12-01 00:49:13][rf.c][recvAndSendMsg1(void)]        : RX index 1: ee
[2019-12-01 00:49:13][rf.c][recvAndSendMsg1(void)]        : RX index 2: e0
[2019-12-01 00:49:13][rf.c][recvAndSendMsg1(void)]        : RX index 3: ee
[2019-12-01 00:49:13][rf.c][recvAndSendMsg1(void)]        : RX index 4: aa
[2019-12-01 00:49:13][rf.c][recvAndSendMsg1(void)]        : RX index 5: ab
[2019-12-01 00:49:13][rf.c][recvAndSendMsg1(void)]        : RX index 6: 0
[2019-12-01 00:49:13][rf.c][recvAndSendMsg1(void)]        : RX index 7: 8
[2019-12-01 00:49:13][rf.c][recvAndSendMsg1(void)]        : 3 bytes read.
[2019-12-01 00:49:13][rf.c][recvAndSendMsg1(void)]        : RX index 0: 7
[2019-12-01 00:49:13][rf.c][recvAndSendMsg1(void)]        : RX index 1: 9
[2019-12-01 00:49:13][rf.c][recvAndSendMsg1(void)]        : RX index 2: ea
[2019-12-01 00:49:13][rf.c][recvAndSendMsg1(void)]        : 4 bytes read.
[2019-12-01 00:49:13][rf.c][recvAndSendMsg1(void)]        : RX index 0: 3
[2019-12-01 00:49:13][rf.c][recvAndSendMsg1(void)]        : RX index 1: ee
[2019-12-01 00:49:13][rf.c][recvAndSendMsg1(void)]        : RX index 2: e0
[2019-12-01 00:49:13][rf.c][recvAndSendMsg1(void)]        : RX index 3: ea
[2019-12-01 00:49:15][rf.c][recvAndSendMsg1(void)]        : 8 bytes read.
[2019-12-01 00:49:15][rf.c][recvAndSendMsg1(void)]        : RX index 0: a
[2019-12-01 00:49:15][rf.c][recvAndSendMsg1(void)]        : RX index 1: ee
[2019-12-01 00:49:15][rf.c][recvAndSendMsg1(void)]        : RX index 2: e0
[2019-12-01 00:49:15][rf.c][recvAndSendMsg1(void)]        : RX index 3: ee
[2019-12-01 00:49:15][rf.c][recvAndSendMsg1(void)]        : RX index 4: aa
[2019-12-01 00:49:15][rf.c][recvAndSendMsg1(void)]        : RX index 5: ab
[2019-12-01 00:49:15][rf.c][recvAndSendMsg1(void)]        : RX index 6: 0
[2019-12-01 00:49:15][rf.c][recvAndSendMsg1(void)]        : RX index 7: 8
[2019-12-01 00:49:15][rf.c][recvAndSendMsg1(void)]        : 3 bytes read.
[2019-12-01 00:49:15][rf.c][recvAndSendMsg1(void)]        : RX index 0: 7
[2019-12-01 00:49:15][rf.c][recvAndSendMsg1(void)]        : RX index 1: 9
[2019-12-01 00:49:15][rf.c][recvAndSendMsg1(void)]        : RX index 2: ea
[2019-12-01 00:49:15][rf.c][recvAndSendMsg1(void)]        : 4 bytes read.
[2019-12-01 00:49:15][rf.c][recvAndSendMsg1(void)]        : RX index 0: 3
[2019-12-01 00:49:15][rf.c][recvAndSendMsg1(void)]        : RX index 1: ee
[2019-12-01 00:49:15][rf.c][recvAndSendMsg1(void)]        : RX index 2: e0
[2019-12-01 00:49:15][rf.c][recvAndSendMsg1(void)]        : RX index 3: ea
[2019-12-01 00:49:18][rf.c][recvAndSendMsg1(void)]        : 8 bytes read.
```

*   실제 운용에 사용되는 처리 과정들을 포함시켰을 경우<br>
```Text
[2019-12-01 00:43:25][process.c][init(void)]              : 
[2019-12-01 00:43:25][process.c][init(void)]              : 
[2019-12-01 00:43:25][process.c][init(void)]              : 
[2019-12-01 00:43:25][process.c][init(void)]              : Start! Mode = beta, Debug message at log.txt
[2019-12-01 00:43:25][rf.c][setupUART(void)]              : Setup UART has been completed.
[2019-12-01 00:43:25][rf.c][setupRF(void)]                : Setup RF has been completed.
[2019-12-01 00:43:25][server.c][setupThread(void)]        : Successfully created 'recvFromServer' thread
[2019-12-01 00:43:25][server.c][recvFromServer2(void)]    : Thread start!
[2019-12-01 00:43:25][process.c][init(void)]              : Init process has been completed.
[2019-12-01 00:43:25][process.c][process(void)]           : Start process().
[2019-12-01 00:43:26][rf.c][recvAndSendMsg2(void)]        : 8 bytes read.
[2019-12-01 00:43:26][crc.c][checkCRC(uint8_t *, size_t, u: index 0: 0a, crc: db
[2019-12-01 00:43:26][crc.c][checkCRC(uint8_t *, size_t, u: index 1: ee, crc: 30
[2019-12-01 00:43:26][crc.c][checkCRC(uint8_t *, size_t, u: index 2: e0, crc: 04
[2019-12-01 00:43:26][crc.c][checkCRC(uint8_t *, size_t, u: index 3: ee, crc: 1a
[2019-12-01 00:43:26][crc.c][checkCRC(uint8_t *, size_t, u: index 4: aa, crc: bf
[2019-12-01 00:43:26][crc.c][checkCRC(uint8_t *, size_t, u: index 5: ab, crc: 87
[2019-12-01 00:43:26][crc.c][checkCRC(uint8_t *, size_t, u: index 6: 00, crc: ed
[2019-12-01 00:43:26][crc.c][checkCRC(uint8_t *, size_t, u: index 7: 08, crc: 34
[2019-12-01 00:43:26][crc.c][updateCRC(uint8_t, uint8_t, u: len: 0a
[2019-12-01 00:43:26][crc.c][updateCRC(uint8_t, uint8_t, u: id: ee
[2019-12-01 00:43:26][crc.c][updateCRC(uint8_t, uint8_t, u: cmd: ef
[2019-12-01 00:43:26][crc.c][updateCRC(uint8_t, uint8_t, u: crc: 2a
[2019-12-01 00:43:26][rf.c][recvAndSendMsg2(void)]        : ----------------------------------------
[2019-12-01 00:43:26][rf.c][recvAndSendMsg2(void)]        : 3 bytes read.
[2019-12-01 00:43:26][crc.c][checkCRC(uint8_t *, size_t, u: index 0: 07, crc: 97
[2019-12-01 00:43:26][crc.c][checkCRC(uint8_t *, size_t, u: index 1: 09, crc: 26
[2019-12-01 00:43:26][crc.c][checkCRC(uint8_t *, size_t, u: index 2: ea, crc: 3a
[2019-12-01 00:43:26][crc.c][updateCRC(uint8_t, uint8_t, u: len: 07
[2019-12-01 00:43:26][crc.c][updateCRC(uint8_t, uint8_t, u: id: 09
[2019-12-01 00:43:26][crc.c][updateCRC(uint8_t, uint8_t, u: cmd: ef
[2019-12-01 00:43:26][crc.c][updateCRC(uint8_t, uint8_t, u: crc: cf
[2019-12-01 00:43:26][rf.c][recvAndSendMsg2(void)]        : ----------------------------------------
[2019-12-01 00:43:26][rf.c][recvAndSendMsg2(void)]        : 4 bytes read.
[2019-12-01 00:43:26][crc.c][checkCRC(uint8_t *, size_t, u: index 0: 03, crc: 53
[2019-12-01 00:43:26][crc.c][checkCRC(uint8_t *, size_t, u: index 1: ee, crc: f3
[2019-12-01 00:43:26][crc.c][checkCRC(uint8_t *, size_t, u: index 2: e0, crc: 10
[2019-12-01 00:43:26][crc.c][checkCRC(uint8_t *, size_t, u: index 3: ea, crc: 59
[2019-12-01 00:43:26][crc.c][updateCRC(uint8_t, uint8_t, u: len: 03
[2019-12-01 00:43:26][crc.c][updateCRC(uint8_t, uint8_t, u: id: ee
[2019-12-01 00:43:26][crc.c][updateCRC(uint8_t, uint8_t, u: cmd: ef
[2019-12-01 00:43:26][crc.c][updateCRC(uint8_t, uint8_t, u: crc: 3e
[2019-12-01 00:43:26][rf.c][recvAndSendMsg2(void)]        : ----------------------------------------
[2019-12-01 00:43:28][rf.c][recvAndSendMsg2(void)]        : 8 bytes read.
[2019-12-01 00:43:28][crc.c][checkCRC(uint8_t *, size_t, u: index 0: 0a, crc: db
[2019-12-01 00:43:28][crc.c][checkCRC(uint8_t *, size_t, u: index 1: ee, crc: 30
[2019-12-01 00:43:28][crc.c][checkCRC(uint8_t *, size_t, u: index 2: e0, crc: 04
[2019-12-01 00:43:28][crc.c][checkCRC(uint8_t *, size_t, u: index 3: ee, crc: 1a
[2019-12-01 00:43:28][crc.c][checkCRC(uint8_t *, size_t, u: index 4: aa, crc: bf
[2019-12-01 00:43:28][crc.c][checkCRC(uint8_t *, size_t, u: index 5: ab, crc: 87
[2019-12-01 00:43:28][crc.c][checkCRC(uint8_t *, size_t, u: index 6: 00, crc: ed
[2019-12-01 00:43:28][crc.c][checkCRC(uint8_t *, size_t, u: index 7: 08, crc: 34
[2019-12-01 00:43:28][crc.c][updateCRC(uint8_t, uint8_t, u: len: 0a
[2019-12-01 00:43:28][crc.c][updateCRC(uint8_t, uint8_t, u: id: ee
[2019-12-01 00:43:28][crc.c][updateCRC(uint8_t, uint8_t, u: cmd: ef
[2019-12-01 00:43:28][crc.c][updateCRC(uint8_t, uint8_t, u: crc: 2a
[2019-12-01 00:43:28][rf.c][recvAndSendMsg2(void)]        : ----------------------------------------
```

*   상기 문제 해결 방안으로는, 간격을 두어 frame이 분리되지 않게 하는 방법이 있음<br>
    *   **그러나, 현재 Arduino에 frame이 8-byte 이내로 전송되도록 구성되어 있기 때문에, 해당 issue를 굳이 해결할 필요는 없음**<br>

*   실제 운용에 1초 간격을 두면서 사용되는 처리 과정들을 포함시켰을 경우<br>
```Text
[2019-12-01 00:53:58][process.c][init(void)]              : 
[2019-12-01 00:53:58][process.c][init(void)]              : 
[2019-12-01 00:53:58][process.c][init(void)]              : 
[2019-12-01 00:53:58][process.c][init(void)]              : Start! Mode = beta, Debug message at log.txt
[2019-12-01 00:53:58][rf.c][setupUART(void)]              : Setup UART has been completed.
[2019-12-01 00:53:58][rf.c][setupRF(void)]                : Setup RF has been completed.
[2019-12-01 00:53:58][server.c][setupThread(void)]        : Successfully created 'recvFromServer' thread
[2019-12-01 00:53:58][server.c][recvFromServer2(void)]    : Thread start!
[2019-12-01 00:53:58][process.c][init(void)]              : Init process has been completed.
[2019-12-01 00:53:58][process.c][process(void)]           : Start process().
[2019-12-01 00:53:59][rf.c][recvAndSendMsg2(void)]        : 15 bytes read.
[2019-12-01 00:53:59][crc.c][checkCRC(uint8_t *, size_t, u: index 0: 0a, crc: db
[2019-12-01 00:53:59][crc.c][checkCRC(uint8_t *, size_t, u: index 1: ee, crc: 30
[2019-12-01 00:53:59][crc.c][checkCRC(uint8_t *, size_t, u: index 2: e0, crc: 04
[2019-12-01 00:53:59][crc.c][checkCRC(uint8_t *, size_t, u: index 3: ee, crc: 1a
[2019-12-01 00:53:59][crc.c][checkCRC(uint8_t *, size_t, u: index 4: aa, crc: bf
[2019-12-01 00:53:59][crc.c][checkCRC(uint8_t *, size_t, u: index 5: ab, crc: 87
[2019-12-01 00:53:59][crc.c][checkCRC(uint8_t *, size_t, u: index 6: 00, crc: ed
[2019-12-01 00:53:59][crc.c][checkCRC(uint8_t *, size_t, u: index 7: 08, crc: 34
[2019-12-01 00:53:59][crc.c][checkCRC(uint8_t *, size_t, u: index 8: 07, crc: 96
[2019-12-01 00:53:59][crc.c][checkCRC(uint8_t *, size_t, u: index 9: 09, crc: 17
[2019-12-01 00:53:59][crc.c][checkCRC(uint8_t *, size_t, u: index 10: ea, crc: ce
[2019-12-01 00:53:59][crc.c][checkCRC(uint8_t *, size_t, u: index 11: 03, crc: 0b
[2019-12-01 00:53:59][crc.c][checkCRC(uint8_t *, size_t, u: index 12: ee, crc: 34
[2019-12-01 00:53:59][crc.c][checkCRC(uint8_t *, size_t, u: index 13: e0, crc: c0
[2019-12-01 00:53:59][crc.c][checkCRC(uint8_t *, size_t, u: index 14: ea, crc: 5d
[2019-12-01 00:53:59][crc.c][updateCRC(uint8_t, uint8_t, u: len: 0a
[2019-12-01 00:53:59][crc.c][updateCRC(uint8_t, uint8_t, u: id: ee
[2019-12-01 00:53:59][crc.c][updateCRC(uint8_t, uint8_t, u: cmd: ef
[2019-12-01 00:53:59][crc.c][updateCRC(uint8_t, uint8_t, u: crc: 2a
[2019-12-01 00:53:59][rf.c][recvAndSendMsg2(void)]        : ----------------------------------------
```

## RF 특징
* RF Module 내에는 세 가지 상태(Rx, Tx, Off)를 가질 수 있음<br>
* RF Off 상태에서 Rx는 사용할 수 없음<br>
* RF Off 상태에서 Tx를 사용할 경우 상태가 Off에서 Tx로 변경되어 Tx 동작을 수행하고, 동작을 마친 후에는 Off 상태로 되돌아가는 것이 아닌 Rx 상태가 됨<br>
* RF Off 상태에서 Rx를 사용하고자 할 경우, RF On Command를 사용하여 RF를 직접 킨 후에 사용해야 함<br>
* Response Time and Current Consumption<br>

| Mode | P_TX mode | P_RX mode | Current in mode|
|------|-----------|-----------|----------------|
| P_TX mode | - | 3 (ms) | 35 (mA) @ + 10 (dBm) |
| P_RX mode | 3 (ms) | - | 16 (mA) |
| Module off mode | - | 1 (ms) | 1 (uA) |

## 차후 구현 내용<br>
* Thing으로부터 메세지를 수신받았을 때, ID마다 자료구조를 할당하여 내부에 저장해놓을 것<br>
* Server와의 Query 송수신 부<br>
* App으로부터의 접근 Interface<br>

## Communication Process<br>
1. Recv. message from the thing
2. Send Ack. to the thing

## 설정사항<br>
* Raspberry Pi는 기본적으로 콘솔이 UART를 사용하도록 설정되어 있다. 하지만 Arduino 등의 보드를 UART를 통해 제어할 경우, 제어 프로그램이 UART를 사용하려면 콘솔이 UART를 잡고 있지 않도록 설정해 줄 필요가 있다.	SD카드를 PC에 연결하고 오픈해보면 cmdline.txt 파일이 있을 것이다.(경로: /boot/cmdline.txt) 기본적으로 수정을 하지 않았다면 파일의 내용은 다음과 같을 것이다.<br>
```bash
dwc_otg.lpm_enable=0 console=ttyAMA0,115200 console=tty1 root=/dev/mmcblk0p2 rootfstype=ext4 elevator=deadline rootwait
```
* 여기서 "ttyAMA0"에 관련된 모든 파라미터를 삭제해주면 된다. 즉 위의 경우 'console=ttyAMA0,115200' 부분을 삭제한 후 파일을 저장해 주면 된다. 또한 /etc/inittab 파일도 수정해 줘야 한다. 에디터로 파일을 열고 'ttyAMA0' 를 검색하면 시리얼 포트를 지정하는 라인을 찾을 수 있다. 해당 라인의 맨 앞에 '#'을 추가해 주고 저장해 주면 된다. 이제 리부팅 하고 나면 UART핀은 다른 프로세스가 사용하거나, 또는 그 핀을 일반 GPIO 용도로 사용할 수 있게 된다.<br>
* 라즈베리안 Jessie 이상 버전에서는 inittab 을 사용하지 않고, systemd 를 사용한다. 따라서 systemd control 명령을 사용하여 비활성화를 해주도록 한다.<br>
```bash
$ sudo systemctl stop serial-getty@ttyAMA0.service
$ sudo systemctl disable serial-getty@ttyAMA0.service
```
* Jessie 에서는 이와 별도로 UART 를 사용하겠다고 설정을 추가해주어야 한다.<br>
```bash
$ sudo vi /boot/config.txt
enable_uart=1
```
* Raspberry PI 3 에서는 UART 디바이스 정보가 바뀌어서 아래와 같이 설정해준다.<br>
```bash
$ sudo systemctl stop serial-getty@ttyS0.service 
$ sudo systemctl disable serial-getty@ttyS0.service
```
출처: [땅뚱 창고](https://pinocc.tistory.com/185)<br>

### 요약<br>
1.	vi /boot/config./txt<br>
	dtoverlay=pi3-disable-bt
2.	sudo systemctl disable hciuart
3.	dmesg | grep tty
4.	RxD <=> Tx
	TxD <=> Rx
	rpi <=> UART Board	

## UART Setting on Raspberry Pi 3<br>
```c
// -------------------------
// ----- SETUP USART 0 -----
// -------------------------
// At bootup, pins 8 and 10 are already set to UART0_TXD, UART0_RXD 
// (ie the alt0 function) respectively


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

uart0_filestream = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY); //Open in non blocking read/write mode
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
options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;	//	Set baud rate
options.c_iflag = IGNPAR;
options.c_oflag = 0;
options.c_lflag = 0;
tcflush(uart0_filestream, TCIFLUSH);			//	flush buffer
tcsetattr(uart0_filestream, TCSANOW, &options);		//	9,600 bps, 8 bit data, no parity, 1 stop bit, non-blocking mode 
```

## RF Module<br>
```c
/*  
*  Pre-processing to use the RF Module
*  Channel(Frequency) Setting -> Tx Power Level Setting -> Data Rate Setting
*  
*  [P_TX Data] Command
*  Header[1](0xAB) + OP Code[1](0x01) + Length[1](0x01 ~ 0x3F) + Data[1 ~ 63](Length)
*    ex) [TX]: 0xAB0103112233 -> [RX]: 0x03112233
*  
*  [Module OFF] Command
*  Header[1](0xAB) + OP Code[1](0x02)
*  
*  [Module ON] Command
*  Header[1](0xAB) + OP Code[1](0x03)
*  
*  [Channel(Frequency) Setting] Command
*  - EEPROM에 저장되므로 자주 변경 시 메모리 수명에 영향을 줌
*  Header[1](0xAB) + OP Code[1](0x04) + Channel Group[1] + Channel Number[1]
*    Group   : 0x00 ~ 0x03 까지 설정 가능, 0x01 이상부터 447MHz
*    Number  : 0x00 ~ 0x18 까지 25개의 채널 중 하나 선택 가능
*    ----------------------------------------------------------------
*    Ch No     Ch Gr = 0x00  Ch Gr = 0x01  Ch Gr = 0x02  Ch Gr = 0x03
*    1(0x00)   424.7000      447.2625      447.6000      447.8625
*    2(0x01)   424.7125      447.2750      447.6125      447.8750
*    3(0x02)   424.7250      447.2875      447.6250      447.8875
*    4(0x03    424.7375      447.3000      447.6375      447.9000
*    5(0x04)   424.7500      447.3125      447.6500      447.9125
*    6(0x05)   424.7625      447.3250      447.6625      447.9250
*    7(0x06)   424.7750      447.3375      447.6750      447.9375
*    8(0x07)   424.7875      447.3500      447.6875      447.9500
*    9(0x08)   424.8000      447.3625      447.7000      447.9625
*    10(0x09)  424.8125      447.3750      447.7125      447.9750
*    11(0x0A)  424.8250      447.3875      447.7250      447.9875
*    12(0x0B)  424.8375      447.4000      447.7375      -
*    13(0x0C)  424.8500      447.4125      447.7500      -
*    14(0x0D)  424.8625      447.4250      447.7625      -
*    15(0x0E)  424.8750      447.4375      447.7750      -
*    16(0x0F)  424.8875      447.4500      447.7875      -
*    17(0x10)  424.9000      447.4625      447.8000      -
*    18(0x11)  424.9125      447.4750      447.8125      -
*    19(0x12)  424.9250      447.4875      447.8250      -
*    20(0x13)  424.9375      447.5000      447.8375      -
*    21(0x14)  424.9500      447.5125      447.8500      -
*    22(0x15)  -             447.5250      -             -
*    23(0x16)  -             447.5375      -             -
*    24(0x17)  -             447.5500      -             -
*    25(0x18)  -             447.5625      -             -
*    ----------------------------------------------------------------
*    
*  [Tx Power Level Setting] Command
*  - EEPROM에 저장되므로 자주 변경 시 메모리 수명에 영향을 줌
*  Header[1](0xAB) + OP Code[1](0x05) + Power Level[1](0x00 ~ 0x0A, Default: 0x0A(10 dBm))
*  
*  [Data Rate Setting] Command
*  - EEPROM에 저장되므로 자주 변경 시 메모리 수명에 영향을 줌
*  Header[1](0xAB) + OP Code[1](0x06) + Data Rate[1](0x01 ~ 0x04, Default: 0x02)
*    0x01: 0.6kbps
*    0x02: 1.2kbps
*    0x03: 2.4kbps
*    0x04: 4.8kbps
*  
*  [RSSI Read] Command
*  Header[1](0xAB) + OP Code[1](0x07)
*    Response data:
*      0xAB0705:  -5 dBm
*      0xAB0750: -80 dBm
*      
*  [TX Test] Command
*  Header[1](0xAB) + OP Code[1](0x08) + Mode[1](0x01 ~ 0x03)
*    0x01: carrier test 시작
*    0x02: 2 kHz modulation 시작
*    0x03: test 종료 후 P_RX Mode로 전환
*    
*  [BER Test] Command
*  Header[1](0xAB) + OP Code[1](0x09)
*    Response data: 
*      0xAB090100수
*        BER 1,000개 중 1kHz 데이터 갯수
*        
*  
*  [Configureation Read] Command
*  Header[1](0xAB) + OP Code[1](0x0A)
*    Response data:
*      0xAB0A01020304
*        0x01: Group
*        0x02: Channel
*        0x03: Data Rate
*        0x04: TX Power Level
*        
*  [Version] Command
*  Header[1](0xAB) + OP Code[1](0x0B)
*    Response data:
*      0xAB0B53312E3030(단말기 버전, S1.00)
*/
```
