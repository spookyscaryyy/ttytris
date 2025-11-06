#include "input.h"

#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <linux/input-event-codes.h>
#include <stdatomic.h>
#include <signal.h>
#include <string.h>
#include <wchar.h>
#include <semaphore.h>

extern sem_t input_ready;

typedef struct input_state_impl
{
   struct termios term_restore;
   FILE* input_fd;
   bool keys[KEY_COUNT];
   bool keys_prev[KEY_COUNT];
   atomic_bool looping;
} input_state;
static input_state state;

typedef struct in_event_impl
{
    struct timeval time;
    unsigned short type;
    unsigned short code;
    unsigned int value;
} in_event;

static void frame_passed(int sig, siginfo_t* info, void* ucontext)
{
    memcpy(state.keys_prev, state.keys, KEY_COUNT);
}

void* input_thread_func()
{
    input_init();
    if (-1 == sem_post(&input_ready))
    {
        perror("sem_post");
        abort();
    }
    input_poll();
    return NULL;
}

void input_init()
{
    // setup frame updater
    struct sigaction act = {0};
    act.sa_sigaction = &frame_passed;
    sigemptyset(&act.sa_mask);
    if (sigaction(SIGUSR1, &act, NULL) == -1)
    {
        perror("sigaction");
        abort();
    }

    // setup terminal
    if (!isatty(TERM_FD))
    {
        perror("Terminal Check");
        abort();
    }
    struct termios term_settings;
    if (tcgetattr(TERM_FD, &term_settings) < 0)
    {
        perror("Terminal Gather");
        abort();
    }
    state.term_restore = term_settings;

    term_settings.c_lflag &= ~(ECHO | ICANON);

    if (tcsetattr(TERM_FD, TCSANOW, &term_settings) < 0)
    {
        perror("Terminal Set");
        abort();
    }

    // setup keyboard input
    state.input_fd = fopen(KEYBOARD_PATH, "r");
    if (state.input_fd == NULL)
    {
        perror("Keyboard Open");
        abort();
    }
    for (int i = 0; i < KEY_COUNT; i++)
    {
        state.keys[i] = false;
        state.keys_prev[i] = false;
    }
    state.looping = true;
}

void input_shutdown()
{
    tcsetattr(TERM_FD, TCSAFLUSH, &state.term_restore);
    fclose(state.input_fd);
    state.looping = false;
}

void input_poll()
{
    in_event event;
    while(state.looping)
    {
        read(fileno(state.input_fd), &event, sizeof(event));
        if (event.type == EV_KEY && event.value == PRESSED)
        {
            state.keys[event.code] = true;
        }
        if (event.type == EV_KEY && event.value == RELEASED)
        {
            state.keys[event.code] = false;
        }
    }
}

bool key_pressed(unsigned short key)
{
    return state.keys[key] && !state.keys_prev[key];
}

bool key_held(unsigned short key)
{
    return state.keys[key];
}