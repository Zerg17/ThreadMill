#pragma once 

#include "stdint.h"

#define LCD_EN (1<<2)
#define LCD_D4 (1<<3)
#define LCD_D5 (1<<4)
#define LCD_D6 (1<<5)
#define LCD_D7 (1<<6)
#define LCD_RS (1)
#define LCD_BKL (1<<7)

void lcdInit();
void lcdWrite(uint8_t d);
void lcdSetPos(uint8_t x, uint8_t y);
void lcdChar(unsigned char c);
void lcdClear();