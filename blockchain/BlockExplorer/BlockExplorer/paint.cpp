/*
    190828 
    
    bitblt 수정할 것
    1. 출력할 내용만큼 hdc 생성(즉, ROW_OFFSET * 갯수 만큼 높이 지정하여 생성)
    2. 스크롤 위치에 따라 생성한 hdc의 시작점 지정
    3. 시작점으로부터 bitblt 사용하여 복사
    4. 각 메뉴마다 hdc를 개별적으로 보관하여 유지해놓을 것
    5. 만약 새로운 검색 명령이 들어오면 hdc 재 할당하여 사용(memory 관리)
    => 1) proc.cpp에서 내용이 바뀌는 부분에 init 변수 0으로 초기화
    => 2) paint.cpp에서 init변수가 0이면 bitblt 사용하여 최초 MemDC 생성하고, 그 이후엔 이 과정 스킵, 만들어져 있는 MemDC에서 좌표만 입력해 화면 옮기는 과정만.
*/

#include "BlockExplorer.h"
#include "paint.h"

extern char         **BTC_tx;
extern TCHAR        convertCharToTCHAR[BUF_SIZE];
extern int          BTC_getBlockCount;          //  현재 메인 체인의 블록 갯수 
extern int          BTC_serializedBlockLen;
extern char         *BTC_serializedBlock;       //  블록을 16진수 값으로 읽어들임
extern char         BTC_hash[HASH_SIZE];        //  현재 조회한 높이의 블록 해더 해쉬 값
extern char         BTC_prevHash[HASH_SIZE];    //  현재 조회한 높이의 블록 해더 해쉬의 이전 값
extern char         BTC_nextHash[HASH_SIZE];    //  현재 조회한 높이의 블록 헤더 해쉬의 다음 값
extern char         BTC_getBlock_hash[HASH_SIZE], BTC_getBlock_versionHex[BITS_SIZE], BTC_getBlock_merkleRoot[HASH_SIZE], BTC_getBlock_bits[BITS_SIZE], BTC_getBlock_chainWork[HASH_SIZE];
extern int          BTC_getBlock_confirmations, BTC_getBlock_size, BTC_getBlock_strippedSize, BTC_getBlock_weight, BTC_getBlock_height, BTC_getBlock_version, BTC_getBlock_time, BTC_getBlock_medianTime, BTC_getBlock_nTx;
extern unsigned int BTC_getBlock_nonce;
extern double       BTC_getBlock_difficulty;
extern int          procState;
extern SCROLLINFO   scr;
extern int          selectedItem;
extern TCHAR        msg[BUF_SIZE];

int BTC_getBlock_initMemDC  = 0;    //  server로 명령을 요청하여 얻은 결과가 바뀌게 되면(즉 화면을 다른 내용으로 갱신해야 하면, 스크롤하여 상하 이동하는 것 제외)
int BTC_tx_initMemDC        = 0;    //  *_initMemDC 변수들을 0으로 초기화하여 MemDC에 새로운 내용으로 채워넣음

TCHAR   columnName[BTC_GET_BLOCK_NUM_OF_ROWS][BUF_SIZE] = {
    TEXT("hash"),
    TEXT("confirmations"),
    TEXT("strippedsize"),
    TEXT("size"),
    TEXT("weight"),
    TEXT("height"),
    TEXT("version"),
    TEXT("versionHex"),
    TEXT("merkleroot"),
    TEXT("tx(total number)"),
    TEXT("time(UNIX time)"),
    TEXT("mediantime"),
    TEXT("nonce"),
    TEXT("bits"),
    TEXT("difficulty"),
    TEXT("chainwork"),
    TEXT("previousblockhash"),
    TEXT("nextblockhash")};

int     gOffset;        //  각 행을 그리기 위한 간격을 저장하는 변수
int     gRowNum;        //  번갈아가며 행의 배경을 바꿔주기 위해 사용하는 변수(홀수는 무슨 색, 짝수는 다른 색..)이면서 동시에 전체 행의 개수를 저장하는 변수

void Wm_Paint(HWND hWnd)
{
    /*
     *  1. 새로운 GDI 폰트 객체 생성
     *  2. 우리가 만든 폰트 객체를 dc가 선택하게 하고 기존 폰트의 핸들을 보관한다.
     *  3. 텍스트 출력
     *  4. 원래의 폰트를 dc에게 다시 저장 시켜준다
     *  5. 사용이 끝난 우리가 만든 폰트는 소멸시킨다.
     */
    HDC				hdc;
    PAINTSTRUCT		ps;

    hdc = BeginPaint(hWnd, &ps);
    /*
     *  TA_TOP:         지정한 좌표가 상단좌표가 된다.
     *  TA_BOTTOM:      지정한 좌표가 하단 좌표가 된다.
     *  TA_CENTER:      지정한 좌표가 수평 중앙 좌표가 된다.
     *  TA_LEFT:        지정한 좌표가 수평 왼쪽 좌표가 된다.
     *  TA_RIGHT:       지정한 좌표가 수평 오른쪽 좌표가 된다.
     *  TA_UPDATECP:    지정한 좌표대신 CP를 사용하며 문자열 출력후에 CP를 변경한다.
     *  TA_NOUPDATACP:  CP를 사용하지 않고 지정한 좌표를 사용하며 CP를 변경하지 않는다.
     */
    gRowNum = 0;
    switch (procState) {
        case IDM_BLOCK:
            BTC_printGetBlock(hdc, hWnd);
            break;
        case IDM_SERIALIZED_BLOCK:
            BTC_printGetSerializedBlock(hdc);
            break;
        case IDM_TX:
            BTC_printTx(hdc, hWnd);
            break;
        default:
            break;
    }

    EndPaint(hWnd, &ps);
}

 void BTC_printGetBlock(HDC hdc, HWND hWnd)
{
    static HDC      headerMemDC, contentMemDC;
    static HBITMAP  headerBit, contentBit; 
    static int      headerWidthSize, headerHeightSize, contentWidthSize, contentHeightSize;

    RECT        crt, rect;
    //HBITMAP     hOldBit;
    BITMAP      bit;
    HFONT       fontContentStyle1, fontContentStyle2, hOldFont;
    TCHAR       *pColumnName;
    BOOL        bitbltResult1, bitbltResult2;
    int         temp;

    fontContentStyle1 = CreateFont(HEADER_FONT_SIZE, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, TEXT("고딕체"));
    fontContentStyle2 = CreateFont(CONTENT_FONT_SIZE, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, TEXT("고딕체"));

    if (!BTC_getBlock_initMemDC) {
        if (headerMemDC && contentMemDC) {
#if (DEBUG_MODE && DEBUG_PAINT_CPP)
            printf("headerMemDC && contentMemDC\n");
#endif      
            DeleteObject(headerBit);        //  꼭 static으로 지워야 할까 ..? 최적화 필요
            DeleteObject(contentBit);
            DeleteDC(headerMemDC);
            DeleteDC(contentMemDC);
        }
        ++BTC_getBlock_initMemDC;
        gOffset = 0;
        SetTextAlign(hdc, TA_LEFT);

        //     int nHeight,                 // 높이
        //     int nWidth,                  // 폭. 0을 넣어주면 높이에 비례함
        //     int nEscapement,             // 글자 전체 기울기
        //     int nOrientation,            // 기준선이 정해져 있고 기우는 기울기
        //     int fnWeight,                // 폰트의 두께
        //     DWORD fdwItalic,             // 이탤릭
        //     DWORD fdwUnderline,          // 밑줄
        //     DWORD fdwStrikeOut,          // 취소선
        //     DWORD fdwCharSet,            // 캐릭터 셋
        //     DWORD fdwOutputPrecision,    // 정밀도
        //     DWORD fdwClipPrecision,      // 정밀도
        //     DWORD fdwQuality,            // 정밀도
        //     DWORD fdwPitchAndFamily,     // 정밀도
        //     LPCTSTR lpszFace             // 글꼴이름


        //  Header Bitblt
        crt.right   = WINDOW_WIDTH;
        crt.bottom  = WINDOW_HEADER_HEIGHT;
        headerBit   = CreateCompatibleBitmap(hdc, crt.right, crt.bottom);
        headerMemDC = CreateCompatibleDC(hdc);
        //hOldBit     = (HBITMAP) SelectObject(headerMemDC, headerBit);            //  store original bitmap info.
        SelectObject(headerMemDC, headerBit);   //  store original bitmap info.
        FillRect(headerMemDC, &crt, GetSysColorBrush(COLOR_WINDOW));
        SelectObject(headerMemDC, GetStockObject(BLACK_PEN));
        hOldFont    = (HFONT) SelectObject(headerMemDC, fontContentStyle1); //  store original font info.

        wsprintf(msg, TEXT("비트코인 블록의 최대 높이는 %d 입니다. 아래 정보는 %d 번 블록 정보입니다."), BTC_getBlockCount, BTC_getBlock_height);
        TextOut(headerMemDC, HEADER_X_POS, HEADER_Y_POS, msg, lstrlen(msg));
        GetObject(headerBit, sizeof(BITMAP), &bit);

        headerWidthSize     = bit.bmWidth;
        headerHeightSize    = bit.bmHeight;

        //  Body Bitblt
        crt.bottom      = (BTC_GET_BLOCK_NUM_OF_ROWS * ROW_OFFSET);
        contentBit      = CreateCompatibleBitmap(hdc, crt.right, crt.bottom);
        contentMemDC    = CreateCompatibleDC(hdc);
        SelectObject(contentMemDC, contentBit);
        FillRect(contentMemDC, &crt, GetSysColorBrush(COLOR_WINDOW));
        SelectObject(contentMemDC, GetStockObject(BLACK_PEN));
        SelectObject(contentMemDC, fontContentStyle2);

        // start
        SetBkMode(contentMemDC, TRANSPARENT);   //  배경 투명하게
        drawRow(contentMemDC, ROW_OFFSET);      // 1. draw line, 2. gOffset += padding
        pColumnName = *columnName;
        SetTextColor(contentMemDC, RGB(HYPERLINK_BLUE_R, HYPERLINK_BLUE_G, HYPERLINK_BLUE_B));
        TextOut(contentMemDC, GET_BLOCK_COLUMN1_X_POS, gOffset, pColumnName, lstrlen(pColumnName));
        MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, BTC_hash, strlen(BTC_hash), convertCharToTCHAR, BUF_SIZE);
        wsprintf(msg, TEXT("%s"), convertCharToTCHAR);
        SetTextColor(contentMemDC, RGB(BLACK_R, BLACK_G, BLACK_B));
        TextOut(contentMemDC, GET_BLOCK_COLUMN2_X_POS, gOffset, msg, lstrlen(msg));
        gOffset += (ROW_OFFSET - CONTENT_PADDING);

        drawRow(contentMemDC, ROW_OFFSET);
        pColumnName += BUF_SIZE;
        SetTextColor(contentMemDC, RGB(HYPERLINK_BLUE_R, HYPERLINK_BLUE_G, HYPERLINK_BLUE_B));
        TextOut(contentMemDC, GET_BLOCK_COLUMN1_X_POS, gOffset, pColumnName, lstrlen(pColumnName));
        wsprintf(msg, TEXT("%d"), BTC_getBlock_confirmations);
        SetTextColor(contentMemDC, RGB(BLACK_R, BLACK_G, BLACK_B));
        TextOut(contentMemDC, GET_BLOCK_COLUMN2_X_POS, gOffset, msg, lstrlen(msg));
        gOffset += (ROW_OFFSET - CONTENT_PADDING);

        drawRow(contentMemDC, ROW_OFFSET);
        pColumnName += BUF_SIZE;
        SetTextColor(contentMemDC, RGB(HYPERLINK_BLUE_R, HYPERLINK_BLUE_G, HYPERLINK_BLUE_B));
        TextOut(contentMemDC, GET_BLOCK_COLUMN1_X_POS, gOffset, pColumnName, lstrlen(pColumnName));
        wsprintf(msg, TEXT("%d"), BTC_getBlock_strippedSize);
        SetTextColor(contentMemDC, RGB(BLACK_R, BLACK_G, BLACK_B));
        TextOut(contentMemDC, GET_BLOCK_COLUMN2_X_POS, gOffset, msg, lstrlen(msg));
        gOffset += (ROW_OFFSET - CONTENT_PADDING);

        drawRow(contentMemDC, ROW_OFFSET);
        pColumnName += BUF_SIZE;
        SetTextColor(contentMemDC, RGB(HYPERLINK_BLUE_R, HYPERLINK_BLUE_G, HYPERLINK_BLUE_B));
        TextOut(contentMemDC, GET_BLOCK_COLUMN1_X_POS, gOffset, pColumnName, lstrlen(pColumnName));
        wsprintf(msg, TEXT("%d"), BTC_getBlock_size);
        SetTextColor(contentMemDC, RGB(BLACK_R, BLACK_G, BLACK_B));
        TextOut(contentMemDC, GET_BLOCK_COLUMN2_X_POS, gOffset, msg, lstrlen(msg));
        gOffset += (ROW_OFFSET - CONTENT_PADDING);

        drawRow(contentMemDC, ROW_OFFSET);
        pColumnName += BUF_SIZE;
        SetTextColor(contentMemDC, RGB(HYPERLINK_BLUE_R, HYPERLINK_BLUE_G, HYPERLINK_BLUE_B));
        TextOut(contentMemDC, GET_BLOCK_COLUMN1_X_POS, gOffset, pColumnName, lstrlen(pColumnName));
        wsprintf(msg, TEXT("%d"), BTC_getBlock_weight);
        SetTextColor(contentMemDC, RGB(BLACK_R, BLACK_G, BLACK_B));
        TextOut(contentMemDC, GET_BLOCK_COLUMN2_X_POS, gOffset, msg, lstrlen(msg));
        gOffset += (ROW_OFFSET - CONTENT_PADDING);

        drawRow(contentMemDC, ROW_OFFSET);
        pColumnName += BUF_SIZE;
        SetTextColor(contentMemDC, RGB(HYPERLINK_BLUE_R, HYPERLINK_BLUE_G, HYPERLINK_BLUE_B));
        TextOut(contentMemDC, GET_BLOCK_COLUMN1_X_POS, gOffset, pColumnName, lstrlen(pColumnName));
        wsprintf(msg, TEXT("%d"), BTC_getBlock_height);
        SetTextColor(contentMemDC, RGB(BLACK_R, BLACK_G, BLACK_B));
        TextOut(contentMemDC, GET_BLOCK_COLUMN2_X_POS, gOffset, msg, lstrlen(msg));
        gOffset += (ROW_OFFSET - CONTENT_PADDING);

        drawRow(contentMemDC, ROW_OFFSET);
        pColumnName += BUF_SIZE;
        SetTextColor(contentMemDC, RGB(HYPERLINK_BLUE_R, HYPERLINK_BLUE_G, HYPERLINK_BLUE_B));
        TextOut(contentMemDC, GET_BLOCK_COLUMN1_X_POS, gOffset, pColumnName, lstrlen(pColumnName));
        wsprintf(msg, TEXT("%d"), BTC_getBlock_version);
        SetTextColor(contentMemDC, RGB(BLACK_R, BLACK_G, BLACK_B));
        TextOut(contentMemDC, GET_BLOCK_COLUMN2_X_POS, gOffset, msg, lstrlen(msg));
        gOffset += (ROW_OFFSET - CONTENT_PADDING);

        drawRow(contentMemDC, ROW_OFFSET);
        pColumnName += BUF_SIZE;
        SetTextColor(contentMemDC, RGB(HYPERLINK_BLUE_R, HYPERLINK_BLUE_G, HYPERLINK_BLUE_B));
        TextOut(contentMemDC, GET_BLOCK_COLUMN1_X_POS, gOffset, pColumnName, lstrlen(pColumnName));
        wsprintf(msg, TEXT("%d"), BTC_getBlock_versionHex);
        SetTextColor(contentMemDC, RGB(BLACK_R, BLACK_G, BLACK_B));
        TextOut(contentMemDC, GET_BLOCK_COLUMN2_X_POS, gOffset, msg, lstrlen(msg));
        gOffset += (ROW_OFFSET - CONTENT_PADDING);

        drawRow(contentMemDC, ROW_OFFSET);
        pColumnName += BUF_SIZE;
        SetTextColor(contentMemDC, RGB(HYPERLINK_BLUE_R, HYPERLINK_BLUE_G, HYPERLINK_BLUE_B));
        TextOut(contentMemDC, GET_BLOCK_COLUMN1_X_POS, gOffset, pColumnName, lstrlen(pColumnName));
        MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, BTC_getBlock_merkleRoot, strlen(BTC_getBlock_merkleRoot), convertCharToTCHAR, BUF_SIZE);
        wsprintf(msg, TEXT("%s"), convertCharToTCHAR);
        SetTextColor(contentMemDC, RGB(BLACK_R, BLACK_G, BLACK_B));
        TextOut(contentMemDC, GET_BLOCK_COLUMN2_X_POS, gOffset, msg, lstrlen(msg));
        gOffset += (ROW_OFFSET - CONTENT_PADDING);

        drawRow(contentMemDC, ROW_OFFSET);
        pColumnName += BUF_SIZE;
        SetTextColor(contentMemDC, RGB(HYPERLINK_BLUE_R, HYPERLINK_BLUE_G, HYPERLINK_BLUE_B));
        TextOut(contentMemDC, GET_BLOCK_COLUMN1_X_POS, gOffset, pColumnName, lstrlen(pColumnName));
        wsprintf(msg, TEXT("%d"), BTC_getBlock_nTx);
        //SetTextColor(hMemDC, RGB(BLACK_R, BLACK_G, BLACK_B));       for tx's color
        TextOut(contentMemDC, GET_BLOCK_COLUMN2_X_POS, gOffset, msg, lstrlen(msg));
        gOffset += (ROW_OFFSET - CONTENT_PADDING);

        drawRow(contentMemDC, ROW_OFFSET);
        pColumnName += BUF_SIZE;
        //SetTextColor(hMemDC, RGB(HYPERLINK_BLUE_R, HYPERLINK_BLUE_G, HYPERLINK_BLUE_B));        set color blue already
        TextOut(contentMemDC, GET_BLOCK_COLUMN1_X_POS, gOffset, pColumnName, lstrlen(pColumnName));
        wsprintf(msg, TEXT("%d"), BTC_getBlock_time);
        SetTextColor(contentMemDC, RGB(BLACK_R, BLACK_G, BLACK_B));
        TextOut(contentMemDC, GET_BLOCK_COLUMN2_X_POS, gOffset, msg, lstrlen(msg));
        gOffset += (ROW_OFFSET - CONTENT_PADDING);

        drawRow(contentMemDC, ROW_OFFSET);
        pColumnName += BUF_SIZE;
        SetTextColor(contentMemDC, RGB(HYPERLINK_BLUE_R, HYPERLINK_BLUE_G, HYPERLINK_BLUE_B));
        TextOut(contentMemDC, GET_BLOCK_COLUMN1_X_POS, gOffset, pColumnName, lstrlen(pColumnName));
        wsprintf(msg, TEXT("%d"), BTC_getBlock_medianTime);
        SetTextColor(contentMemDC, RGB(BLACK_R, BLACK_G, BLACK_B));
        TextOut(contentMemDC, GET_BLOCK_COLUMN2_X_POS, gOffset, msg, lstrlen(msg));
        gOffset += (ROW_OFFSET - CONTENT_PADDING);

        drawRow(contentMemDC, ROW_OFFSET);
        pColumnName += BUF_SIZE;
        SetTextColor(contentMemDC, RGB(HYPERLINK_BLUE_R, HYPERLINK_BLUE_G, HYPERLINK_BLUE_B));
        TextOut(contentMemDC, GET_BLOCK_COLUMN1_X_POS, gOffset, pColumnName, lstrlen(pColumnName));
        wsprintf(msg, TEXT("%u"), BTC_getBlock_nonce);
        SetTextColor(contentMemDC, RGB(BLACK_R, BLACK_G, BLACK_B));
        TextOut(contentMemDC, GET_BLOCK_COLUMN2_X_POS, gOffset, msg, lstrlen(msg));
        gOffset += (ROW_OFFSET - CONTENT_PADDING);

        drawRow(contentMemDC, ROW_OFFSET);
        pColumnName += BUF_SIZE;
        SetTextColor(contentMemDC, RGB(HYPERLINK_BLUE_R, HYPERLINK_BLUE_G, HYPERLINK_BLUE_B));
        TextOut(contentMemDC, GET_BLOCK_COLUMN1_X_POS, gOffset, pColumnName, lstrlen(pColumnName));
        wsprintf(msg, TEXT("%d"), BTC_getBlock_bits);
        SetTextColor(contentMemDC, RGB(BLACK_R, BLACK_G, BLACK_B));
        TextOut(contentMemDC, GET_BLOCK_COLUMN2_X_POS, gOffset, msg, lstrlen(msg));
        gOffset += (ROW_OFFSET - CONTENT_PADDING);

        drawRow(contentMemDC, ROW_OFFSET);
        pColumnName += BUF_SIZE;
        SetTextColor(contentMemDC, RGB(HYPERLINK_BLUE_R, HYPERLINK_BLUE_G, HYPERLINK_BLUE_B));
        TextOut(contentMemDC, GET_BLOCK_COLUMN1_X_POS, gOffset, pColumnName, lstrlen(pColumnName));
        wsprintf(msg, TEXT("%d"), BTC_getBlock_difficulty);
        SetTextColor(contentMemDC, RGB(BLACK_R, BLACK_G, BLACK_B));
        TextOut(contentMemDC, GET_BLOCK_COLUMN2_X_POS, gOffset, msg, lstrlen(msg));
        gOffset += (ROW_OFFSET - CONTENT_PADDING);

        drawRow(contentMemDC, ROW_OFFSET);
        pColumnName += BUF_SIZE;
        SetTextColor(contentMemDC, RGB(HYPERLINK_BLUE_R, HYPERLINK_BLUE_G, HYPERLINK_BLUE_B));
        TextOut(contentMemDC, GET_BLOCK_COLUMN1_X_POS, gOffset, pColumnName, lstrlen(pColumnName));
        MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, BTC_getBlock_chainWork, strlen(BTC_getBlock_chainWork), convertCharToTCHAR, BUF_SIZE);
        wsprintf(msg, TEXT("%s"), convertCharToTCHAR);
        SetTextColor(contentMemDC, RGB(BLACK_R, BLACK_G, BLACK_B));
        TextOut(contentMemDC, GET_BLOCK_COLUMN2_X_POS, gOffset, msg, lstrlen(msg));
        gOffset += (ROW_OFFSET - CONTENT_PADDING);

        drawRow(contentMemDC, ROW_OFFSET);
        pColumnName += BUF_SIZE;
        SetTextColor(contentMemDC, RGB(HYPERLINK_BLUE_R, HYPERLINK_BLUE_G, HYPERLINK_BLUE_B));
        TextOut(contentMemDC, GET_BLOCK_COLUMN1_X_POS, gOffset, pColumnName, lstrlen(pColumnName));
        if (!BTC_getBlock_height) {
            wsprintf(msg, TEXT("The genesis block doesn't have a previous block header hash"));
        } else {
            MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, BTC_prevHash, strlen(BTC_prevHash), convertCharToTCHAR, BUF_SIZE);
            wsprintf(msg, TEXT("%s"), convertCharToTCHAR);
        }
        SetTextColor(contentMemDC, RGB(BLACK_R, BLACK_G, BLACK_B));
        TextOut(contentMemDC, GET_BLOCK_COLUMN2_X_POS, gOffset, msg, lstrlen(msg));
        gOffset += (ROW_OFFSET - CONTENT_PADDING);

        drawRow(contentMemDC, ROW_OFFSET);
        pColumnName += BUF_SIZE;
        SetTextColor(contentMemDC, RGB(HYPERLINK_BLUE_R, HYPERLINK_BLUE_G, HYPERLINK_BLUE_B));
        TextOut(contentMemDC, GET_BLOCK_COLUMN1_X_POS, gOffset, pColumnName, lstrlen(pColumnName));
        if (BTC_getBlock_height == BTC_getBlockCount) {
            wsprintf(msg, TEXT("The best(tip) block doesn't have a next block header hash"));
        } else {
            MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, BTC_nextHash, strlen(BTC_nextHash), convertCharToTCHAR, BUF_SIZE);
            wsprintf(msg, TEXT("%s"), convertCharToTCHAR);
        }
        SetTextColor(contentMemDC, RGB(BLACK_R, BLACK_G, BLACK_B));
        TextOut(contentMemDC, GET_BLOCK_COLUMN2_X_POS, gOffset, msg, lstrlen(msg));
        gOffset += (ROW_OFFSET - CONTENT_PADDING);

        //  end line
        MoveToEx(contentMemDC, 0, gOffset, NULL);
        LineTo(contentMemDC, WINDOW_WIDTH, gOffset);

        GetObject(contentBit, sizeof(BITMAP), &bit);
        contentWidthSize    = bit.bmWidth;
        contentHeightSize   = bit.bmHeight;

        bitbltResult1 = BitBlt(hdc, 0, 0, headerWidthSize, headerHeightSize, headerMemDC, 0, 0, SRCCOPY);
        bitbltResult2 = BitBlt(hdc, 0, WINDOW_HEADER_HEIGHT, contentWidthSize, contentHeightSize, contentMemDC, 0, scr.nPos, SRCCOPY);

        SelectObject(headerMemDC, hOldFont);                    //  recover
    } else {
        hOldFont = (HFONT) SelectObject(hdc, fontContentStyle2);
        bitbltResult2 = BitBlt(hdc, 0, WINDOW_HEADER_HEIGHT, contentWidthSize, contentHeightSize, contentMemDC, 0, scr.nPos, SRCCOPY);
        //  represent click area
        SetBkMode(hdc, TRANSPARENT);   //  배경 투명하게
        if (selectedItem >= 0) {
            if (selectedItem < BTC_GET_BLOCK_NUM_OF_ROWS) { 
                temp    = WINDOW_HEADER_HEIGHT + ((selectedItem * ROW_OFFSET) - scr.nPos);
                rect    = {0, (temp + 1), GET_BLOCK_COLUMN1_WIDTH, (temp + ROW_OFFSET)};  // +1: 영역이 겹쳐지는 것을 피하기 위한 목적
                FillRect(hdc, &rect, CreateSolidBrush(RGB(LIGHT_BLUE_R, LIGHT_BLUE_G, LIGHT_BLUE_B)));
                TextOut(hdc, (GET_BLOCK_COLUMN1_X_POS), (temp + CONTENT_PADDING), *(columnName + selectedItem), lstrlen(*(columnName + selectedItem)));
            } else if (selectedItem = BTC_TX_SELECTED) {
                temp    = WINDOW_HEADER_HEIGHT + ((BTC_TX_INDEX * ROW_OFFSET) - scr.nPos);
                rect    = {GET_BLOCK_COLUMN1_WIDTH, (temp + 1), WINDOW_WIDTH, (temp + ROW_OFFSET)};  // +1: 영역이 겹쳐지는 것을 피하기 위한 목적
                FillRect(hdc, &rect, CreateSolidBrush(RGB(LIGHT_BLUE_R, LIGHT_BLUE_G, LIGHT_BLUE_B)));
                wsprintf(msg, TEXT("%d"), BTC_getBlock_nTx);
                TextOut(hdc, (GET_BLOCK_COLUMN2_X_POS), (temp + CONTENT_PADDING), msg, lstrlen(msg));
            }
        }
        bitbltResult1 = BitBlt(hdc, 0, 0, headerWidthSize, headerHeightSize, headerMemDC, 0, 0, SRCCOPY);       //  hovering 겹치는 영역 방지하기 위해 hovering 후 bitblt
        SelectObject(hdc, hOldFont); //  recover
    }
    #if (DEBUG_MODE && DEBUG_PAINT_CPP)
        printf("bitbltResult1: %d, bitbltResult2: %d, BTC_getBlock_initMemDC: %d\n", bitbltResult1, bitbltResult2, BTC_getBlock_initMemDC);
    #endif
    ReleaseDC(hWnd, hdc);
    DeleteObject(fontContentStyle1);
    DeleteObject(fontContentStyle2);

    return;
}
/*
void BTC_printGetSerializedBlock(HDC hdc)
{
    int row, temp;
    int i, j, k, index;
    int x, y, step, state[18] = {8, 72, 136, 144, 152, 160, 162, 170, 172, 244, 246, 400, 408, 410, 426, 428, 562, 570};    //  의미에 따른 바이트 스트림을 구분하기 위해 각 번지들을 지정 
    index   = 0;    //  현재 몇 번째 바이트인지 나타내는 변수
    step    = 0;    //  바이트 스트림을 의미에 맞게 쪼개기 위해 구분자 역할을 수행하는 변수
    row     = 0;    //  바이트 스트림 전체 행 수
    x       = PRINT_HEX_X_START_POS;            //  바이트 스트림 시작 X 좌표
    y       = PRINT_HEX_Y_START_POS - scr.nPos; //  바이트 스트림 시작 Y 좌표, 스크롤에 따라 화면 이동 
    temp    = BTC_serializedBlockLen;
    while (temp > 0) {
        ++row;
        temp -= COLUMN;
    }
     *  01000000 - version(8, 8)
     *  0000000000000000000000000000000000000000000000000000000000000000 - prev block(64, 72)
     *  3BA3EDFD7A7B12B27AC72C3E67768F617FC81BC3888A51323A9FB8AA4B1E5E4A - merkle root(64, 136)
     *  29AB5F49 - timestamp(8, 144)
     *  FFFF001D - bits(8, 152)
     *  1DAC2B7C - nonce(8, 160)
     *  01 - number of transactions(2, 162)
     *  01000000 - version(8, 170)
     *  01 - input(2, 172)
     *  0000000000000000000000000000000000000000000000000000000000000000FFFFFFFF - prev output(72, 244)
     *  4D - script length(2, 246)
     *  04FFFF001D0104455468652054696D65732030332F4A616E2F32303039204368616E63656C6C6F72206F6E206272696E6B206F66207365636F6E64206261696C6F757420666F722062616E6B73 - scriptsig(154, 400)
     *  FFFFFFFF - sequence(8, 408)
     *  01 - outputs(2, 410)
     *  00F2052A01000000 - 50 BTC(16, 426)
     *  43 - pk_script length(2, 428)
     *  4104678AFDB0FE5548271967F1A67130B7105CD6A828E03909A67962E0EA1F61DEB649F6BC3F4CEF38C4F35504E51EC112DE5C384DF7BA0B8D578A4C702B6BF11D5FAC - pk_script(134, 562)
     *  00000000 - lock time(8, 570)
    HFONT serialized, content, old;
    SetTextAlign(hdc, TA_UPDATECP);
    serialized = CreateFont(24, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, TEXT("고딕체"));
    content = CreateFont(24, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, TEXT("명조체"));
    old = (HFONT)SelectObject(hdc, serialized);             //  store
    for (i = 0; i < row; ++i) {
        for (j = 0; j < 2; ++j) {
            for (k = 0; k < 8; ++k) {
                MoveToEx(hdc, x, y, NULL);
                TextOut(hdc, 0, 0, TEXT("FF"), lstrlen(TEXT("FF")));
                x += PRINT_HEX_X_OFFSET;
                ++index;
                if (index == state[step]) {
                    SetTextColor(hdc, RGB(!(step % 3) ? 255 : 127, !((step + 1) % 3) ? 255 : 127, !((step + 2) % 3) ? 255 : 127));
                    ++step;
                }
                if (index >= BTC_serializedBlockLen) {
                    break;
                }
            }
            x += PRINT_HEX_X_OFFSET;
        }
        y += PRINT_HEX_Y_OFFSET;
        x = PRINT_HEX_X_START_POS;
    }
    SetTextColor(hdc, RGB(255, 0, 0));
    TextOut(hdc, 0, 0, TEXT("Hello\nHello"), lstrlen(TEXT("Hello\nHello")));
    MoveToEx(hdc, 500, 500, NULL);
    SetTextColor(hdc, RGB(0, 255, 0));
    TextOut(hdc, 0, 0, TEXT("Hello"), lstrlen(TEXT("Hello")));
    SelectObject(hdc, old);                                 //  recover
    DeleteObject(content);
    DeleteObject(serialized);
    return;
}
*/

 void BTC_printGetSerializedBlock(HDC hdc)
{
    /*
     *  p       : BTC_serializedBlock 가리키는 포인터
     *  i, j, k : for문용 제어 변수
     *  x, y    : 좌표 지정 변수 
     *  temp    : BTC_serializedBlockLen 값 임시 저장 변수
     *  row     : serialized 값을 hexdump 형태로 출력해줄 때 총 몇 행인지 저장하는 변수    
     *  index   : serialized block 문자열 내에서 사용하는 index
     */
    char    *p;
    int     i, j, k, x, y, temp, row, index;
    HFONT   content, old;

    p       = BTC_serializedBlock;
    temp    = BTC_serializedBlockLen;
    row     = 0;
    index   = 0;
    while (temp > 0) {
        ++row;
        temp -= COLUMN;
    }
    x = PRINT_HEX_X_START_POS;            //  바이트 스트림 시작 X 좌표
    y = PRINT_HEX_Y_START_POS - scr.nPos; //  바이트 스트림 시작 Y 좌표, 스크롤에 따라 화면 이동 
    SetTextAlign(hdc, TA_UPDATECP);
    content = CreateFont(20, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, TEXT("명조체"));
    old = (HFONT) SelectObject(hdc, content);   //  store
    for (i = 0; i < row; ++i) {
        for (j = 0; j < 2; ++j) {
            for (k = 0; k < 8; ++k) {
                wsprintf(msg, TEXT("%c"), *p++);
                MoveToEx(hdc, x, y, NULL);
                TextOut(hdc, 0, 0, msg, 2);
                ++index;
                x += PRINT_HEX_X_OFFSET1;
                wsprintf(msg, TEXT("%c"), *p++);
                MoveToEx(hdc, x, y, NULL);
                TextOut(hdc, 0, 0, msg, 2);
                x += PRINT_HEX_X_OFFSET2;
                ++index;
                if (index >= BTC_serializedBlockLen) {
                    break;
                }
            }
            x += PRINT_HEX_X_OFFSET;
        }
        y += PRINT_HEX_Y_OFFSET;
        x = PRINT_HEX_X_START_POS;
    }
    SelectObject(hdc, old);                     //  recover
    DeleteObject(content);
    return;
}

 void BTC_printTx(HDC hdc, HWND hWnd)
{
    static HDC      headerMemDC, contentMemDC;
    static HBITMAP  headerBit, contentBit; 
    static int      headerWidthSize, headerHeightSize, contentWidthSize, contentHeightSize;

    RECT        crt, rect;
    //HBITMAP     hOldBit;
    BITMAP      bit;
    HFONT       fontContentStyle1, fontContentStyle2, hOldFont;
    BOOL        bitbltResult1, bitbltResult2;
    int         i, temp;
    char        **temp_BTC_tx;

    fontContentStyle1 = CreateFont(HEADER_FONT_SIZE, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, TEXT("고딕체"));
    fontContentStyle2 = CreateFont(CONTENT_FONT_SIZE, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, TEXT("고딕체"));

    if (!BTC_tx_initMemDC) {
        if (headerMemDC && contentMemDC) {
#if (DEBUG_MODE && DEBUG_PAINT_CPP)
            printf("headerMemDC && contentMemDC\n");
#endif      
            DeleteObject(headerBit);        //  꼭 static으로 지워야 할까 ..? 최적화 필요
            DeleteObject(contentBit);
            DeleteDC(headerMemDC);
            DeleteDC(contentMemDC);
        }
        ++BTC_tx_initMemDC;
        SetTextAlign(hdc, TA_LEFT);
        gOffset = 0;
        //더블버퍼링 공식
        //GetClientRect(hWnd, &crt); //영역을 잡아준다.

        //  Header Bitblt
        crt.right   = WINDOW_WIDTH;
        crt.bottom  = WINDOW_HEADER_HEIGHT;
        headerBit   = CreateCompatibleBitmap(hdc, crt.right, crt.bottom);
        headerMemDC = CreateCompatibleDC(hdc);
        SelectObject(headerMemDC, headerBit);
        FillRect(headerMemDC, &crt, GetSysColorBrush(COLOR_WINDOW));
        SelectObject(headerMemDC, GetStockObject(BLACK_PEN));
        hOldFont    = (HFONT)SelectObject(headerMemDC, fontContentStyle1);             //  store

        wsprintf(msg, TEXT("아래 정보는 %d 번 블록의 거래내역 입니다."), BTC_getBlock_height);
        TextOut(headerMemDC, HEADER_X_POS, HEADER_Y_POS, msg, lstrlen(msg));
        GetObject(headerBit, sizeof(BITMAP), &bit);

        headerWidthSize     = bit.bmWidth;
        headerHeightSize    = bit.bmHeight;

        //  Body Bitblt
        crt.bottom      = (ROW_OFFSET * BTC_getBlock_nTx);;
        contentBit      = CreateCompatibleBitmap(hdc, crt.right, crt.bottom);
        contentMemDC    = CreateCompatibleDC(hdc);
        SelectObject(contentMemDC, contentBit);
        FillRect(contentMemDC, &crt, GetSysColorBrush(COLOR_WINDOW));
        SelectObject(contentMemDC, GetStockObject(BLACK_PEN));
        SelectObject(contentMemDC, fontContentStyle2);    //  store

        // start
        SetBkMode(contentMemDC, TRANSPARENT);   //  배경 투명하게
        SetTextColor(contentMemDC, RGB(HYPERLINK_BLUE_R, HYPERLINK_BLUE_G, HYPERLINK_BLUE_B));
        temp_BTC_tx = BTC_tx;
        //  모든 거래 내역 MemDC에 저장
        for (i = 0; i < BTC_getBlock_nTx; ++i) {
            drawRow(contentMemDC, ROW_OFFSET);      // 1. draw line, 2. gOffset += padding
            MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, *temp_BTC_tx, strlen(*temp_BTC_tx), convertCharToTCHAR, BUF_SIZE);
            *temp_BTC_tx++;
            wsprintf(msg, TEXT("%s"), convertCharToTCHAR);
            TextOut(contentMemDC, GET_BLOCK_COLUMN1_X_POS, gOffset, msg, lstrlen(msg));
            if (!i) {   //  check coinbase
                TextOut(contentMemDC, BTC_COINBASE_OFFSET, gOffset, TEXT("COINBASE"), lstrlen(TEXT("COINBASE")));;
            }
            gOffset += (ROW_OFFSET - CONTENT_PADDING);
        }
        //  end line
        MoveToEx(contentMemDC, 0, gOffset, NULL);
        LineTo(contentMemDC, WINDOW_WIDTH, gOffset);

        GetObject(contentBit, sizeof(BITMAP), &bit);
        contentWidthSize    = bit.bmWidth;
        contentHeightSize   = bit.bmHeight;

        bitbltResult1 = BitBlt(hdc, 0, 0, headerWidthSize, headerHeightSize, headerMemDC, 0, 0, SRCCOPY);
        bitbltResult2 = BitBlt(hdc, 0, WINDOW_HEADER_HEIGHT, contentWidthSize, contentHeightSize, contentMemDC, 0, scr.nPos, SRCCOPY);
        SelectObject(headerMemDC, hOldFont);                    //  recover
    } else {
        hOldFont        = (HFONT) SelectObject(hdc, fontContentStyle2);
        bitbltResult2   = BitBlt(hdc, 0, WINDOW_HEADER_HEIGHT, contentWidthSize, contentHeightSize, contentMemDC, 0, scr.nPos, SRCCOPY);
        //  represent click area
        SetBkMode(hdc, TRANSPARENT);   //  배경 투명하게
        if (selectedItem >= 0) {
            if (selectedItem < BTC_getBlock_nTx) { 
                temp    = WINDOW_HEADER_HEIGHT + ((selectedItem * ROW_OFFSET) - scr.nPos);
                rect    = {0, (temp + 1), WINDOW_WIDTH, (temp + ROW_OFFSET)};  // +1: 영역이 겹쳐지는 것을 피하기 위한 목적
                FillRect(hdc, &rect, CreateSolidBrush(RGB(LIGHT_BLUE_R, LIGHT_BLUE_G, LIGHT_BLUE_B)));
                MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, *(BTC_tx + selectedItem), strlen(*(BTC_tx + selectedItem)), convertCharToTCHAR, BUF_SIZE);
                wsprintf(msg, TEXT("%s"), convertCharToTCHAR);
                TextOut(hdc, (GET_BLOCK_COLUMN1_X_POS), (temp + CONTENT_PADDING), msg, lstrlen(msg));
                if (selectedItem == 0) {   //  check coinbase, selectedItem is (-1) when object not selected
                    TextOut(hdc, BTC_COINBASE_OFFSET, (temp + CONTENT_PADDING), TEXT("COINBASE"), lstrlen(TEXT("COINBASE")));;
                }
            }
        }
        bitbltResult1   = BitBlt(hdc, 0, 0, headerWidthSize, headerHeightSize, headerMemDC, 0, 0, SRCCOPY); //  hovering 겹치는 영역 방지하기 위해 hovering 후 bitblt
        SelectObject(hdc, hOldFont); //  recover
    }
    #if (DEBUG_MODE && DEBUG_PAINT_CPP)
        printf("bitbltResult1: %d, bitbltResult2: %d, BTC_getBlock_initMemDC: %d\n", bitbltResult1, bitbltResult2, BTC_getBlock_initMemDC);
    #endif
    ReleaseDC(hWnd, hdc);
    DeleteObject(fontContentStyle1);
    DeleteObject(fontContentStyle2);

    return;
}

 void drawRow(HDC hdc, int offset)
{
    RECT rect;

    MoveToEx(hdc, 0, gOffset, NULL);
    LineTo(hdc, WINDOW_WIDTH, gOffset);
    if (!(gRowNum & 01)) {
        rect = { 0, gOffset + 1, WINDOW_WIDTH, gOffset + offset};      // +1: 영역이 겹쳐지는 것을 피하기 위한 목적
        FillRect(hdc, &rect, CreateSolidBrush(RGB(LIGHT_GRAY_R, LIGHT_GRAY_G, LIGHT_GRAY_B)));
    }
    ++gRowNum;
    gOffset += CONTENT_PADDING;

    return;
}