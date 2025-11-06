#ifndef TERMGAME_H
#define TERMGAME_H

#include <stdbool.h>
#include <stdint.h>

#define SCREEN_FD STDOUT_FILENO
#define FRAMETIME_GOAL 16666666

void game_init(uint8_t level_select);
bool game_loop();
void game_shutdown();
void draw_gamescreen();

#endif
