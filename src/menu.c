#include "menu.h"
#include "sysControl.h"
#include "display.h"
#include "system.h"

void logoMenu();
void mainMenu();

void (*menuFn) (void) = logoMenu;
uint8_t firstExec = 1;
uint32_t timeExec = 0;
tap_settings currentSettings;

void nextMenu(void (*nextFn) (void)){
    firstExec=1;
    timeExec=tick;
    menuFn=nextFn;
}

void logoMenu(){
    // xprintf("logoMenu\n");
    if(firstExec){
        lcdSetPos(0,0);
    }
    if(tick>200)nextMenu(mainMenu);
}

void mainMenu(){
    // xprintf("mainMenu\n");
    if(firstExec){
        firstExec=0;
        lcdSetPos(0,0);
        xfprintf(lcdChar,"  Dia         Pitch ");
        lcdSetPos(0,1);
        xfprintf(lcdChar,"%3s %-6s    1.5mm ");
        lcdSetPos(0,2);
        xfprintf(lcdChar," 60 rpm       10 mm ");
        lcdSetPos(0,3);
        xfprintf(lcdChar," Speed        Depth ");
        // xfprintf(lcdChar, "Main");
    }
}

/*
┌────────────────────┐
│  Dia         Pitch │
│ M3 32Ncm     1.5mm │
│ 60 rpm       10 mm │
│ Speed        Depth │
└────────────────────┘
*/

/*  
    xprintf("%d", 1234);                        "1234"
    xprintf("%6d,%3d%%", -200, 5);      "  -200,  5%"
    xprintf("%-6u", 100);                       "100   "
    xprintf("%ld", 12345678);           "12345678"
    xprintf("%llu", 0x100000000);       "4294967296"    <_USE_LONGLONG>
    xprintf("%04x", 0xA3);                      "00a3"
    xprintf("%08lX", 0x123ABC);         "00123ABC"
    xprintf("%016b", 0x550F);           "0101010100001111"
    xprintf("%s", "String");            "String"
    xprintf("%-5s", "abc");                     "abc  "
    xprintf("%5s", "abc");                      "  abc"
    xprintf("%c", 'a');                         "a"
    xprintf("%f", 10.0);            <xprintf lacks floating point support. Use regular printf.>
*/

void procMenu(void){
    menuFn();
}