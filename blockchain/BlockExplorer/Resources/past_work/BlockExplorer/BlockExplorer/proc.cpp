#include "common.h"

/*
 *  typedef struct tagMSG {
 *      HWND        hwnd;
 *      UINT        message;
 *      WPARAM      wParam;
 *      LPARAM      lParam;
 *      DWORD       time;
 *      POINT       pt;
 *  } MSG;
 * 
 *      hwnd    : 메시지를 받을 윈도우 핸들이다.
 *      message : 어떤 종류의 메시지인가를 나타낸다. 가장 중요한 값이다.
 *      wParam  : 전달된 메시지에 대한 부가적인 정보를 가진다. 어떤 의미를 가지는가는 메시지별로 다르다. 32비트값이다.
 *      lParam  : 전달된 메시지에 대한 부가적인 정보를 가진다. 어떤 의미를 가지는가는 메시지별로 다르다. 32비트값이다.
 *      time    : 메시지가 발생한 시간이다.
 *      pt      : 메시지가 발생했을 때의 마우스 위치이다.
 */

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    /*
     *  가장 자주 사용되는 메시지의 종류   
     *      WM_QUIT         : 프로그램을 끝낼 때 발생하는 메시지이다.
     *      WM_LBUTTONDOWN  : 마우스의 좌측 버튼을 누를 경우 발생한다.
     *      WM_CHAR         : 키보드로부터 문자가 입력될 때 발생한다
     *      WM_PAINT        : 화면을 다시 그려야 할 필요가 있을 때 발생한다.
     *      WM_DESTROY      : 윈도우가 메모리에서 파괴될 때 발생한다.
     *      WM_CREATE       : 윈도우가 처음 만들어질 때 발생한다.
     */
    switch (iMessage) {
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case M_FILE:
                    /*  Do something ...    */
                    break;
                case COINS_BITCOIN:
                    /*  Do something ...    */
                    break;
                case COINS_ETHEREUM:
                    /*  Do something ...    */
                    break;
                case COINS_EXIT:
                    PostMessage(
                        hWnd,
                        WM_CLOSE,
                        0,
                        0);
                    break;
                case M_BLOCK:
                    //getBlockHash();
                    break;
                case M_ABOUT:
                    MessageBox(
                        hWnd,
                        TEXT("CONTENT"),
                        TEXT("TITLE"),
                        MB_OK);
                    break;
                case M_EXIT:
                    PostMessage(
                        hWnd,
                        WM_CLOSE,
                        0,
                        0);
                    break;
            }
            break;
        case WM_PAINT:
            Wm_Paint(hWnd);
            break;
        case WM_CLOSE: 
            DestroyWindow(hWnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);

            return 0;
        default:
            break;
    }

    /*
     *  DefWindowProc(): WinProc과 함수 인자가 동일하며, 시스템 캡션을 눌렀을 때 나오는 메뉴나 최소화, 최대화, 윈도우 이동, 크기 변경 등을 
     *  직접 처리해주지 않아도 운영체제가 알아서 처리해주는 역할을 수행
     */
    return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}