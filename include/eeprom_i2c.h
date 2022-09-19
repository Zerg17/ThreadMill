#pragma once

#include "stdint.h"

uint8_t eepromRead(uint16_t adr, uint8_t* dst, uint16_t len);
uint8_t eepromWrite(uint16_t adr, uint8_t* src, uint16_t len);