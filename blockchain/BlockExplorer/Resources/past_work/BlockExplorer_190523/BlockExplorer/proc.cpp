#include "BlockExplorer.h"
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

extern HINSTANCE hInst;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    /*
     *  ���� ���� ���Ǵ� �޽����� ����
     *      WM_COMMAND      : ���� ���α׷� �޴��� ó���Ѵ�.
     *      WM_QUIT         : ���α׷��� ���� �� �߻��ϴ� �޽����̴�.
     *      WM_LBUTTONDOWN  : ���콺�� ���� ��ư�� ���� ��� �߻��Ѵ�.
     *      WM_CHAR         : Ű����κ��� ���ڰ� �Էµ� �� �߻��Ѵ�
     *      WM_PAINT        : ȭ���� �ٽ� �׷��� �� �ʿ䰡 ���� �� �߻��Ѵ�.
     *      WM_DESTROY      : �����찡 �޸𸮿��� �ı��� �� �߻��Ѵ�.
     *      WM_CREATE       : �����찡 ó�� ������� �� �߻��Ѵ�.
     *
     *      BOOL InvlidateRect(HWND hWnd, CONST RECT *lpRect, BOOL bErase); 
     *          �� �Լ��� �������� �۾� ������ ��ȿȭ���� ��������� �Ͽ��� WM_PAINT �޽����� �ش� ������� �������� �Ѵ�. 
     *
     *          ù��° �μ� hWnd�� ��ȿȭ�� ����� �Ǵ� ������, �� �ٽ� �׷����� �� �������� �ڵ��̴�. 
     *          �� ���� WndProc�� ȣ��� �� ���޵Ǵ� ù��° �μ� hWnd�� �״�� �� �ָ� �ȴ�.
     *
     *          �ι�° �μ� lpRect�� ��ȿȭ�� ����� �Ǵ� �簢 ������ �� �ֵ� �� ���� NULL�̸� �������� �� ������ ��ȿȭ�ȴ�. 
     *          �� ������ ��ȿȭ�ϸ� �����찡 ������ �ٽ� �׷����Ƿ� Ȯ���ϰ� �׷������ �ϰ����� �׸�ŭ ����ӵ��� �ʾ����� �ȴ�. 
     *          �ӵ��� �ִ��� ���̷��� ����� �ּ����� �������� ����Ͽ� ��ȿȭ�ϴ� ���� ����. 
     *          
     *          ����° �μ� bErase�� ��ȿȭ�Ǳ� ���� ����� ��� ���� �� �ٽ� �׸� ������ �ƴϸ� ����� ������ �ʰ� �׸� �������� �����Ѵ�. 
     *          �� ���� TRUE�̸� ����� ���� �� �ٽ� �׸��� FALSE�̸� ����� ������ ����ä�� �ٽ� �׸���. 
     */

    switch (message) {
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDM_HELP_ABOUT:
                    DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                    
                    return 0;
                case IDM_FILE_EXIT:
                    DestroyWindow(hWnd);
                    
                    return 0;
                case IDM_COIN_BITCOIN:
                    /*  Do something... */
                    
                    return 0;
                case IDM_COIN_ETHEREUM:
                    /*  Do something... */
                    
                    return 0;
                case IDM_BLOCK:
                    getBestBlockHash();
                    InvalidateRect(hWnd, NULL, TRUE);

                    return 0;
                case IDM_TX:
                    /*  Do something... */

                    return 0;
                case IDM_BLOCKCHAIN:
                    getBlockchainInfo();
                    InvalidateRect(hWnd, NULL, TRUE);

                    return 0;
                case IDM_MEMORYPOOL:
                    getMemPoolInfo();
                    InvalidateRect(hWnd, NULL, TRUE);

                    return 0;
                case IDM_MINING:
                    getMiningInfo();
                    InvalidateRect(hWnd, NULL, TRUE);

                    return 0;
                case IDM_NETWORK:
                    getNetworkInfo();
                    InvalidateRect(hWnd, NULL, TRUE);

                    return 0;
                case IDM_WALLET:
                    getWalletInfo();
                    InvalidateRect(hWnd, NULL, TRUE);

                    return 0;
                default:

                    return DefWindowProc(hWnd, message, wParam, lParam);
            }
        case WM_PAINT:
            Wm_Paint(hWnd);

            return 0;
        case WM_CLOSE:
            DestroyWindow(hWnd);

            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);

            return 0;
        default:
            /*
             *  DefWindowProc(): WinProc�� �Լ� ���ڰ� �����ϸ�, �ý��� ĸ���� ������ �� ������ �޴��� �ּ�ȭ, �ִ�ȭ, ������ �̵�, ũ�� ���� ����
             *  ���� ó�������� �ʾƵ� �ü���� �˾Ƽ� ó�����ִ� ������ ����
             */

            return DefWindowProc(hWnd, message, wParam, lParam);
    }
}