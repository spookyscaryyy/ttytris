#include <stdio.h>
#include <wchar.h>
#include <stdbool.h>

#include "menu.h"
#include "input.h"

#define FLASH_FRAMES 3

static void flash_left();
static void flash_right();
static void clear_flash();

int menu_loop()
{
    static int flash_cooldown = 0;
    if (flash_cooldown > 0)
    {
        flash_cooldown--;
    }
    else
    {
        clear_flash();
    }
    static int level_select = 1;
    if (key_pressed(KEY_ENTER))
    {
        return level_select;
    }
    if (key_pressed(KEY_LEFT))
    {
        if (level_select > 1)
        {
            level_select--;
            flash_left();
            flash_cooldown = FLASH_FRAMES;
        }
    }
    if (key_pressed(KEY_RIGHT))
    {
        if (level_select < 9)
        {
            level_select++;
            flash_right();
            flash_cooldown = FLASH_FRAMES;
        }
    }
    wprintf(L"\e[29;35H%d", level_select);
    return 0;
}

bool loss_loop()
{
    return key_pressed(KEY_ENTER);
}

static void flash_left()
{
    wprintf(L"\e[27;24H\e[48;5;15m \e[m");
    wprintf(L"\e[28;22H\e[48;5;15m   \e[m");
    wprintf(L"\e[29;22H\e[48;5;15m   \e[m");
    wprintf(L"\e[30;24H\e[48;5;15m \e[m");
}

static void flash_right()
{
    wprintf(L"\e[27;45H\e[48;5;15m \e[m");
    wprintf(L"\e[28;45H\e[48;5;15m   \e[m");
    wprintf(L"\e[29;45H\e[48;5;15m   \e[m");
    wprintf(L"\e[30;45H\e[48;5;15m \e[m");
}

static void clear_flash()
{
    wprintf(L"\e[27;24H\e[48;5;0m \e[m");
    wprintf(L"\e[28;22H\e[48;5;0m   \e[m");
    wprintf(L"\e[29;22H\e[48;5;0m   \e[m");
    wprintf(L"\e[30;24H\e[48;5;0m \e[m");

    wprintf(L"\e[27;45H\e[48;5;0m \e[m");
    wprintf(L"\e[28;45H\e[48;5;0m   \e[m");
    wprintf(L"\e[29;45H\e[48;5;0m   \e[m");
    wprintf(L"\e[30;45H\e[48;5;0m \e[m");
}

void draw_menuscreen()
{
    wprintf(L"\n"
            L"\n"
            L"\n"
            L"    █▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀█\n"
            L"    █                                                           █\n"
            L"    █  ┏━━━━━━━┓ ┏━━━━━┓ ┏━━━━━━━┓ ┏━━━━━━━┓ ┏━━━━━━━┓ ┏━━━━━┓  █\n"
            L"    █  ┗━━┓ ┏━━┛ ┃ ┏━━━┛ ┗━━┓ ┏━━┛ ┃ ┏━━━┓ ┃ ┗━━┓ ┏━━┛ ┃ ┏━━━┛  █\n"
            L"    █     ┃ ┃    ┃ ┗━━━┓    ┃ ┃    ┃ ┗━━━┛ ┃    ┃ ┃    ┃ ┗━━━┓  █\n"
            L"    █     ┃ ┃    ┃ ┏━━━┛    ┃ ┃    ┃ ┏━┓  ┏┛    ┃ ┃    ┗━━━┓ ┃  █\n"
            L"    █     ┃ ┃    ┃ ┗━━━┓    ┃ ┃    ┃ ┃ ┗┓ ┗┓ ┏━━┛ ┗━━┓ ┏━━━┛ ┃  █\n"
            L"    █     ┗━┛    ┗━━━━━┛    ┗━┛    ┗━┛  ┗━━┛ ┗━━━━━━━┛ ┗━━━━━┛  █\n"
            L"    █                                                           █\n"
            L"    █▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄                   ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█\n"
            L"                        █                   █\n"
            L"                        █                   █\n"
            L"                        █                   █\n"
            L"                        █                   █\n"
            L"                        █                   █\n"
            L"                        █                   █\n"
            L"                        █                   █\n"
            L"                        █                   █\n"
            L"                        █                   █\n"
            L"                        ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀\n"
            L"                                             \n"
            L"                                             \n"
            L"                      ┌─┐  ┌─────────────┐ ┌─┐     \n"
            L"                    ┌─┘ │  │             │ │ └─┐   \n"
            L"                  ┌─┘   │  │    LEVEL    │ │   └─┐  \n"
            L"                  └─┐   │  │      9      │ │   ┌─┘  \n"
            L"                    └─┐ │  │             │ │ ┌─┘   \n"
            L"                      └─┘  └─────────────┘ └─┘     \n"
            L"\n");
}

void draw_lossscreen()
{
    wprintf(L"\e[11;15H┌──────────────┐\n"
            L"\e[12;15H│  GAME  OVER  │\n"
            L"\e[13;15H│              │\n"
            L"\e[14;15H│ PRESS  ENTER │\n"
            L"\e[15;15H└──────────────┘\n");
}