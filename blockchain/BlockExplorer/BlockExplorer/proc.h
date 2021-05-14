#pragma once

#define MIN                         0
#define SCROLL_INTERVAL             4
#define SCROLL_PAGE_INTERVAL        (SCROLL_INTERVAL << 3)
#define SCROLL_MOVE_INTERVAL        (SCROLL_INTERVAL << 2)

#define FIELD_AREA_X_MIN    0
#define FIELD_AREA_X_MAX    GET_BLOCK_FIELD_WIDTH
#define BTC_TX_AREA_X_MIN   (GET_BLOCK_FIELD_WIDTH + 1)
#define BTC_TX_AREA_X_MAX   WINDOW_WIDTH
#define BTC_TX_AREA_Y_MIN   ((ROW_OFFSET * BTC_TX_INDEX) + WINDOW_HEADER_HEIGHT)
#define BTC_TX_AREA_Y_MAX   ((ROW_OFFSET * (BTC_TX_INDEX + 1)) + WINDOW_HEADER_HEIGHT)

void printGetBlock(HWND);
void printGetSerializedBlock(HWND);
void printTx(HWND);
void saveCurrScrollPos(int);
void selectObjectInGetBlock(HWND, LPARAM);
void selectObjectInTx(HWND, LPARAM);