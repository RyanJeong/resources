#include "BlockExplorer.h"
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

extern HINSTANCE hInst;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    /*
     *  가장 자주 사용되는 메시지의 종류
     *      WM_COMMAND      : 응용 프로그램 메뉴를 처리한다.
     *      WM_QUIT         : 프로그램을 끝낼 때 발생하는 메시지이다.
     *      WM_LBUTTONDOWN  : 마우스의 좌측 버튼을 누를 경우 발생한다.
     *      WM_CHAR         : 키보드로부터 문자가 입력될 때 발생한다
     *      WM_PAINT        : 화면을 다시 그려야 할 필요가 있을 때 발생한다.
     *      WM_DESTROY      : 윈도우가 메모리에서 파괴될 때 발생한다.
     *      WM_CREATE       : 윈도우가 처음 만들어질 때 발생한다.
     *
     *      BOOL InvlidateRect(HWND hWnd, CONST RECT *lpRect, BOOL bErase); 
     *          이 함수는 윈도우의 작업 영역을 무효화시켜 윈도우즈로 하여금 WM_PAINT 메시지를 해당 윈도우로 보내도록 한다. 
     *
     *          첫번째 인수 hWnd는 무효화의 대상이 되는 윈도우, 즉 다시 그려져야 할 윈도우의 핸들이다. 
     *          이 값은 WndProc이 호출될 때 전달되는 첫번째 인수 hWnd를 그대로 써 주면 된다.
     *
     *          두번째 인수 lpRect는 무효화의 대상이 되는 사각 영역을 써 주되 이 값이 NULL이면 윈도우의 전 영역이 무효화된다. 
     *          전 영역을 무효화하면 윈도우가 완전히 다시 그려지므로 확실하게 그려지기는 하겠지만 그만큼 실행속도는 늦어지게 된다. 
     *          속도를 최대한 높이려면 변경된 최소한의 영역만을 계산하여 무효화하는 것이 좋다. 
     *          
     *          세번째 인수 bErase는 무효화되기 전에 배경을 모두 지운 후 다시 그릴 것인지 아니면 배경을 지우지 않고 그릴 것인지를 지정한다. 
     *          이 값이 TRUE이면 배경을 지운 후 다시 그리고 FALSE이면 배경을 지우지 않은채로 다시 그린다. 
     */

    switch (message) {
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDM_HELP_ABOUT:
                    DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                    
                    return 0;
                case IDM_FILE_EXIT:
                    DestroyWindow(hWnd);
                    
                    return 0;
                case IDM_COIN_BITCOIN:
                    /*  Do something... */
                    
                    return 0;
                case IDM_COIN_ETHEREUM:
                    /*  Do something... */
                    
                    return 0;
                case IDM_BLOCK:
                    getBestBlockHash();
                    InvalidateRect(hWnd, NULL, TRUE);

                    return 0;
                case IDM_TX:
                    /*  Do something... */

                    return 0;
                case IDM_BLOCKCHAIN:
                    getBlockchainInfo();
                    InvalidateRect(hWnd, NULL, TRUE);

                    return 0;
                case IDM_MEMORYPOOL:
                    getMemPoolInfo();
                    InvalidateRect(hWnd, NULL, TRUE);

                    return 0;
                case IDM_MINING:
                    getMiningInfo();
                    InvalidateRect(hWnd, NULL, TRUE);

                    return 0;
                case IDM_NETWORK:
                    getNetworkInfo();
                    InvalidateRect(hWnd, NULL, TRUE);

                    return 0;
                case IDM_WALLET:
                    getWalletInfo();
                    InvalidateRect(hWnd, NULL, TRUE);

                    return 0;
                default:

                    return DefWindowProc(hWnd, message, wParam, lParam);
            }
        case WM_PAINT:
            Wm_Paint(hWnd);

            return 0;
        case WM_CLOSE:
            DestroyWindow(hWnd);

            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);

            return 0;
        default:
            /*
             *  DefWindowProc(): WinProc과 함수 인자가 동일하며, 시스템 캡션을 눌렀을 때 나오는 메뉴나 최소화, 최대화, 윈도우 이동, 크기 변경 등을
             *  직접 처리해주지 않아도 운영체제가 알아서 처리해주는 역할을 수행
             */

            return DefWindowProc(hWnd, message, wParam, lParam);
    }
}