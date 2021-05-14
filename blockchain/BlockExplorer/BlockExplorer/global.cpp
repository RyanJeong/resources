#include "BlockExplorer.h"

HINSTANCE   hInst;                              //  현재 인스턴스입니다.
HWND        hWnd;                               //  현재 윈도우 정보입니다. 
WCHAR       szheader2[MAX_LOADSTRING];          //  제목 표시줄 텍스트입니다.  
WCHAR       szWindowClass[MAX_LOADSTRING];      //  기본 창 클래스 이름입니다. 

extern int BTC_tempScroll, BTC_getBlock_maxScroll, BTC_getSerializedBlock_maxScroll;  //  실제 윈도우 크기에 기반한 최대 스크롤 값

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
 *
 *  MyRegisterClass: 윈도우 클래스 등록
 */
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = {0};

    wcex.cbSize = sizeof(WNDCLASSEXW);  //  cb: Count of Bytes
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
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = (WNDPROC) WndProc;        //  lpfnWndProc - 윈도우에서 발생되는 메시지를 어느 함수에서 처리할지 메시지 처리함수의 주소를 받는다.
    wcex.cbClsExtra     = 0;                        //  cbClsExtra - 윈도우 클래스에서 사용할 여분 메모리를 바이트로 지정한다.
    wcex.cbWndExtra     = 0;                        //  cbWndExtra - 개별 윈도우에서 사용할 여분 메로리 영역 지정
    wcex.hInstance      = hInstance;                //  hInstance - 윈도우 클래스가 사용하는 인스턴스 핸들
    /*
     *  hIcon - 아이콘 모양 지정, LoadIcon() 또는 LoadImage() 호출
     *
     *      IDI_ASTERISK    : 정보 모양
     *      IDI_ERROR       : 에러 모양
     *      IDI_EXCLAMATION : 경고 모양
     *      IDI_QUESTION    : 질문 모양
     */
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BLOCKEXPLORER));
    /*
     *  hCursor - 마우스커서 모양 지정, LoadCursor() 호출
     *            
     *      IDC_ARROW       : 화살표
     *      IDC_IBEAM       : 캐럿
     *      IDC_WAIT        : 모래시계
     *      IDC_CROSS       : 십자모양 커서
     *      IDC_UPARROW     : 수직 화살표
     *      IDCBTC_getBlock_sizeNWSE    : 역슬래시 모양 크기조절
     *      IDCBTC_getBlock_sizeNESW    : 슬래시 모양 크기조절
     *      IDCBTC_getBlock_sizeWE      : 수평 크기조절
     *      IDCBTC_getBlock_sizeNS      : 수직 크기조절
     *      IDCBTC_getBlock_sizeALL     : 4방향 화살표
     *      IDC_NO          : 금지모양
     *      IDC_HAND        : 손 모양
     *      IDC_APPSTARTING : 화살표 + 모래시계
     *      IDC_HELP        : 화살표 + ?
     */
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    /*
     *  hbrBackground - 윈도우 배경색상 지정.
     *
     *  GetStockObject() 함수를 쓰면 윈도우에서 지정하는 기본 윈도우 색을 지정할 수 있음. (HBRUSH) 형변환필요.
     *      WHITE_BRUSH     : 하얀색 배경화면
     *      BLACK_BRUSH     : 검은색 배경화면
     *      LTGRAY_BRUSH    : 회색 배경화면
     *
     *  CreateSolidBrush() 함수를 쓰면 RGB 색상으로 색 지정 가능
     *      CreateSolidBrush(RGB(10,20,50)) : 레드(10),그린(20),블루(50)의 RGB 색상
     *
     *  NULL값으로 지정할경우는 WM_ERASEBKGND 메시지에서 직접 배경을 설정해야 한다.
     */
    wcex.hbrBackground  = (HBRUSH) (COLOR_WINDOW + 1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_BLOCKEXPLORER);              //  lpszMenuName - 프로그램이 사용할 메뉴지정
    wcex.lpszClassName  = szWindowClass;                                    //  lpszClassName - 윈도우 클래스의 이름 지정
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));    //  hIconSm - 작은 아이콘 지정
    RegisterClassExW(&wcex);                                                //  메인 윈도우 등록
    wcex.lpszMenuName   = NULL;                                             //  위에 부모 윈도우 등록할 때 메뉴를 등록했기 때문에, 다시 제거해야 함
    wcex.hbrBackground  = (HBRUSH) GetStockObject(WHITE_BRUSH);
    wcex.lpfnWndProc    = (WNDPROC) WndProcPopup;
    wcex.lpszClassName  = TEXT("mypopup");
    
    return RegisterClassExW(&wcex);                                         //  팝업 윈도우 등록 후, 등록한 결과를 반환
}

/*
 *  InitInstance:   인스턴스 핸들을 저장하고 메인 윈도우를 생성합니다.
 *                  이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고 주 프로그램 윈도우를 만든 다음 표시합니다.
 */
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    RECT rect;

    rect    = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    AdjustWindowRect(&rect, (WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX), true);   //  프레임 크기를 포함하여 사용자 정의 크기에 맞춤
    hInst   = hInstance;                                                            //인스턴스 핸들을 전역 변수에 저장합니다.
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
        szWindowClass, 
        szheader2,
        /*
         * WS_OVERLAPPED        : 디폴트 윈도우. 타이틀 바와 크기 조절이 안되는 경계선을 가진다. 아무런 스타일도 주지 않으면 이 스타일이 적용된다.
         * WS_POPUP             : 팝업 윈도우를 만든다. WS_CHILD와 함께 쓸 수 없다.
         * WS_CHILD             : 차일드 윈도우를 만든다. WS_POPUP과 함께 쓸 수 없다.
         * WS_MINIMIZE          : 최초 최소화된 상태로 윈도우를 만든다.
         * WS_VISIBLE           : 윈도우를 만들자 마자 화면에 출력한다.
         * WS_DISABLED          : 사용금지된 상태, 즉 입력을 받아들일 수 없는 상태로 만든다.
         * WS_CLIPSIBLINGS      : 차일드끼리 상호 겹친 영역은 그리기 영역에서 제외된다.
         * WS_CLIPCHILDREN      : 차일드가 위치한 영역은 그리기 영역에서 제외된다.
         * WS_MAXIMIZE          : 최초 최대화된 상태로 윈도우를 만든다.
         * WS_CAPTION           : 타이틀 바를 가진 윈도우를 만들며 WS_BORDER 스타일을 포함한다.
         * WS_BORDER            : 단선으로 된 경계선을 만들며 크기 조정은 할 수 없다.
         * WS_DLGFRAME          : 두꺼운 경계선을 가지며 타이틀 바를 가질 수 없다.
         * WS_VSCROLL           : 수직 스크롤 바를 가진 윈도우를 만든다.
         * WS_HSCROLL           : 수평 스크롤 바를 가진 윈도우를 만든다.
         * WS_SYSMENU           : 시스템 메뉴를 가진 윈도우를 만든다.
         * WS_THICKFRAME        : 크기 조정이 가능한 두꺼운 경계선을 가진다.
         * WS_GROUP             : 라디오 버튼 등 그룹을 이루는 컨트롤의 그룹 지정에 사용된다.
         * WS_TABSTOP           : Tab키로 포커스를 이동할 수 있도록 한다.
         * WS_MINIMIZEBOX       : 최소화 버튼을 만든다.
         * WS_MAXIMIZEBOX       : 최대화 버튼을 만든다.
         * WS_OVERLAPPEDWINDOW  : 가장 일반적인 윈도우 스타일. WS_OVERLAPPED |WS_CAPTION |WS_SYSMENU |WS_THICKFRAME |WS_MINIMIZEBOX |WS_MAXIMIZEBOX 스타일들이 조합된 스타일이다.
         * WS_POPUPWINDOW       : 일반적인 팝업 윈도우. WS_POPUP |WS_BORDER |WS_SYSMENU
         * WS_CHILDWINDOW       : WS_CHILD와 같다.
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
    GetClientRect(hWnd, &rect);                                                             //  실제 개발자가 사용할 수 있는 크기를 얻기 위해 client window size를 얻어옴
    BTC_tempScroll                      = WINDOW_HEADER_HEIGHT - (rect.bottom - rect.top);  //  - ((rect.bottom - rect.top) - WINDOW_HEADER_HEIGHT)
    BTC_getBlock_maxScroll              = BTC_GETBLOCK_LENGTH + BTC_tempScroll;             //  (18 * OFFSET(40)) - (윈도우 크기 중 실제로 사용 가능한 윈도우 크기(bottom - top) - WINDOW_HEADER_HEIGHT(128))
    BTC_getSerializedBlock_maxScroll    = 1024;                                             //  need to change
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
     *  UpdateWindow 함수란 생성된 윈도우의 일부가 다른 윈도우 등에 가려졌거나 리사이즈 되었을 경우,
     *  즉 무효화 영역(Invalid Region)에 대해 WM_PAINT 메시지를 해당 윈도우에 보냄으로써 무효화 영역을
     *  갱신시켜 주는 함수입니다.
     *
     *  윈도우에 발생된는 메시지의 일반적인 처리방법은 발생된 메시지를 해당 윈도우 메시지 큐에 넣은 후
     *  어플리케이션이 발생된 메시지를 메시지 큐에 들어온 순서대로 메시지를 처리하는 것입니다.
     *
     *  하지만 UpdateWindow 함수 호출을 통해 발생되는 WM_PAINT 메시지는 해당 윈도우의
     *  메시지 큐를 거치지 않고 해당 윈도우에 바로 전달되는 특징을 가지고 있습니다.
     *
     *  단, 윈도우에 무효화 영역(Invalid Region)이 없는 경우에 UpdataWindow 함수를 호출한다면
     *  WM_PAINT 메시지는 발생되지 않는 점을 주의하셔야 합니다.
     *  
     *  InvalidateRect(): 메세지 큐를 거치지 못함
     *  ----------------------------------------------------------
     *  InvalidateRect()는 윈도우의 클라이언트 영역 중에서 일부분을 무효화 시키는 것입니다.
     *  이때, 첫번째 인자는 어떤 윈도우인지를 가리키는 윈도우 핸들이고,
     *  두번째 인자는 무효과시킬 사각 영역 좌표입니다. 이게 NULL이면 클라이언트 영역 전체라는 것이구요.
     *  그리고 세번째 인자는 그 무효화 영역이 지워지고 다시 그려져야 하는지를 설정합니다. 
     *  만약 TRUE이면, 그 윈도우에게는 WM_ERASEBKGND 메시지가 한번 날려지고, WM_PAINT 메시지가 날라갑니다. 만약 FALSE이면 WM_PAINT 메시지만 날라갑니다.
     *  위의 답변에 보시면 InvalidateRect()가 WM_PAINT 메시지를 발생시키지 않는다고 했는데요, WM_PAINT 메시지를 발생 시킵니다
     *  그리고 UpdateWindow()의 역할은 WM_PAINT 메시지를 발생시키는 것이며, 이때 윈도우가 무효화 영역이 있어야 합니다.
     *
     *  즉, 다시 그려져야할 필요가 있는 영역이 있다면 다시 그리도록 만드는 것입니다.
     *  정리하면, InvalidateRect()는 특정 영역을 무조건 다시 그리도록 만드는 함수이며, 
     *  UpdateWindow()는 현재 윈도우에서 다시 그릴 필요가 있는 영역이 있는지 확인하고 있다면 다시 그리도록 하는 함수입니다.
     *  
     */
    UpdateWindow(hWnd);

    return TRUE;
}

/*  
 *  About: 정보 대화 상자의 메시지 처리기입니다.   
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