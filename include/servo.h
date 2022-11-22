#pragma once

#include "stdint.h"

#define SERVO_ADR                      1

// #define REVERSE_ANGLE                  835  //~30 градусов, 360 градусов = 10000 попугаев
#define REVERSE_ANGLE                  5000  //180 градусов, 360 градусов = 10000 попугаев
#define EXTRA_UNSCREW_ANGLE            10000 * 5  //5 * 360 градусов

// #define NO_PRE_TAP

typedef struct _servoState {
    uint8_t state;
    int16_t speed;
    uint16_t torque;
    int32_t initialPos;
    int32_t targetPos;
    int32_t finalPos;
} servoState_t;

typedef enum servoStatus_e{
    SERVO_STATUS_S_RDY=0,               // Servo ready
    SERVO_STATUS_ALM,                   // Servo alarm
    SERVO_STATUS_COIN,                  // Location arrival
    SERVO_STATUS_BRK_OFF,               // Brake Release
    SERVO_STATUS_ZSP,                   // Zero speed detection
    SERVO_STATUS_TLC,                   // Torque limiting
    SERVO_STATUS_V_COIN,                // Speed consistency
    SERVO_STATUS_AT_SPEED,              // Speed arrival
    SERVO_STATUS_EX_COIN,               // Full closed loop position arrival
    SERVO_STATUS_OVERLOAD_O,            // Overload warning
    SERVO_STATUS_BRAKE_ON,              // Brake pipe conduction state
    SERVO_STATUS_ORG_FOUND,             // Origin has been found
    SERVO_STATUS_BRAKE_ON_ERR_O=14,     // Brake error message
    SERVO_STATUS_EEPROM_STATE_O,        // EEPROm completion status
    SERVO_STATUS_JOG_RUN,               // JOG run
    SERVO_STATUS_HOMING_ACTIVE,         // Return to zero
    SERVO_STATUS_UNKNOWN=255
} servoStatus_t;

typedef enum DI_configuration_e{
    SERVO_DI_CONF_SRV_ON,               //Servo enabling
    SERVO_DI_CONF_A_CLR,                //Clear alarm
    SERVO_DI_CONF_CWL,                  //Clockwise stroke limit
    SERVO_DI_CONF_CCWL,                 //Counterclockwise stroke limit
    SERVO_DI_CONF_C_MODE,               //Control mode switching
    SERVO_DI_CONF_ZEROSPD,              //Zero speed clamp
    SERVO_DI_CONF_DIV,                  //Command pulse division frequency selection
    SERVO_DI_CONF_SPD_DIR,              //Speed command direction (valid if PA_006=2)
    SERVO_DI_CONF_INH,                  //Command pulse prohibition
    SERVO_DI_CONF_GAIN,                 //Gain switching
    SERVO_DI_CONF_CL,                   //Clear the deviation counter to 0
    SERVO_DI_CONF_INTSPD1,              //Internal speed 1
    SERVO_DI_CONF_INTSPD2,              //Internal speed 2
    SERVO_DI_CONF_INTSPD3,              //Internal speed 3
    SERVO_DI_CONF_INTSPD4,              //Internal speed 4
    SERVO_DI_CONF_TL_SEL,               //Torque limit switchover
    SERVO_DI_CONF_HOMING,               //Start position of "back to zero"
    SERVO_DI_CONF_ORG_SW,               //Origin switch position
    SERVO_DI_CONF_POS_LOCK,             //Servo locking
    SERVO_DI_CONF_JOG_BIT,              //JOG starting position
    SERVO_DI_CONF_POS_LOAD,             //Position loading signal
    SERVO_DI_CONF_EMG                   //Emergency stop or external error input
} DI_configuration_t;

extern servoState_t servoState;

/// @brief                             Запись одного регистра
/// @param adr                         Адрес регистра
/// @param val                         Значение регистра для записи
/// @return                            Результат выполнения операции - 0 успешно
int8_t servoWriteReg(uint16_t adr, uint16_t val);

/// @brief Запись пары регистров
/// @param adr Адрес первого регистра
/// @param val Значение регистров для записи
/// @return Результат выполнения операции - 0 успешно
int8_t servoWriteReg32(uint16_t adr, uint32_t val);

/// @brief                             Чтение одного регистра
/// @param adr                         Адрес регистра
/// @param dst                         Приемник читаемого регистра
/// @return                            Результат выполнения операции - 0 успешно
int8_t servoReadReg(uint16_t adr, uint16_t* dst);

/// @brief                             Чтение нескольких регистров
/// @param startAdr                    Адрес первого регистра
/// @param numRegs                     Количество регистров
/// @param dst                         Приемник читаемых регистров
/// @return                            Результат выполнения операции - 0 успешно
int8_t servoReadRegs(uint16_t startAdr, uint8_t numRegs, uint16_t* dst);

/// @brief Select the speed command type under speed mode
/// (0x05)
/// @param v
//0: analog speed command input
//1: Internal command (1st to 4th internal speeds
//2: Internal command (1st to 3rd internal speed command or analog)
//3: Internal command (1st to 8th internal speeds)
void setSpeedCommandType(uint8_t v);




/// @brief Получить текущее положение шпинделя
/// (0x1BC)
/// @return Положение шпинделя
int32_t getServoPos();

/// @brief
/// @param v
void setExtIO(uint8_t v);

/// @brief Установить значение виртуального регистра ввода
/// 0x1D2
/// @param v значение регистра
void setSimDI(uint8_t v);

/// @brief Получить значение виртуального регистра вывода
/// 0x1D3
/// @return значение регистра вывода или 0xFFFF при ошибке чтения
uint16_t getSimDO();

/// @brief
/// 0x90
/// @param m
void setStandardExtendedMode(uint8_t m);

/// @brief
/// @param v
void setCommSendPulse(uint8_t v);

/// @brief Установить положение, к которому нужно приехать
/// (0x168)
/// @param p Положение
/// @return Результат выполнения операции - 0 успешно
int8_t setTargetPos(int32_t p);

/// @brief Установить скорость вращения
/// (0x140)
/// @param s Скорость (-3000..3000)
void setSpeed(int16_t s);

/// @brief Установить ограничение крутящего момента
/// (0x5e)
/// @param t Крутящий момент (0..3000)
void setTorqueLimit(uint16_t t);

/// @brief Установить уровень срабатывания ошибки 16 (Excessive Load)
/// (0x72)
/// @param t Значение в промилле от максимального крутящего момента (0..3000)
void setOverloadLevel(uint16_t t);

/// @brief Настройка функции входного пина
/// (0x80+d)
/// @param d Номер пина (0..8)
/// @param conf Режим пина (0..21)
void setDIConfig(uint8_t d, DI_configuration_t conf);

/// @brief Настройка функции выходного пина
/// (0x88+d)
/// @param d Номер пина (0..8)
/// @param conf Режим пина (0..17)
void setDOConfig(uint8_t d, servoStatus_t conf);

/// @brief Установить абсолютный или относительный режим управления положением
/// 0x94
/// @param v
/// 0 Абсолютный
/// 1 Относительный
void setPositionControlMode(uint8_t v);

/// @brief 0x2
/// @param v
void setControlMode(uint8_t v);

/// @brief 0x1D0
/// @return
servoStatus_t getStatus(void);

/// @brief Сохранение настроек сервопривода в постоянную память
void servoSaveSettings(void);

/// @brief Конечный автомат реализующий логику сервопривода
/// @param state Указатель на состояние автомата
/// @return Результат выполнения операции - 0 успешно
int8_t servoStateMachineHandler(servoState_t* servoState);
