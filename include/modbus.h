#pragma once

#include "stdint.h"

#define MB_BAUD 19200
#define RX_BUF 263

/// @brief Шаги конечного автомата для приема
typedef enum{
    entry,
    getCmd,
    getLen,
    get4Bytes,
    get4BytesThenLen,
    checkCRCh,
    checkCRCl
} mbStep;

typedef enum{
    readCoils=1,
    readDiscreteInputs,
    readHoldingRegisters,
    readInputRegisters,
    writeSingleCoil,
    writeSingleRegister,
    writeMultipleCoils=0x0F,
    writeMultipleRegisters
} mbFunction;

#pragma pack(push, 1)

typedef struct {
    uint8_t adr;
    uint8_t code;
} mbHead_t;

typedef struct {
    uint8_t adr;
    uint8_t code;
    uint16_t regAdr;
    uint16_t regCnt;
} mbRequest_03_04_t;

typedef struct {
    uint8_t adr;
    uint8_t code;
    uint16_t regAdr;
    uint16_t regData;
} mbRequest_06_t;

typedef struct {
    uint8_t adr;
    uint8_t code;
    uint16_t regAdr;
    uint16_t regCnt;
    uint8_t dataLen;
    uint16_t regData[127];
} mbRequest_10_t;

typedef struct {
    uint8_t adr;
    uint8_t code;
    uint8_t dataLen;
    uint16_t regData[127];
} mbResponse_03_04_t;

typedef struct {
    uint8_t adr;
    uint8_t code;
    uint16_t regAdr;
    uint16_t regData;
} mbResponse_06_t;

typedef struct {
    uint8_t adr;
    uint8_t code;
    uint16_t regAdr;
    uint16_t regCnt;
} mbResponse_10_t;

typedef struct {
    uint8_t adr;
    uint8_t code;
    uint8_t err;
} mbResponse_err_t;

/// @brief Контекст приема MODBUS
typedef struct {
    uint16_t crc;
    uint16_t ptr;
    mbStep step;
    uint8_t cnt;
    volatile uint8_t rxne;
    uint8_t buf[RX_BUF];
} mbRX_t;

#pragma pack(pop)

extern mbRX_t mbRX;

/// @brief Инициализация периферии(USART1, DMA) для MODBUS 
void mbInit(void);

/// @brief Передача данных через USART1 с помощью DMA, CRC16 рассчитывается и добавляется в конец
/// @param d Массив передаваемых данных должен быть длиннее на 2 байт чем длина передаваемых данных
/// @param l Длина передаваемых данных
void mbTX(uint8_t* d, uint16_t l);