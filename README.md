## Bitcoin Ver.0.1.3 Alpha
![v0.1.3](https://github.com/RyanJeong/Bitcoin-v0.1.3-Alpha/blob/master/img/v0.1.3.PNG)
![about](https://github.com/RyanJeong/Bitcoin-v0.1.3-Alpha/blob/master/img/about.PNG)
## 설치 방법
### 설치 환경
* Windows 7 Professional K Version 6.1(Build 7601: Service Pack 1)

### 필요 프로그램<br>
#### 아래 프로그램들을 모두 설치 혹은 압축을 해제하여야 함<br>
* Bitcoin ver. 0.1.3 alpha source code<br>
git clone https://github.com/RyanJeong/Bitcoin-v0.1.3-Alpha
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
1. MSYS 설치 후에 생성된 MSYS root 디렉토리에 Berkeley DB, Boost, wxWidgets, OpenSSL 폴더들을 위치한다.<br>
이때 각 폴더의 이름들은 아래처럼 수정해야 함<br>
    
| 수정 전 | 수정 후 |
|:--------|:--------:|
| db-4.8.30 | DB |
| boost_1_35_0 | boost |
| wxWidgets-2.8.12 | wxWidgets |
| openssl-1.0.2d | OpenSSL |

2. 환경변수를 아래와 같이 추가한다.<br>
```text
* <MinGW3.4.5>\bin
* <MinGW3.4.5>\MinGW\mingw32\bin
* <Strawberry Perl>\perl\bin
```

3. MinGW3.4.5 폴더의 lib 폴더를 <MSYS>\1.0\mingw 폴더 안으로 복사한다.<br>
 
4. Bitcoin-v0.1.3-Alpha\Bitcoin\src 디렉토리 내에 obj 디렉토리를 생성한다.<br>

5. MSYS를 사용하여 Berkeley DB를 compile 한다.<br>
```Bash
$ cd /DB/build_unix
$ ../dist/configure --enable-mingw --enable-cxx
$ make
```

6. MSYS를 사용하여 wxWidgets을 compile 한다.<br>
```Bash
$ cd /wxWidgets
$ ./configure --with-msw --enable-debug --enable-debug_gdb --disable-shared
$ make
```

7. VS2013 x86 Native Tools Command Prompt를 사용하여 OpenSSL을 compile 한다.<br>
```Bash
$ cd c:\msys\1.0\OpenSSL
$ perl Configure VC-WIN32 no-asm --prefix=C:\src\OpenSSL\Build-VC-32-release
$ ms\do_ms.bat
$ nmake -f ms\nt.mak clean
$ nmake -f ms\nt.mak 
$ nmake -f ms\nt.mak install
$ nmake -f ms\nt.mak clean
```

8. C:\src\OpenSSL\Build-VC-32-release 폴더 내에 있는 include 폴더를 <MSYS>\1.0\OpenSSL 폴더 내에 존재하는 include 폴더와 교체한다.<br>
 
9. MSYS를 사용하여 Bitcoin을 compile 한다.<br>
```Bash
$ cd <Bitcoin-v0.1.3-Alpha>/Bitcoin/src
$ make
```
