#include "sysControl.h"
#include "system.h"
#include "key.h"

uint32_t volatile tick=0;
uint32_t sec_d=0;
uint32_t volatile sec=0;

void SysTick_Handler(void) {
    tick++;
    if(++sec_d==100){
        sec_d=0;
        sec++;
    }
    if(tick == 0xFFFFFFFF) NVIC_SystemReset();

    keyProc();
    keyBklProc();
}