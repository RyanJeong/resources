#pragma once

#define HEADER_X_POS            10
#define HEADER_Y_POS            0
#define HEADER_FONT_SIZE        40
#define CONTENT_FONT_SIZE       24
#define CONTENT_PADDING         8       // ROW_OFFSET = CONTENT_FONT_SIZE + (CONTENT_PADDING * 2)

#define GET_BLOCK_COLUMN1_X_POS 16
#define GET_BLOCK_COLUMN1_WIDTH GET_BLOCK_FIELD_WIDTH
#define GET_BLOCK_COLUMN2_X_POS (GET_BLOCK_COLUMN1_X_POS + GET_BLOCK_COLUMN1_WIDTH)
#define GET_BLOCK_COLUMN2_WIDTH (WINDOW_HEIGHT - GET_BLOCK_COLUMN1_WIDTH)

#define GET_TX_COLUMN1_X_POS 16
#define GET_TX_COLUMN1_WIDTH GET_TX_VALUE_WIDTH
#define GET_TX_COLUMN2_X_POS (GET_TX_COLUMN1_X_POS + GET_TX_COLUMN1_WIDTH)
#define GET_TX_COLUMN2_WIDTH (WINDOW_HEIGHT - GET_TX_COLUMN1_WIDTH)

#define LIGHT_GRAY_R    0xF7
#define LIGHT_GRAY_G    0xF7
#define LIGHT_GRAY_B    0xF7

#define LIGHT_BLUE_R    0xAD
#define LIGHT_BLUE_G    0xD8
#define LIGHT_BLUE_B    0xE6

#define HYPERLINK_BLUE_R    0x06
#define HYPERLINK_BLUE_G    0x45
#define HYPERLINK_BLUE_B    0xAD

#define BLACK_R         0x00
#define BLACK_G         0x00
#define BLACK_B         0x00

#define COLUMN                      32
#define PRINT_HEX_X_START_POS       30
#define PRINT_HEX_X_OFFSET          30
#define PRINT_HEX_X_OFFSET1         10
#define PRINT_HEX_X_OFFSET2         20
#define PRINT_HEX_Y_START_POS       30
#define PRINT_HEX_Y_OFFSET          30

#define BTC_COINBASE_OFFSET         900
 void BTC_printGetBlock(HDC, HWND);
 void BTC_printGetSerializedBlock(HDC);
 void BTC_printTx(HDC, HWND);
 void drawRow(HDC, int);
