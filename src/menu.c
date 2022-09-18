#include "menu.h"
#include "sysControl.h"
#include "display.h"
#include "system.h"

void logoMenu();
void mainMenu();

void (*menuFn) (void) = logoMenu;
uint8_t firstExec = 1;
uint32_t timeExec = 0;

void nextMenu(void (*nextFn) (void)){
    firstExec=1;
    timeExec=tick;
    menuFn=nextFn;
}

void logoMenu(){
    if(firstExec){
        firstExec=0;
        lcdClear();
        lcdSetPos(0,1);
        xfprintf(lcdChar, "Hello, World!");
        lcdSetPos(20,0);
        xfprintf(lcdChar, "Hello, World!");
    }
    if(tick>200)nextMenu(mainMenu);
}

void mainMenu(){
    if(firstExec){
        firstExec=0;
        lcdClear();
        lcdSetPos(0,0);
        xfprintf(lcdChar, "Main");
    }
}

void procMenu(void){
    menuFn();
}