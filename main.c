#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <semaphore.h>
#include <errno.h>

#include "menu.h"
#include "game.h"
#include "input.h"
#include "termwrapper.h"

sem_t input_ready;

typedef enum screen_state_impl
{
    MENU_STATE = 0,
    GAME_STATE = 1,
    LOSS_STATE = 2,
    MENU_TO_GAME = 3,
    GAME_TO_LOSS = 4,
    LOSS_TO_MENU = 5
} screen_state;

static void sig_handler(int sig, siginfo_t* info, void* ucontext);
static void main_shutdown();
static void main_init();

typedef struct main_state_impl
{
    pthread_t input_thread;
    screen_state screen;
    uint8_t level_select;
} main_state;
static main_state state;

static void main_shutdown()
{
    input_shutdown();
    pthread_join(state.input_thread, NULL);
    game_shutdown();
    TSW_ScreenClear();
    TSW_DrawShowPen();
    TSW_ScreenSwitchBuffer();
}

static void main_init()
{
    if (-1 == sem_init(&input_ready, 0, 0))
    {
        perror("sem_init");
        abort();
    }

    pthread_create(&state.input_thread, NULL, &input_thread_func, NULL);

    // wait for input to finish setup
    while(sem_wait(&input_ready) == -1 && errno == EINTR)
    {
        continue;
    }

    setvbuf(stdout, NULL, _IOFBF, 1024);

    // Signal handling
    struct sigaction act = {0};
    act.sa_sigaction = &sig_handler;
    act.sa_flags |= SA_SIGINFO;
    sigemptyset(&act.sa_mask);
    if (sigaction(SIGINT, &act, NULL) == -1)
    {
        perror("sigaction");
        abort();
    }
    if (sigaction(SIGABRT, &act, NULL) == -1)
    {
        perror("sigaction");
        abort();
    }
    if (sigaction(SIGSEGV, &act, NULL) == -1)
    {
        perror("sigaction");
        abort();
    }

    state.screen = LOSS_TO_MENU;

    TSW_ScreenSwitchBuffer();
    TSW_ScreenClear();
    TSW_DrawResetPen();
    TSW_DrawHidePen();
}

static void sig_handler(int sig, siginfo_t* info, void* ucontext)
{
    main_shutdown();
    exit(info->si_status);
}

static void main_loop()
{
    // setup framerate limiter
    struct timespec start_time;
    struct timespec delta_time;
    struct timespec sleep_time;
    clock_getres(CLOCK_TAI, &start_time);
    clock_getres(CLOCK_TAI, &delta_time);
    clock_getres(CLOCK_TAI, &sleep_time);
    while (!key_pressed(KEY_ESC))
    {
        if (clock_gettime(CLOCK_TAI, &start_time) == -1)
        {
            perror("clock_gettime");
        }

        // send input frame update signal
        pthread_kill(state.input_thread, SIGUSR1);

        switch(state.screen)
        {
            case(MENU_STATE):
            state.level_select = menu_loop();
            if (state.level_select > 0)
            {
                state.screen = MENU_TO_GAME;
            }
            break;
            case(GAME_STATE):
            if (game_loop())
            {
                game_shutdown();
                state.screen = GAME_TO_LOSS;
            }
            break;
            case(LOSS_STATE):
            if (loss_loop())
            {
                state.screen = LOSS_TO_MENU;
            }
            break;
            case(MENU_TO_GAME):
            TSW_ScreenClear();
            TSW_DrawResetPen();
            draw_gamescreen();
            game_init(state.level_select);
            state.screen = GAME_STATE;
            break;
            case(GAME_TO_LOSS):
            draw_lossscreen();
            state.screen = LOSS_STATE;
            break;
            case(LOSS_TO_MENU):
            TSW_ScreenClear();
            TSW_DrawResetPen();
            draw_menuscreen();
            state.screen = MENU_STATE;
            break;
            default:
            break;
        }
        fflush(NULL);

        if (clock_gettime(CLOCK_TAI, &delta_time) == -1)
        {
            perror("clock_gettime");
        }
        sleep_time.tv_nsec = FRAMETIME_GOAL - (delta_time.tv_nsec - start_time.tv_nsec);
        if (clock_nanosleep(CLOCK_TAI, 0, &sleep_time, NULL) == -1)
        {
            perror("nanosleep");
        }
    }
}

int main(int ac, char** av)
{
    main_init();
    main_loop();
    main_shutdown();
    return 0;
}
