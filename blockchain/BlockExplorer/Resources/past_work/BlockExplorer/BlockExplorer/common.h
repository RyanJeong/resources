#define CURL_STATICLIB

#include <curl/curl.h>
#include <windows.h>
#include <stdlib.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wldap32.lib")
#pragma comment(lib, "libcurl.lib")

#define MAINMENU        101
#define M_FILE          1001
#define COINS_BITCOIN   2000
#define COINS_ETHEREUM  2001
#define COINS_EXIT      2002
#define M_BLOCK         3001
#define M_ABOUT         4001
#define M_EXIT          5001

/*  paint.cpp    */
void				Wm_Paint(HWND);

/*  proc.cpp    */
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

/*  main.cpp    */
int WINAPI			WinMain(HINSTANCE, HINSTANCE, LPSTR, int);

/*  bitcoin.cpp    
size_t              WriteMemoryCallback(void *, size_t, size_t, void *);
int                 getBlockHash();*/