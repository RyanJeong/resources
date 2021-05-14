/*  ************************************************************************************************************************
 *  DestroyWindow �κ� ����ó�� �߰��� ��(ȭ���� ���� �ʰ� UI �� ���� �޼����� ��� ���α׷��� ��� ����� �� �ֵ��� ��ġ)
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
     *      hwnd    : �޽����� ���� ������ �ڵ��̴�.
     *      message : � ������ �޽����ΰ��� ��Ÿ����. ���� �߿��� ���̴�.
     *      wParam  : ���޵� �޽����� ���� �ΰ����� ������ ������. � �ǹ̸� �����°��� �޽������� �ٸ���. 32��Ʈ���̴�.
     *      lParam  : ���޵� �޽����� ���� �ΰ����� ������ ������. � �ǹ̸� �����°��� �޽������� �ٸ���. 32��Ʈ���̴�.
     *      time    : �޽����� �߻��� �ð��̴�.
     *      pt      : �޽����� �߻����� ���� ���콺 ��ġ�̴�.
     */

extern char         **BTC_tx;
extern HINSTANCE    hInst;
extern int          BTC_getBlockCount, BTC_getBlock_nTx;    //  BTC_getBlockCount: ����� �� ������ ��Ÿ��, BTC_getBlock_nTx: �ش� ����� �� �ŷ����� ��Ÿ��
extern int          BTC_getBlock_initMemDC;                 //  server�� ����� ��û�Ͽ� ���� ����� �ٲ�� �Ǹ�(�� ȭ���� �ٸ� �������� �����ؾ� �ϸ�, ��ũ���Ͽ� ���� �̵��ϴ� �� ����)
extern int          BTC_tx_initMemDC;                       //  *_initMemDC �������� 0���� �ʱ�ȭ�Ͽ� MemDC�� ���ο� �������� ä������

static int  countPopup = 0;     //  ����� field�� Ŭ���Ͽ� ��� �˾� â ���� ��Ÿ��
static HWND hEdit, hBtn;        //  ����Ʈ â�� ��ư â�� ����� �� ���
static int  index = NOT_USED;   //  index ��Ÿ��

/*
�� �޴����� scroll position ���
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
TCHAR       msg[BUF_SIZE];              //  sprintf�� ����ϱ� ���� ����
int         BTC_tempScroll, BTC_getBlock_maxScroll, BTC_getSerializedBlock_maxScroll, BTC_tx_maxScroll; //  BTC_tempScroll�� ��ũ���� ����ϱ� ���� ���� �����س���
int         selectedItem = NOT_USED;    //  ���콺�� ��� �ʵ带 ����Ű�°��� �����ϴ� ����
int         procState;                  //  ���� �޴� ���°��� ����
int         input_number;
int         currX, currY;               //  ���콺 ����Ʈ ��ǥ ��

SCROLLINFO  scr;                        //  ��ũ�� ���� ���� ����ü

extern TCHAR    columnName[BTC_GET_BLOCK_NUM_OF_ROWS][BUF_SIZE];
TCHAR           columnNameDetail[BTC_GET_BLOCK_NUM_OF_ROWS][BUF_SIZE] = {
    TEXT("��� �ش��� �ؽ� ���Դϴ�."),
    TEXT("����� ���� ü�ο� ����Ǳ� ���� �������� Ƚ���� ��Ÿ���ϴ�."),
    TEXT("��� ������ ���� ����(witness data)�� ������ ũ��(Byte)�� ��Ÿ���ϴ�."),
    TEXT("����� �� ũ��(Byte)�� ��Ÿ���ϴ�."),
    TEXT("BIP 141���� ���ǵ� ����� ������ ���� ��Ÿ����, ������ Weight-Unit(WU)�Դϴ�."),
    TEXT("���� ü�� �������� ����� ������� ��ġ�� ��Ÿ����, �ε������ �Ҹ��ϴ�."),
    TEXT("����� ������ ��Ÿ���� 10���� ���Դϴ�."),
    TEXT("����� ������ 16���� �� ���·� ǥ���� ���Դϴ�."),
    TEXT("��Ͽ� ���ԵǾ� �ִ� �ŷ����� ��Ŭ Ʈ�� �ڷᱸ���� �����Ͽ� ���� ��Ʈ�� �ؽ� ���Դϴ�."),
    TEXT("��Ͽ� ���ԵǾ� �ִ� �ŷ����� �ŷ��� ���� ��ȣ(Tx ID)�� ǥ���˴ϴ�.\r\n �ش� ���� ���� ��ȸ�� ��Ͽ� ���Ե� �� �ŷ� ������ ��Ÿ���ϴ�."),
    TEXT("�ش� ����� ���� �����Ǿ����� ���н� �ð��� �������� �ʷ� ȯ���� ���Դϴ�."),
    TEXT("�ش� ��� ������ ������ 11���� ��ϵ��� time ���� �� �߰���(median)�� ��Ÿ���ϴ�."),
    TEXT("��� ����� �ؽ� ���� ����ϱ� ���� ���Ǵ� �ʵ� �� �ϳ���,\r\n��� ����� �ؽ� �� ��꿡�� �� 6���� �Է°��� �ʿ��մϴ�."),
    TEXT("ä�� ���̵��� �����ϴ� difficulty ���� �����ϴµ� ���Ǵ� ���Դϴ�."),
    TEXT("��� ä�� ���̵��� ��Ÿ���ϴ�."),
    TEXT("Genesis ��Ϻ��� �ش� ��ϱ��� ä���ϱ� ���� ��ŭ�� �ؽð� �ʿ����� ������ ���Դϴ�."),
    TEXT("�ش� ����� ���� �ؽ� ���� ��Ÿ���ϴ�."),
    TEXT("�ش� ����� ���� �ؽ� ���� ��Ÿ���ϴ�.") };

/*
 *  WndProc:    Event�� ó���ϴ� �Լ��Դϴ�. 
 */
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    /*
     *  ���� ���� ���Ǵ� �޽����� ����
     *      WM_COMMAND      :   ���� ���α׷� �޴��� ó���Ѵ�.
     *      WM_QUIT         :   ���α׷��� ���� �� �߻��ϴ� �޽����̴�.
     *      WM_LBUTTONDOWN  :   ���콺�� ���� ��ư�� ���� ��� �߻��Ѵ�.
     *      WM_CHAR         :   Ű����κ��� ���ڰ� �Էµ� �� �߻��Ѵ�
     *      WM_PAINT        :   ȭ���� �ٽ� �׷��� �� �ʿ䰡 ���� �� �߻��Ѵ�.
     *      WM_DESTROY      :   �����찡 �޸𸮿��� �ı��� �� �߻��Ѵ�.
     *      WM_CREATE       :   �����찡 ó�� ������� �� �߻��Ѵ�.
     *      WM_NCPAINT      :   ���۾������� �׷����� �Ҷ� �ü���� ������ �޽����̸� �������α׷� �� �޽�����
     *                          �޾��� �� Ÿ��Ʋ ��, ��輱, �ý��� �޴� ���� �׷��� �Ѵ�.
     *                          NCPAINT�� ���ξ� NC�� Non Clien Area ��, ���۾������̶� ���̴�.
     *                          �� �޽����� ���� ó������ �ʾƵ� DefWindowProc���� ó���ϸ� ��κ��� ���� ���α׷��� �� �޽����� ó������ �ʴ´�. 
     *
     *      BOOL InvlidateRect(HWND hWnd, CONST RECT *lpRect, BOOL bErase); 
     *          �� �Լ��� �������� �۾� ������ ��ȿȭ���� ��������� �Ͽ��� WM_PAINT �޽����� �ش� ������� �������� �Ѵ�. 
     *
     *          ù��° �μ� hWnd�� ��ȿȭ�� ����� �Ǵ� ������, �� �ٽ� �׷����� �� �������� �ڵ��̴�. 
     *          �� ���� WndProc�� ȣ��� �� ���޵Ǵ� ù��° �μ� hWnd�� �״�� �� �ָ� �ȴ�.
     *
     *          �ι�° �μ� lpRect�� ��ȿȭ�� ����� �Ǵ� �簢 ������ �� �ֵ� �� ���� NULL�̸� �������� �� ������ ��ȿȭ�ȴ�. 
     *          �� ������ ��ȿȭ�ϸ� �����찡 ������ �ٽ� �׷����Ƿ� Ȯ���ϰ� �׷������ �ϰ����� �׸�ŭ ����ӵ��� �ʾ����� �ȴ�. 
     *          �ӵ��� �ִ��� ���̷��� ����� �ּ����� �������� ����Ͽ� ��ȿȭ�ϴ� ���� ����. 
     *          
     *          ����° �μ� bErase�� ��ȿȭ�Ǳ� ���� ����� ��� ���� �� �ٽ� �׸� ������ �ƴϸ� ����� ������ �ʰ� �׸� �������� �����Ѵ�. 
     *          �� ���� TRUE�̸� ����� ���� �� �ٽ� �׸��� FALSE�̸� ����� ������ ����ä�� �ٽ� �׸���. 
     */

    BOOL            check;          //  handle ���, ���� �� false �Է�
    int             prevPos;  //  prevPos�� ���� ��ũ�� ��ġ�� �����ϰ�, temp�� ��� ����� �ӽ÷� �����ϴ� ����
    LPMINMAXINFO    mmi;            //  ������ â ũ�� ������ ���
    int             popupOffset;    //  ���� �˾��� ��� �� �˾� �� ������ ���� ���� ���
    switch (message) {
    case WM_CREATE:
        scr.cbSize = sizeof(SCROLLINFO);
        /*
        *  SIF_ALL             ��� �Ӽ��� ����
        *  SIF_DISABLENOSCROLL ��Ŭ�ι� ��ü ��Ȱ��ȭ
        *  SIF_PAGE            ������ ������
        *  SIF_POS             ��ũ�� �ڽ��� ��ġ
        *  SIF_RANGE           ��ũ�� ������ �ּ�/�ִ�
        *  SIF_TRACKPOS        �巡�� ������ ��ũ�ѹڽ� ���� ��ġ
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
    *  ��ư	    ����	            ����	        ����Ŭ��
    *  ����	    WM_LBUTTONDOWN	    WM_LBUTTONUP	WM_LBUTTONDBLCLK
    *  ����	    WM_RBUTTONDOWN	    WM_RBUTTONUP	WM_RBUTTONDBLCLK
    *  �߾�	    WM_MBUTTONDOWN	    WM_MBUTTONUP	WM_MBUTTONDBLCLK
    */
    case WM_LBUTTONDOWN:
        if (procState == IDM_BLOCK) {
            selectObjectInGetBlock(hWnd, lParam);       // ���� ���콺 ��ġ ���� update
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
            selectObjectInTx(hWnd, lParam);     //  ���� ���콺 ��ġ ���� update
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
        //  (short)HIWORD(wParam) �Ʒ� �� ��ũ��: -120~, �� �� ��ũ��: 120~ 
        //  �ܼ��� ��� ���� ������ �����ϱ� ���� ���ǹ�
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
                MessageBox(hWnd, TEXT("�߸� �Է��ϼ̽��ϴ�."), TEXT("Error"), MB_OK);
            } else {
                if (input_number != index) {        //  ���� �˻��� ���� ���� �˻��� ���� �ٸ� ���
                    index = input_number;
                    BTC_tx_initMemDC        = 0;
                    BTC_getBlock_initMemDC  = 0;
                    if (getBlock(index) < 0) { // negative: besthash, else: index
                        DestroyWindow(hWnd);
                    }
                    if (procState == IDM_TX) {              // must do getBlock(index) before getRawTransaction()
                        if (getRawTransaction() < 0) {      // param: �˻��� ����� tx �ε���
                            DestroyWindow(hWnd);
                        }
                        BTC_tx_maxScroll    = (BTC_getBlock_nTx * ROW_OFFSET) + BTC_tempScroll;
                        input_number        = (BTC_tx_maxScroll < 0) ? 0 : BTC_tx_maxScroll;
                        if (scr.nMax != input_number) {     //  ��ũ�� �ִ� ���� �ٲ� ���, ���ο� ��ũ�� ������ �����ؾ� ��
                            scr.nMax = input_number;
                            SetScrollInfo(hWnd, SB_VERT, &scr, TRUE);
                        }
                        //  scr.nPos <-> BTC_tx_scrollPos
                        //  nPos�� ���� ��ũ�� ��, BTC_tx_scrollPos�� �ٸ� ���·� ��ȯ�� ���� ���� ��ũ�� ���� ������ ��, ���� scr.nPos�� ����ؾ� ��
                        scr.nPos = (scr.nPos > scr.nMax) ? scr.nMax : scr.nPos;           // tx view���� ��ũ���� �����̹Ƿ�, ���� �˻縦 ���� �´� ��ũ�� ���� �־�� ��
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
        *  DefWindowProc(): WinProc�� �Լ� ���ڰ� �����ϸ�, �ý��� ĸ���� ������ �� ������ �޴��� �ּ�ȭ, �ִ�ȭ, ������ �̵�, ũ�� ���� ����
        *  ���� ó�������� �ʾƵ� �ü���� �˾Ƽ� ó�����ִ� ������ ����
        */

        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

/*
 *  WndProcPopup:   �˾� �������� event�� ó���ϴ� �Լ��Դϴ�. 
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
            DrawText(hdc, TEXT("�ŷ� ���� �ڼ��� ���"), AUTO_CALC_LENGTH, &rt, DT_CENTER | DT_WORDBREAK);
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
 *  printGetBlock: getBlock ������ ����ϴ� �Լ��Դϴ�. 
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
        *  Window Class    |   ��Ʈ��
        *  ----------------+--------------------------
        *  button          |   ��ư, üũ, ����
        *  static          |   �ؽ�Ʈ
        *  scrollbar       |   ��ũ�� ��
        *  edit            |   ����Ʈ
        *  listbox         |   ����Ʈ �ڽ�
        *  combobox        |   �޺� �ڽ�
        *      ->  edit Ŭ���� ����Ϸ��� ù ��° �Ķ���Ϳ� "edit"�� �Է��ؾ� �Ѵ�.
        *
        *  ES_AUTOHSCROLL  : ���� ��ũ���� �����Ѵ�.
        *  ES_AUTOVSCROLL  : ������ ������ ���� ��ũ���� �����Ѵ�.
        *  ES_LEFT         : ���� �����Ѵ�.
        *  ES_CENTER       : �߾� �����Ѵ�.
        *  ES_RIGHT        : ������ �����Ѵ�.
        *  ES_LOWERCASE    : �ҹ��ڷ� ��ȯ�Ͽ� ǥ���Ѵ�.
        *  ES_UPPERCASE    : �빮�ڷ� ��ȯ�Ͽ� ǥ���Ѵ�.
        *  ES_MULTILINE    : �������� ������ �� �ֵ��� �Ѵ�.
        *  ES_NOHIDESEL    : ��Ŀ���� �Ҵ��� ���õ� ������ ǥ���Ѵ�.
        *  ES_READONLY     : �б��������� ����� ������ �����Ѵ�.
        *
        *  SS_LEFT	            : �������� ���ĵǴ� �ؽ�Ʈ�̸� �ڵ����� ����ȴ�.
        *  SS_LEFTNOWORDWRAP    : �������� ���ĵǴ� �ؽ�Ʈ�̸� �ڵ����� ������� �ʴ´�.
        *  SS_CENTER	        : �߾����� ���ĵǴ� �ؽ�Ʈ�̸� �ڵ����� ����ȴ�.
        *  SS_RIGHT	            : ���������� ���Ĵ� �ؽ�Ʈ�̸� �ڵ����� ����ȴ�.
        *  SS_SIMPLE	        : �ܼ��� ���ڿ��̸� �ڵ�������� �ʴ´�.
        *  SS_WHITEFRAME	    : �������� �������� �׷����� �簢��(�⺻ �Ͼ��)
        *  SS_WHITERECT	        : �������� �������� �׷����� ���� ä���� �簢��(�⺻ �Ͼ��)
        *  SS_BLACKFRAME	    : ȭ�� �������� �׷����� �簢��(�⺻ ������)
        *  SS_BLACKRECT	        : ȭ�� �������� �׷����� ���� ä���� �簢��(�⺻ ������)
        *  SS_GRAYFRAME	        : ������ ������ �������� �׷����� �簢��
        *  SS_GRAYRECT	        : ������ ������ �������� �׷����� ���� ä���� �簢��
        *  SS_ICON	            : ��ȭ���ڳ����� ������ ���
        *  SS_NOPREFIX	        : &���ڸ� ����Ű ������ ������� �ʰ� �״�� ����Ѵ�.
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
 *  printGetSerializedBlock: getSerializedBlock ������ ����ϴ� �Լ��Դϴ�.
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
 *  printTx: tx ������ ����ϴ� �Լ��Դϴ�.
 */
void printTx(HWND hWnd)
{
    saveCurrScrollPos(procState);
    procState = IDM_TX;
    if (getRawTransaction() < 0) {   // param: �˻��� ����� tx �ε���
        DestroyWindow(hWnd);
    } else {
        BTC_tx_maxScroll    = (BTC_getBlock_nTx * ROW_OFFSET) + BTC_tempScroll;
        scr.nMax            = (BTC_tx_maxScroll < 0) ? 0 : BTC_tx_maxScroll;
        scr.nPos            = (BTC_tx_scrollPos > scr.nMax) ? scr.nMax : BTC_tx_scrollPos;           // tx view���� ��ũ���� �����̹Ƿ�, ���� �˻縦 ���� �´� ��ũ�� ���� �־�� ��
        SetScrollInfo(hWnd, SB_VERT, &scr, TRUE);
        InvalidateRect(hWnd, NULL, TRUE);
        ShowWindow(hEdit, SW_SHOW);
        ShowWindow(hBtn, SW_SHOW);
    }

    return;
}

/*
 *  saveCurrScrollPos: ��� �޴� �ٿ��� �ٸ� �޴��� Ŭ���Ͽ� ȭ���� ��ȯ�� ��, ���� ȭ�鿡���� ��ũ�� ��ġ�� ������ �����ϴ� ������ �����ϴ� �Լ�
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