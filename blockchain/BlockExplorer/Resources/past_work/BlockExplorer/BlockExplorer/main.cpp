#include "common.h"
#include "stdafx.h"
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

extern HINSTANCE    g_hInst;
LPCWSTR             lpszClass = TEXT("BlockViewer");

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
    HWND 		hWnd;
    MSG 		Message;
    WNDCLASS 	WndClass;
    /*  멀티바이트일때 "", 유니코드일때 L""로 변환하는 매크로 (TCHAR) */
    

    /*
     *  g_hInst: WinMain의 인수값인 hInstance를 재사용하기 위해 전역변수에 대입
     */
    g_hInst = hInstance;

    /*
     *  WNDCLASS 구조체
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
     *  style - 윈도우 스타일 옵션을 지정하는 것으로 윈도우 변경시 업데이트 여부 설정(비트연산)
     *      CS_HREDRAW      - 수평 폭에 대해 윈도우를 다시 그림
     *      CS_VREDRAW      - 수직 폭에 대해 윈도우를 다시 그림
     *      CS_DBLCLKS      - 마우스가 더블클릭되는 메시지를 받는다. (미지정시 마우스 더블클릭 메시지 무시됨)
     *      CS_NOCLOSE      - 시스템 메뉴중 닫기를 비활성화 시킴 (ALT + F4, 닫기버튼 모두 해당)
     *      CS_CLASSDC      - 클래스별로 만들어진 로컬 인스턴스의 모든 윈도우가 DC를 공유
     *      CS_OWNDC        - 로컬 인스턴스 내의 윈도우들이 개별적인 DC를 사용
     *      CS_PARENTDC     - 자식윈도우가 부모윈도우 DC를 사용
     *      CS_GLOBALCLASS  - 응용프로그램 전역 클래스로 등록
     *      CS_SAVEBITS     - 윈도우의 배경화면 영역을 비트맵으로 저장
     */
    WndClass.style = (CS_HREDRAW | CS_VREDRAW);

    /*  lpfnWndProc - 윈도우에서 발생되는 메시지를 어느 함수에서 처리할지 메시지 처리함수의 주소를 받는다.   */
    WndClass.lpfnWndProc = (WNDPROC) WndProc;

    /*  cbClsExtra - 윈도우 클래스에서 사용할 여분 메모리를 바이트로 지정한다.   */
    WndClass.cbClsExtra = 0;

    /*  cbWndExtra - 개별 윈도우에서 사용할 여분 메로리 영역 지정  */
    WndClass.cbWndExtra = 0;
    
    /*  hInstance - 윈도우 클래스가 사용하는 인스턴스 핸들   */
    WndClass.hInstance = hInstance;
    
    /*
     *  hIcon - 아이콘 모양 지정
     *          LoadIcon 또는 LoadImage 호출
     *      IDI_ASTERISK    : 정보 모양 
     *      IDI_ERROR       : 에러 모양
     *      IDI_EXCLAMATION : 경고 모양
     *      IDI_QUESTION    : 질문 모양
     */
    WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    
    /*
     *  hCursor - 마우스커서 모양 지정
     *            LoadCursor 호출
     *      IDC_ARROW       : 화살표
     *      IDC_IBEAM       : 캐럿
     *      IDC_WAIT        : 모래시계
     *      IDC_CROSS       : 십자모양 커서
     *      IDC_UPARROW     : 수직 화살표
     *      IDC_SIZENWSE    : 역슬래시 모양 크기조절
     *      IDC_SIZENESW    : 슬래시 모양 크기조절
     *      IDC_SIZEWE      : 수평 크기조절
     *      IDC_SIZENS      : 수직 크기조절
     *      IDC_SIZEALL     : 4방향 화살표
     *      IDC_NO          : 금지모양
     *      IDC_HAND        : 손 모양
     *      IDC_APPSTARTING : 화살표 + 모래시계
     *      IDC_HELP        : 화살표 + ?
     */
    WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    
    /*
     *  hbrBackground - 윈도우 배경색상 지정. 
     *                  GetStockObject() 함수를 쓰면 윈도우에서 지정하는 기본 윈도우 색을 지정할 수 있음. (HBRUSH) 형변환필요. 
     *      WHITE_BRUSH     : 하얀색 배경화면
     *      BLACK_BRUSH     : 검은색 배경화면
     *      LTGRAY_BRUSH    : 회색 배경화면
     *
     *                  CreateSolidBrush() 함수를 쓰면 RGB 색상으로 색 지정 가능
     *      CreateSolidBrush(RGB(10,20,50)) : 레드(10),그린(20),블루(50)의 RGB 색상
     *
     *                  NULL값으로 지정할경우는 WM_ERASEBKGND 메시지에서 직접 배경을 설정해야 한다.
     */
    WndClass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
    
    /*  lpszMenuName - 프로그램이 사용할 메뉴지정   */
    WndClass.lpszMenuName = MAKEINTRESOURCE(MAINMENU);

    /*  lpszClassName - 윈도우 클래스의 이름 지정   */
    WndClass.lpszClassName = lpszClass;


    if (!RegisterClass(&WndClass)) {
        /*
         *  RegisterClass(): 윈도우 클래스를 등록
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

        return 1;
    }

    /*
     *  CreateWindow(): 메모리 상에 윈도우 생성
     *      lpszClassName           : 생성하고자 하는 윈도우의 클래스를 지정하는 문자열이다. 
     *                                앞에서 정의한 WndClass구조체의 lpszClassName 멤버의 이름을 여기에 기입해 준다. 
     *                                우리의 예제에서는 lpszClass 문자열에 윈도우 클래스 이름을 기억시켜 두었으므로 
     *                                이 문자열을 그대로 넘겨주면 된다.
     *      lpszWindowName          : 윈도우의 타이틀 바에 나타날 문자열이다. 
     *                                여기서 지정한 문자열이 윈도우의 타이틀 바에 나타난다. 
     *      dwStyle                 : 만들고자 하는 윈도우의 형태를 지정하는 인수이다. 
     *                                일종의 비트 필드값이며 거의 수십개를 헤아리는 매크로 상수들이 정의되어 있고 이 상수들을 OR연산자로 
     *                                연결하여 윈도우의 다양한 형태를 지정한다. 윈도우가 경계선을 가질 것인가, 타이틀 바를 가질 것인가 
     *                                또는 스크롤 바의 유무 등등을 세세하게 지정해 줄 수 있다. 가능한 스타일값에 관한 자세한 내용은 
     *                                레퍼런스를 참조하되 WS_OVERLAPPEDWINDOW를 사용하면 가장 무난한 윈도우 설정 상태가 된다. 
     *                                즉 시스템 메뉴, 최대 최소 버튼, 타이틀 바, 경계선을 가진 윈도우를 만들어 준다.
     *      X, Y, nWidth, nHeight   : 인수의 이름이 의미하듯이 윈도우의 크기와 위치를 지정하며 픽셀 단위를 사용한다. 
     *                                x, y좌표는 메인 윈도우의 경우는 전체 화면을 기준으로 하며 차일드 윈도우는 부모 윈도우의 좌상단을 기준으로 한다. 
     *                                정수값을 바로 지정해도 되며 CW_USEDEFAULT를 사용하면 윈도우즈가 알아서 적당한 크기와 위치를 설정해 준다. 
     *                                예제에서는 모두 CW_USEDEFAULT를 사용하였다.
     *      hWndParent              : 부모 윈도우가 있을 경우 부모 윈도우의 핸들을 지정해 준다. 
     *                                MDI 프로그램이나 팝업 윈도우는 윈도우끼리 수직적인 상하관계를 가져 부자(parent-child) 관계가 성립되는데 
     *                                이 관계를 지정해 주는 인수이다. 부모 윈도우가 없을 경우는 이 값을 NULL로 지정하면 된다.
     *      hmenu                   : 윈도우에서 사용할 메뉴의 핸들을 지정한다. WndClass에도 메뉴를 지정하는 멤버가 있는데 윈도우 클래스의 메뉴는 
     *                                그 윈도우 클래스를 기반으로 하는 모든 윈도우에서 사용되는 반면 이 인수로 지정된 메뉴는 현재 CreateWindow 함수로 
     *                                만들어지는 윈도우에서만 사용된다. 만약 WndClass에서 지정한 메뉴를 그대로 사용하려면 이 인수를 NULL로 지정하면 되며 
     *                                WndClass에서 지정한 메뉴 대신 다른 메뉴를 사용하려면 이 인수에 원하는 메뉴 핸들을 주면 된다. 
     *      hinst                   : 윈도우를 만드는 주체, 즉 프로그램의 핸들을 지정한다. WinMain의 인수로 전달된 hInstance를 대입해 주면 된다.
     *      lpvParam                : CREATESTRUCT라는 구조체의 번지이며 특수한 목적에 사용된다. 보통은 NULL값을 사용한다.
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
     *  CreateWindow() 과정이 실패한 경우,
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
     *  ShowWindow(): CreateWindow()을 통해 메모리에 생성한 윈도우를 화면에 표시하는 함수
     *  nCmdShow 옵션
     *      SW_HIDE         : 윈도우를 숨긴다.
     *      SW_MINIMIZE     : 윈도우를 최소화시키고 활성화시키지 않는다.
     *      SW_RESTORE      : 윈도우를 활성화시킨다.
     *      SW_SHOW         : 윈도우를 활성화시켜 보여준다.
     *      SW_SHOWNORMAL   : 윈도우를 활성화시켜 보여준다.
     */
    ShowWindow(hWnd, nCmdShow);
    /*
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
    while (GetMessage(&Message, 0, 0, 0)) {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }

    return Message.wParam;
}