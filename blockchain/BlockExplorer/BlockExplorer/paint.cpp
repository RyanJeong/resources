/*
    190828 
    
    bitblt ������ ��
    1. ����� ���븸ŭ hdc ����(��, ROW_OFFSET * ���� ��ŭ ���� �����Ͽ� ����)
    2. ��ũ�� ��ġ�� ���� ������ hdc�� ������ ����
    3. ���������κ��� bitblt ����Ͽ� ����
    4. �� �޴����� hdc�� ���������� �����Ͽ� �����س��� ��
    5. ���� ���ο� �˻� ����� ������ hdc �� �Ҵ��Ͽ� ���(memory ����)
    => 1) proc.cpp���� ������ �ٲ�� �κп� init ���� 0���� �ʱ�ȭ
    => 2) paint.cpp���� init������ 0�̸� bitblt ����Ͽ� ���� MemDC �����ϰ�, �� ���Ŀ� �� ���� ��ŵ, ������� �ִ� MemDC���� ��ǥ�� �Է��� ȭ�� �ű�� ������.
*/

#include "BlockExplorer.h"
#include "paint.h"

extern char         **BTC_tx;
extern TCHAR        convertCharToTCHAR[BUF_SIZE];
extern int          BTC_getBlockCount;          //  ���� ���� ü���� ��� ���� 
extern int          BTC_serializedBlockLen;
extern char         *BTC_serializedBlock;       //  ����� 16���� ������ �о����
extern char         BTC_hash[HASH_SIZE];        //  ���� ��ȸ�� ������ ��� �ش� �ؽ� ��
extern char         BTC_prevHash[HASH_SIZE];    //  ���� ��ȸ�� ������ ��� �ش� �ؽ��� ���� ��
extern char         BTC_nextHash[HASH_SIZE];    //  ���� ��ȸ�� ������ ��� ��� �ؽ��� ���� ��
extern char         BTC_getBlock_hash[HASH_SIZE], BTC_getBlock_versionHex[BITS_SIZE], BTC_getBlock_merkleRoot[HASH_SIZE], BTC_getBlock_bits[BITS_SIZE], BTC_getBlock_chainWork[HASH_SIZE];
extern int          BTC_getBlock_confirmations, BTC_getBlock_size, BTC_getBlock_strippedSize, BTC_getBlock_weight, BTC_getBlock_height, BTC_getBlock_version, BTC_getBlock_time, BTC_getBlock_medianTime, BTC_getBlock_nTx;
extern unsigned int BTC_getBlock_nonce;
extern double       BTC_getBlock_difficulty;
extern int          procState;
extern SCROLLINFO   scr;
extern int          selectedItem;
extern TCHAR        msg[BUF_SIZE];

int BTC_getBlock_initMemDC  = 0;    //  server�� ����� ��û�Ͽ� ���� ����� �ٲ�� �Ǹ�(�� ȭ���� �ٸ� �������� �����ؾ� �ϸ�, ��ũ���Ͽ� ���� �̵��ϴ� �� ����)
int BTC_tx_initMemDC        = 0;    //  *_initMemDC �������� 0���� �ʱ�ȭ�Ͽ� MemDC�� ���ο� �������� ä������

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

int     gOffset;        //  �� ���� �׸��� ���� ������ �����ϴ� ����
int     gRowNum;        //  �����ư��� ���� ����� �ٲ��ֱ� ���� ����ϴ� ����(Ȧ���� ���� ��, ¦���� �ٸ� ��..)�̸鼭 ���ÿ� ��ü ���� ������ �����ϴ� ����

void Wm_Paint(HWND hWnd)
{
    /*
     *  1. ���ο� GDI ��Ʈ ��ü ����
     *  2. �츮�� ���� ��Ʈ ��ü�� dc�� �����ϰ� �ϰ� ���� ��Ʈ�� �ڵ��� �����Ѵ�.
     *  3. �ؽ�Ʈ ���
     *  4. ������ ��Ʈ�� dc���� �ٽ� ���� �����ش�
     *  5. ����� ���� �츮�� ���� ��Ʈ�� �Ҹ��Ų��.
     */
    HDC				hdc;
    PAINTSTRUCT		ps;

    hdc = BeginPaint(hWnd, &ps);
    /*
     *  TA_TOP:         ������ ��ǥ�� �����ǥ�� �ȴ�.
     *  TA_BOTTOM:      ������ ��ǥ�� �ϴ� ��ǥ�� �ȴ�.
     *  TA_CENTER:      ������ ��ǥ�� ���� �߾� ��ǥ�� �ȴ�.
     *  TA_LEFT:        ������ ��ǥ�� ���� ���� ��ǥ�� �ȴ�.
     *  TA_RIGHT:       ������ ��ǥ�� ���� ������ ��ǥ�� �ȴ�.
     *  TA_UPDATECP:    ������ ��ǥ��� CP�� ����ϸ� ���ڿ� ����Ŀ� CP�� �����Ѵ�.
     *  TA_NOUPDATACP:  CP�� ������� �ʰ� ������ ��ǥ�� ����ϸ� CP�� �������� �ʴ´�.
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

    fontContentStyle1 = CreateFont(HEADER_FONT_SIZE, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, TEXT("���ü"));
    fontContentStyle2 = CreateFont(CONTENT_FONT_SIZE, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, TEXT("���ü"));

    if (!BTC_getBlock_initMemDC) {
        if (headerMemDC && contentMemDC) {
#if (DEBUG_MODE && DEBUG_PAINT_CPP)
            printf("headerMemDC && contentMemDC\n");
#endif      
            DeleteObject(headerBit);        //  �� static���� ������ �ұ� ..? ����ȭ �ʿ�
            DeleteObject(contentBit);
            DeleteDC(headerMemDC);
            DeleteDC(contentMemDC);
        }
        ++BTC_getBlock_initMemDC;
        gOffset = 0;
        SetTextAlign(hdc, TA_LEFT);

        //     int nHeight,                 // ����
        //     int nWidth,                  // ��. 0�� �־��ָ� ���̿� �����
        //     int nEscapement,             // ���� ��ü ����
        //     int nOrientation,            // ���ؼ��� ������ �ְ� ���� ����
        //     int fnWeight,                // ��Ʈ�� �β�
        //     DWORD fdwItalic,             // ���Ÿ�
        //     DWORD fdwUnderline,          // ����
        //     DWORD fdwStrikeOut,          // ��Ҽ�
        //     DWORD fdwCharSet,            // ĳ���� ��
        //     DWORD fdwOutputPrecision,    // ���е�
        //     DWORD fdwClipPrecision,      // ���е�
        //     DWORD fdwQuality,            // ���е�
        //     DWORD fdwPitchAndFamily,     // ���е�
        //     LPCTSTR lpszFace             // �۲��̸�


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

        wsprintf(msg, TEXT("��Ʈ���� ����� �ִ� ���̴� %d �Դϴ�. �Ʒ� ������ %d �� ��� �����Դϴ�."), BTC_getBlockCount, BTC_getBlock_height);
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
        SetBkMode(contentMemDC, TRANSPARENT);   //  ��� �����ϰ�
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
        SetBkMode(hdc, TRANSPARENT);   //  ��� �����ϰ�
        if (selectedItem >= 0) {
            if (selectedItem < BTC_GET_BLOCK_NUM_OF_ROWS) { 
                temp    = WINDOW_HEADER_HEIGHT + ((selectedItem * ROW_OFFSET) - scr.nPos);
                rect    = {0, (temp + 1), GET_BLOCK_COLUMN1_WIDTH, (temp + ROW_OFFSET)};  // +1: ������ �������� ���� ���ϱ� ���� ����
                FillRect(hdc, &rect, CreateSolidBrush(RGB(LIGHT_BLUE_R, LIGHT_BLUE_G, LIGHT_BLUE_B)));
                TextOut(hdc, (GET_BLOCK_COLUMN1_X_POS), (temp + CONTENT_PADDING), *(columnName + selectedItem), lstrlen(*(columnName + selectedItem)));
            } else if (selectedItem = BTC_TX_SELECTED) {
                temp    = WINDOW_HEADER_HEIGHT + ((BTC_TX_INDEX * ROW_OFFSET) - scr.nPos);
                rect    = {GET_BLOCK_COLUMN1_WIDTH, (temp + 1), WINDOW_WIDTH, (temp + ROW_OFFSET)};  // +1: ������ �������� ���� ���ϱ� ���� ����
                FillRect(hdc, &rect, CreateSolidBrush(RGB(LIGHT_BLUE_R, LIGHT_BLUE_G, LIGHT_BLUE_B)));
                wsprintf(msg, TEXT("%d"), BTC_getBlock_nTx);
                TextOut(hdc, (GET_BLOCK_COLUMN2_X_POS), (temp + CONTENT_PADDING), msg, lstrlen(msg));
            }
        }
        bitbltResult1 = BitBlt(hdc, 0, 0, headerWidthSize, headerHeightSize, headerMemDC, 0, 0, SRCCOPY);       //  hovering ��ġ�� ���� �����ϱ� ���� hovering �� bitblt
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
    int x, y, step, state[18] = {8, 72, 136, 144, 152, 160, 162, 170, 172, 244, 246, 400, 408, 410, 426, 428, 562, 570};    //  �ǹ̿� ���� ����Ʈ ��Ʈ���� �����ϱ� ���� �� �������� ���� 
    index   = 0;    //  ���� �� ��° ����Ʈ���� ��Ÿ���� ����
    step    = 0;    //  ����Ʈ ��Ʈ���� �ǹ̿� �°� �ɰ��� ���� ������ ������ �����ϴ� ����
    row     = 0;    //  ����Ʈ ��Ʈ�� ��ü �� ��
    x       = PRINT_HEX_X_START_POS;            //  ����Ʈ ��Ʈ�� ���� X ��ǥ
    y       = PRINT_HEX_Y_START_POS - scr.nPos; //  ����Ʈ ��Ʈ�� ���� Y ��ǥ, ��ũ�ѿ� ���� ȭ�� �̵� 
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
    serialized = CreateFont(24, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, TEXT("���ü"));
    content = CreateFont(24, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, TEXT("����ü"));
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
     *  p       : BTC_serializedBlock ����Ű�� ������
     *  i, j, k : for���� ���� ����
     *  x, y    : ��ǥ ���� ���� 
     *  temp    : BTC_serializedBlockLen �� �ӽ� ���� ����
     *  row     : serialized ���� hexdump ���·� ������� �� �� �� ������ �����ϴ� ����    
     *  index   : serialized block ���ڿ� ������ ����ϴ� index
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
    x = PRINT_HEX_X_START_POS;            //  ����Ʈ ��Ʈ�� ���� X ��ǥ
    y = PRINT_HEX_Y_START_POS - scr.nPos; //  ����Ʈ ��Ʈ�� ���� Y ��ǥ, ��ũ�ѿ� ���� ȭ�� �̵� 
    SetTextAlign(hdc, TA_UPDATECP);
    content = CreateFont(20, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, TEXT("����ü"));
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

    fontContentStyle1 = CreateFont(HEADER_FONT_SIZE, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, TEXT("���ü"));
    fontContentStyle2 = CreateFont(CONTENT_FONT_SIZE, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, TEXT("���ü"));

    if (!BTC_tx_initMemDC) {
        if (headerMemDC && contentMemDC) {
#if (DEBUG_MODE && DEBUG_PAINT_CPP)
            printf("headerMemDC && contentMemDC\n");
#endif      
            DeleteObject(headerBit);        //  �� static���� ������ �ұ� ..? ����ȭ �ʿ�
            DeleteObject(contentBit);
            DeleteDC(headerMemDC);
            DeleteDC(contentMemDC);
        }
        ++BTC_tx_initMemDC;
        SetTextAlign(hdc, TA_LEFT);
        gOffset = 0;
        //������۸� ����
        //GetClientRect(hWnd, &crt); //������ ����ش�.

        //  Header Bitblt
        crt.right   = WINDOW_WIDTH;
        crt.bottom  = WINDOW_HEADER_HEIGHT;
        headerBit   = CreateCompatibleBitmap(hdc, crt.right, crt.bottom);
        headerMemDC = CreateCompatibleDC(hdc);
        SelectObject(headerMemDC, headerBit);
        FillRect(headerMemDC, &crt, GetSysColorBrush(COLOR_WINDOW));
        SelectObject(headerMemDC, GetStockObject(BLACK_PEN));
        hOldFont    = (HFONT)SelectObject(headerMemDC, fontContentStyle1);             //  store

        wsprintf(msg, TEXT("�Ʒ� ������ %d �� ����� �ŷ����� �Դϴ�."), BTC_getBlock_height);
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
        SetBkMode(contentMemDC, TRANSPARENT);   //  ��� �����ϰ�
        SetTextColor(contentMemDC, RGB(HYPERLINK_BLUE_R, HYPERLINK_BLUE_G, HYPERLINK_BLUE_B));
        temp_BTC_tx = BTC_tx;
        //  ��� �ŷ� ���� MemDC�� ����
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
        SetBkMode(hdc, TRANSPARENT);   //  ��� �����ϰ�
        if (selectedItem >= 0) {
            if (selectedItem < BTC_getBlock_nTx) { 
                temp    = WINDOW_HEADER_HEIGHT + ((selectedItem * ROW_OFFSET) - scr.nPos);
                rect    = {0, (temp + 1), WINDOW_WIDTH, (temp + ROW_OFFSET)};  // +1: ������ �������� ���� ���ϱ� ���� ����
                FillRect(hdc, &rect, CreateSolidBrush(RGB(LIGHT_BLUE_R, LIGHT_BLUE_G, LIGHT_BLUE_B)));
                MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, *(BTC_tx + selectedItem), strlen(*(BTC_tx + selectedItem)), convertCharToTCHAR, BUF_SIZE);
                wsprintf(msg, TEXT("%s"), convertCharToTCHAR);
                TextOut(hdc, (GET_BLOCK_COLUMN1_X_POS), (temp + CONTENT_PADDING), msg, lstrlen(msg));
                if (selectedItem == 0) {   //  check coinbase, selectedItem is (-1) when object not selected
                    TextOut(hdc, BTC_COINBASE_OFFSET, (temp + CONTENT_PADDING), TEXT("COINBASE"), lstrlen(TEXT("COINBASE")));;
                }
            }
        }
        bitbltResult1   = BitBlt(hdc, 0, 0, headerWidthSize, headerHeightSize, headerMemDC, 0, 0, SRCCOPY); //  hovering ��ġ�� ���� �����ϱ� ���� hovering �� bitblt
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
        rect = { 0, gOffset + 1, WINDOW_WIDTH, gOffset + offset};      // +1: ������ �������� ���� ���ϱ� ���� ����
        FillRect(hdc, &rect, CreateSolidBrush(RGB(LIGHT_GRAY_R, LIGHT_GRAY_G, LIGHT_GRAY_B)));
    }
    ++gRowNum;
    gOffset += CONTENT_PADDING;

    return;
}