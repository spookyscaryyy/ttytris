#ifndef TERMWRAPPER_H_
#define TERMWRAPPER_H_

/* General Screen Controls */
void TSW_ScreenSwitchBuffer();

/* Screen Drawing */
// x and y are reversed since terminal escape codes use row,col
void TSW_DrawTextYX(const char* text, int y, int x);
void TSW_ChangeFGColor(int color);
void TSW_ChangeBGColor(int color);
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
void TSW_ShiftCursor(int row_s, int col_s);

/* Keyboard Input */

/* Threads and Signals */
void TSW_ThreadCreate();
void TSW_ThreadSignal();
void TSW_ThreadJoin();
void TSW_ThreadWait();

#endif