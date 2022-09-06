#include "system.h"
#include "sysControl.h"
#include "display.h"

uint32_t tim1 = 0;

int main(void){ 
    sysInit();



    while(1){
        if(tim1<=tick){
            tim1+=200;
            xprintf("Hello, World!");

            // lcdClear();
            lcdSetPos(0,0);
            xfprintf(lcdChar, "Hello, World! %u ", tick);
            lcdSetPos(0,1);
            xfprintf(lcdChar, "Hello, World! %u ", tick);
            lcdSetPos(20,0);
            xfprintf(lcdChar, "Hello, World! %u ", tick);
            lcdSetPos(20,1);
            xfprintf(lcdChar, "Hello, World! %u ", tick);
        }
    }
}