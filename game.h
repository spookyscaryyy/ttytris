#ifndef TERMGAME_H
#define TERMGAME_H

#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#define SCREEN_FD STDOUT_FILENO
#define FRAMETIME_GOAL 16666666

void game_init(int level_select);
bool game_loop();
void game_shutdown();
void draw_gamescreen();

#endif
