#pragma once

#include "stdint.h"
#include "system.h"

#define KEY_NUM 12
#define IS_PRESSED_TICK 10
#define IS_LONG_PRESSED_TICK 250
#define IS_PRESSED_CLICK 10
#define IS_LONG_PRESSED_CLICK 250

#define BKL1 15
#define BKL2 14
#define BKL3 13
#define BKL4 12
#define BKL5 11
#define BKL6 10
#define BKL7 9
#define BKL8 7
#define BKL9 6
#define BKL10 5
#define BKL11 4
#define LASER1 3
#define LASER2 2

typedef struct {
    uint8_t pressCnt;
    uint8_t isPressed : 1;
    uint8_t isLongPressed : 1;
    uint8_t isClick : 1;
    uint8_t isLongClick : 1;
} key_t;

uint8_t lastA;
uint8_t lastB;

extern volatile key_t key[KEY_NUM];
extern volatile int32_t encPos;

void keyProc(void);
void keyBklSet(uint8_t key, uint8_t state);
void keyBklProc(void);
