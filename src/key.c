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

    static uint8_t prevNextCode = 0;
    static uint16_t store=0;
    const int8_t rot_enc_table[] = {0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0};

    prevNextCode <<= 2;
    if (((GPIOB->IDR) & (1 << 0)) != 0) prevNextCode |= 0x02;
    if (((GPIOB->IDR) & (1 << 1)) != 0) prevNextCode |= 0x01;
    prevNextCode &= 0x0f;

    // If valid then store as 16 bit data.
    if  (rot_enc_table[prevNextCode] ) {
        store <<= 4;
        store |= prevNextCode;
        if (store==0xd42b) encPos--;
        if (store==0xe817) encPos++;
        // if ((store&0xff)==0x2b) encPos--;
        // if ((store&0xff)==0x17) encPos++;
    }

    // static uint8_t lastA, lastB;

    // uint8_t A = ((GPIOB->IDR) & (1 << 0)) != 0;
    // uint8_t B = ((GPIOB->IDR) & (1 << 1)) != 0;

    // if(lastA != A){
    //     if(A) encPos -= A != B ? 1 : 0;
    //     lastA = A;
    // }
    // if(lastB != B){
    //     if(B) encPos -= A != B ? -1 : 0;
    //     lastB = B;
    // }

    // encPos+=read_rotary();

    // static uint8_t lastA, lastB, tmpA, tmpB, cntA, cntB, valA, valB;
    // uint8_t A = ((GPIOB->IDR) & (1 << 0)) != 0;
    // uint8_t B = ((GPIOB->IDR) & (1 << 1)) != 0;

    // if(A!=tmpA){
    //     tmpA=A;
    //     cntA=0;
    // }else{
    //     cntA++;
    //     if(cntA>20){
    //         valA=A;
    //         cntA=0;
    //     }
    // }

    // if(B!=tmpB){
    //     tmpB=B;
    //     cntB=0;
    // }else{
    //     cntB++;
    //     if(cntB>20){
    //         valB=B;
    //         cntB=0;
    //     }
    // }

    // static int8_t encCnt;
    // if(lastA != valA){
    //     if(valA) encCnt -= valA != valB ? 1 : -1;
    //     lastA = valA;
    // }
    // if(lastB != valB){
    //     if(valB) encCnt -= valA != valB ? -1 : 1;
    //     lastB = valB;
    // }
    // if(encCnt>2){ encPos++; encCnt-=2; }
    // if(encCnt<-2){ encPos--; encCnt+=2; }
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

// key 1    SR bit: 15
// key 2    SR bit: 14
// key 3    SR bit: 13
// key 4    SR bit: 12
// key 5    SR bit: 11
// key 6    SR bit: 10
// key 7    SR bit: 9
// key 8    SR bit: 7
// key 9    SR bit: 6
// key 10   SR bit: 5
// key 11   SR bit: 4
// key 12   SR bit: 3   (LASER1)
// key 13   SR bit: 2   (LASER2)

void keyBklSet(uint8_t key, uint8_t state) {
    if(key<8)   key = 16 - key;
    else        key = 15 - key;
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
