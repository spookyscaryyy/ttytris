#ifndef TERMGAME_H
#define TERMGAME_H

#include <unistd.h>
#include <wchar.h>
#include <pthread.h>

#define SCREEN_FD STDOUT_FILENO
#define FRAMETIME_GOAL 16666666
#define ERASE_SCREEN L"\e[2J"
#define RESET_CURSOR L"\e[;H"

#define HIDE_CURSOR L"\e[?25l"
#define SHOW_CURSOR L"\e[?25h"

#define ORIG_BUFFER L"\e[?1049l"
#define ALT_BUFFER L"\e[?1049h"

void game_init(int level_select);
bool game_loop();
void game_shutdown();
void draw_gamescreen();

#endif
