#include "menu.h"

#include "key.h"
#include "sysControl.h"
#include "display.h"
#include "system.h"
#include "rammap.h"
#include "servo.h"

// #define RUS_MENU

static const uint8_t menuKeysMap[] ={
    KNOB_KEY,
    TOP_LEFT_KEY,
    TOP_MIDDLE_KEY,
    TOP_RIGHT_KEY,
    BOT_LEFT_KEY,
    BOT_MIDDLE_KEY,
    BOT_RIGHT_KEY,
    SAVE_TO_EEPROM_KEY
};

void logoMenu();
void mainMenu();
void tapMenu();

char buf1[4],buf2[4];
void (*menuFn) (void) = logoMenu;
uint8_t firstExec = 1;
uint32_t timeExec = 0;

static struct{
    uint8_t selectedSetting;
} menuState;

void nextMenu(void (*nextFn) (void)){
    firstExec=1;
    timeExec=tick;
    menuFn=nextFn;
}

void logoMenu(){
    // xprintf("logoMenu\n");
    if(firstExec){
        firstExec = 0;
        lcdSetPos(0,0);
        #ifdef RUS_MENU
        xfprintf(lcdChar, "====================");  // 1st string
        xfprintf(lcdChar, "    Strunion ltd.   ");  // 3rd string
        xfprintf(lcdChar, " Резьборез вер. 0.1 ");  // 2nd string
        xfprintf(lcdChar, "====================");  // 4th string
        #else
        xfprintf(lcdChar, "====================");  // 1st string
        xfprintf(lcdChar, "    Strunion ltd.   ");  // 3rd string
        xfprintf(lcdChar, "   ThreadMill v0.1  ");  // 2nd string
        xfprintf(lcdChar, "====================");  // 4th string
        #endif
    }
    if(tick>500)nextMenu(mainMenu);
}
 ///@todo   reset selection by pressing same button again

inline void wrapMenuSetting(uint8_t x, uint8_t y){
                lcdSetPos(x, y);
                lcdChar('>');
                lcdSetPos(x+8, y);
                lcdChar('<');
}

inline void unwrapMenuSetting(uint8_t x, uint8_t y){
                lcdSetPos(x, y);
                lcdChar(' ');
                lcdSetPos(x+8, y);
                lcdChar(' ');
}

void mainMenu() {
    // xprintf("mainMenu\n");
    if (firstExec) {
        firstExec = 0;
        lcdSetPos(0, 0);
        #ifdef RUS_MENU
            xfprintf(lcdChar, " Момент         Шаг ");  // 1st string
            xfprintf(lcdChar, " Обороты    Глубина ");  // 3rd string
            // if(currentProfile.thread%10)
            //     xsprintf(buf1,"M%1d.%1d",currentProfile.thread/10,currentProfile.thread%10);
            // else
            //     xsprintf(buf1,"M%-3d",currentProfile.thread/10);
            // xfprintf(lcdChar, " %s       %2d.%02dмм ",buf1,currentProfile.pitch/100,currentProfile.pitch%100);  // 2nd string
            xfprintf(lcdChar, " %5u      %2d.%02dмм ",currentProfile.torque,currentProfile.pitch/100,currentProfile.pitch%100);  // 2nd string
            xfprintf(lcdChar, " %3dоб/м    %2d.%d мм ",currentProfile.speed, currentProfile.depth/10, currentProfile.depth%10);  // 4rd string
        #else
            xfprintf(lcdChar, " Torque       Pitch ");  // 1st string
            xfprintf(lcdChar, " RPM          Depth ");  // 3rd string
            // if(currentProfile.thread%10)
            //     xsprintf(buf1,"M%1d.%1d",currentProfile.thread/10,currentProfile.thread%10);
            // else
            //     xsprintf(buf1,"M%-3d",currentProfile.thread/10);
            // xfprintf(lcdChar, " %s       %2d.%02dmm ",buf1,currentProfile.pitch/100,currentProfile.pitch%100);  // 2nd string
            xfprintf(lcdChar, " %5u      %2d.%02dmm ",currentProfile.torque,currentProfile.pitch/100,currentProfile.pitch%100);  // 2nd string
            xfprintf(lcdChar, " %3dRPM     %2d.%d mm ",currentProfile.speed, currentProfile.depth/10, currentProfile.depth%10);  // 4rd string
        #endif
    }
    // lcdSetPos(6, 0);
    // xfprintf(lcdChar, "%5d ", getServoPos());  // 1st string
    switch (menuState.selectedSetting){
    case 1:
        if(encPos){
            currentProfile.torque+=encPos*TORQUE_STP;
            encPos=0;
            if(currentProfile.torque<TORQUE_MIN) currentProfile.torque = TORQUE_MAX;
            else if(currentProfile.torque >TORQUE_MAX) currentProfile.torque=TORQUE_MIN;
            // if(currentProfile.thread%10)
            //     xsprintf(buf1,"М%1d.%1d",currentProfile.thread/10,currentProfile.thread%10);
            // else
            //     xsprintf(buf1,"M%-3d",currentProfile.thread/10);
            lcdSetPos(1, 1);
            xfprintf(lcdChar, "%5u",currentProfile.torque);  // 2nd string
            // xprintf("torque: %u\n",currentProfile.torque);  // 2nd string
        }
        break;
    case 3:
        if(encPos){
            currentProfile.pitch+=encPos*PITCH_STP;
            encPos=0;
            if(currentProfile.pitch<PITCH_MIN) currentProfile.pitch = PITCH_MAX;
            else if(currentProfile.pitch >PITCH_MAX) currentProfile.pitch=PITCH_MIN;
            lcdSetPos(12, 1);
            #ifdef RUS_MENU
                xfprintf(lcdChar, "%2d.%02dмм",currentProfile.pitch/100,currentProfile.pitch%100);
            #else
                xfprintf(lcdChar, "%2d.%02dmm",currentProfile.pitch/100,currentProfile.pitch%100);
            #endif
        }
        break;
    case 4:
        if(encPos){
            currentProfile.speed+=encPos*SPEED_STP;
            encPos=0;
            if(currentProfile.speed<SPEED_MIN) currentProfile.speed = SPEED_MAX;
            else if(currentProfile.speed >SPEED_MAX) currentProfile.speed=SPEED_MIN;
            lcdSetPos(1, 3);
            #ifdef RUS_MENU
                xfprintf(lcdChar, "%3dоб/м",currentProfile.speed);
            #else
                xfprintf(lcdChar, "%3d rpm",currentProfile.speed);
            #endif
        }
        break;
    case 6:
        if(encPos){
            currentProfile.depth+=encPos*DEPTH_STP;
            encPos=0;
            if(currentProfile.depth<DEPTH_MIN) currentProfile.depth = DEPTH_MAX;
            else if(currentProfile.depth >DEPTH_MAX) currentProfile.depth=DEPTH_MIN;
            lcdSetPos(12, 3);
            #ifdef RUS_MENU
                xfprintf(lcdChar,  "%2d.%d мм",currentProfile.depth/10, currentProfile.depth%10);
            #else
                xfprintf(lcdChar,  "%2d.%d mm",currentProfile.depth/10, currentProfile.depth%10);
            #endif
        }
        break;
    }

    for(uint8_t i = 0; i<sizeof(menuKeysMap)-1; i++){
        // if((i==2) || (i==5)) {
        //     if(key[TOP_MIDDLE_KEY].isClick){ 
        //         encPos++;
        //         key[TOP_MIDDLE_KEY].isClick = 0;
        //     }
        //     else if(key[BOT_MIDDLE_KEY].isClick){ 
        //         encPos--;
        //         key[BOT_MIDDLE_KEY].isClick = 0;
        //     }
        //     continue;
        // }
        if(key[menuKeysMap[i]].isClick){
            key[menuKeysMap[i]].isClick = 0;
            encPos=0;   // Костыль от страшных вещей
            keyBklSet(menuKeysMap[menuState.selectedSetting],0);
            switch (menuState.selectedSetting){
            case 1:
                unwrapMenuSetting(0, 1);
                break;
            case 3:
                unwrapMenuSetting(11, 1);
                break;
            case 4:
                unwrapMenuSetting(0, 3);
                break;
            case 6:
                unwrapMenuSetting(11, 3);
                break;
            }

            if(menuState.selectedSetting == i){ 
                menuState.selectedSetting = 0;
                break;
            }
            menuState.selectedSetting = i;
            keyBklSet(menuKeysMap[menuState.selectedSetting],1);
            switch (menuState.selectedSetting){
            case 1:
                wrapMenuSetting(0, 1);
                break;
            case 3:
                wrapMenuSetting(11, 1);
                break;
            case 4:
                wrapMenuSetting(0, 3);
                break;
            case 6:
                wrapMenuSetting(11, 3);
                break;
            }
            break;
        }
    }
    // xprintf("Polling keys... 11:%1d\n",key[SAVE_TO_EEPROM_KEY].isClick);
    if(key[SAVE_TO_EEPROM_KEY].isClick){
        // saveProfile();
        // xprintf("Start Mill\n");
        keyBklSet(menuKeysMap[menuState.selectedSetting],0);
        menuState.selectedSetting = 0;
        servoState.speed = currentProfile.speed;
        servoState.torque = currentProfile.torque;
        servoState.finalPos= (((uint32_t)currentProfile.depth*10000) / currentProfile.pitch)*10;
        #ifdef NO_PRE_TAP
        servoState.state = 2;
        #else
        servoState.state = 1;
        #endif
        nextMenu(tapMenu);
        key[SAVE_TO_EEPROM_KEY].isClick = 0;
        keyBklSet(SAVE_TO_EEPROM_KEY,2);
    }
}

void tapMenu(){
    if(firstExec){
        firstExec = 0;
        lcdSetPos(0,0);
        #ifdef RUS_MENU
        xfprintf(lcdChar, "  Нарезка резьбы... ");  // 1st string
        xfprintf(lcdChar, " Текущий этап:      ");  // 3rd string
        xfprintf(lcdChar, "                    ");  // 2nd string
        xfprintf(lcdChar, " Глубина:           ");  // 4th string
        #else
        xfprintf(lcdChar, " Current stage:     ");  // 1st string
        xfprintf(lcdChar, " Target:            ");  // 3rd string
        xfprintf(lcdChar, " CurPos:            ");  // 2nd string
        xfprintf(lcdChar, " Init:              ");  // 4th string
        #endif
    }
    lcdSetPos(16,0);
    xfprintf(lcdChar, "%1d", servoState.state);  // 1st string
    lcdSetPos(9,1);
    xfprintf(lcdChar, "%-10d", getServoPos());    // 2nd string
    lcdSetPos(9,2);
    xfprintf(lcdChar, "%-10d", servoState.targetPos);    // 3rd string
    lcdSetPos(9,3);
    xfprintf(lcdChar, "%-10d", servoState.initialPos);    // 4th string
    
    if(key[SAVE_TO_EEPROM_KEY].isClick){
        servoState.state = 0;
        key[SAVE_TO_EEPROM_KEY].isClick = 0;
    }
    if(!servoState.state) nextMenu(mainMenu);
}

/*
┌────────────────────┐
│  Dia         Pitch │
│ M3          1.5 mm │
│ 60 rpm     10.5 mm │
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
