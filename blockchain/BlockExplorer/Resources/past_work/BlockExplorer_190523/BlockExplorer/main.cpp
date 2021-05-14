   /*
    *  WinAPI 프로그램 실행 절차
    *      1. WndClass 정의
    *      2. 메모리 상에 윈도우 생성(CreateWindow)
    *      3. 윈도우를 화면에 표시(ShowWindow)
    *      4. 사용자로부터 입력되는 메세지 처리(Message Loop)
    *
    *  hInstance       : 프로그램의 인스턴스 핸들
    *  hPrevInstance   : 바로 앞에 실행된 현재 프로그램의 인스턴스 핸들.
                         없을 경우는 NULL이 되며 WIN32에서는 항상 NULL이다.
                         호환성을 위해서만 존재하는 인수이므로 신경쓰지 않아도 된다.
    *  lpCmdLine       : 명령행으로 입력된 프로그램 인수이다. 도스의 argv인수에 해당한다.
    *  nCmdShow        : 프로그램이 실행될 형태이며 최소화, 보통모양 등이 전달된다.
    */

#include "BlockExplorer.h"

extern WCHAR szTitle[MAX_LOADSTRING];
extern WCHAR szWindowClass[MAX_LOADSTRING];
    /*
     *  SAL 이해
     *
     *                                          매개 변수는 필수	매개 변수는 선택 사항
     *  입력 함수를 호출                            _In_                    _In_opt_
     *  입력 함수를 호출하고 호출자에게 출력        _Inout_                 _Inout_opt_
     *  호출자에게 출력	                            _Out_                   _Out_opt_
     *  호출자에 대한 포인터의 출력                 _Outptr_                _Outptr_opt_
     */

int APIENTRY wWinMain(_In_ HINSTANCE    hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR        lpCmdLine,
                     _In_ int           nCmdShow)
{
    HACCEL  hAccelTable;
    MSG     msg;

    showConsole();/* 디버깅 목적 콘솔 창 열기  */

    /*
     *  프로그램을 작성 시 변수나 인자 선언 후 참조를 안할 경우 컴파일러는 C4100 오류를 발생
     *  이러한 warning을 피하기 위해 참조 안 된 변수 또는 인자에 UNREFERENCED_PARAMETER를 사용
     */
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);


    /*  전역 문자열을 초기화합니다. */
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_BLOCKEXPLORER, szWindowClass, MAX_LOADSTRING);
    if (!MyRegisterClass(hInstance)) {
        /*
         *  MyRegisterClass()->RegisterClass(): 윈도우 클래스를 등록
         *      윈도우 클래스 등록이 실패할 경우,
         *
         *  int MessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);
         *      hWnd        : 메시지 박스의 오너(Owner) 윈도우이다.
         *                    오너 윈도우란 메시지 상자를 소유한 윈도우를 말하며,
         *                    메시지 박스는 오너 윈도우의 중앙에 나타난다.
         *                    메시지 박스가 떠 있는 동안 오너 윈도우는 사용할 수 없는 상태가 된다.
         *                    메시지 박스를 닫아야만 오너 윈도우를 사용할 수 있다.
         *      lpText      : 메시지 박스에 출력할 문자열
         *      lpCaption   : 메시지 상자의 타이틀 바에 나타날 제목 문자열(NULL: Default값 출력, Error)
         *      uType       : DWORD값 참조
         *          그룹 1 - 메시지 박스에 나타날 버튼의 종류를 지정한다.
         *              MB_ABORTRETRYIGNORE : 중단 | 다시 시도 | 무시
         *              MB_OK               : 확인
         *              MB_OKCANCEL         : 확인 | 취소
         *              MB_RETRYCANCEL      : 다시 시도 | 취소
         *              MB_YESNO            : 예 | 아니요
         *              MB_YESNOCANCEL      : 예 | 아니오 | 취소
         *          그룹 2 - 메시지 박스에 나타날 아이콘을 지정한다.
         *              MB_ICONEXCLAMATION, MB_ICONWARNING      : 노란색 배경 느낌표 아이콘
         *              MB_ICONINFORMATION, MB_ICONASTERISK     : 하얀색 배경 느낌표 아이콘
         *              MB_ICONQUESTION                         : 하얀색 배경 물음표 아이콘
         *              MB_ICONSTOP, MB_ICONERROR, MB_ICONHAND  : 빨간색 아이콘
         *          그룹 3 - 디폴트 버튼을 지정한다.
         *              MB_DEFBUTTON1   : 첫번째 버튼이 디폴트 버튼이 된다. 별다른 지정이 없으면 이 플래그가 선택된다.
         *              MB_DEFBUTTON2   : 두번째 버튼이 디폴트 버튼이 된다.
         *              MB_DEFBUTTON3   : 세두번째 버튼이 디폴트 버튼이 된다.
         *              MB_DEFBUTTON4   : 네번째 버튼이 디폴트 버튼이 된다.
         *          그룹 4 - 대화상자의 모달 여부를 지정한다.
         *              MB_APPMODAL     : hWnd 윈도우의 모달 대화상자로 열리므로 메시지 박스를 닫기 전에는
         *                                같은 스레드에 속한 다른 윈도우로 전환할 수 없다.
         *                                그러나 다른 스레드의 윈도우로 전환할 수는 있으며 팝업 윈도우로도 전환할 수 있다.
         *                                아무 플래그도 지정되지 않으면 이 모드가 디폴트로 적용된다.
         *              MB_SYSTEMMODAL  : MB_APPMODAL과 동일하되 항상 위 스타일을 가져 모든 윈도우보다 위에 배치된다.
         *                                메모리 부족 등의 긴급한 상황을 알리고자 할 때 이 플래그를 사용한다.
         *              MB_TASKMODAL    : MS_APPMODAL과 동일하되 hWnd가 NULL일 경우 현재 스레드에 속한 모든 최상위 윈도우가 사용 금지된다는 점이
         *                                다르다. DLL이나 라이브러리에서 부모 윈도우로 지정할만한 윈도우가 없을 때 이 플래그를 사용한다.
         *          그룹 5 - 기타 몇가지 옵션을 지정한다.
         *              MB_DEFAULT_DESKTOP_ONLY         : 사용자가 로그온한 후 응용 프로그램이 실행되고 있는 디폴트 데스크 탑인 경우만
         *                                                메시지 박스가 나타나도록 한다.
         *              MB_HELP                         : 도움말 버튼을 보여준다. 이 버튼을 누르거나 F1키를 누르면 Help 이벤트가 발생한다.
         *              MB_RIGHT                        : 메시지를 오른쪽으로 정렬한다.
         *              MB_RTLREADING                   : 아랍이나 헤브루 윈도우즈에서 오른쪽에서 왼쪽 읽기로 출력한다. 한글 윈도우즈에서는
         *                                                의미가 없다.
         *              MB_SETFOREGROUND                : 메시지 박스를 포그라운드 윈도우로 만든다. 시스템은 내부적으로 SetForegroundWindow 함수를
         *                                                호출한다.
         *              MB_TOPMOST                      : 항상 위 옵션으로 메시지 박스를 생성한다.
         *              MB_SERVICE_NOTIFICATION         : NT의 서비스 프로그램만 사용할 수 있는 플래그이다.
         *                                                현재 로그인한 사용자가 없더라도 액티브되어 있는 데스크 탑에 메시지 박스를 출력한다.
         *                                                이 경우 hWnd는 반드시 NULL이어야 한다.
         *              MB_SERVICE_NOTIFICATION_NT3X    : MB_SERVICE_NOTIFICATION와 동일하되 NT 3.51버전에서 사용한다.
         *
         *      반환값
         *          IDABORT     : 중단 버튼을 눌렀다.
         *          IDCANCEL    : 취소 버튼을 눌렀다. Esc키를 눌러 대화상자를 닫을 때도 이 값이 리턴된다.
         *          IDIGNORE    : 무시 버튼을 눌렀다.
         *          IDNO        : 아니오 버튼을 눌렀다.
         *          IDOK        : 확인 버튼을 눌렀다.
         *          IDRETRY     : 재시도 버튼을 눌렀다.
         *          IDYES       : 예 버튼을 눌렀다.
         */
        MessageBox(
            NULL,
            TEXT("Window Registration Failed!"),
            NULL,
            MB_ICONEXCLAMATION | MB_OK | MB_SYSTEMMODAL);

        return FALSE;
    }

    /*  
     *  응용 프로그램 초기화를 수행합니다.
     *  InitInstance()->CreateWindowW() 과정이 실패한 경우,
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
     *  이 함수는 리소스로부터 액셀러레이터 테이블을 읽어들인다. 
     *  두번째 인수는 액셀러레이터 테이블의 이름 문자열 포인터이되 우리가 작성한 액셀러레이터 테이블은 정수값이므로 MAKEINTRESOURCE 매크로를 사용해야 한다.
     *  이 함수는 리소스에서 액셀러레이터 테이블을 읽은 후 그 핸들값을 리턴해 준다. 
     *  이 핸들값을 hAccelTable이라는 변수에 대입해 두면 다음부터 hAccelTable을 통해 액셀러레이터 테이블을 읽을 수 있다.
     */
    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BLOCKEXPLORER));

    /*
     *   기본 메시지 루프입니다.
     *
     *  BOOL GetMessage(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax);
     *      이 함수는 시스템이 유지하는 메시지 큐에서 메시지를 읽어들인다. 읽어들인 메시지는 첫번째 인수가 지정하는 MSG 구조체에 저장된다.
     *      이 함수는 읽어들인 메시지가 프로그램을 종료하라는 WM_QUIT일 경우 False를 리턴하며 그 외의 메시지이면 True를 리턴한다.
     *      따라서 WM_QUIT 메시지가 읽혀질 때까지, 즉 프로그램이 종료될 때까지 전체 while 루프가 계속 실행된다.
     *      나머지 세 개의 인수는 읽어들일 메시지의 범위를 지정하는데 잘 사용되지 않으므로 일단 무시하기로 한다.
     *  BOOL TranslateMessage(CONST MSG *lpMsg);
     *      키보드 입력 메시지를 가공하여 프로그램에서 쉽게 쓸 수 있도록 해 준다.
     *      윈도우즈는 키보드의 어떤 키가 눌러졌다거나 떨어졌을 때 키보드 메시지를 발생시키는데 이 함수는 키보드의 눌림(WM_KEYDOWN)과 떨어짐(WM_KEYUP)이 연속적으로
     *      발생할 때 문자가 입력되었다는 메시지(WM_CHAR)를 만드는 역할을 한다.
     *      예를 들어 A키를 누른 후 다시 A키를 떼면 A문자가 입력되었다는 메시지를 만들어 낸다.
     *  LONG DispatchMessage(CONST MSG *lpmsg);
     *      시스템 메시지 큐에서 꺼낸 메시지를 프로그램의 메시지 처리 함수(WndProc)로 전달한다. 이 함수에 의해 메시지가 프로그램으로 전달되며 프로그램에서는
     *      전달된 메시지를 점검하여 다음 동작을 결정하게 된다. 메시지 루프에서 하는 일은 메시지를 꺼내고, 필요한 경우 약간 형태를 바꾼 후 응용 프로그램으로
     *      전달하는 것 뿐이다. 이 과정은 WM_QUIT 메시지가 전달될 때까지, 즉 프로그램이 종료될때까지 반복된다.
     *      결국 메시지 루프가 하는 일이란 메시지 큐에서 메시지를 꺼내 메시지 처리 함수로 보내주는 것 뿐이다.
     *
     *      실제 메시지 처리는 별도의 메시지 처리 함수(WndProc)에서 수행한다.
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





