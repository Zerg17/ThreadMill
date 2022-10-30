#include "key.h"
#include "sysControl.h"

#define ENC_INVERT

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

void TIM14_IRQHandler() {
    TIM14->SR = 0; 

    static uint8_t lastA, lastB, tmpA, tmpB, cntA, cntB, valA, valB;
    uint8_t A = ((GPIOB->IDR) & (1 << 0)) != 0;
    uint8_t B = ((GPIOB->IDR) & (1 << 1)) != 0;

    if(A!=tmpA){
        tmpA=A;
        cntA=0;
    }else{
        cntA++;
        if(cntA>50){
            valA=A;
            cntA=0;
        }
    }

    if(B!=tmpB){
        tmpB=B;
        cntB=0;
    }else{
        cntB++;
        if(cntB>50){
            valB=B;
            cntB=0;
        }
    }

    if(lastA != valA){
        if(valA) encPos -= valA != valB ? 1 : -1;
        lastA = valA;
    }
    if(lastB != valB){
        if(valB) encPos -= valA != valB ? -1 : 1;
        lastB = valB;
    }
}

void resetAllKeys(void){
    for (uint8_t i = 0; i < KEY_NUM; i++) 
        *(uint16_t*)(&key[i])=0;
}

volatile int32_t encPos;

void EXTI0_1_IRQHandler(void) {
    uint8_t A = ((GPIOB->IDR) & (1 << 0)) != 0;
    uint8_t B = ((GPIOB->IDR) & (1 << 1)) != 0;
    if (EXTI->PR & EXTI_PR_PR0) {
        EXTI->PR = EXTI_PR_PR0;
        #ifdef ENC_INVERT
        encPos -= A != B ? 1 : -1;
        #else
        encPos += A != B ? 1 : -1;
        #endif
    }
}

uint32_t keyBklStat;

void keyBklSet(uint8_t key, uint8_t state) {
    keyBklStat=
        (keyBklStat&(~(0b11<<(key*2))))|((state&0b11)<<(key*2));
}

void keyBklProc(void){
    uint16_t d=0;
    for(uint8_t i=0; i<16; i++){ 
        uint8_t m = keyBklStat>>i*2&0b11;
        switch(m){
            case 1: d|=1<<i; break;
            case 2: d|=(tick/40%2)<<i; break;
            case 3: d|=(tick/20%2)<<i; break;
        }
    }
    SPI2->DR = d;
    while ((SPI2->SR & SPI_SR_BSY));
    GPIOB->BSRR = GPIO_BSRR_BS_14;
    for (volatile uint32_t i = 0; i < 10; i++);
    GPIOB->BSRR = GPIO_BSRR_BR_14;
}
