#include "display.h"
#include "system.h"
#include "sysControl.h"

void lcdWrite(uint8_t d){
    *(uint8_t *)&(SPI1->DR) = d;
    while((SPI1->SR & SPI_SR_BSY));
    GPIOB->BSRR = GPIO_BSRR_BS_4;
    for(volatile uint32_t i=0; i<10; i++);
    GPIOB->BSRR = GPIO_BSRR_BR_4;
}

void lcdWrite4(uint8_t d){
    lcdWrite(d|LCD_EN|LCD_BKL);
    for(volatile uint32_t i=0; i<500; i++);
    lcdWrite(d|LCD_BKL);
    for(volatile uint32_t i=0; i<500; i++);
}

void lcdWriteByte(uint8_t d, uint8_t m){
    lcdWrite4((((d>>4)&0xF)<<3)|m);
    lcdWrite4(((d&0xF)<<3)|m);
}

void lcdSetPos(uint8_t x, uint8_t y){
    if(y>2){
        x+=20;
        y-=20;
    }
	lcdWriteByte((0x40*y+x)|0b10000000, 0);
}

void lcdChar(unsigned char c){
    lcdWriteByte(c, 1);
}

void lcdClear(){
	lcdWriteByte(0b00000001, 0);
}

void lcdInit(){
    lcdWrite(0);
    while(tick<20);
    lcdWrite4(3<<3);
    while(tick<21)
    lcdWrite4(3<<3);
    while(tick<22)
    lcdWrite4(3<<3);
    while(tick<23);
    lcdWrite4(2<<3);
    lcdWriteByte(0b00101000, 0);
    lcdWriteByte(0b00001100, 0);
    lcdWriteByte(0b00000110, 0);
}