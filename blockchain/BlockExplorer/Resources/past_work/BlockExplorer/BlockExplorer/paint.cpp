#include "common.h"

extern LPCWSTR Text;

void Wm_Paint(HWND hWnd)
{
    HDC				hdc;
    PAINTSTRUCT		ps;
    RECT			rt;
    HFONT			font, oldfont;

    hdc = BeginPaint(hWnd, &ps);
    font = CreateFont(
        30,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        ANSI_CHARSET,
        3,
        2,
        1,
        VARIABLE_PITCH | FF_ROMAN, NULL);

    oldfont = (HFONT) SelectObject(hdc, font);

    SetTextColor(hdc, RGB(0, 0, 59));

    SetRect(&rt, 200, 250, 1200, 1200);

    DrawText(
        hdc,
        Text,
        -1,
        &rt,
        DT_CENTER | DT_WORDBREAK);

    SelectObject(hdc, oldfont);

    DeleteObject(font);

    EndPaint(hWnd, &ps);
}