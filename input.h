#ifndef INPUT_H_
#define INPUT_H_

#include <unistd.h>
#include <stdbool.h>
#include <linux/input-event-codes.h>

#define TERM_FD STDIN_FILENO

#define DEFAULT_EVENT_NUM 9
#define KEYBOARD_PATH "/dev/input/event"
#define KEYBOARD_PATH_MAX 64

#define KEY_COUNT 250
#define PRESSED 1
#define RELEASED 0

void* input_thread_func(void *eventnum);
void input_init(int eventnum);
void input_shutdown();
void input_poll();

bool key_pressed(unsigned short key);
bool key_held(unsigned short key);

#endif
