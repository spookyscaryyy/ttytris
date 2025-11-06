#include <stdio.h>
#include <stdbool.h>

#include "termwrapper.h"

bool main_buffer = true;

void TSW_ScreenSwitchBuffer()
{
    if (main_buffer)
    {
        printf("\e[?1049h");
    }
    else
    {
        printf("\e[?1049l");
    }
    main_buffer = !main_buffer;
}

void TSW_ChangeFGColor(int color)
{
    printf("\e[38;5;%dm", color);
}

void TSW_ChangeBGColor(int color)
{
    printf("\e[48;5;%dm", color);
}

void TSW_ClearColor()
{
    printf("\e[m");
}

void TSW_ScreenClear()
{
    TSW_DrawText("\e[2J");
}

void TSW_DrawResetPen()
{
    TSW_DrawText("\e[;H");
}

void TSW_DrawHidePen()
{
    TSW_DrawText("\e[?25l");
}

void TSW_DrawShowPen()
{
    TSW_DrawText("\e[?25h");
}

void TSW_DrawBlock()
{
    // Blocks drawn using background color
    TSW_DrawText("  ");
}

void TSW_DrawSpacer()
{
    TSW_DrawText("  ");
}

void TSW_DrawGhost()
{
    TSW_DrawText("▒▒");
}

void TSW_DrawBoardVertical()
{

}

void TSW_DrawBoardHorizontal()
{
    TSW_DrawText("══");
}

void TSW_NextLine()
{
    TSW_DrawText("\e[20D\e[B");
}

void TSW_ShiftCursor(int row_s, int col_s)
{
    if (row_s < 0)
    {
        printf("\e[%dA", -1 * row_s);
    }
    else if (row_s > 0)
    {
        printf("\e[%dB", row_s);
    }
    if (col_s < 0)
    {
        printf("\e[%dD", -2 * col_s);
    }
    else if (col_s > 0)
    {
        printf("\e[%dC", 2 * col_s);
    }
}

void TSW_DrawTextYX(const char* text, int y, int x)
{
    printf("\e[%d;%dH%s", y, x, text);
}

void TSW_DrawText(const char* text)
{
    printf("%s", text);
}