#pragma once

#include "stdint.h"

#define SERVO_ADR 1

/// @brief Запись 1 регистра
/// @param adr Адрес регистра
/// @param val Значение регистра для записи
/// @return Результат выполнения операции - 0 успешно
int8_t servoWriteReg(uint16_t adr, uint16_t val);


/// @brief Чтение 1 регистра
/// @param adr Адрес регистра
/// @param dst Приемник читаемого регистра
/// @return Результат выполнения операции - 0 успешно
int8_t servoReadReg(uint16_t adr, uint16_t* dst);

/// @brief Чтение нескольких регистров
/// @param startAdr Адрес первого регистра
/// @param numRegs Количество регистров
/// @param dst Приемник читаемых регистров
/// @return Результат выполнения операции - 0 успешно
int8_t servoReadRegs(uint16_t startAdr, uint8_t numRegs, uint16_t* dst);
