#ifndef INPUT_H_
#define INPUT_H_

#include <unistd.h>
#include <stdbool.h>
#include <linux/input-event-codes.h>

#define TERM_FD STDIN_FILENO

#define KEYBOARD_PATH "/dev/input/event9"

#define KEY_COUNT 250
#define PRESSED 1
#define RELEASED 0

void* input_thread_func();
void input_init();
void input_shutdown();
void input_poll();

bool key_pressed(unsigned short key);
bool key_held(unsigned short key);

#endif
