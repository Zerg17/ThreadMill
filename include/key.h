#pragma once

#include "stdint.h"
#include "system.h"

#define KEY_NUM 12
#define IS_PRESSED_TICK 10
#define IS_LONG_PRESSED_TICK 250
#define IS_PRESSED_CLICK 10
#define IS_LONG_PRESSED_CLICK 250

typedef struct{
    uint8_t pressCnt;
    uint8_t isPressed:1;
    uint8_t isLongPressed:1;
    uint8_t isClick:1;
    uint8_t isLongClick:1;
} key_t;

extern key_t key[KEY_NUM];

void keyProc(void);
