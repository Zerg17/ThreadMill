#pragma once

#include "stm32f0xx.h"
#include "xprintf.h"

#define BAUD1 19200
#define BAUD2 500000

void sysInit();
void uart2Write(uint8_t d);
void tim17pwm(uint16_t p, uint16_t a);
