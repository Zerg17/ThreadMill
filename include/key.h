#pragma once

#include "stdint.h"
#include "system.h"

#define KEY_NUM 12
#define IS_PRESSED_TICK 10
#define IS_LONG_PRESSED_TICK 250
#define IS_PRESSED_CLICK 10
#define IS_LONG_PRESSED_CLICK 250

#define KNOB_KEY                       0
#define TOP_LEFT_KEY                   7
#define TOP_MIDDLE_KEY                 6
#define TOP_RIGHT_KEY                  5
#define BOT_LEFT_KEY                   9
#define BOT_MIDDLE_KEY                 10
#define BOT_RIGHT_KEY                  11
#define SAVE_TO_EEPROM_KEY             1

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

/// @brief Статус кнопок
typedef struct {
    uint8_t pressCnt;           // Счетчик времени нажатия
    uint8_t isPressed : 1;      // Нажата
    uint8_t isLongPressed : 1;  // Долго нажата
    uint8_t isClick : 1;        // Было нажатие
    uint8_t isLongClick : 1;    // Было долгое нажатие
} key_t;
extern volatile key_t key[KEY_NUM];

extern volatile int32_t encPos; // Положение энкодера

/// @brief Процесс обработки нажатия кнопок
void keyProc(void);

/// @brief Задать состояние подсветки кнопки или доп оборудования
/// @param key Номер кнопки
/// @param state Состояние (0 - не горит, 1 - горит, 2 - моргает, 3 - быстро моргает)
void keyBklSet(uint8_t key, uint8_t state);

/// @brief Процесс обработки подсветки кнопок и доп оборудования
void keyBklProc(void);

/// @brief Сбрасывает состояние всех кнопок до начального
void resetAllKeys(void);
