#ifndef TERMWRAPPER_H_
#define TERMWRAPPER_H_

#include <stdint.h>

/* General Screen Controls */
void TSW_ScreenSwitchBuffer();

/* Screen Drawing */
// x and y are reversed since terminal escape codes use row,col
void TSW_DrawTextYX(const char* text, uint8_t y, uint8_t x);
void TSW_ChangeFGColor(uint8_t color);
void TSW_ChangeBGColor(uint8_t color);
void TSW_DrawText(const char* text);
void TSW_ScreenClear();
void TSW_DrawResetPen();
void TSW_DrawHidePen();
void TSW_DrawShowPen();
void TSW_DrawBlock();
void TSW_DrawSpacer();
void TSW_DrawGhost();
void TSW_DrawBoardVertical();
void TSW_DrawBoardHorizontal();
void TSW_ClearColor();
void TSW_NextLine();
void TSW_ShiftCursor(int8_t row_s, int8_t col_s);

/* Keyboard Input */

/* Threads and Signals */
void TSW_ThreadCreate();
void TSW_ThreadSignal();
void TSW_ThreadJoin();
void TSW_ThreadWait();

#endif