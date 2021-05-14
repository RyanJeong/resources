#include "common.h"

/*
 *  typedef struct tagMSG {
 *      HWND        hwnd;
 *      UINT        message;
 *      WPARAM      wParam;
 *      LPARAM      lParam;
 *      DWORD       time;
 *      POINT       pt;
 *  } MSG;
 * 
 *      hwnd    : �޽����� ���� ������ �ڵ��̴�.
 *      message : � ������ �޽����ΰ��� ��Ÿ����. ���� �߿��� ���̴�.
 *      wParam  : ���޵� �޽����� ���� �ΰ����� ������ ������. � �ǹ̸� �����°��� �޽������� �ٸ���. 32��Ʈ���̴�.
 *      lParam  : ���޵� �޽����� ���� �ΰ����� ������ ������. � �ǹ̸� �����°��� �޽������� �ٸ���. 32��Ʈ���̴�.
 *      time    : �޽����� �߻��� �ð��̴�.
 *      pt      : �޽����� �߻����� ���� ���콺 ��ġ�̴�.
 */

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    /*
     *  ���� ���� ���Ǵ� �޽����� ����   
     *      WM_QUIT         : ���α׷��� ���� �� �߻��ϴ� �޽����̴�.
     *      WM_LBUTTONDOWN  : ���콺�� ���� ��ư�� ���� ��� �߻��Ѵ�.
     *      WM_CHAR         : Ű����κ��� ���ڰ� �Էµ� �� �߻��Ѵ�
     *      WM_PAINT        : ȭ���� �ٽ� �׷��� �� �ʿ䰡 ���� �� �߻��Ѵ�.
     *      WM_DESTROY      : �����찡 �޸𸮿��� �ı��� �� �߻��Ѵ�.
     *      WM_CREATE       : �����찡 ó�� ������� �� �߻��Ѵ�.
     */
    switch (iMessage) {
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case M_FILE:
                    /*  Do something ...    */
                    break;
                case COINS_BITCOIN:
                    /*  Do something ...    */
                    break;
                case COINS_ETHEREUM:
                    /*  Do something ...    */
                    break;
                case COINS_EXIT:
                    PostMessage(
                        hWnd,
                        WM_CLOSE,
                        0,
                        0);
                    break;
                case M_BLOCK:
                    //getBlockHash();
                    break;
                case M_ABOUT:
                    MessageBox(
                        hWnd,
                        TEXT("CONTENT"),
                        TEXT("TITLE"),
                        MB_OK);
                    break;
                case M_EXIT:
                    PostMessage(
                        hWnd,
                        WM_CLOSE,
                        0,
                        0);
                    break;
            }
            break;
        case WM_PAINT:
            Wm_Paint(hWnd);
            break;
        case WM_CLOSE: 
            DestroyWindow(hWnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);

            return 0;
        default:
            break;
    }

    /*
     *  DefWindowProc(): WinProc�� �Լ� ���ڰ� �����ϸ�, �ý��� ĸ���� ������ �� ������ �޴��� �ּ�ȭ, �ִ�ȭ, ������ �̵�, ũ�� ���� ���� 
     *  ���� ó�������� �ʾƵ� �ü���� �˾Ƽ� ó�����ִ� ������ ����
     */
    return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}