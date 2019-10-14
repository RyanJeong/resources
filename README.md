## Bitcoin Ver.0.1.3 Alpha
![v0.1.3](https://github.com/RyanJeong/Bitcoin-v0.1.3-Alpha/blob/master/img/v0.1.3.PNG)
![about](https://github.com/RyanJeong/Bitcoin-v0.1.3-Alpha/blob/master/img/about.PNG)
## 설치 방법
### 설치 환경
* Windows 7 Professional K Version 6.1(Build 7601: Service Pack 1)

### 필요 프로그램
* MinGW 3.4.5
* MSYS
* Berkeley DB 4.8.30
* Boost 1.35.0
* wxWidgets 2.8.12
* OpenSSL 1.0.2d
* Visual Studio 2013
  * VS2013 x86 Native Tools Command Prompt 필요
* Strawberry Perl
  * OpenSSL compile에 필요
* 7-Zip
  * tar.gz 확장자 압축 파일을 처리하기 위해 필요

### 설치 순서
1. MSYS root 디렉토리에 Berkeley DB, Boost, wxWidgets, OpenSSL 폴더들을 위치한다.<br>
    * 이때 각 폴더의 이름들은 아래처럼 수정해야 함<br>
      * db-4.8.30 -> **DB**<br>
      * boost_1_35_0 -> **boost**<br>
      * wxWidgets-2.8.12 -> **wxWidgets**<br>
      * openssl-1.0.2d -> **OpenSSL**<br><br>

2. 환경변수를 아래와 같이 추가한다.<br>
    * <MinGW3.4.5>\bin<br>
    * <MinGW3.4.5>\MinGW\mingw32\bin<br>
    * <Perl><br><br>

3. MinGW3.4.5 폴더의 lib 폴더를 <MSYS>\1.0\mingw 폴더 안으로 복사한다.<br>
4. 
