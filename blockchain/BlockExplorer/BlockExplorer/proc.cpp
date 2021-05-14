/*  ************************************************************************************************************************
 *  DestroyWindow 부분 예외처리 추가할 것(화면을 끄지 않고 UI 상에 상태 메세지를 띄워 프로그램이 계속 실행될 수 있도록 조치)
 *  ************************************************************************************************************************/

#include "BlockExplorer.h"
#include "proc.h"
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

extern char         **BTC_tx;
extern HINSTANCE    hInst;
extern int          BTC_getBlockCount, BTC_getBlock_nTx;    //  BTC_getBlockCount: 블록의 총 갯수를 나타냄, BTC_getBlock_nTx: 해당 블록의 총 거래수를 나타냄
extern int          BTC_getBlock_initMemDC;                 //  server로 명령을 요청하여 얻은 결과가 바뀌게 되면(즉 화면을 다른 내용으로 갱신해야 하면, 스크롤하여 상하 이동하는 것 제외)
extern int          BTC_tx_initMemDC;                       //  *_initMemDC 변수들을 0으로 초기화하여 MemDC에 새로운 내용으로 채워넣음

static int  countPopup = 0;     //  블록의 field를 클릭하여 띄운 팝업 창 수를 나타냄
static HWND hEdit, hBtn;        //  에디트 창과 버튼 창을 만드는 데 사용
static int  index = NOT_USED;   //  index 나타냄

/*
각 메뉴마다 scroll position 기억
#define IDM_FILE_EXIT           1001
#define IDM_COIN_BITCOIN        2001
#define IDM_COIN_ETHEREUM       2002
#define IDM_BLOCK               3000
#define IDE_BLOCK               3001
#define IDB_BLOCK               3002
#define IDP_BLOCK               3003
#define IDM_SERIALIZED_BLOCK    4000
#define IDE_SERIALIZED_BLOCK    4001
#define IDM_TX                  5000
#define IDM_BLOCKCHAIN          6000
#define IDM_MEMPOOL             7000
#define IDM_MINING              8000
#define IDM_NETWORK             9000
#define IDM_HELP_ABOUT          9999
*/
static int          BTC_getBlock_scrollPos;
static int          BTC_getSerializedBlock_scrollPos;
static int          BTC_tx_scrollPos;
static int          isFieldClick    = false;
static int          isTxClick       = false;

TCHAR       convertCharToTCHAR[BUF_SIZE] = { 0, };
TCHAR       msg[BUF_SIZE];              //  sprintf에 사용하기 위한 버퍼
int         BTC_tempScroll, BTC_getBlock_maxScroll, BTC_getSerializedBlock_maxScroll, BTC_tx_maxScroll; //  BTC_tempScroll에 스크롤을 계산하기 위한 값을 저장해놓음
int         selectedItem = NOT_USED;    //  마우스가 어느 필드를 가리키는가를 저장하는 변수
int         procState;                  //  현재 메뉴 상태값을 저장
int         input_number;
int         currX, currY;               //  마우스 포인트 좌표 값

SCROLLINFO  scr;                        //  스크롤 정보 저장 구조체

extern TCHAR    columnName[BTC_GET_BLOCK_NUM_OF_ROWS][BUF_SIZE];
TCHAR           columnNameDetail[BTC_GET_BLOCK_NUM_OF_ROWS][BUF_SIZE] = {
    TEXT("블록 해더의 해시 값입니다."),
    TEXT("블록이 메인 체인에 연결되기 위해 검증받은 횟수를 나타냅니다."),
    TEXT("블록 내에서 전자 서명(witness data)을 제외한 크기(Byte)를 나타냅니다."),
    TEXT("블록의 총 크기(Byte)를 나타냅니다."),
    TEXT("BIP 141에서 정의된 블록의 데이터 양을 나타내며, 단위는 Weight-Unit(WU)입니다."),
    TEXT("메인 체인 내에서의 블록의 상대적인 위치를 나타내며, 인덱스라고도 불립니다."),
    TEXT("블록의 버전을 나타내는 10진수 값입니다."),
    TEXT("블록의 버전을 16진수 값 형태로 표현한 값입니다."),
    TEXT("블록에 포함되어 있는 거래들을 머클 트리 자료구조에 적용하여 얻은 루트의 해시 값입니다."),
    TEXT("블록에 포함되어 있는 거래들은 거래의 고유 번호(Tx ID)로 표현됩니다.\r\n 해당 값은 현재 조회한 블록에 포함된 총 거래 갯수를 나타냅니다."),
    TEXT("해당 블록이 언제 생성되었는지 유닉스 시간을 기준으로 초로 환산한 값입니다."),
    TEXT("해당 블록 이전에 생성된 11개의 블록들의 time 값들 중 중간값(median)을 나타냅니다."),
    TEXT("블록 헤더의 해시 값을 계산하기 위해 사용되는 필드 중 하나로,\r\n블록 헤더의 해시 값 계산에는 총 6개의 입력값이 필요합니다."),
    TEXT("채굴 난이도를 결정하는 difficulty 값을 조절하는데 사용되는 값입니다."),
    TEXT("블록 채굴 난이도를 나타냅니다."),
    TEXT("Genesis 블록부터 해당 블록까지 채굴하기 위해 얼마큼의 해시가 필요한지 예상한 값입니다."),
    TEXT("해당 블록의 이전 해시 값을 나타냅니다."),
    TEXT("해당 블록의 다음 해시 값을 나타냅니다.") };

/*
 *  WndProc:    Event를 처리하는 함수입니다. 
 */
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    /*
     *  가장 자주 사용되는 메시지의 종류
     *      WM_COMMAND      :   응용 프로그램 메뉴를 처리한다.
     *      WM_QUIT         :   프로그램을 끝낼 때 발생하는 메시지이다.
     *      WM_LBUTTONDOWN  :   마우스의 좌측 버튼을 누를 경우 발생한다.
     *      WM_CHAR         :   키보드로부터 문자가 입력될 때 발생한다
     *      WM_PAINT        :   화면을 다시 그려야 할 필요가 있을 때 발생한다.
     *      WM_DESTROY      :   윈도우가 메모리에서 파괴될 때 발생한다.
     *      WM_CREATE       :   윈도우가 처음 만들어질 때 발생한다.
     *      WM_NCPAINT      :   비작업영역이 그려져야 할때 운영체제가 보내는 메시지이며 응용프로그램 이 메시지를
     *                          받았을 때 타이틀 바, 경계선, 시스템 메뉴 등을 그려야 한다.
     *                          NCPAINT의 접두어 NC는 Non Clien Area 즉, 비작업영역이란 뜻이다.
     *                          이 메시지는 직접 처리하지 않아도 DefWindowProc에서 처리하며 대부분의 응용 프로그램이 이 메시지를 처리하지 않는다. 
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

    BOOL            check;          //  handle 결과, 에러 시 false 입력
    int             prevPos;  //  prevPos는 이전 스크롤 위치를 저장하고, temp는 계산 결과를 임시로 저장하는 변수
    LPMINMAXINFO    mmi;            //  윈도우 창 크기 조절에 사용
    int             popupOffset;    //  여러 팝업을 띄울 때 팝업 간 간격을 띄우기 위해 사용
    switch (message) {
    case WM_CREATE:
        scr.cbSize = sizeof(SCROLLINFO);
        /*
        *  SIF_ALL             모든 속성의 조합
        *  SIF_DISABLENOSCROLL 스클로바 전체 비활성화
        *  SIF_PAGE            페이지 사이즈
        *  SIF_POS             스크롤 박스의 위치
        *  SIF_RANGE           스크롤 범위의 최소/최대
        *  SIF_TRACKPOS        드래깅 상태의 스크롤박스 현재 위치
        */
        scr.fMask   = SIF_ALL;      //SIF_PAGE | SIF_RANGE | SIF_POS | SIF_TRACKPOS;
        scr.nMin    = MIN;
        scr.nMax    = MIN;
        //scr.nPage   = PAGE;   difference each types, so deprecated
        SetScrollInfo(hWnd, SB_VERT, &scr, TRUE);
        hEdit   = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_BORDER, SEARCH_BOX_X_POS, SEARCH_BOX_Y_POS, SEARCH_BOX_WIDTH, SEARCH_BOX_HEIGHT, hWnd, (HMENU) IDE_BLOCK, hInst, NULL);
        hBtn    = CreateWindow(TEXT("button"), TEXT("Search"), WS_CHILD | BS_PUSHBUTTON, SEARCH_BTN_X_POS, SEARCH_BTN_Y_POS, SEARCH_BTN_WIDTH, SEARCH_BTN_HEIGHT, hWnd, (HMENU) IDB_BLOCK, hInst, NULL);
        
        return 0;
    case WM_MOUSEMOVE:
        if (procState == IDM_BLOCK) {
            selectObjectInGetBlock(hWnd, lParam);
        } else if (procState == IDM_TX) {
            selectObjectInTx(hWnd, lParam);
        } else {
            // nothing ...
        }

        return 0;
    case WM_SETCURSOR:
        if (procState == IDM_BLOCK) {
            isFieldClick ? SetCursor(LoadCursor(nullptr, IDC_HAND)) : (isTxClick ? SetCursor(LoadCursor(nullptr, IDC_HAND)) : SetCursor(LoadCursor(nullptr, IDC_ARROW)));
        } else if (procState == IDM_TX) {
            isFieldClick ? SetCursor(LoadCursor(nullptr, IDC_HAND)) : SetCursor(LoadCursor(nullptr, IDC_ARROW));
        } else {
            SetCursor(LoadCursor(nullptr, IDC_ARROW));
        }

        return 0;

    /*
    *  버튼	    누름	            놓음	        더블클릭
    *  좌측	    WM_LBUTTONDOWN	    WM_LBUTTONUP	WM_LBUTTONDBLCLK
    *  우측	    WM_RBUTTONDOWN	    WM_RBUTTONUP	WM_RBUTTONDBLCLK
    *  중앙	    WM_MBUTTONDOWN	    WM_MBUTTONUP	WM_MBUTTONDBLCLK
    */
    case WM_LBUTTONDOWN:
        if (procState == IDM_BLOCK) {
            selectObjectInGetBlock(hWnd, lParam);       // 현재 마우스 위치 정보 update
            if (isFieldClick) {
                popupOffset = (countPopup * POPUP_OFFSET);
                if (countPopup > MAX_POPUP_NUM) {
                    MessageBox(hWnd, EXCEED_MAX_MUN, NULL, MB_ICONSTOP | MB_OK | MB_SYSTEMMODAL);
                } else {
                    CreateWindow(TEXT("mypopup"), *(columnName + selectedItem), WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE, (POPUP_X_POS + POPUP_OFFSET), (POPUP_Y_POS + POPUP_OFFSET), POPUP_WIDTH, POPUP_HEIGHT, hWnd, (HMENU)0, hInst, NULL);
#if (DEBUG_MODE && DEBUG_PROC_CPP)
                    printf("IDM_BLOCK Clicked\n");
#endif
                    ++countPopup;
                }
            } else if (isTxClick) {
                isTxClick = false;
                selectedItem = NOT_USED;
                printTx(hWnd);
            }
        } else if (procState == IDM_TX) {
            selectObjectInTx(hWnd, lParam);     //  현재 마우스 위치 정보 update
            if (isFieldClick) {
                popupOffset = (countPopup * POPUP_OFFSET);
                if (countPopup > MAX_POPUP_NUM) {
                    MessageBox(hWnd, EXCEED_MAX_MUN, NULL, MB_ICONSTOP | MB_OK | MB_SYSTEMMODAL);
                } else {
                    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, *(BTC_tx + selectedItem), strlen(*(BTC_tx + selectedItem)), convertCharToTCHAR, BUF_SIZE);
                    wsprintf(msg, TEXT("%s"), convertCharToTCHAR);
                    CreateWindow(TEXT("mypopup"), msg, WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE, (POPUP_X_POS + POPUP_OFFSET), (POPUP_Y_POS + POPUP_OFFSET), POPUP_WIDTH, POPUP_HEIGHT, hWnd, (HMENU)0, hInst, NULL);
#if (DEBUG_MODE && DEBUG_PROC_CPP)
                    printf("IDM_TX Clicked\n");
#endif
                    ++countPopup;
                }
            }
        } else {
            // nothing .. 
        }

        return 0;
    case WM_GETMINMAXINFO:
        mmi = (LPMINMAXINFO) lParam;
        mmi->ptMinTrackSize.x   = WINDOW_WIDTH;
        mmi->ptMinTrackSize.y   = WINDOW_HEIGHT;
        mmi->ptMaxTrackSize.x   = WINDOW_WIDTH;
        mmi->ptMaxTrackSize.y   = WINDOW_HEIGHT;

        //mmi->ptMaxPosition.x    = 10;
        //mmi->ptMaxPosition.y    = 10;
        //mmi->ptMaxSize.x        = 800;
        //mmi->ptMaxSize.y        = 200;

        return 0;
    case WM_MOUSEWHEEL:
        prevPos = scr.nPos;
        //  (short)HIWORD(wParam) 아래 쪽 스크롤: -120~, 위 쪽 스크롤: 120~ 
        //  단순히 어느 방향 휠인지 구분하기 위한 조건문
        scr.nPos = (((short) HIWORD(wParam)) < 0) ? min(scr.nMax, scr.nPos + SCROLL_MOVE_INTERVAL) : max(scr.nMin, scr.nPos - SCROLL_MOVE_INTERVAL);
        if (prevPos != scr.nPos) {
            SetScrollInfo(hWnd, SB_VERT, &scr, TRUE);
            InvalidateRect(hWnd, NULL, FALSE);
        }

        return 0;
    case WM_VSCROLL:
        prevPos = scr.nPos;
        if (!GetScrollInfo(hWnd, SB_VERT, &scr)) {

            return OCCURRED_ERROR; // GetScrollInfo failed
        }

#if (DEBUG_MODE && DEBUG_PROC_CPP)
        printf("scr.nTrackPos : %d\n", scr.nTrackPos);
#endif
        switch (LOWORD(wParam)) {
            case SB_TOP:
                scr.nPos = scr.nMin;
                break;
            case SB_BOTTOM:
                scr.nPos = scr.nMax;
                break;
            case SB_LINEUP:
                scr.nPos = max(scr.nMin, scr.nPos - SCROLL_MOVE_INTERVAL);
                break;
            case SB_LINEDOWN:
                scr.nPos = min(scr.nMax, scr.nPos + SCROLL_MOVE_INTERVAL);
                break;
            case SB_PAGEUP:
                scr.nPos = max(scr.nMin, scr.nPos - SCROLL_PAGE_INTERVAL);
                break;
            case SB_PAGEDOWN:
                scr.nPos = min(scr.nMax, scr.nPos + SCROLL_PAGE_INTERVAL);
                break;
            case SB_THUMBTRACK:
                scr.nPos = (scr.nTrackPos == scr.nMax) ? scr.nMax : (scr.nTrackPos & ((~0) << SCROLL_INTERVAL));
                break;
        }
        if (prevPos != scr.nPos) {
            SetScrollInfo(hWnd, SB_VERT, &scr, TRUE);
            InvalidateRect(hWnd, NULL, FALSE);
        }

        return 0;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDM_HELP_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_FILE_EXIT:
            if (procState != IDM_FILE_EXIT) {
                DestroyWindow(hEdit);
                DestroyWindow(hBtn);
            }
            procState = IDM_FILE_EXIT;
            DestroyWindow(hWnd);
            break;
        case IDM_COIN_BITCOIN:
            if (procState != IDM_COIN_BITCOIN) {
                DestroyWindow(hEdit);
                DestroyWindow(hBtn);
            }
            procState = IDM_COIN_BITCOIN;

            /*  Do something... */
            break;
        case IDM_COIN_ETHEREUM:
            if (procState != IDM_COIN_ETHEREUM) {
                DestroyWindow(hEdit);
                DestroyWindow(hBtn);
            }
            procState = IDM_COIN_ETHEREUM;

            /*  Do something... */
            break;
        case IDM_BLOCK:
            printGetBlock(hWnd);
            break;
        case IDB_BLOCK:
            input_number = GetDlgItemInt(hWnd, IDE_BLOCK, &check, FALSE);
            if (!check || (index > BTC_getBlockCount)) {
                MessageBox(hWnd, TEXT("잘못 입력하셨습니다."), TEXT("Error"), MB_OK);
            } else {
                if (input_number != index) {        //  현재 검색한 값과 이전 검색한 값이 다를 경우
                    index = input_number;
                    BTC_tx_initMemDC        = 0;
                    BTC_getBlock_initMemDC  = 0;
                    if (getBlock(index) < 0) { // negative: besthash, else: index
                        DestroyWindow(hWnd);
                    }
                    if (procState == IDM_TX) {              // must do getBlock(index) before getRawTransaction()
                        if (getRawTransaction() < 0) {      // param: 검색한 블록의 tx 인덱스
                            DestroyWindow(hWnd);
                        }
                        BTC_tx_maxScroll    = (BTC_getBlock_nTx * ROW_OFFSET) + BTC_tempScroll;
                        input_number        = (BTC_tx_maxScroll < 0) ? 0 : BTC_tx_maxScroll;
                        if (scr.nMax != input_number) {     //  스크롤 최대 값이 바뀐 경우, 새로운 스크롤 정보로 갱신해야 함
                            scr.nMax = input_number;
                            SetScrollInfo(hWnd, SB_VERT, &scr, TRUE);
                        }
                        //  scr.nPos <-> BTC_tx_scrollPos
                        //  nPos는 현재 스크롤 값, BTC_tx_scrollPos는 다른 상태로 전환할 때의 현재 스크롤 값을 저장한 값, 따라서 scr.nPos를 사용해야 함
                        scr.nPos = (scr.nPos > scr.nMax) ? scr.nMax : scr.nPos;           // tx view에서 스크롤은 가변이므로, 조건 검사를 통해 맞는 스크롤 값을 넣어야 함
                    }
                    InvalidateRect(hWnd, NULL, FALSE);
                }
            }
            break;
        case IDM_SERIALIZED_BLOCK:
            printGetSerializedBlock(hWnd);
            break;
        case IDM_TX:        //  TO-DO: must do printTx(hWnd) after getBlock(index);
            printTx(hWnd);
            break;
        case IDM_BLOCKCHAIN:
            if (getBlockchainInfo() < 0) {
                DestroyWindow(hWnd);
            } else {
                procState = IDM_BLOCKCHAIN;
                InvalidateRect(hWnd, NULL, TRUE);
                ShowWindow(hEdit, SW_HIDE);
                ShowWindow(hBtn, SW_HIDE);
            }
            break;
        case IDM_MEMPOOL:
            if (getMempoolInfo() < 0) {
                DestroyWindow(hWnd);
            } else {
                procState = IDM_MEMPOOL;
                InvalidateRect(hWnd, NULL, TRUE);
                ShowWindow(hEdit, SW_HIDE);
                ShowWindow(hBtn, SW_HIDE);
            }
            break;
        case IDM_MINING:
            if (getMiningInfo() < 0) {
                DestroyWindow(hWnd);
            } else {
                procState = IDM_MINING;
                InvalidateRect(hWnd, NULL, TRUE);
                ShowWindow(hEdit, SW_HIDE);
                ShowWindow(hBtn, SW_HIDE);
            }
            break;
        case IDM_NETWORK:
            if (getNetworkInfo() < 0) {
                DestroyWindow(hWnd);
            } else {
                procState = IDM_NETWORK;
                InvalidateRect(hWnd, NULL, TRUE);
                ShowWindow(hEdit, SW_HIDE);
                ShowWindow(hBtn, SW_HIDE);
            }
            break;
        }

        return 0;
    case WM_PAINT:
        Wm_Paint(hWnd);

        return 0;
    case WM_CLOSE:
        DestroyWindow(hWnd);

        return 0;
    case WM_DESTROY:
#if (DEBUG_MODE && DEBUG_PROC_CPP)
        printf("MAIN_WM_DESTROY\n");
#endif
        freeMemory();
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

/*
 *  WndProcPopup:   팝업 윈도우의 event를 처리하는 함수입니다. 
 */
LRESULT CALLBACK WndProcPopup(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    HDC         hdc;
    PAINTSTRUCT ps;
    RECT        rt;
    
    rt = {POPUP_CONTENT_OFFSET, POPUP_CONTENT_OFFSET, (POPUP_WIDTH - (POPUP_CONTENT_OFFSET << 1)), (POPUP_HEIGHT - (POPUP_CONTENT_OFFSET << 1))};
#if (DEBUG_MODE && DEBUG_PROC_CPP)
    printf("WndProcPopup: %s\n", "WndProcPopup");
#endif
    switch (iMessage) {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        if (procState == IDM_BLOCK) { 
            DrawText(hdc, columnNameDetail[selectedItem], AUTO_CALC_LENGTH, &rt, DT_CENTER | DT_WORDBREAK);
        } else if (procState == IDM_TX) {
            //  add info. here
            DrawText(hdc, TEXT("거래 정보 자세히 기술"), AUTO_CALC_LENGTH, &rt, DT_CENTER | DT_WORDBREAK);
        }
        EndPaint(hWnd, &ps);

        return 0;
    case WM_DESTROY:
        --countPopup;
#if (DEBUG_MODE && DEBUG_PROC_CPP)
        printf("Popup destroyed\n");
#endif

        return 0;
    default:

        return DefWindowProc(hWnd, iMessage, wParam, lParam);
    }
}

/*
 *  printGetBlock: getBlock 내용을 출력하는 함수입니다. 
 */
void printGetBlock(HWND hWnd)
{
    saveCurrScrollPos(procState);
    if (procState != IDM_BLOCK) {
        procState = IDM_BLOCK;
        if (!BTC_getBlock_initMemDC) {
            if (getBlockCount() < 0) {
                DestroyWindow(hWnd);
            }
            if (getBlock(((index < 0) ? BTC_getBlockCount : index)) < 0) { // getBlock(index), if (index = negative) then besthash, else index
                DestroyWindow(hWnd);
            }
        }
        scr.nMax = BTC_getBlock_maxScroll;
        scr.nPos = BTC_getBlock_scrollPos;
        /*
        *  Window Class    |   컨트롤
        *  ----------------+--------------------------
        *  button          |   버튼, 체크, 라디오
        *  static          |   텍스트
        *  scrollbar       |   스크롤 바
        *  edit            |   에디트
        *  listbox         |   리스트 박스
        *  combobox        |   콤보 박스
        *      ->  edit 클래스 사용하려면 첫 번째 파라미터에 "edit"을 입력해야 한다.
        *
        *  ES_AUTOHSCROLL  : 수평 스크롤을 지원한다.
        *  ES_AUTOVSCROLL  : 여러줄 편집시 수직 스크롤을 지원한다.
        *  ES_LEFT         : 왼쪽 정렬한다.
        *  ES_CENTER       : 중앙 정렬한다.
        *  ES_RIGHT        : 오른쪽 정렬한다.
        *  ES_LOWERCASE    : 소문자로 변환하여 표시한다.
        *  ES_UPPERCASE    : 대문자로 변환하여 표시한다.
        *  ES_MULTILINE    : 여러줄을 편집할 수 있도록 한다.
        *  ES_NOHIDESEL    : 포커스를 잃더라도 선택된 영역을 표시한다.
        *  ES_READONLY     : 읽기전용으로 만들어 편집을 금지한다.
        *
        *  SS_LEFT	            : 왼쪽으로 정렬되는 텍스트이며 자동으로 개행된다.
        *  SS_LEFTNOWORDWRAP    : 왼쪽으로 정렬되는 텍스트이며 자동으로 개행되지 않는다.
        *  SS_CENTER	        : 중앙으로 정렬되는 텍스트이며 자동으로 개행된다.
        *  SS_RIGHT	            : 오른쪽으로 정렬는 텍스트이며 자동으로 개행된다.
        *  SS_SIMPLE	        : 단순한 문자열이며 자동개행되지 않는다.
        *  SS_WHITEFRAME	    : 윈도우의 배경색으로 그려지는 사각형(기본 하얀색)
        *  SS_WHITERECT	        : 윈도우의 배경색으로 그려지는 속이 채워진 사각형(기본 하얀색)
        *  SS_BLACKFRAME	    : 화면 배경색으로 그려지는 사각형(기본 검은색)
        *  SS_BLACKRECT	        : 화면 배경색으로 그려지는 속이 채워진 사각형(기본 검은색)
        *  SS_GRAYFRAME	        : 윈도우 프레임 색상으로 그려지는 사각형
        *  SS_GRAYRECT	        : 윈도우 프레임 색상으로 그려지는 속이 채워진 사각형
        *  SS_ICON	            : 대화상자내에서 아이콘 출력
        *  SS_NOPREFIX	        : &문자를 단축키 지정에 사용하지 않고 그대로 출력한다.
        */
    } else {
        scr.nPos = 0;
    }
    SetScrollInfo(hWnd, SB_VERT, &scr, TRUE);
    InvalidateRect(hWnd, NULL, TRUE);
    ShowWindow(hEdit, SW_SHOW);
    ShowWindow(hBtn, SW_SHOW);

    return;
}

/*
 *  printGetSerializedBlock: getSerializedBlock 내용을 출력하는 함수입니다.
 */
void printGetSerializedBlock(HWND hWnd)
{
    saveCurrScrollPos(procState);
    procState = IDM_SERIALIZED_BLOCK;
    if (getBlockCount() < 0) {
        DestroyWindow(hWnd);
    }
    if (getSerializedBlock(index) < 0) { // negative: besthash, else: index
        DestroyWindow(hWnd);
    } else {
        scr.nMax = BTC_getSerializedBlock_maxScroll;
        scr.nPos = BTC_getSerializedBlock_scrollPos;
        SetScrollInfo(hWnd, SB_VERT, &scr, TRUE);
        InvalidateRect(hWnd, NULL, TRUE);
        ShowWindow(hEdit, SW_SHOW);
        ShowWindow(hBtn, SW_SHOW);
    }

    return;
}

/*
 *  printTx: tx 내용을 출력하는 함수입니다.
 */
void printTx(HWND hWnd)
{
    saveCurrScrollPos(procState);
    procState = IDM_TX;
    if (getRawTransaction() < 0) {   // param: 검색한 블록의 tx 인덱스
        DestroyWindow(hWnd);
    } else {
        BTC_tx_maxScroll    = (BTC_getBlock_nTx * ROW_OFFSET) + BTC_tempScroll;
        scr.nMax            = (BTC_tx_maxScroll < 0) ? 0 : BTC_tx_maxScroll;
        scr.nPos            = (BTC_tx_scrollPos > scr.nMax) ? scr.nMax : BTC_tx_scrollPos;           // tx view에서 스크롤은 가변이므로, 조건 검사를 통해 맞는 스크롤 값을 넣어야 함
        SetScrollInfo(hWnd, SB_VERT, &scr, TRUE);
        InvalidateRect(hWnd, NULL, TRUE);
        ShowWindow(hEdit, SW_SHOW);
        ShowWindow(hBtn, SW_SHOW);
    }

    return;
}

/*
 *  saveCurrScrollPos: 상단 메뉴 바에서 다른 메뉴를 클릭하여 화면을 전환할 때, 이전 화면에서의 스크롤 위치를 변수에 저장하는 역할을 수행하는 함수
 */
void saveCurrScrollPos(int procState)
{
    switch (procState) {
    case IDM_BLOCK:
        BTC_getBlock_scrollPos = scr.nPos;
        break;
    case IDM_SERIALIZED_BLOCK:
        BTC_getSerializedBlock_scrollPos = scr.nPos;
        break;
    case IDM_TX:
        BTC_tx_scrollPos = scr.nPos;
        break;
    }

    return;
}

void selectObjectInGetBlock(HWND hWnd, LPARAM lParam)
{
    currX = LOWORD(lParam);
    currY = HIWORD(lParam);
#if (DEBUG_MODE && DEBUG_PROC_CPP)
    printf("IDM_BLOCK x: %d, y: %d, selectedItem: %d\n", currX, currY, selectedItem);
#endif
    if (((currY < WINDOW_HEIGHT) && (currY > WINDOW_HEADER_HEIGHT)) && ((currX > FIELD_AREA_X_MIN) && (currX < FIELD_AREA_X_MAX))) {
        isFieldClick    = true;
        isTxClick       = false;
        selectedItem    = ((currY - WINDOW_HEADER_HEIGHT + scr.nPos) / ROW_OFFSET);
        InvalidateRect(hWnd, NULL, FALSE);
    } else if ((((currX > BTC_TX_AREA_X_MIN) && (currX < BTC_TX_AREA_X_MAX)) && ((currY > (BTC_TX_AREA_Y_MIN - scr.nPos)) && (currY < (BTC_TX_AREA_Y_MAX - scr.nPos))))) {
        isFieldClick    = false;
        isTxClick       = true;
        selectedItem    = BTC_TX_SELECTED;
        InvalidateRect(hWnd, NULL, FALSE);
    } else {
        isFieldClick    = false;
        isTxClick       = false;
        selectedItem    = NOT_USED;
        InvalidateRect(hWnd, NULL, FALSE);
    }

    return;
}

void selectObjectInTx(HWND hWnd, LPARAM lParam)
{
    currX = LOWORD(lParam);
    currY = HIWORD(lParam);
#if (DEBUG_MODE && DEBUG_PROC_CPP)
    printf("procState x: %d, y: %d, selectedItem: %d\n", currX, currY, selectedItem);
#endif
    if (((currY < WINDOW_HEIGHT) && (currY > WINDOW_HEADER_HEIGHT)) && ((currX > WINDOW_MIN_SIZE) && (currX < WINDOW_WIDTH))) {
        isFieldClick    = true;
        selectedItem    = ((currY - WINDOW_HEADER_HEIGHT + scr.nPos) / ROW_OFFSET);
        InvalidateRect(hWnd, NULL, FALSE);
    } else {
        isFieldClick    = false;
        selectedItem    = NOT_USED;
        InvalidateRect(hWnd, NULL, FALSE);
    }

    return;
}