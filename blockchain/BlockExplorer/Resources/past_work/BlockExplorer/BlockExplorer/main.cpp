#include "common.h"
#include "stdafx.h"
/*
 *  WinAPI ���α׷� ���� ����
 *      1. WndClass ����
 *      2. �޸� �� ������ ����(CreateWindow)
 *      3. �����츦 ȭ�鿡 ǥ��(ShowWindow)
 *      4. ����ڷκ��� �ԷµǴ� �޼��� ó��(Message Loop)
 *
 *  hInstance       : ���α׷��� �ν��Ͻ� �ڵ�
 *  hPrevInstance   : �ٷ� �տ� ����� ���� ���α׷��� �ν��Ͻ� �ڵ�. 
                      ���� ���� NULL�� �Ǹ� WIN32������ �׻� NULL�̴�. 
                      ȣȯ���� ���ؼ��� �����ϴ� �μ��̹Ƿ� �Ű澲�� �ʾƵ� �ȴ�.
 *  lpCmdLine       : ��������� �Էµ� ���α׷� �μ��̴�. ������ argv�μ��� �ش��Ѵ�.
 *  nCmdShow        : ���α׷��� ����� �����̸� �ּ�ȭ, ������ ���� ���޵ȴ�.
 */

extern HINSTANCE    g_hInst;
LPCWSTR             lpszClass = TEXT("BlockViewer");

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
    HWND 		hWnd;
    MSG 		Message;
    WNDCLASS 	WndClass;
    /*  ��Ƽ����Ʈ�϶� "", �����ڵ��϶� L""�� ��ȯ�ϴ� ��ũ�� (TCHAR) */
    

    /*
     *  g_hInst: WinMain�� �μ����� hInstance�� �����ϱ� ���� ���������� ����
     */
    g_hInst = hInstance;

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
     */

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
    WndClass.style = (CS_HREDRAW | CS_VREDRAW);

    /*  lpfnWndProc - �����쿡�� �߻��Ǵ� �޽����� ��� �Լ����� ó������ �޽��� ó���Լ��� �ּҸ� �޴´�.   */
    WndClass.lpfnWndProc = (WNDPROC) WndProc;

    /*  cbClsExtra - ������ Ŭ�������� ����� ���� �޸𸮸� ����Ʈ�� �����Ѵ�.   */
    WndClass.cbClsExtra = 0;

    /*  cbWndExtra - ���� �����쿡�� ����� ���� �޷θ� ���� ����  */
    WndClass.cbWndExtra = 0;
    
    /*  hInstance - ������ Ŭ������ ����ϴ� �ν��Ͻ� �ڵ�   */
    WndClass.hInstance = hInstance;
    
    /*
     *  hIcon - ������ ��� ����
     *          LoadIcon �Ǵ� LoadImage ȣ��
     *      IDI_ASTERISK    : ���� ��� 
     *      IDI_ERROR       : ���� ���
     *      IDI_EXCLAMATION : ��� ���
     *      IDI_QUESTION    : ���� ���
     */
    WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    
    /*
     *  hCursor - ���콺Ŀ�� ��� ����
     *            LoadCursor ȣ��
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
    WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    
    /*
     *  hbrBackground - ������ ������ ����. 
     *                  GetStockObject() �Լ��� ���� �����쿡�� �����ϴ� �⺻ ������ ���� ������ �� ����. (HBRUSH) ����ȯ�ʿ�. 
     *      WHITE_BRUSH     : �Ͼ�� ���ȭ��
     *      BLACK_BRUSH     : ������ ���ȭ��
     *      LTGRAY_BRUSH    : ȸ�� ���ȭ��
     *
     *                  CreateSolidBrush() �Լ��� ���� RGB �������� �� ���� ����
     *      CreateSolidBrush(RGB(10,20,50)) : ����(10),�׸�(20),���(50)�� RGB ����
     *
     *                  NULL������ �����Ұ��� WM_ERASEBKGND �޽������� ���� ����� �����ؾ� �Ѵ�.
     */
    WndClass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
    
    /*  lpszMenuName - ���α׷��� ����� �޴�����   */
    WndClass.lpszMenuName = MAKEINTRESOURCE(MAINMENU);

    /*  lpszClassName - ������ Ŭ������ �̸� ����   */
    WndClass.lpszClassName = lpszClass;


    if (!RegisterClass(&WndClass)) {
        /*
         *  RegisterClass(): ������ Ŭ������ ���
         *      ������ Ŭ���� ����� ������ ���, 
         * 
         *  int MessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType); 
         *      hWnd        : �޽��� �ڽ��� ����(Owner) �������̴�.
         *                    ���� ������� �޽��� ���ڸ� ������ �����츦 ���ϸ�,
         *                    �޽��� �ڽ��� ���� �������� �߾ӿ� ��Ÿ����. 
         *                    �޽��� �ڽ��� �� �ִ� ���� ���� ������� ����� �� ���� ���°� �ȴ�. 
         *                    �޽��� �ڽ��� �ݾƾ߸� ���� �����츦 ����� �� �ִ�.
         *      lpText      : �޽��� �ڽ��� ����� ���ڿ�
         *      lpCaption   : �޽��� ������ Ÿ��Ʋ �ٿ� ��Ÿ�� ���� ���ڿ�(NULL: Default�� ���, Error)
         *      uType       : DWORD�� ����
         *          �׷� 1 - �޽��� �ڽ��� ��Ÿ�� ��ư�� ������ �����Ѵ�.
         *              MB_ABORTRETRYIGNORE : �ߴ� | �ٽ� �õ� | ����
         *              MB_OK               : Ȯ��
         *              MB_OKCANCEL         : Ȯ�� | ���
         *              MB_RETRYCANCEL      : �ٽ� �õ� | ���
         *              MB_YESNO            : �� | �ƴϿ�
         *              MB_YESNOCANCEL      : �� | �ƴϿ� | ���
         *          �׷� 2 - �޽��� �ڽ��� ��Ÿ�� �������� �����Ѵ�.
         *              MB_ICONEXCLAMATION, MB_ICONWARNING      : ����� ��� ����ǥ ������
         *              MB_ICONINFORMATION, MB_ICONASTERISK     : �Ͼ�� ��� ����ǥ ������
         *              MB_ICONQUESTION                         : �Ͼ�� ��� ����ǥ ������
         *              MB_ICONSTOP, MB_ICONERROR, MB_ICONHAND  : ������ ������
         *          �׷� 3 - ����Ʈ ��ư�� �����Ѵ�.
         *              MB_DEFBUTTON1   : ù��° ��ư�� ����Ʈ ��ư�� �ȴ�. ���ٸ� ������ ������ �� �÷��װ� ���õȴ�.
         *              MB_DEFBUTTON2   : �ι�° ��ư�� ����Ʈ ��ư�� �ȴ�.
         *              MB_DEFBUTTON3   : ���ι�° ��ư�� ����Ʈ ��ư�� �ȴ�.
         *              MB_DEFBUTTON4   : �׹�° ��ư�� ����Ʈ ��ư�� �ȴ�.
         *          �׷� 4 - ��ȭ������ ��� ���θ� �����Ѵ�.
         *              MB_APPMODAL     : hWnd �������� ��� ��ȭ���ڷ� �����Ƿ� �޽��� �ڽ��� �ݱ� ������ 
         *                                ���� �����忡 ���� �ٸ� ������� ��ȯ�� �� ����.
         *                                �׷��� �ٸ� �������� ������� ��ȯ�� ���� ������ �˾� ������ε� ��ȯ�� �� �ִ�. 
         *                                �ƹ� �÷��׵� �������� ������ �� ��尡 ����Ʈ�� ����ȴ�.
         *              MB_SYSTEMMODAL  : MB_APPMODAL�� �����ϵ� �׻� �� ��Ÿ���� ���� ��� �����캸�� ���� ��ġ�ȴ�. 
         *                                �޸� ���� ���� ����� ��Ȳ�� �˸����� �� �� �� �÷��׸� ����Ѵ�.
         *              MB_TASKMODAL    : MS_APPMODAL�� �����ϵ� hWnd�� NULL�� ��� ���� �����忡 ���� ��� �ֻ��� �����찡 ��� �����ȴٴ� ���� 
         *                                �ٸ���. DLL�̳� ���̺귯������ �θ� ������� �����Ҹ��� �����찡 ���� �� �� �÷��׸� ����Ѵ�.
         *          �׷� 5 - ��Ÿ ��� �ɼ��� �����Ѵ�.
         *              MB_DEFAULT_DESKTOP_ONLY         : ����ڰ� �α׿��� �� ���� ���α׷��� ����ǰ� �ִ� ����Ʈ ����ũ ž�� ��츸 
         *                                                �޽��� �ڽ��� ��Ÿ������ �Ѵ�.
         *              MB_HELP                         : ���� ��ư�� �����ش�. �� ��ư�� �����ų� F1Ű�� ������ Help �̺�Ʈ�� �߻��Ѵ�.
         *              MB_RIGHT                        : �޽����� ���������� �����Ѵ�.
         *              MB_RTLREADING                   : �ƶ��̳� ���� ��������� �����ʿ��� ���� �б�� ����Ѵ�. �ѱ� ����������� 
         *                                                �ǹ̰� ����.
         *              MB_SETFOREGROUND                : �޽��� �ڽ��� ���׶��� ������� �����. �ý����� ���������� SetForegroundWindow �Լ��� 
         *                                                ȣ���Ѵ�.
         *              MB_TOPMOST                      : �׻� �� �ɼ����� �޽��� �ڽ��� �����Ѵ�.
         *              MB_SERVICE_NOTIFICATION         : NT�� ���� ���α׷��� ����� �� �ִ� �÷����̴�. 
         *                                                ���� �α����� ����ڰ� ������ ��Ƽ��Ǿ� �ִ� ����ũ ž�� �޽��� �ڽ��� ����Ѵ�. 
         *                                                �� ��� hWnd�� �ݵ�� NULL�̾�� �Ѵ�.
         *              MB_SERVICE_NOTIFICATION_NT3X    : MB_SERVICE_NOTIFICATION�� �����ϵ� NT 3.51�������� ����Ѵ�.
         *
         *      ��ȯ��
         *          IDABORT     : �ߴ� ��ư�� ������.
         *          IDCANCEL    : ��� ��ư�� ������. EscŰ�� ���� ��ȭ���ڸ� ���� ���� �� ���� ���ϵȴ�.
         *          IDIGNORE    : ���� ��ư�� ������.
         *          IDNO        : �ƴϿ� ��ư�� ������.
         *          IDOK        : Ȯ�� ��ư�� ������.
         *          IDRETRY     : ��õ� ��ư�� ������.
         *          IDYES       : �� ��ư�� ������.
         */
        MessageBox(
            NULL,
            TEXT("Window Registration Failed!"),
            NULL,
            MB_ICONEXCLAMATION | MB_OK | MB_SYSTEMMODAL);

        return 1;
    }

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
        lpszClass, 
        lpszClass, 
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 
        CW_USEDEFAULT, 
        CW_USEDEFAULT,
        CW_USEDEFAULT, 
        NULL, 
        (HMENU) NULL, 
        hInstance, 
        NULL);

    /*
     *  CreateWindow() ������ ������ ���,
     */
    if (hWnd == NULL) {
        MessageBox(
            NULL,
            TEXT("Window Creation Failed!"),
            NULL,
            MB_ICONEXCLAMATION | MB_OK | MB_SYSTEMMODAL);

        return 1;
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
     *  BOOL GetMessage(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax);
     *      �� �Լ��� �ý����� �����ϴ� �޽��� ť���� �޽����� �о���δ�. �о���� �޽����� ù��° �μ��� �����ϴ� MSG ����ü�� ����ȴ�. 
     *      �� �Լ��� �о���� �޽����� ���α׷��� �����϶�� WM_QUIT�� ��� False�� �����ϸ� �� ���� �޽����̸� True�� �����Ѵ�. 
     *      ���� WM_QUIT �޽����� ������ ������, �� ���α׷��� ����� ������ ��ü while ������ ��� ����ȴ�. 
     *      ������ �� ���� �μ��� �о���� �޽����� ������ �����ϴµ� �� ������ �����Ƿ� �ϴ� �����ϱ�� �Ѵ�.
     *  BOOL TranslateMessage(CONST MSG *lpMsg); 
     *      Ű���� �Է� �޽����� �����Ͽ� ���α׷����� ���� �� �� �ֵ��� �� �ش�. 
     *      ��������� Ű������ � Ű�� �������ٰų� �������� �� Ű���� �޽����� �߻���Ű�µ� �� �Լ��� Ű������ ����(WM_KEYDOWN)�� ������(WM_KEYUP)�� ���������� 
     *      �߻��� �� ���ڰ� �ԷµǾ��ٴ� �޽���(WM_CHAR)�� ����� ������ �Ѵ�. 
     *      ���� ��� AŰ�� ���� �� �ٽ� AŰ�� ���� A���ڰ� �ԷµǾ��ٴ� �޽����� ����� ����.
     *  LONG DispatchMessage(CONST MSG *lpmsg);
     *      �ý��� �޽��� ť���� ���� �޽����� ���α׷��� �޽��� ó�� �Լ�(WndProc)�� �����Ѵ�. �� �Լ��� ���� �޽����� ���α׷����� ���޵Ǹ� ���α׷������� 
     *      ���޵� �޽����� �����Ͽ� ���� ������ �����ϰ� �ȴ�. �޽��� �������� �ϴ� ���� �޽����� ������, �ʿ��� ��� �ణ ���¸� �ٲ� �� ���� ���α׷����� 
     *      �����ϴ� �� ���̴�. �� ������ WM_QUIT �޽����� ���޵� ������, �� ���α׷��� ����ɶ����� �ݺ��ȴ�. 
     *      �ᱹ �޽��� ������ �ϴ� ���̶� �޽��� ť���� �޽����� ���� �޽��� ó�� �Լ��� �����ִ� �� ���̴�.
     *      
     *      ���� �޽��� ó���� ������ �޽��� ó�� �Լ�(WndProc)���� �����Ѵ�.
     */
    while (GetMessage(&Message, 0, 0, 0)) {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }

    return Message.wParam;
}