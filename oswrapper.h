#ifndef OSWRAPPER_H_
#define OSWRAPPER_H_

/* General Screen Controls */
void OSW_ScreenClear();
void OSW_ScreenSwitchBuffer();

/* Screen Drawing */
void OSW_DrawResetPen();
void OSW_DrawHidePen();
void OSW_DrawShowPen();
void OSW_DrawBlock();
void OSW_DrawGhost();
void OSW_DrawSpacer();

/* Keyboard Input */

/* Threads and Signals */
void OSW_ThreadCreate();
void OSW_ThreadSignal();
void OSW_ThreadJoin();
void OSW_ThreadWait();

#endif