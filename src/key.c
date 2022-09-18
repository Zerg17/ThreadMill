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
    {GPIOA, 1 << 8}
};

volatile key_t key[KEY_NUM];

void keyProc(void){
    for(uint8_t i=0; i<KEY_NUM; i++){
        uint8_t k = (gpioKeys[i].port->IDR & gpioKeys[i].pin) == 0;
        if(k){
            if(key[i].pressCnt!=255)key[i].pressCnt++;
            if(key[i].pressCnt>IS_PRESSED_TICK)key[i].isPressed=1;
            if(key[i].pressCnt>IS_LONG_PRESSED_TICK)key[i].isLongPressed=1;
        }else{
            if(key[i].pressCnt>IS_PRESSED_CLICK)key[i].isClick=1;
            if(key[i].pressCnt>IS_LONG_PRESSED_CLICK)key[i].isLongClick=1;
            key[i].isPressed=0;
            key[i].isLongPressed=0;
            key[i].pressCnt=0;
        }
    }
}
