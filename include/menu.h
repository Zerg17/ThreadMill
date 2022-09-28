#pragma once

#include "stdint.h"

typedef struct {
    uint8_t thread;
    uint8_t torque;
    int8_t speed;
    uint8_t pitch;
    uint8_t depth;
} tap_settings;



void procMenu(void);