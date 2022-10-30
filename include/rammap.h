#pragma once

#include "stdint.h"

#define THREAD_MIN 10
#define THREAD_DEF 25
#define THREAD_MAX 100
#define THREAD_STP 5

#define TORQUE_MIN 10
#define TORQUE_DEF 320
#define TORQUE_MAX 10000
#define TORQUE_STP 5

#define SPEED_MIN 10
#define SPEED_DEF 100
#define SPEED_MAX 900
#define SPEED_STP 5

#define PITCH_MIN 35
#define PITCH_DEF 150
#define PITCH_MAX 150
#define PITCH_STP 5

#define DEPTH_MIN 50
#define DEPTH_DEF 100
#define DEPTH_MAX 300
#define DEPTH_STP 5

typedef struct {
    uint16_t thread; //fixed point divided by 10 
    uint16_t torque; //fixed point divided by 10
    int16_t speed;   
    uint16_t pitch;  //fixed point divided by 100
    uint16_t depth;  //fixed point divided by 10
} tap_profile;

extern tap_profile currentProfile;

void saveProfile(void);
void loadProfile(void);
