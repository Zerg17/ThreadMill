#include "display.h"

#include "sysControl.h"
#include "system.h"

void lcdWrite(uint8_t d) {
    *(uint8_t *)&(SPI1->DR) = d;
    while ((SPI1->SR & SPI_SR_BSY));
    GPIOB->BSRR = GPIO_BSRR_BS_4;
    for (volatile uint32_t i = 0; i < 10; i++);
    GPIOB->BSRR = GPIO_BSRR_BR_4;
}

void lcdWrite4(uint8_t d) {
    lcdWrite(d | LCD_EN | LCD_BKL);
    for (volatile uint32_t i = 0; i < 500; i++);
    lcdWrite(d | LCD_BKL);
    for (volatile uint32_t i = 0; i < 500; i++);
}

void lcdWriteByte(uint8_t d, uint8_t m) {
    lcdWrite4((((d >> 4) & 0xF) << 3) | m);
    lcdWrite4(((d & 0xF) << 3) | m);
}

void lcdSetPos(uint8_t x, uint8_t y) {
    if (y >= 2) {
        x += 20;
        y -= 2;
    }
    lcdWriteByte(((y<<6) + x) | 0b10000000, 0);
}

const char win12512disp[] = {
    0x41, 0xA0, 0x42, 0xA1, 0xE0, 0x45, 0xA3, 0xA4, 0xA5, 0xA6, 0x4B, 0xA7, 0x4D, 0x48, 0x4F, 0xA8,
    0x50, 0x43, 0x54, 0xA9, 0xAA, 0x58, 0xE1, 0xAB, 0xAC, 0xE2, 0xAD, 0xAE, 0x62, 0xAF, 0xB0, 0xB1,
    0x61, 0xB2, 0xB3, 0xB4, 0xE3, 0x65, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0x6F, 0xBE,
    0x70, 0x63, 0xBF, 0x79, 0xE4, 0x78, 0xE5, 0xC0, 0xC1, 0xE6, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7
};

void lcdChar(unsigned char c) {
    if(c<0xC0)lcdWriteByte(c, 1);
    else lcdWriteByte(win12512disp[c-0xC0], 1);
}

void lcdClear() {
    lcdWriteByte(0b00000001, 0);
    for(volatile uint32_t i=0; i<100; i++);
}

void lcdInit() {
    lcdWrite(0);
    while (tick < 20);
    lcdWrite4(3 << 3);
    while (tick < 21)
        lcdWrite4(3 << 3);
    while (tick < 22)
        lcdWrite4(3 << 3);
    while (tick < 23);
    lcdWrite4(2 << 3);
    lcdWriteByte(0b00101000, 0);
    lcdWriteByte(0b00001100, 0);
    lcdWriteByte(0b00000110, 0);
}