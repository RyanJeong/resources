# Arduino_KEM README.md Backup<br>
* Thing program using Arduino<br>

## 주요 기능<br>
1. 화재 감지<br>
2. 테스트 결과 반환<br>
3. 테스트 명령 수신<br>
4. 데이터 전송<br>

## 주의사항<br>
### Feb 29, 2020<br>
*   테스트 도중, 오류를 발견하여 아래와 같이 정정한다. <br>
    *   Module Off Mode는 소비 전류가 1uA로 매우 작지만, 이 mode 로 진입하기 위한 시간 1 (ms)가 필요하므로, 사용자 시스템에 맞는 적절한 mode 전환이 필요하다. ...<br>
```C
/*  위 내용에 맞게 수정한 결과  */
#define ENABLE_RF               \
do {                            \
    Serial.print((char) 0xAB);  \
    Serial.print((char) 0x03);  \
} while (0)

#define DISABLE_RF              \
do {                            \
    Serial.print((char) 0xAB);  \
    Serial.print((char) 0x02);  \
    delay(1);                   \
} while (0)
```

*   RF module On, Off 명령어가 제대로 동작하는 것인지 확인 하기 위해 아래와 같이 코드를 작성하여 테스트를 수행하였다. 
```C
void loop() {
    DISABLE_RF;
    prepareFrame();
    sendAndRecv();
    parseFrame();
    sleep(MILSEC_500);
}

void sendAndRecv()
{
    int i, j;

    for (i = 0; i < 9; ++i) {
        Serial.print(TX_DATA[i]);
    }
    delay(1 << 9);
    for (j = 0; Serial.available(); ++j) {
        buff[j] = Serial.read();
    }
    for (i = 0; i < 6; ++i) {
        Serial.print(P_TX_DATA[i]);
    }
    delay(1 << 9);
    for (; Serial.available(); ++j) {
        buff[j] = Serial.read();
    }

    return;
}
```

*   **우선, sleep() 함수는 항상 맨 마지막에 위치해야 한다. Serial 통신과 근접해 있으면, Serial 통신에 영향을 주는 것을 확인하였다.** <br>
*   sendAndRecv() 함수 내부에 두 번 프레임을 게이트웨이 측으로 전송하는 테스트를 수행하였다.<br>
*   수행 결과, 두 번째 프레임만이 게이트웨이에 도달하였으며, 두 번째 프레임에 대한 응답이 아두이노 측으로 전송되었다. <br>
*   loop() 함수의 맨 상단에 DISABLE_RF 명령이 수행됨에 따라, RF Module은 off 상태를 유지하게 되며, 그 이후에 sendAndRecv() 함수 내부에서 P_TX_Mode로 전환하는 명령인 P_TX_Data 명령(0xAB, 0x01, ...)을 처리하면서 RF Module이 켜진 듯 하다. <br>
*   RF Module이 꺼져있는 상태(off)에서 0xAB, 0x01 명령을 읽으면서 P_TX_Mode로 전환되면서, 이어 들어오는 프레임을 처리하고 나서 P_RX_Mode로 다시 복귀한 상태를 유지하다가, 다음 프레임 전송 명령을 처리하게 되면서 프레임 전송이 이루어진 듯 하다. <br>
*   첫 번째 프레임이 도달하지 않은 이유는, 0xAB, 0x01을 RF Module 상태 변화에 사용하고, 나머지 프레임 안에는 P_TX_Data 명령이 누락됨에 따라 무시되는 것이다. <br>
*   **그러면, 0xAB, 0x01, 0xAB, 0x01, ... 순으로 프레임을 구성한다면, 두 개의 프레임이 전송되는가?**<br>
*   0xAB, 0x01, 0xAB, 0x01, ... 순으로 구성한 결과, 해당 프레임은 바로 폐기되는 듯 하다. <br>
*   대신, 동일한 프레임을 두 번 전송한다면, 첫 번째 프레임에서 상태 전이가 이루어지면서 폐기, 두 번째 프레임은 정상 전송된다<br>
*   동일한 프레임 두 번 전송을 ENABLE_RF + 프레임 전송으로 대체하여 테스트해본 결과 동일 프레임 두번 전송했을 때와 동일한 결과가 나온다. <br>

```C
/*  3, 4 경우에만 작동  */
    DISABLE_RF;
    send(1);
    DISABLE_RF;
    recv();

    DISABLE_RF;
    send(2);
    ENABLE_RF;
    recv();

    ENABLE_RF;
    send(3);
    DISABLE_RF;
    recv();

    ENABLE_RF;
    send(4);
    ENABLE_RF;
    recv();

/*  아래의 경우에도 3, 4 경우에만 작동  */
    DISABLE_RF;
    send(1);
    DISABLE_RF;
    delay(1 << 9);
    recv();

    DISABLE_RF;
    send(2);
    ENABLE_RF;
    delay(1 << 9);
    recv();

    ENABLE_RF;
    send(3);
    DISABLE_RF;
    delay(1 << 9);
    recv();

    ENABLE_RF;
    send(4);
    ENABLE_RF;
    delay(1 << 9);
    recv();
```

*   **정리 1! sleep 과 serial 통신 부분은 근접시키지 말아야 한다. 그리고 RF off 상태에서 0xAB, 0x01, ..., 0xAB, 0x02, 또는 0xAB, 0x03 명령을 만나면 RF Module이 켜진다. 마지막으로 DISABLE 시킨 상태에서도 수신이 가능하다!(아니면 위 테스트 결과를 설명할 방법이 없다. 애초에 ENABLE_RF 명령과 DISABLE_RF 명령이 작동 안 한다면 모를까?**<br>

*   다음 테스트는 정리 1을 가정한 상태로 진행한다. <br>
*   RF Module 설정 코드는 아래와 같이 사용한다. 
```C
/*  
 *  각 설정이 적용되기까지 delay(1 << 7) 만큼의 시간이 있어야 함을 실험을 통해 확인하였음.
 *  자세한 내용은 아래에 기록해놓은 테스트 내용을 참고.
 */
    for(i = 0; i < 4; i++) {
        Serial.print(CH_SET[i]);
    }
    delay(1 << 7);
    for(i = 0; i < 3; i++) {                            
        Serial.print(TX_PWR[i]);
    }
    delay(1 << 7);
    for(i = 0; i < 3; i++) {                            
        Serial.print(DATA_RATE[i]);
    }
    delay(1 << 7);
```
*   지금까지 putsForDebug() 함수를 사용할 경우 Serial 포트에 영향을 주는지 주지 않는지에 대한 정보가 부재한 상태였다. 실험 결과 putsForDebug() 함수를 사용한 후에 serial 포트를 사용할 경우 통신에 영향을 주는 것을 확인하였다. 자세한 실험 내용은 아래 쪽에 기록해놓은 테스트 일지를 참고 바란다. 따라서 이러한 문제를 해결하기 위해 여러 시도를 하던 중에, putsForDebug 함수 내에 DISABLE_RF 매크로를 삽입한 결과, 기대한 대로 통신이 되는 것을 확인하였다. 아래 코드는 테스트에 사용한 전체 코드다. <br>
```C
void loop() 
{
    dummy();
    prepareFrame();
    dummy();
    sendAndRecv();
    dummy();
    parseFrame();
    dummy();
    sleep(MILSEC_500);
}

void dummy()
{
    static const char *func = "[void dummy()] ";

    sprintf(tempDbg, "%x", MSG_START);
    putsForDebug(__CRC__, func, tempDbg);
    return;
}

void prepareFrame()
{
    TX_DATA[0] = 0xAB;
    TX_DATA[1] = 0x01;
    TX_DATA[2] = 0x06;
    TX_DATA[3] = ccc;
    TX_DATA[4] = 0xEE;
    TX_DATA[5] = 0xEE;
    TX_DATA[6] = 0xEE;
    TX_DATA[7] = 0xEE;
    TX_DATA[8] = 0xEE;
    ++ccc;
    buff[0] = 0;
    buff[1] = 0;
    buff[2] = 0;
    buff[3] = 0;
    buff[4] = 0;
    buff[5] = 0;
    buff[6] = 0;
    buff[7] = 0;

    return;
}

void sendAndRecv()
{
    int i, j;

/*  
    가정

    off 상태에서 0xAB, 0x01 만나면 상태 변이가 이루어 짐. 
    그러면, 전송 후 off 시키고, read 후 send 시 전송이 될까?

    README.md 정리 1 참고
*/
    ENABLE_RF;
    for (i = 0; i < 9; ++i) {
        Serial.print(TX_DATA[i]);
    }
    DISABLE_RF;
    delay(1 << 9);
    for (j = 0; Serial.available(); ++j) {
        buff[j] = Serial.read();
    }

    return;
}

void parseFrame()
{
    int i;
    
   // for (i = 0; i < 8; ++i) { 프레임 두게 전송 결과를 볼 경우
    for (i = 0; i < 4; ++i) {
        Serial.print(buff[i], DEC);
        Serial.print(" ");
    }
    Serial.println("");
    Serial.flush();

    return;
}

void putsForDebug(const char *file, const char *func, const char *msg)
{
/*  실제 코드에서는, RELEASE 버전일 때 아래 부분을 무시해야 하므로 이러한 전처리기를 사용함 */
#ifndef ALALALA
    DISABLE_RF;
    Serial.print('\n');
    Serial.print('\r');
    Serial.print(file);
    Serial.print(func);
    Serial.println(msg);
    Serial.flush();
#endif

    return;
}
```

*   **정리 2! putsForDebug 함수를 사용해야 할 경우, 함수 내부에 DISABLE_RF 명령을 삽입하여 사용해야 한다.**<br>

*   putsForDebug() 함수는 테스트 시에만 사용되고, 실제 제품화 단계에서는 사용하지 않는다. 따라서 RELEASE 단계에서는 putsForDebug()를 전부 무시해야 한다. <br>
*   아두이노는 사용되지 않는 함수를 미리 확인하여(pretty aggressive) 컴파일 시 무시한다. 아래는 코드 상태에 따른 컴파일 결과를 나타낸다. <br>
```C
void setup()
{
    Serial.begin(9600);
}

void loop()
{
    func();
}

void func()
{
    Serial.println("Hello, world!");

    return;
}

```
```Text
...
Sketch uses 1492 bytes (4%) of program storage space. Maximum is 32256 bytes.
Global variables use 202 bytes (9%) of dynamic memory, leaving 1846 bytes for local variables.
...
```
```C
void setup()
{
    Serial.begin(9600);
}

void loop()
{
    /*  func(); */
}

void func()
{
    Serial.println("Hello, world!");

    return;
}


```
```Text
...
Sketch uses 1442 bytes (4%) of program storage space. Maximum is 32256 bytes.
Global variables use 184 bytes (8%) of dynamic memory, leaving 1864 bytes for local variables.
...
```
```C
void setup()
{
    Serial.begin(9600);
}

void loop()
{
    funcA();
    funcB();
}

void funcA()
{
#ifdef A
    Serial.println("Hello, world!");

    return;
#endif
}

void funcB()
{
    /*
    Serial.println("Hello, world!");

    return;
    */
}
```
```Text
...
Sketch uses 1442 bytes (4%) of program storage space. Maximum is 32256 bytes.
Global variables use 184 bytes (8%) of dynamic memory, leaving 1864 bytes for local variables.
...
```

*   **정리 3! putsForDebug 내부에 주석 또는 전처리기를 사용하여 모든 내용을 지울 경우, 컴파일 단계에서 putsForDebug 함수 내용을 전부 무시한다. putsForDebug 함수를 참조하더라도 내부에 내용이 없으므로 해당 내용을 무시한 결과와 참조한 결과가 같기 때문이다. 따라서 내부에 전처리기를 사용하여 코드를 선택하게 할 수 있도록 구현해 놓는다면, 실제 컴파일 시에 해당 함수 내용을 반영할지, 무시할지 결정할 수 있게 된다.**<br>

### Mar 1, 2020<br>
*   현재 프로그램 상에 배열과 포인터를 혼용하고 있는데, 성능과 일관성(Consistency)를 위해 배열을 사용하거나 플어쓰는 방법으로 통일하고자 한다. <br>
*   테스트 결과, 아두이노 단에서 가장 긴 프레임의 길이인 9 크기의 배열을 사용하였을 경우 송수신에 문제 없음을 확인하였다. <br>
*   따라서, RF Module 설정에는 풀어서 설정하고, 송수신에는 배열식을 사용하여 통일하고자 한다. <br><br>

####    Comparison Between Array and Pointer<br>
```C
/*
 *  Array: 
 *  movb    $0x00, 0x18(%esp)   
 */
    buf[2] = 0x00;  
/*
 *  Pointer: 
 *  mov     0x1c(%esp), %eax
 *  add     $0x2, %eax
 *  movb    $0x00, (%eax)
 */
    *(buf + 2) = 0x00;
```

*   Macro 내용과 function 내용이 동일할 경우, 컴파일 시 어떠한 차이가 발생하는가?(즉, function을 inline 처리하는가? 아니면 별도의 stack으로 간주하는가?)<br>
```C
#define ENABLE_RF               \
do {                            \
    Serial.print((char) 0xAB);  \
    Serial.print((char) 0x03);  \
} while (0)

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    ENABLE_RF;
}

```
```Text
Sketch uses 1458 bytes (4%) of program storage space. Maximum is 32256 bytes.
Global variables use 184 bytes (8%) of dynamic memory, leaving 1864 bytes for local variables.
```
```C
void setup()
{
    Serial.begin(9600);
}

void loop()
{
    funcC();
}

void funcC()
{
    Serial.print((char) 0xAB); 
    Serial.print((char) 0x03); 

    return;
}

```
```Text
Sketch uses 1458 bytes (4%) of program storage space. Maximum is 32256 bytes.
Global variables use 184 bytes (8%) of dynamic memory, leaving 1864 bytes for local variables.
```

*   **정리 4!. Arduino compilier는 함수 내용을 inline으로 치환하므로, MACRO 선언과 function을 엄격하게 구분하여 사용할 필요는 없다. 가독성에 맞게 선택하여 사용하면 된다.**<br>

*   DISABLE_RF 상태에서 프레임을 수신받을 수 있는가?<br>
*   예전 실험을 통해 프레임 수신이 된다는 것을 확인하였음<br>
*   아래 실험은 예전 실험에 대한 추가 검증임<br>
```C
void sendAndRecv()
{
    int i, j;

/*  
    가정

    off 상태에서 0xAB, 0x01 만나면 상태 변이가 이루어 짐. 
    그러면, 전송 후 off 시키고, read 후 send 시 전송이 될까?

    README.md 정리 1 참고
*/
    ENABLE_RF;
    for (i = 0; i < 9; ++i) {
        Serial.print(TX_DATA[i]);
    }
/*
    DISABLE_RF;
    delay(1 << 9);
*/
/*  DISABLE_RF 내부에 delay(1)이 포함되어 있음  */
    for (i = 0; i < 500; ++i) {
        DISABLE_RF;
    }
    for (j = 0; Serial.available(); ++j) {
        buff[j] = Serial.read();
    }

    return;
}
```
*   위 코드는 이상 없이 제대로 동작 하였음<br>

### Mar 2, 2020<br>
*   테스트 중간 중간에 RF 통신이 원활이 이루어지지 않아, 여러 방법을 시도해 보던 중, 건전지 위치 그리고 기판 위치를 옮겨 보았더니 문제가 종종 해결되었다. <br>
*   테스트 중에 될 수 있도록 다른 기판 끼리 근접하게 놓지 않고, 건전지 선도 최대한 구부러트리지 않도록 주의하며 테스트를 진행하도록 하겠다. <br>

### Mar 6, 2020<br>
*   **통신 먹통현상은 기판의 문제일수도, 접촉 불량 문제일수도, 기타 다른 이유일수도 있다. 오늘 테스트해본 결과, putsForDebug() 내부에 delay(32)를 삽입하여 운용한 결과 통신 문제가 어느 정도 해결되었다. 최종 출시 단계에서는 삭제될 함수이므로, 테스트할 때만 delay()를 최소 운용하여 코드를 완성시키고자 한다. **<br>
*   RF 통신부 코드는 검증이 완료되었고, 앞으로 아래 목록의 코드 검증이 이루어져야 한다. <br>
    *   열감지 센서<br>
    *   연기감지 센서<br>
    *   LED<br>
    *   Buzzer<br>
    *   Battery Check<br>

*   오늘부터 센서 검증에 들어간다. 우선 DISABLE_ADC, ENABLE_ADC 작동에 대해 검증한다. 
```C
#define ENABLE_ADC          \
do {                        \
    ADCSRA  |= (1 << ADEN); \
    delay(1);               \
} while (0)                 \

#define DISABLE_ADC         \
do {                        \
    ADCSRA  = 0;            \
} while (0)                 \
```
*   아래와 같이 운용한 결과, ENABLE_ADC 상태에서는 값이 실시간으로 변화하나, DISABLE_ADC 상태에서는 마지막으로 읽은 값으로 고정되어 있음을 아래 실험을 통해 확인하였다. <br>
```C
    ENABLE_ADC;
    sprintf(tempDbg, "1. ADC ON -> READ -> delay(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    delay(10);
    sprintf(tempDbg, "1. ADC ON -> READ -> delay(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    delay(10);
    sprintf(tempDbg, "1. ADC ON -> READ -> delay(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    delay(10);
    sprintf(tempDbg, "1. ADC ON -> READ -> delay(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    delay(10);
    sprintf(tempDbg, "1. ADC ON -> READ -> delay(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    delay(1000);
    
    DISABLE_ADC;
    sprintf(tempDbg, "2. ADC OFF -> READ -> delay(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    delay(10);
    sprintf(tempDbg, "2. ADC OFF -> READ -> delay(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    delay(10);
    sprintf(tempDbg, "2. ADC OFF -> READ -> delay(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    delay(10);
    sprintf(tempDbg, "2. ADC OFF -> READ -> delay(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    delay(10);
    sprintf(tempDbg, "2. ADC OFF -> READ -> delay(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    delay(1000);
```
```Text
...
[sensor.ino][void test1()]1. ADC ON -> READ -> delay(1000), Sensing: 480
[sensor.ino][void test1()]1. ADC ON -> READ -> delay(1000), Sensing: 384
[sensor.ino][void test1()]1. ADC ON -> READ -> delay(1000), Sensing: 448
[sensor.ino][void test1()]1. ADC ON -> READ -> delay(1000), Sensing: 384
[sensor.ino][void test1()]1. ADC ON -> READ -> delay(1000), Sensing: 448
[sensor.ino][void test1()]2. ADC OFF -> READ -> delay(1000), Sensing: 448
[sensor.ino][void test1()]2. ADC OFF -> READ -> delay(1000), Sensing: 448
[sensor.ino][void test1()]2. ADC OFF -> READ -> delay(1000), Sensing: 448
[sensor.ino][void test1()]2. ADC OFF -> READ -> delay(1000), Sensing: 448
[sensor.ino][void test1()]2. ADC OFF -> READ -> delay(1000), Sensing: 448
[sensor.ino][void test1()]1. ADC ON -> READ -> delay(1000), Sensing: 480
[sensor.ino][void test1()]1. ADC ON -> READ -> delay(1000), Sensing: 399
[sensor.ino][void test1()]1. ADC ON -> READ -> delay(1000), Sensing: 455
[sensor.ino][void test1()]1. ADC ON -> READ -> delay(1000), Sensing: 399
[sensor.ino][void test1()]1. ADC ON -> READ -> delay(1000), Sensing: 462
[sensor.ino][void test1()]2. ADC OFF -> READ -> delay(1000), Sensing: 462
[sensor.ino][void test1()]2. ADC OFF -> READ -> delay(1000), Sensing: 462
[sensor.ino][void test1()]2. ADC OFF -> READ -> delay(1000), Sensing: 462
[sensor.ino][void test1()]2. ADC OFF -> READ -> delay(1000), Sensing: 462
[sensor.ino][void test1()]2. ADC OFF -> READ -> delay(1000), Sensing: 462
...
```

*   delay(1000) 함수를 sleep(SEC_1)로 대체한 결과 위와 동일한 결과가 나왔다. <br>
```C
    ENABLE_ADC;
    sprintf(tempDbg, "3. ADC ON -> READ -> sleep(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    delay(10);
    sprintf(tempDbg, "3. ADC ON -> READ -> sleep(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    delay(10);
    sprintf(tempDbg, "3. ADC ON -> READ -> sleep(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    delay(10);
    sprintf(tempDbg, "3. ADC ON -> READ -> sleep(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    delay(10);
    sprintf(tempDbg, "3. ADC ON -> READ -> sleep(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    sleep(SEC_1);
    
    DISABLE_ADC;
    sprintf(tempDbg, "4. ADC OFF -> READ -> sleep(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    delay(10);
    sprintf(tempDbg, "4. ADC OFF -> READ -> sleep(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    delay(10);
    sprintf(tempDbg, "4. ADC OFF -> READ -> sleep(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    delay(10);
    sprintf(tempDbg, "4. ADC OFF -> READ -> sleep(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    delay(10);
    sprintf(tempDbg, "4. ADC OFF -> READ -> sleep(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    sleep(SEC_1);
```
*   sleep() 직후에 바로 ENABLE_ADC, DISABLE_ADC가 등장해도 정상 동작한다. <br>
```C
    ENABLE_ADC;
    sprintf(tempDbg, "3. ADC ON -> READ -> sleep(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    delay(10);
    sprintf(tempDbg, "3. ADC ON -> READ -> sleep(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    sleep(SEC_1);
    
    DISABLE_ADC;
    sprintf(tempDbg, "4. ADC OFF -> READ -> sleep(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    delay(10);
    sprintf(tempDbg, "4. ADC OFF -> READ -> sleep(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    sleep(SEC_1);

    ENABLE_ADC;
    sprintf(tempDbg, "3. ADC ON -> READ -> sleep(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    delay(10);
    sprintf(tempDbg, "3. ADC ON -> READ -> sleep(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    sleep(SEC_1);
    
    DISABLE_ADC;
    sprintf(tempDbg, "4. ADC OFF -> READ -> sleep(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    delay(10);
    sprintf(tempDbg, "4. ADC OFF -> READ -> sleep(1000), Sensing: %d", (int) analogRead(ACTUAL_SENSOR));
    putsForDebug(__SENSOR__, func, tempDbg);
    sleep(SEC_1);
```
*   **정리 5!. ENABLE_ADC, DISABLE_ADC는 정상 동작하며, sleep()과 붙여 사용해도 잘 동작한다.**<br>

*   열감지 센서에 대해 동작을 확인한 결과, 값이 수시로 변화함을 확인할 수 있었다. 따라서 측정 시, 10번 측정 후 평균값을 내어 최종 값으로 사용하고자 한다. 아래는 온도 감지 함수를 정의한 것이다. <br>
```C
void getTemperature(uint8_t pin)
{
    static char *func = "[void getTemperature(uint8_t pin)]";
    int         i;
    float       temp, total;

    temp = total = 0.0;
    ENABLE_ADC;
    for (i = 0; i < 10; ++i) {
        temp = analogRead(pin);
        sprintf(tempDbg, "%d", (int) temp);
        putsForDebug(__SENSOR__, func, tempDbg);
        total += temp;
    }
    DISABLE_ADC;
    total /= 10;
    sprintf(tempDbg, "Average value: %d", (int) total);
    putsForDebug(__SENSOR__, func, tempDbg);
    sleep(SEC_1);

    return;
}
```
```Text
...
[sensor.ino][void getTemperature(uint8_t pin)]399
[sensor.ino][void getTemperature(uint8_t pin)]385
[sensor.ino][void getTemperature(uint8_t pin)]384
[sensor.ino][void getTemperature(uint8_t pin)]384
[sensor.ino][void getTemperature(uint8_t pin)]384
[sensor.ino][void getTemperature(uint8_t pin)]384
[sensor.ino][void getTemperature(uint8_t pin)]384
[sensor.ino][void getTemperature(uint8_t pin)]384
[sensor.ino][void getTemperature(uint8_t pin)]391
[sensor.ino][void getTemperature(uint8_t pin)]384
[sensor.ino][void getTemperature(uint8_t pin)]Average value: 386
[sensor.ino][void getTemperature(uint8_t pin)]448
[sensor.ino][void getTemperature(uint8_t pin)]384
[sensor.ino][void getTemperature(uint8_t pin)]391
[sensor.ino][void getTemperature(uint8_t pin)]391
[sensor.ino][void getTemperature(uint8_t pin)]399
[sensor.ino][void getTemperature(uint8_t pin)]399
[sensor.ino][void getTemperature(uint8_t pin)]384
[sensor.ino][void getTemperature(uint8_t pin)]391
[sensor.ino][void getTemperature(uint8_t pin)]384
[sensor.ino][void getTemperature(uint8_t pin)]399
[sensor.ino][void getTemperature(uint8_t pin)]Average value: 397
...
```

*   LED와 Buzzer를 제어하는 함수는 아래와 같이 정의하였으며, LED 또는 Buzzer를 킨 상태에서 sleep 모드로 들어갈 경우, 각 port를 holding한 상태를 유지하기 때문에 정상 동작하는 것을 확인하였다. 총 8가지 상태를 정의하여 운용할 계획이다. 3-bit 값을 사용하여, 0번째 bit는 초록 LED, 1번째 bit는 빨강 LED, 2번째 bit는 buzzer를 나타낸다. <br>
```C
/*
    3-bit
    --------+-----------+------------
    2       |   1       |   0
    --------+-----------+------------
    Buzzer  |   Red LED |   Green LED
    --------+-----------+------------
    B: On   |   R: On   |   G: On
    X: Off  |   X: Off  |   X: Off
    --------+-----------+------------
    
#define STATE_NORMAL            00    
#define STATE_PREPARE           01
#define STATE_XXG               01
#define STATE_FULLED_BUFFER     02
#define STATE_XRX               02
#define STATE_CHECK_BATTERY     03
#define STATE_XRG               03
#define STATE_RECEIVED_TEST     04
#define STATE_BXX               04
#define STATE_TESTING_PASSED    05
#define STATE_BXG               05
#define STATE_TESTING_FAILED    06
#define STATE_BRX               06
#define STATE_FIRE              07
#define STATE_BRG               07

void handleBuzzerAndLED(uint8_t op)
{
    digitalWrite(STATUS1, (op & 01) ? HIGH : LOW);
    op >>= 1;
    digitalWrite(STATUS0, (op & 01) ? HIGH : LOW);
    op >>= 1;
    digitalWrite(BUZZER, (op & 01) ? HIGH : LOW);
    sleep(MILSEC_125);
    digitalWrite(STATUS1, LOW);
    digitalWrite(STATUS0, LOW);
    digitalWrite(BUZZER, LOW);

    return;
}
*/
```
*   **정리 6! MCU가 sleep 모드에 진입할 경우, 각 port를 그대로 holding한 채로 sleep 모드에 진입한다.**<br>

*   검증에 통과된 Heat sensor에서 사용하는 prepareSensor(void) 함수는 아래와 같다.<br>
```C
/*  putsForDebug가 굳이 필요할까 .. 왜냐하면 LED를 통해 확인 가능하기 때문이다. */
void prepareSensor(void)
{
    /*
     *  func        : 디버깅 메세지를 작성하는 데 사용되는 식별 변수
     *  i           : 조건문에서 사용하는 제어 변수
     *  temp        : analogRead 결과를 임시 저장하는 변수
     *  filtered    : 센서로부터 얻은 값에 필터링을 거친 값
     */
    static char *func = "[void prepareSensor(void)]";
    uint8_t     i;
    float       temp, filtered;

    sprintf(tempDbg, "%x", MSG_START);
    putsForDebug(__SENSOR__, func, tempDbg);
    pastValue   = INIT_SENSOR_TEMPERATURE;
    for (i = 0; i < PREPARE_TIME; ++i) {
        handleBuzzerAndLED(STATE_PREPARE);
        ENABLE_ADC;
        analogRead(ACTUAL_SENSOR);
        DISABLE_ADC;
        sleep(SEC_1);
    }

    return;
}
```

*   getTemperature 함수를 최적화 하기 위해 아래와 같은 테스트를 수행하였고, 수행 결과 새로운 함수를 정의할 수 있었다. 
```C
void loop()
{
    int i;

    prepareSensor();
    ENABLE_ADC;
    for (i = 0; i < 10; ++i) {
        Serial.println(analogRead(ACTUAL_SENSOR));
    }
    DISABLE_ADC;
}
```
```Text
중간 중간 깨진 값들이 보이지만, 실제 운용할 땐 Serial을 사용하지 않은 채 값만 사용할 것이다. 
동작하는 것에는 이상이 없어 보인다. 
...
412
408
408
408
408
399
448
...
```
```C
void loop()
{
    int i;

    prepareSensor();
    Serial.println(getValue(ACTUAL_SENSOR));
    Serial.flush();
}

/*  새로 정의한 함수    getValue()  */
float getValue(uint8_t pin)
{
    int         i;
    float       temp;

    temp = 0;
    ENABLE_ADC;
    for (i = 0; i < 10; ++i) {
        temp += analogRead(pin);
    }
    DISABLE_ADC;

    /*  temp를 반환하는 이유는, 300 정도의 값이 10번 곱해지는가 확인하기 위해서임   */
    //return (temp / 10.0);
    return temp;
}
```
```Text
3223
3223
4161
4267
4040
3868
3840
```

### 정리<br>
*   그동안 정리한 내용들을 토대로, 아래 주제에 맞게 새로 정리해둘 것<br>
*   마지막 수정(Mar 6, 2020)<br>

####    RF Module<br>
1. <br>

####    Frame<br>
1. <br>

####    Heat<br>
1. <br>

####    Smoke<br>
1. <br>

####    Utilities<br>
1. LED<br>

2. Buzzer<br>

3. Check Voltage<br>

4. Interrupt<br>

5. Sleep<br>

6. ADC<br>


### Before<br>
####    아래 내용들은, 기판이 완성되기 이전 테스트 자료들로써 현재 상황과 맞지 않거나 다른 부분이 있을 수 있기에, 기록으로만 남겨 두고 참고하지는 않는다. <br><br>
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
### Feb 28, 2020<br>
*   KEM 프로젝트에서 사용하는 RF Module(A3300A)의 데이터시트를 보면, Module Off Mode에서 RX Mode로 전환될 상황만 명시되어 있으며, 그 외 상황은 표기되어 있지 않다. <br>
*   Module Off Mode에서 RX 상태로의 전이 시(즉, RF Module On 명령) 명령 전달까지 필요한 시간은 1 (ms)다. <br>
*   이러한 자료들을 토대로, RF Module On, RF Module Off 명령을 테스트해본 결과, RF Module Off 명령에는 delay() 함수를 사용하나, 사용하지 않으나 정상 작동한다. <br>
*   반면에, RF Module On 명령에는 delay(1) 함수를 같이 사용해야 정상 작동한다. <br>
*   이때의 delay(1)은 테스트를 통해 대기할 수 있는 최소 시간을 구했다. <br>
*   또한, sleep() 함수가 아닌 delay() 함수를 사용한 것은, 테스트 결과 sleep 함수를 사용하면 명령을 RF Module에 보내는 중간에 MCU가 동작을 멈추므로, 기대와 달리 제대로 동작하지 않는다. <br>
*   결국, 통신을 할 때 RF Module On -> Send -> RF Module Off, RF Module On -> Recv -> RF Module Off 동작 순서를 따라야 제대로 동작한다. <br><br>

####    ENABLE_RF<br>  
```C
#define ENABLE_RF               \
do {                            \
    Serial.print((char) 0xAB);  \
    Serial.print((char) 0x03);  \
    delay(1);                   \
} while (0)
```

####    ENABLE_RF<br>  
```C
#define DISABLE_RF              \
do {                            \
    Serial.print((char) 0xAB);  \
    Serial.print((char) 0x02);  \
} while (0)
```
*   현재 구현되어 있는 프로그램에는, delay() 함수를 사용하여 주기를 조절하였다. 그러나, 실제 제품화 단계에서는 전력을 적게 사용해야 한다. 즉, 저전력 문제를 해결해야만 실제 운용이 가능하다. 따라서 delay() 함수를 최대한 sleep() 함수로 대체하여 전력을 적게 사용해야 한다. <br>
*   통신 시 동작 흐름은 RF Module On -> Send -> RF Module Off, RF Module On -> Recv -> RF Module Off이다. <br>
*   테스트 수행 결과 Send 동작 시에는 delay() 함수만 사용 가능하고, Recv 동작 시에는 delay()와 sleep() 함수 둘 다 사용 가능하다. <br>
*   따라서, Send에는 delay(), Recv에는 sleep() 함수를 사용하며, delay()는 (1 << 9) 밀리초 만큼의 시간이 필요하다. sleep()은 1초 주기를 맞추어야 하므로, 1,000 (ms) - (1 << 9) (ms) 만큼의 시간을 사용하게 된다. <br>
*   Watchdog 타이머에서는 125 -> 250 -> 500 -> 1,000 -> ... 간격으로 밀리초 시간을 설정할 수 있으므로, 가장 근접한 시간인 500 (ms)를 사용한다. <br>
*   결국, 전체적인 통신 동작은 ENABLE_RF -> SEND with delay(1 << 9) -> DISABLE_RF, ENABLE_RF -> RECV with sleep(MILSEC_500) -> DISABLE_RF가 된다. <br><br>

####    SEND with delay(1 \<\< 9)<br>
```C
void send()
{
    Serial.print((char) 0xAB);
    Serial.print((char) 0x01);
    Serial.print((char) 0x03);
    Serial.print((char) 0x22);
    Serial.print((char) 0x33);
    Serial.print((char) 0x44);
    delay(1 << 9);

    return;
}
```

####    RECV with sleep(MILSEC_500)<br>
```C
void recv()
{
    while (Serial.available()) {
        Serial.print(Serial.read());
        Serial.print(" ");
    }
    Serial.println("");
    Serial.flush();
    sleep(MILSEC_500);

    return;
}
```
*   위 테스트 내용에 덧붙이자면, recv 함수 내에 sleep 함수가 구현되어 있는데, sleep 함수를 수행한 후에 return, 곧 바로 DISABLE_RF 매크로를 수행한다. 만약 sleep() 함수 직후의 명령이 동작 안 할 수도 있어 ON -> Recv -> OFF -> Send 동작을 수행하였다. 수행 결과 게이트웨이로 RF 송신이 되지 않는 것을 확인하였고, 결국 sleep() 함수 직후의 DISABLE_RF 명령이 잘 동작함을 확인하였다. 다음으로 ON -> Recv -> OFF -> ON -> Send -> OFF 순서로 동작시킨 결과 게이트웨이로 프레임이 정상적으로 송신되는 것을 확인하였다. <br>
*   즉, sleep() 함수 바로 다음에 수행하는 DISABLE_RF 명령은 잘 동작 한다. <br>

*   RF Module을 운용하는 데 세 가지 설정을 하게 되는데, 각각 항목을 나열하면 SET\_CHANNEL, SET\_TX\_POWER, SET\_DATA\_RATE다.<br>
*   이때, 각 항목을 설정하는데, 얼마큼의 시간이 필요한가에 대한 내용은 데이터시트에 누락되어 있다.<br>
*   테스트 수행 결과, 각 항목들을 수행하기 위해 필요한 시간은 delay(1 << 7)초이며, sleep() 함수를 이용하여 테스트해본 결과, 정상 작동하지 않았다. <br><br>

####    Configure<br>
```C
#define SET_CHANNEL_0           \
do {                            \
    Serial.print((char) 0xAB);  \
    Serial.print((char) 0x04);  \
    Serial.print((char) 0x01);  \
    Serial.print((char) 0x00);  \
    delay(1 << 7);              \
} while (0)

#define SET_CHANNEL_1           \
do {                            \
    Serial.print((char) 0xAB);  \
    Serial.print((char) 0x04);  \
    Serial.print((char) 0x01);  \
    Serial.print((char) 0x01);  \
    delay(1 << 7);              \
} while (0)

#define SET_TX_PWR_0            \
do {                            \
    Serial.print((char) 0xAB);  \
    Serial.print((char) 0x05);  \
    Serial.print((char) 0x00);  \
    delay(1 << 7);              \
} while (0)
```

*   마지막으로, 현재 프로그램 상에 배열과 포인터를 혼용하고 있는데, 성능과 일관성(Consistency)를 위해 배열을 사용하거나 플어쓰는 방법으로 통일하고자 한다. <br>
*   테스트 결과, 아두이노 단에서 가장 긴 프레임의 길이인 9 크기의 배열을 사용하였을 경우 송수신에 문제 없음을 확인하였다. <br>
*   따라서, RF Module 설정에는 풀어서 설정하고, 송수신에는 배열식을 사용하여 통일하고자 한다. <br><br>

####    Comparison Between Array and Pointer<br>
```C
/*
 *  Array: 
 *  movb    $0x00, 0x18(%esp)   
 */
    buf[2] = 0x00;  
/*
 *  Pointer: 
 *  mov     0x1c(%esp), %eax
 *  add     $0x2, %eax
 *  movb    $0x00, (%eax)
 */
    *(buf + 2) = 0x00;
```
*   추가 확인해야 할 내용들(29 Feb, 2020 테스트 예정)<br>
1.  sensing, testing에 sleep 함수를 적용하였을 경우, 정상 동작하는가?<br>
2.  sensing, testing에 putsForDebug 함수를 적용하였을 경우, 정상 동작하는가?<br>
3.  ON -> Send -> OFF, ON -> Recv -> OFF 사이에 putsForDebug 함수를 적용하였을 경우, 정상 동작하는가?<br>

*   현재 기판에서 디버깅용 함수 putsForDebug를 사용하는데, 하나의 serial 포트를 공유하기 때문에, 해당 함수를 사용함으로써 통신에 방해 또는 오류를 야기할 수 있다. <br>
*   따라서, 통신을 상태했던 예제에 putsForDebug 함수를 테스트해보면서 통신에 영향을 주는가에 대한 여부를 확인한다. <br>
*   테스트 결과, 검증된 소스 코드에 putsForDebug 함수를 단순히 한 번 호출했음에도 불구하고 첫 동작만 이루어진 채 다음 동작은 무시되는 현상을 발견하였다. <br>
```C
    /*  
     *  05 ~ 10 라인 코드들은 이미 검증이 된 코드들이며,
     *  01 ~ 04 라인 코드들은 테스트를 위해 새로 추가 된 코드들이다. 
     *  처음 한 번만 동작되고, 그 이후 동작들은 무시된다. 
     */
    01: static const char *func = "[void loop()] ";
    02: 
    03: sprintf();
    04: putsForDebug();
    05: ENABLE_RF;
    06: send();
    07: DISABLE_RF;
    08: ENABLE_RF;
    09: recv();
    10: DISABLE_RF;
```
```C
/*  Test에서 사용된 매크로와 함수들 */
#define ENABLE_RF               \
do {                            \
    Serial.print((char) 0xAB);  \
    Serial.print((char) 0x03);  \
    delay(1);                   \
} while (0)

#define DISABLE_RF              \
do {                            \
    Serial.print((char) 0xAB);  \
    Serial.print((char) 0x02);  \
} while (0)

void send()
{
    int i;

    for (i = 0; i < 9; ++i) {
        Serial.print(TX_DATA[i]);
    } 
    delay(500);

    return;
}

void recv(void)
{
    int i, j;

    i = 0; 
    while (Serial.available()) {
        buff[i++] = Serial.read();
    }
    j = 0;
    while (j < i) {
        Serial.print(buff[j++], HEX);
        Serial.print(" ");
    }
    Serial.println("");
    Serial.flush();
    sleep(MILSEC_500);

    return;
}

void putsForDebug(const char *file, const char *func, const char *msg)
{
    Serial.print('\n');
    Serial.print('\r');
    Serial.print(file);
    Serial.print(func);
    Serial.println(msg);
    Serial.flush();

    return;
}
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

## Watchdog<br>
```c
/*  SETUP WATCHDOG TIMER
 *  ---------------------------------------------------------
 *  WDTCSR = (24);      : change enable and WDE - also resets
 *  WDTCSR = (6);       : prescalers only - get rid of the WDE and WDCE bit
 *  WDF | WDIE | WDP3 | WDCE | WDE | WDP2 | WDP1 | WDP0
 *      WDP3 - WDP2 - WPD1 - WDP0 - time
 *      0      0      0      0      16 ms
 *      0      0      0      1      32 ms
 *      0      0      1      0      64 ms
 *      0      0      1      1      0.125 s
 *      0      1      0      0      0.25 s
 *      0      1      0      1      0.5 s
 *      0      1      1      0      1.0 s
 *      0      1      1      1      2.0 s
 *      1      0      0      0      4.0 s
 *      1      0      0      1      8.0 s
 *      
 *  WDTCSR |= (1<<6);  : enable interrupt mode
 */
```
