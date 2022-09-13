#include "key.h"

static const struct {
    GPIO_TypeDef* port;
    uint32_t pin;
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
    {GPIOF, 1 << 9},
    {GPIOA, 1 << 11},
    {GPIOA, 1 << 8}
};

key_t key[KEY_NUM];

void keyProc(void){
    for(uint8_t i=0; i<KEY_NUM; i++){
        uint8_t k = (gpioKeys[i].port->IDR & gpioKeys[KEY_NUM].pin) == 0;
        if(k){
            if(key->pressCnt!=255)key->pressCnt++;
            if(key->pressCnt>IS_PRESSED_TICK)key->isPressed=1;
            if(key->pressCnt>IS_LONG_PRESSED_TICK)key->isLongPressed=1;
        }else{
            if(key->pressCnt>IS_PRESSED_CLICK)key->isClick=1;
            if(key->pressCnt>IS_LONG_PRESSED_CLICK)key->isLongClick=1;
            key->pressCnt=0;
        }
    }
}
