#pragma once

#include "targetver.h"
#include "Resource.h"

#define WIN32_LEAN_AND_MEAN /*  거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.  */

/*  DEBUG 관련 매크로 입니다    */
#define DEBUG_MODE          1
#define DEBUG_BITCOIN_CPP   1
#define DEBUG_GLOBAL_CPP    0
#define DEBUG_MAIN_CPP      0
#define DEBUG_PAINT_CPP     0
#define DEBUG_PROC_CPP      1

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

/*  마우스 휠 이벤트 처리를 위한 헤더 함수 및 Macro */
#include "winuser.h"

#ifndef WM_MOUSEWHEEL 
#define WM_MOUSEWHEEL 0x020A    //마우스 휠 이벤트 메세지 이름 
#endif

/*  Macro 선언부 입니다   */
#define MAX_LOADSTRING  100     //  제목 표시줄 텍스트 길이

#define BTC_GET_BLOCK_NUM_OF_ROWS   18
#define BTC_TX_SELECTED             18
#define BTC_TX_INDEX                9
#define HASH_SIZE               65
#define HASH_OFFSET             64
#define BITS_SIZE               9
#define BUF_SIZE                1024
#define WINDOW_MIN_SIZE         0
#define WINDOW_WIDTH            1280    //  16:9, 1280 x 720
#define WINDOW_HEIGHT           720     //  16:9, 1280 x 720
#define WINDOW_HEADER_HEIGHT    128
#define ROW_OFFSET              40
#define GET_BLOCK_FIELD_WIDTH   240
#define GET_TX_VALUE_WIDTH      640
#define POPUP_WIDTH             480
#define POPUP_HEIGHT            144
#define POPUP_X_POS             640
#define POPUP_Y_POS             240
#define POPUP_OFFSET            20
#define POPUP_CONTENT_OFFSET    10
#define MAX_POPUP_NUM           20
#define EXCEED_MAX_MUN          TEXT("Exceeding maximum number of popup.")


#define MAX_NUM_OF_ROWS     15

#define POPUP_WIDTH             480
#define POPUP_HEIGHT            144
#define POPUP_X_POS             640
#define POPUP_Y_POS             240
#define POPUP_OFFSET            20
#define POPUP_CONTENT_OFFSET    10
#define MAX_POPUP_NUM           20
#define EXCEED_MAX_MUN          TEXT("Exceeding maximum number of popup.")

#define NOT_USED            (-1)

#define OCCURRED_ERROR      (-1)

#define AUTO_CALC_LENGTH    (-1)

#define SEARCH_BOX_X_POS    8
#define SEARCH_BOX_Y_POS    (WINDOW_HEADER_HEIGHT >> 1)
#define SEARCH_BOX_WIDTH    128
#define SEARCH_BOX_HEIGHT   24
#define SEARCH_BTN_X_POS    SEARCH_BOX_X_POS + SEARCH_BOX_WIDTH
#define SEARCH_BTN_Y_POS    SEARCH_BOX_Y_POS
#define SEARCH_BTN_WIDTH    (SEARCH_BOX_WIDTH >> 1)
#define SEARCH_BTN_HEIGHT   SEARCH_BOX_HEIGHT

#define BTC_GETBLOCK_LENGTH 720

/*  paint.cpp    */
void				Wm_Paint(HWND);

/*  proc.cpp    */
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	WndProcPopup(HWND, UINT, WPARAM, LPARAM);

/*  main.cpp   */
int APIENTRY        wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int);
void                showConsole(void);

/*  global.cpp  */
ATOM                MyRegisterClass(HINSTANCE);
BOOL                InitInstance(HINSTANCE, int);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

/*  bitcoin.cpp */
 int    getBlockCount(void);
 int    getBlock(int);
 int    getSerializedBlock(int);
 int    getRawTransaction(void);
 int    getBlockchainInfo(void);
 int    getMempoolInfo(void);
 int    getMiningInfo(void);
 int    getNetworkInfo(void);
 void   freeMemory(void);