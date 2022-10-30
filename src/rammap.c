#include "rammap.h"
#include "eeprom_i2c.h"

tap_profile currentProfile = {
    .thread = THREAD_DEF,
    .torque = TORQUE_DEF,
    .speed = SPEED_DEF,
    .pitch = PITCH_DEF,
    .depth = DEPTH_DEF
};

void saveProfile(void){
    eepromWrite(0, (uint8_t*)&currentProfile, sizeof(currentProfile));
}

void loadProfile(void){
    uint8_t tmp;
    eepromRead(0, &tmp, sizeof(tmp));
    if(tmp!=0xFF){
        eepromRead(0, (uint8_t*)&currentProfile, sizeof(currentProfile));
    }
}
