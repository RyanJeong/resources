#include "BlockExplorer.h"

HINSTANCE   hInst;                              /*  ���� �ν��Ͻ��Դϴ�. */
WCHAR       szTitle[MAX_LOADSTRING];            /*  ���� ǥ���� �ؽ�Ʈ�Դϴ�.  */
WCHAR       szWindowClass[MAX_LOADSTRING];      /*  �⺻ â Ŭ���� �̸��Դϴ�. */

/*  paint.cpp���� �ӽ÷� ����� �����Դϴ�. �׽�Ʈ �� ���� ���� */
LPCWSTR     Text = TEXT("MSOL Block Information Viewer");

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
    WNDCLASSEXW wcex;

    /*  cb: Count of Bytes  */
    wcex.cbSize = sizeof(WNDCLASSEX);
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
    wcex.style = CS_HREDRAW | CS_VREDRAW;

    /*  lpfnWndProc - �����쿡�� �߻��Ǵ� �޽����� ��� �Լ����� ó������ �޽��� ó���Լ��� �ּҸ� �޴´�.   */
    wcex.lpfnWndProc = WndProc;
    
    /*  cbClsExtra - ������ Ŭ�������� ����� ���� �޸𸮸� ����Ʈ�� �����Ѵ�.   */
    wcex.cbClsExtra = 0;

    /*  cbWndExtra - ���� �����쿡�� ����� ���� �޷θ� ���� ����  */
    wcex.cbWndExtra = 0;

    /*  hInstance - ������ Ŭ������ ����ϴ� �ν��Ͻ� �ڵ�   */
    wcex.hInstance = hInstance;

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
     *      IDC_SIZENWSE    : �������� ��� ũ������
     *      IDC_SIZENESW    : ������ ��� ũ������
     *      IDC_SIZEWE      : ���� ũ������
     *      IDC_SIZENS      : ���� ũ������
     *      IDC_SIZEALL     : 4���� ȭ��ǥ
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
    wcex.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);

    /*  lpszMenuName - ���α׷��� ����� �޴�����   */
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_BLOCKEXPLORER);

    /*  lpszClassName - ������ Ŭ������ �̸� ����   */
    wcex.lpszClassName = szWindowClass;
    
    /*  hIconSm - ���� ������ ���� */
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

/*
 *  InitInstance: �ν��Ͻ� �ڵ��� �����ϰ� ���� �����츦 �����մϴ�.
 *
 *  �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ� �� ���α׷� �����츦 ���� ���� ǥ���մϴ�.
 */
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;

    hInst = hInstance;  /*  �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.  */

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
    hWnd = CreateWindowW(
        szWindowClass, 
        szTitle, 
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, 
        nullptr, 
        nullptr, 
        hInstance, 
        nullptr);

    if (!hWnd) {

        return FALSE;
    }

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
     *  
     */
    UpdateWindow(hWnd);

    return TRUE;
}

/*  ���� ��ȭ ������ �޽��� ó�����Դϴ�.   */
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
