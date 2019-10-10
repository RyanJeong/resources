# Bitcoin-v0.1
Bitcoin ver. 0.1
```text
[WIP]
윈도우 10에서 비트코인 초기 버전 컴파일하기

필요 프로그램(각 항목에 다운로드 링크 걸 것)
1. VS2015 - x64 Native Tools Command Prompt
2. Strawberry Perl
3. 7-Zip
4. MSYS 1.0.11
5. MinGW 3.4.5
6. OpenSSL 1.0.2d
7. wxWixgets 2.8.12 -> wxWidgets 3.0.4(아래 링크 주소도 바뀌어야 함)
8. Berkeley DB 4.8.30
9. Boost 1.35.0
(10.) 최신 버전의 MinGW(만약, Strawberry Perl 설치할 때 같이 설치되는 bin 프로그램들로 DB와 wxWidgets 컴파일이 정상 수행된다면, 해당 프로그램은 설치하지 않아도 된다.)

설치 순서
1. 필요 프로그램들을 모두 설치한다.
2. MSYS의 root 폴더 안에 OpenSSL 1.0.2d, wxWidgets 2.8.12, Berkeley DB 4.8.30, Boost 1.35.0 디렉토리를 위치시킨다.
3. 각 폴더들의 이름을 OpenSSL, wxWidgets, DB, boost로 변경한다.
(예,  <MSYS>\OpenSSL,  <MINGW>\wxWidgets,  <MINGW>\DB,  <MINGW>\boost
4. 최신 버전의 gcc와 g++를 환경 변수에 등록한 후, MSYS를 사용하여 DB와 wxWidgets 컴파일
$ cd \DB\build_unix
$ sh ../dist/configure --enable-mingw --enable-cxx
$ cd \wxWidgets
$ ../dist/configure --enable-mingw --enable-cxx
$ make
5. 아래 영상을 확인하여, OpenSSL을 컴파일
https://www.youtube.com/watch?v=icNn-eJif9k
VS2015 x64 Native Tools Command Prompt:
$ mkdir c:\debug
$ mkdir c:\debug
$ cd c:\OpenSSL
$ perl configure debug-VC-WIN64A --prefix=c:\debug no-asm
$ ms\do_win64a
//$ notepad ms\nt.mak
$ nmake -f ms\nt.mak
$ nmake -f ms\nt.mak install
$ nmake -f ms\nt.mak clean
$ perl configure VC-WIN64A --prefix=c:\release no-asm
$ ms\do_win64a
$ nmake -f ms\nt.mak
$ nmake -f ms\nt.mak install
$ nmake -f ms\nt.mak clean
6. GCC와 G++ 3.4.5 버전으로 환경 변수를 다시 변경한 후, 비트코인 소스 디렉토리에서 컴파일
$ make

참고자료
boost: http://sourceforge.net/projects/boost/files/boost/1.35.0/boost_1_35_0.zip
db: https://download.oracle.com/berkeley-db/db-4.8.30.zip
wxWixgets: https://github.com/wxWidgets/wxWidgets/releases/download/v2.8.12/wxWidgets-2.8.12.zip\
openSSL: https://ftp.openssl.org/source/old/1.0.2/openssl-1.0.2d.tar.gz

MinGW 3.4.5: https://master.dl.sourceforge.net/project/gimpact/Tool%20MingW3.4.5%20compiler/MingW%203.4.5./MinGW_3.4.5_wx263_cg1.5b3.zip
MSYS 1.0.11: http://downloads.sourceforge.net/mingw/MSYS-1.0.11.exe
Strawberry Perl: http://strawberryperl.com/download/5.30.0.1/strawberry-perl-5.30.0.1-64bit.msi (OpenSSL compile 시 필요)
VS2015: 
7-Zip: 
```
