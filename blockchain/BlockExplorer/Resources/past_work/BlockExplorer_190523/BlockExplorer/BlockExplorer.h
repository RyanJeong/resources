#pragma once

#include "targetver.h"
#include "Resource.h"
#include <tchar.h>

#define WIN32_LEAN_AND_MEAN /*  거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.  */

/*  Windows 헤더 파일입니다.  */
#include <windows.h>

/*  C 런타임 헤더 파일입니다. */
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

/*  cURL 사용에 필요한 파일입니다. */
#define CURL_STATICLIB

#include <curl/curl.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wldap32.lib")
#pragma comment(lib, "libcurl.lib")

/*  Macro 선언부 입니다   */
#define MAX_LOADSTRING 100

/*  paint.cpp    */
void				Wm_Paint(HWND);

/*  proc.cpp    */
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

/*  main.cpp   */
int APIENTRY        wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int);
void                showConsole(void);

/*  global.cpp  */
ATOM                MyRegisterClass(HINSTANCE);
BOOL                InitInstance(HINSTANCE, int);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

/*  bitcoin.cpp */
void                getBlockHash(int);
void                getBlock(char *);
void                getRawTransaction(char *);
void                decodeRawTransaction(char *);
void                getBlockchainInfo(void);
void                getMemPoolInfo(void);
void                getMiningInfo(void);
void                getNetworkInfo(void);
void                getWalletInfo(void);
void                getBestBlockHash(void);
size_t              WriteMemoryCallback(void *, size_t, size_t, void *);