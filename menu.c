#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "menu.h"
#include "input.h"
#include "termwrapper.h"
#include "assets.h"

#define FLASH_FRAMES 3

static void flash_left();
static void flash_right();
static void clear_flash();

uint8_t menu_loop()
{
    static int8_t flash_cooldown = 0;
    if (flash_cooldown > 0)
    {
        flash_cooldown--;
    }
    else
    {
        clear_flash();
    }
    static uint8_t level_select = 1;
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
    char buf[2] = {level_select + '0', '\0'};
    TSW_DrawTextYX(buf, 18, 35);
    return 0;
}

bool loss_loop()
{
    return key_pressed(KEY_ENTER);
}

static void flash_left()
{
    char* save = NULL;
    char* cpy = malloc(strlen(arrow_fill)+1);
    memcpy(cpy, arrow_fill, strlen(arrow_fill)+1);
    char* token = strtok_r(cpy, "\x03", &save);

    TSW_ChangeBGColor(15);
    TSW_DrawTextYX(token, 16, 20);
    token = strtok_r(NULL, "\x03", &save);

    TSW_DrawTextYX(token, 17, 18);
    token = strtok_r(NULL, "\x03", &save);

    TSW_DrawTextYX(token, 18, 18);
    token = strtok_r(NULL, "\x03", &save);

    TSW_DrawTextYX(token, 19, 20);

    TSW_ClearColor();
    free(cpy);
    cpy = NULL;
}

static void flash_right()
{
    char* save = NULL;
    char* cpy = malloc(strlen(arrow_fill)+1);
    memcpy(cpy, arrow_fill, strlen(arrow_fill)+1);
    char* token = strtok_r(cpy, "\x03", &save);

    TSW_ChangeBGColor(15);
    TSW_DrawTextYX(token, 16, 50);
    token = strtok_r(NULL, "\x03", &save);

    TSW_DrawTextYX(token, 17, 50);
    token = strtok_r(NULL, "\x03", &save);

    TSW_DrawTextYX(token, 18, 50);
    token = strtok_r(NULL, "\x03", &save);

    TSW_DrawTextYX(token, 19, 50);

    TSW_ClearColor();
    free(cpy);
    cpy = NULL;
}

static void clear_flash()
{
    char* save = NULL;
    char* cpy = malloc(strlen(arrow_fill)+1);
    memcpy(cpy, arrow_fill, strlen(arrow_fill)+1);
    char* token = strtok_r(cpy, "\x03", &save);

    TSW_ChangeBGColor(0);
    TSW_DrawTextYX(token, 16, 20);
    token = strtok_r(NULL, "\x03", &save);

    TSW_DrawTextYX(token, 17, 18);
    token = strtok_r(NULL, "\x03", &save);

    TSW_DrawTextYX(token, 18, 18);
    token = strtok_r(NULL, "\x03", &save);

    TSW_DrawTextYX(token, 19, 20);
    TSW_ClearColor();

    save = NULL;
    memcpy(cpy, arrow_fill, strlen(arrow_fill)+1);
    token = strtok_r(cpy, "\x03", &save);

    TSW_ChangeBGColor(0);
    TSW_DrawTextYX(token, 16, 50);
    token = strtok_r(NULL, "\x03", &save);

    TSW_DrawTextYX(token, 17, 50);
    token = strtok_r(NULL, "\x03", &save);

    TSW_DrawTextYX(token, 18, 50);
    token = strtok_r(NULL, "\x03", &save);

    TSW_DrawTextYX(token, 19, 50);

    TSW_ClearColor();

    free(cpy);
    cpy = NULL;
}

void draw_menuscreen()
{
    TSW_DrawText(menuscreen);
}

void draw_lossscreen()
{
    char* save = NULL;
    char* cpy = malloc(strlen(lossscreen)+1);
    memcpy(cpy, lossscreen, strlen(lossscreen)+1);
    char* token = strtok_r(cpy, "\x03", &save);
    uint8_t i = 11;
    while (token != NULL)
    {
        TSW_DrawTextYX(token, i++, 15);
        token = strtok_r(NULL, "\x03", &save);
    }
    free(cpy);
    cpy = NULL;
}