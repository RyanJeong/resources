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

#include "BlockExplorer.h"

extern WCHAR szTitle[MAX_LOADSTRING];
extern WCHAR szWindowClass[MAX_LOADSTRING];
    /*
     *  SAL ����
     *
     *                                          �Ű� ������ �ʼ�	�Ű� ������ ���� ����
     *  �Է� �Լ��� ȣ��                            _In_                    _In_opt_
     *  �Է� �Լ��� ȣ���ϰ� ȣ���ڿ��� ���        _Inout_                 _Inout_opt_
     *  ȣ���ڿ��� ���	                            _Out_                   _Out_opt_
     *  ȣ���ڿ� ���� �������� ���                 _Outptr_                _Outptr_opt_
     */

int APIENTRY wWinMain(_In_ HINSTANCE    hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR        lpCmdLine,
                     _In_ int           nCmdShow)
{
    HACCEL  hAccelTable;
    MSG     msg;

    showConsole();/* ����� ���� �ܼ� â ����  */

    /*
     *  ���α׷��� �ۼ� �� ������ ���� ���� �� ������ ���� ��� �����Ϸ��� C4100 ������ �߻�
     *  �̷��� warning�� ���ϱ� ���� ���� �� �� ���� �Ǵ� ���ڿ� UNREFERENCED_PARAMETER�� ���
     */
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);


    /*  ���� ���ڿ��� �ʱ�ȭ�մϴ�. */
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_BLOCKEXPLORER, szWindowClass, MAX_LOADSTRING);
    if (!MyRegisterClass(hInstance)) {
        /*
         *  MyRegisterClass()->RegisterClass(): ������ Ŭ������ ���
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

        return FALSE;
    }

    /*  
     *  ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
     *  InitInstance()->CreateWindowW() ������ ������ ���,
     */
    if (!InitInstance (hInstance, nCmdShow)) {
        MessageBox(
            NULL,
            TEXT("Window Creation Failed!"),
            NULL,
            MB_ICONEXCLAMATION | MB_OK | MB_SYSTEMMODAL);

        return FALSE;
    }
    /*
     *  LoadAccelerators()
     *  �� �Լ��� ���ҽ��κ��� �׼��������� ���̺��� �о���δ�. 
     *  �ι�° �μ��� �׼��������� ���̺��� �̸� ���ڿ� �������̵� �츮�� �ۼ��� �׼��������� ���̺��� �������̹Ƿ� MAKEINTRESOURCE ��ũ�θ� ����ؾ� �Ѵ�.
     *  �� �Լ��� ���ҽ����� �׼��������� ���̺��� ���� �� �� �ڵ鰪�� ������ �ش�. 
     *  �� �ڵ鰪�� hAccelTable�̶�� ������ ������ �θ� �������� hAccelTable�� ���� �׼��������� ���̺��� ���� �� �ִ�.
     */
    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BLOCKEXPLORER));

    /*
     *   �⺻ �޽��� �����Դϴ�.
     *
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
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

void showConsole(void)
{
    AllocConsole();

    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);

    return;
}





