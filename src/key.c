#include "key.h"

static const struct {
    GPIO_TypeDef* port;
    uint16_t pin;
} gpioKeys[KEY_NUM] = {
    {GPIOB, 1 << 2},
    {GPIOA, 1 << 7},
    {GPIOA, 1 << 6},
    {GPIOA, 1 << 5},
    {GPIOA, 1 << 4},
    {GPIOA, 1 << 1},
    {GPIOA, 1 << 0},
    {GPIOA, 1 << 15},
    {GPIOF, 1 << 7},
    {GPIOF, 1 << 6},
    {GPIOA, 1 << 11},
    {GPIOA, 1 << 8}};

volatile key_t key[KEY_NUM];

void keyProc(void) {
    for (uint8_t i = 0; i < KEY_NUM; i++) {
        uint8_t k = (gpioKeys[i].port->IDR & gpioKeys[i].pin) == 0;
        if (k) {
            if (key[i].pressCnt != 255) key[i].pressCnt++;
            if (key[i].pressCnt > IS_PRESSED_TICK) key[i].isPressed = 1;
            if (key[i].pressCnt > IS_LONG_PRESSED_TICK) key[i].isLongPressed = 1;
        } else {
            if (key[i].pressCnt > IS_PRESSED_CLICK) key[i].isClick = 1;
            if (key[i].pressCnt > IS_LONG_PRESSED_CLICK) key[i].isLongClick = 1;
            key[i].isPressed = 0;
            key[i].isLongPressed = 0;
            key[i].pressCnt = 0;
        }
    }
}

volatile int32_t encPos;
volatile uint8_t debounce;

void EXTI0_1_IRQHandler() {
    // static int32_t ept;
    // uint8_t A = ((GPIOB->IDR) & (1 << 0)) != 0;
    // uint8_t B = ((GPIOB->IDR) & (1 << 1)) != 0;
    // if (EXTI->PR & EXTI_PR_PR0) {
    //     EXTI->PR = EXTI_PR_PR0;
    //     ept += A != B ? 1 : -1;
    // }
    // if (EXTI->PR & EXTI_PR_PR1) {
    //     EXTI->PR = EXTI_PR_PR1;
    //     //ept += A == B ? 1 : -1;
    // }
    // encPos=ept/2;
    uint8_t A = ((GPIOB->IDR) & (1 << 0)) != 0;
    uint8_t B = ((GPIOB->IDR) & (1 << 1)) != 0;

    if (EXTI->PR & EXTI_PR_PR0) {
        EXTI->PR = EXTI_PR_PR0;
        if (!debounce) {
            encPos++;
            debounce = 1;
        } else if ((A == 1) && (B == 1))
            debounce = 0;
    }

    if (EXTI->PR & EXTI_PR_PR1) {
        EXTI->PR = EXTI_PR_PR1;
        if (!debounce) {
            encPos--;
            debounce = 1;
        } else if ((A == 1) && (B == 1))
            debounce = 0;
    }
}

// 00 01 11 10
// 00 10 11 01

uint16_t muxState = 0;

void keyBklSet(uint8_t key, uint8_t state) {
    // *(uint8_t*)&(SPI1->DR) = d;
    // while ((SPI1->SR & SPI_SR_BSY))
    //     ;
    // GPIOB->BSRR = GPIO_BSRR_BS_4;
    // for (volatile uint32_t i = 0; i < 10; i++)
    //     ;
    // GPIOB->BSRR = GPIO_BSRR_BR_4;
}

void keyBklSetAll(uint8_t state) {
}
