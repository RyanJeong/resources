#include "BlockExplorer.h"

HINSTANCE   hInst;                              //  ���� �ν��Ͻ��Դϴ�.
HWND        hWnd;                               //  ���� ������ �����Դϴ�. 
WCHAR       szheader2[MAX_LOADSTRING];          //  ���� ǥ���� �ؽ�Ʈ�Դϴ�.  
WCHAR       szWindowClass[MAX_LOADSTRING];      //  �⺻ â Ŭ���� �̸��Դϴ�. 

extern int BTC_tempScroll, BTC_getBlock_maxScroll, BTC_getSerializedBlock_maxScroll;  //  ���� ������ ũ�⿡ ����� �ִ� ��ũ�� ��

/*
 *  WNDCLASS ����ü
 *  typedef struct tagWNDCLASS{
 *      UINT        style;
 *      WNDPROC     lpfnWndProc;
 *      int         cbClsExtra;
 *      int         cbWndExtra;
 *      HINSTANCE   hInstance;
 *      HICON       hIcon;
 *      HCURSOR     hCursor;
 *      HBRUSH      hbrBackground;
 *      LPCSTR      lpszMenuName;
 *      LPCSTR      lpszClassName;
 *  };
 *
 *  MyRegisterClass: ������ Ŭ���� ���
 */
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = {0};

    wcex.cbSize = sizeof(WNDCLASSEXW);  //  cb: Count of Bytes
    /*
     *  style - ������ ��Ÿ�� �ɼ��� �����ϴ� ������ ������ ����� ������Ʈ ���� ����(��Ʈ����)
     *      CS_HREDRAW      - ���� ���� ���� �����츦 �ٽ� �׸�
     *      CS_VREDRAW      - ���� ���� ���� �����츦 �ٽ� �׸�
     *      CS_DBLCLKS      - ���콺�� ����Ŭ���Ǵ� �޽����� �޴´�. (�������� ���콺 ����Ŭ�� �޽��� ���õ�)
     *      CS_NOCLOSE      - �ý��� �޴��� �ݱ⸦ ��Ȱ��ȭ ��Ŵ (ALT + F4, �ݱ��ư ��� �ش�)
     *      CS_CLASSDC      - Ŭ�������� ������� ���� �ν��Ͻ��� ��� �����찡 DC�� ����
     *      CS_OWNDC        - ���� �ν��Ͻ� ���� ��������� �������� DC�� ���
     *      CS_PARENTDC     - �ڽ������찡 �θ������� DC�� ���
     *      CS_GLOBALCLASS  - �������α׷� ���� Ŭ������ ���
     *      CS_SAVEBITS     - �������� ���ȭ�� ������ ��Ʈ������ ����
     */
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = (WNDPROC) WndProc;        //  lpfnWndProc - �����쿡�� �߻��Ǵ� �޽����� ��� �Լ����� ó������ �޽��� ó���Լ��� �ּҸ� �޴´�.
    wcex.cbClsExtra     = 0;                        //  cbClsExtra - ������ Ŭ�������� ����� ���� �޸𸮸� ����Ʈ�� �����Ѵ�.
    wcex.cbWndExtra     = 0;                        //  cbWndExtra - ���� �����쿡�� ����� ���� �޷θ� ���� ����
    wcex.hInstance      = hInstance;                //  hInstance - ������ Ŭ������ ����ϴ� �ν��Ͻ� �ڵ�
    /*
     *  hIcon - ������ ��� ����, LoadIcon() �Ǵ� LoadImage() ȣ��
     *
     *      IDI_ASTERISK    : ���� ���
     *      IDI_ERROR       : ���� ���
     *      IDI_EXCLAMATION : ��� ���
     *      IDI_QUESTION    : ���� ���
     */
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BLOCKEXPLORER));
    /*
     *  hCursor - ���콺Ŀ�� ��� ����, LoadCursor() ȣ��
     *            
     *      IDC_ARROW       : ȭ��ǥ
     *      IDC_IBEAM       : ĳ��
     *      IDC_WAIT        : �𷡽ð�
     *      IDC_CROSS       : ���ڸ�� Ŀ��
     *      IDC_UPARROW     : ���� ȭ��ǥ
     *      IDCBTC_getBlock_sizeNWSE    : �������� ��� ũ������
     *      IDCBTC_getBlock_sizeNESW    : ������ ��� ũ������
     *      IDCBTC_getBlock_sizeWE      : ���� ũ������
     *      IDCBTC_getBlock_sizeNS      : ���� ũ������
     *      IDCBTC_getBlock_sizeALL     : 4���� ȭ��ǥ
     *      IDC_NO          : �������
     *      IDC_HAND        : �� ���
     *      IDC_APPSTARTING : ȭ��ǥ + �𷡽ð�
     *      IDC_HELP        : ȭ��ǥ + ?
     */
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    /*
     *  hbrBackground - ������ ������ ����.
     *
     *  GetStockObject() �Լ��� ���� �����쿡�� �����ϴ� �⺻ ������ ���� ������ �� ����. (HBRUSH) ����ȯ�ʿ�.
     *      WHITE_BRUSH     : �Ͼ�� ���ȭ��
     *      BLACK_BRUSH     : ������ ���ȭ��
     *      LTGRAY_BRUSH    : ȸ�� ���ȭ��
     *
     *  CreateSolidBrush() �Լ��� ���� RGB �������� �� ���� ����
     *      CreateSolidBrush(RGB(10,20,50)) : ����(10),�׸�(20),���(50)�� RGB ����
     *
     *  NULL������ �����Ұ��� WM_ERASEBKGND �޽������� ���� ����� �����ؾ� �Ѵ�.
     */
    wcex.hbrBackground  = (HBRUSH) (COLOR_WINDOW + 1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_BLOCKEXPLORER);              //  lpszMenuName - ���α׷��� ����� �޴�����
    wcex.lpszClassName  = szWindowClass;                                    //  lpszClassName - ������ Ŭ������ �̸� ����
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));    //  hIconSm - ���� ������ ����
    RegisterClassExW(&wcex);                                                //  ���� ������ ���
    wcex.lpszMenuName   = NULL;                                             //  ���� �θ� ������ ����� �� �޴��� ����߱� ������, �ٽ� �����ؾ� ��
    wcex.hbrBackground  = (HBRUSH) GetStockObject(WHITE_BRUSH);
    wcex.lpfnWndProc    = (WNDPROC) WndProcPopup;
    wcex.lpszClassName  = TEXT("mypopup");
    
    return RegisterClassExW(&wcex);                                         //  �˾� ������ ��� ��, ����� ����� ��ȯ
}

/*
 *  InitInstance:   �ν��Ͻ� �ڵ��� �����ϰ� ���� �����츦 �����մϴ�.
 *                  �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ� �� ���α׷� �����츦 ���� ���� ǥ���մϴ�.
 */
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    RECT rect;

    rect    = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    AdjustWindowRect(&rect, (WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX), true);   //  ������ ũ�⸦ �����Ͽ� ����� ���� ũ�⿡ ����
    hInst   = hInstance;                                                            //�ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.
    /*
     *  CreateWindow(): �޸� �� ������ ����
     *      lpszClassName           : �����ϰ��� �ϴ� �������� Ŭ������ �����ϴ� ���ڿ��̴�.
     *                                �տ��� ������ WndClass����ü�� lpszClassName ����� �̸��� ���⿡ ������ �ش�.
     *                                �츮�� ���������� lpszClass ���ڿ��� ������ Ŭ���� �̸��� ������ �ξ����Ƿ�
     *                                �� ���ڿ��� �״�� �Ѱ��ָ� �ȴ�.
     *      lpszWindowName          : �������� Ÿ��Ʋ �ٿ� ��Ÿ�� ���ڿ��̴�.
     *                                ���⼭ ������ ���ڿ��� �������� Ÿ��Ʋ �ٿ� ��Ÿ����.
     *      dwStyle                 : ������� �ϴ� �������� ���¸� �����ϴ� �μ��̴�.
     *                                ������ ��Ʈ �ʵ尪�̸� ���� ���ʰ��� ��Ƹ��� ��ũ�� ������� ���ǵǾ� �ְ� �� ������� OR�����ڷ�
     *                                �����Ͽ� �������� �پ��� ���¸� �����Ѵ�. �����찡 ��輱�� ���� ���ΰ�, Ÿ��Ʋ �ٸ� ���� ���ΰ�
     *                                �Ǵ� ��ũ�� ���� ���� ����� �����ϰ� ������ �� �� �ִ�. ������ ��Ÿ�ϰ��� ���� �ڼ��� ������
     *                                ���۷����� �����ϵ� WS_OVERLAPPEDWINDOW�� ����ϸ� ���� ������ ������ ���� ���°� �ȴ�.
     *                                �� �ý��� �޴�, �ִ� �ּ� ��ư, Ÿ��Ʋ ��, ��輱�� ���� �����츦 ����� �ش�.
     *      X, Y, nWidth, nHeight   : �μ��� �̸��� �ǹ��ϵ��� �������� ũ��� ��ġ�� �����ϸ� �ȼ� ������ ����Ѵ�.
     *                                x, y��ǥ�� ���� �������� ���� ��ü ȭ���� �������� �ϸ� ���ϵ� ������� �θ� �������� �»���� �������� �Ѵ�.
     *                                �������� �ٷ� �����ص� �Ǹ� CW_USEDEFAULT�� ����ϸ� ������� �˾Ƽ� ������ ũ��� ��ġ�� ������ �ش�.
     *                                ���������� ��� CW_USEDEFAULT�� ����Ͽ���.
     *      hWndParent              : �θ� �����찡 ���� ��� �θ� �������� �ڵ��� ������ �ش�.
     *                                MDI ���α׷��̳� �˾� ������� �����쳢�� �������� ���ϰ��踦 ���� ����(parent-child) ���谡 �����Ǵµ�
     *                                �� ���踦 ������ �ִ� �μ��̴�. �θ� �����찡 ���� ���� �� ���� NULL�� �����ϸ� �ȴ�.
     *      hmenu                   : �����쿡�� ����� �޴��� �ڵ��� �����Ѵ�. WndClass���� �޴��� �����ϴ� ����� �ִµ� ������ Ŭ������ �޴���
     *                                �� ������ Ŭ������ ������� �ϴ� ��� �����쿡�� ���Ǵ� �ݸ� �� �μ��� ������ �޴��� ���� CreateWindow �Լ���
     *                                ��������� �����쿡���� ���ȴ�. ���� WndClass���� ������ �޴��� �״�� ����Ϸ��� �� �μ��� NULL�� �����ϸ� �Ǹ�
     *                                WndClass���� ������ �޴� ��� �ٸ� �޴��� ����Ϸ��� �� �μ��� ���ϴ� �޴� �ڵ��� �ָ� �ȴ�.
     *      hinst                   : �����츦 ����� ��ü, �� ���α׷��� �ڵ��� �����Ѵ�. WinMain�� �μ��� ���޵� hInstance�� ������ �ָ� �ȴ�.
     *      lpvParam                : CREATESTRUCT��� ����ü�� �����̸� Ư���� ������ ���ȴ�. ������ NULL���� ����Ѵ�.
     */
    hWnd = CreateWindow(
        szWindowClass, 
        szheader2,
        /*
         * WS_OVERLAPPED        : ����Ʈ ������. Ÿ��Ʋ �ٿ� ũ�� ������ �ȵǴ� ��輱�� ������. �ƹ��� ��Ÿ�ϵ� ���� ������ �� ��Ÿ���� ����ȴ�.
         * WS_POPUP             : �˾� �����츦 �����. WS_CHILD�� �Բ� �� �� ����.
         * WS_CHILD             : ���ϵ� �����츦 �����. WS_POPUP�� �Բ� �� �� ����.
         * WS_MINIMIZE          : ���� �ּ�ȭ�� ���·� �����츦 �����.
         * WS_VISIBLE           : �����츦 ������ ���� ȭ�鿡 ����Ѵ�.
         * WS_DISABLED          : �������� ����, �� �Է��� �޾Ƶ��� �� ���� ���·� �����.
         * WS_CLIPSIBLINGS      : ���ϵ峢�� ��ȣ ��ģ ������ �׸��� �������� ���ܵȴ�.
         * WS_CLIPCHILDREN      : ���ϵ尡 ��ġ�� ������ �׸��� �������� ���ܵȴ�.
         * WS_MAXIMIZE          : ���� �ִ�ȭ�� ���·� �����츦 �����.
         * WS_CAPTION           : Ÿ��Ʋ �ٸ� ���� �����츦 ����� WS_BORDER ��Ÿ���� �����Ѵ�.
         * WS_BORDER            : �ܼ����� �� ��輱�� ����� ũ�� ������ �� �� ����.
         * WS_DLGFRAME          : �β��� ��輱�� ������ Ÿ��Ʋ �ٸ� ���� �� ����.
         * WS_VSCROLL           : ���� ��ũ�� �ٸ� ���� �����츦 �����.
         * WS_HSCROLL           : ���� ��ũ�� �ٸ� ���� �����츦 �����.
         * WS_SYSMENU           : �ý��� �޴��� ���� �����츦 �����.
         * WS_THICKFRAME        : ũ�� ������ ������ �β��� ��輱�� ������.
         * WS_GROUP             : ���� ��ư �� �׷��� �̷�� ��Ʈ���� �׷� ������ ���ȴ�.
         * WS_TABSTOP           : TabŰ�� ��Ŀ���� �̵��� �� �ֵ��� �Ѵ�.
         * WS_MINIMIZEBOX       : �ּ�ȭ ��ư�� �����.
         * WS_MAXIMIZEBOX       : �ִ�ȭ ��ư�� �����.
         * WS_OVERLAPPEDWINDOW  : ���� �Ϲ����� ������ ��Ÿ��. WS_OVERLAPPED |WS_CAPTION |WS_SYSMENU |WS_THICKFRAME |WS_MINIMIZEBOX |WS_MAXIMIZEBOX ��Ÿ�ϵ��� ���յ� ��Ÿ���̴�.
         * WS_POPUPWINDOW       : �Ϲ����� �˾� ������. WS_POPUP |WS_BORDER |WS_SYSMENU
         * WS_CHILDWINDOW       : WS_CHILD�� ����.
         */
        WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_VSCROLL | WS_CLIPCHILDREN,
        CW_USEDEFAULT, CW_USEDEFAULT, (rect.right - rect.left), (rect.bottom - rect.top),
        nullptr, 
        nullptr, 
        hInstance, 
        nullptr);
    if (!hWnd) {

        return FALSE;
    }
    GetClientRect(hWnd, &rect);                                                             //  ���� �����ڰ� ����� �� �ִ� ũ�⸦ ��� ���� client window size�� ����
    BTC_tempScroll                      = WINDOW_HEADER_HEIGHT - (rect.bottom - rect.top);  //  - ((rect.bottom - rect.top) - WINDOW_HEADER_HEIGHT)
    BTC_getBlock_maxScroll              = BTC_GETBLOCK_LENGTH + BTC_tempScroll;             //  (18 * OFFSET(40)) - (������ ũ�� �� ������ ��� ������ ������ ũ��(bottom - top) - WINDOW_HEADER_HEIGHT(128))
    BTC_getSerializedBlock_maxScroll    = 1024;                                             //  need to change
    /*
     *  ShowWindow(): CreateWindow()�� ���� �޸𸮿� ������ �����츦 ȭ�鿡 ǥ���ϴ� �Լ�
     *  nCmdShow �ɼ�
     *      SW_HIDE         : �����츦 �����.
     *      SW_MINIMIZE     : �����츦 �ּ�ȭ��Ű�� Ȱ��ȭ��Ű�� �ʴ´�.
     *      SW_RESTORE      : �����츦 Ȱ��ȭ��Ų��.
     *      SW_SHOW         : �����츦 Ȱ��ȭ���� �����ش�.
     *      SW_SHOWNORMAL   : �����츦 Ȱ��ȭ���� �����ش�.
     */
    ShowWindow(hWnd, nCmdShow);
    /*
     *  UpdateWindow �Լ��� ������ �������� �Ϻΰ� �ٸ� ������ � �������ų� �������� �Ǿ��� ���,
     *  �� ��ȿȭ ����(Invalid Region)�� ���� WM_PAINT �޽����� �ش� �����쿡 �������ν� ��ȿȭ ������
     *  ���Ž��� �ִ� �Լ��Դϴ�.
     *
     *  �����쿡 �߻��ȴ� �޽����� �Ϲ����� ó������� �߻��� �޽����� �ش� ������ �޽��� ť�� ���� ��
     *  ���ø����̼��� �߻��� �޽����� �޽��� ť�� ���� ������� �޽����� ó���ϴ� ���Դϴ�.
     *
     *  ������ UpdateWindow �Լ� ȣ���� ���� �߻��Ǵ� WM_PAINT �޽����� �ش� ��������
     *  �޽��� ť�� ��ġ�� �ʰ� �ش� �����쿡 �ٷ� ���޵Ǵ� Ư¡�� ������ �ֽ��ϴ�.
     *
     *  ��, �����쿡 ��ȿȭ ����(Invalid Region)�� ���� ��쿡 UpdataWindow �Լ��� ȣ���Ѵٸ�
     *  WM_PAINT �޽����� �߻����� �ʴ� ���� �����ϼž� �մϴ�.
     *  
     *  InvalidateRect(): �޼��� ť�� ��ġ�� ����
     *  ----------------------------------------------------------
     *  InvalidateRect()�� �������� Ŭ���̾�Ʈ ���� �߿��� �Ϻκ��� ��ȿȭ ��Ű�� ���Դϴ�.
     *  �̶�, ù��° ���ڴ� � ������������ ����Ű�� ������ �ڵ��̰�,
     *  �ι�° ���ڴ� ��ȿ����ų �簢 ���� ��ǥ�Դϴ�. �̰� NULL�̸� Ŭ���̾�Ʈ ���� ��ü��� ���̱���.
     *  �׸��� ����° ���ڴ� �� ��ȿȭ ������ �������� �ٽ� �׷����� �ϴ����� �����մϴ�. 
     *  ���� TRUE�̸�, �� �����쿡�Դ� WM_ERASEBKGND �޽����� �ѹ� ��������, WM_PAINT �޽����� ���󰩴ϴ�. ���� FALSE�̸� WM_PAINT �޽����� ���󰩴ϴ�.
     *  ���� �亯�� ���ø� InvalidateRect()�� WM_PAINT �޽����� �߻���Ű�� �ʴ´ٰ� �ߴµ���, WM_PAINT �޽����� �߻� ��ŵ�ϴ�
     *  �׸��� UpdateWindow()�� ������ WM_PAINT �޽����� �߻���Ű�� ���̸�, �̶� �����찡 ��ȿȭ ������ �־�� �մϴ�.
     *
     *  ��, �ٽ� �׷������� �ʿ䰡 �ִ� ������ �ִٸ� �ٽ� �׸����� ����� ���Դϴ�.
     *  �����ϸ�, InvalidateRect()�� Ư�� ������ ������ �ٽ� �׸����� ����� �Լ��̸�, 
     *  UpdateWindow()�� ���� �����쿡�� �ٽ� �׸� �ʿ䰡 �ִ� ������ �ִ��� Ȯ���ϰ� �ִٸ� �ٽ� �׸����� �ϴ� �Լ��Դϴ�.
     *  
     */
    UpdateWindow(hWnd);

    return TRUE;
}

/*  
 *  About: ���� ��ȭ ������ �޽��� ó�����Դϴ�.   
 */
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (message) {
        case WM_INITDIALOG:

            return (INT_PTR) TRUE;
        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
                EndDialog(hDlg, LOWORD(wParam));

                return (INT_PTR) TRUE;
            }
            break;
    }

    return (INT_PTR) FALSE;
}