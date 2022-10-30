#include "system.h"

#define M24C32_ADR 0XA0

int8_t eepromRead(uint16_t adr, uint8_t* dst, uint16_t len){
    if((I2C2->ISR & I2C_ISR_BUSY)) return -1;
    uint8_t* adrA = (uint8_t*)(&adr)+1;
    I2C2->CR2 = (2 << I2C_CR2_NBYTES_Pos) | I2C_CR2_START | M24C32_ADR;
    while(1){
        if(I2C2->ISR & I2C_ISR_NACKF) {I2C2->ICR = I2C_ICR_NACKCF|I2C_ICR_STOPCF; return -2;}
        else if(I2C2->ISR & I2C_ISR_TC) break;
        else if(I2C2->ISR & I2C_ISR_TXIS) I2C2->TXDR = *adrA--;
    }
    if(len>255){
        I2C2->CR2 = I2C_CR2_RELOAD | (0xFF << I2C_CR2_NBYTES_Pos) | I2C_CR2_START | I2C_CR2_RD_WRN | M24C32_ADR;
        len-=255;
    }else I2C2->CR2 = I2C_CR2_AUTOEND | (len<< I2C_CR2_NBYTES_Pos) | I2C_CR2_START | I2C_CR2_RD_WRN | M24C32_ADR;
    while(1){
        if(I2C2->ISR & I2C_ISR_NACKF) {I2C2->ICR = I2C_ICR_NACKCF|I2C_ICR_STOPCF; return -3;}
        else if(I2C2->ISR & I2C_ISR_STOPF) {I2C2->ICR = I2C_ICR_STOPCF;break;}
        else if(I2C2->ISR & I2C_ISR_TCR){
            if(len>255){
                I2C2->CR2 = I2C_CR2_RELOAD | (0xFF << I2C_CR2_NBYTES_Pos);
                len-=255;
            }else I2C2->CR2 = I2C_CR2_AUTOEND | (len<< I2C_CR2_NBYTES_Pos);
        }
        else if(I2C2->ISR & I2C_ISR_RXNE) *dst++ = I2C2->RXDR;
    }
    return 0;
}

int8_t eepromWrite(uint16_t adr, uint8_t* src, uint16_t len){
    if((I2C2->ISR & I2C_ISR_BUSY)) return -1;
    uint8_t* adrA = (uint8_t*)(&adr)+1;
    I2C2->CR2 = I2C_CR2_RELOAD | (2 << I2C_CR2_NBYTES_Pos) | I2C_CR2_START | M24C32_ADR;
    while(1){
        if(I2C2->ISR & I2C_ISR_NACKF) {I2C2->ICR = I2C_ICR_NACKCF|I2C_ICR_STOPCF; return -2;}
        else if(I2C2->ISR & I2C_ISR_TCR) break;
        else if(I2C2->ISR & I2C_ISR_TXIS) I2C2->TXDR = *adrA--;
    }
    while(1){
        if(I2C2->ISR & I2C_ISR_NACKF) {I2C2->ICR = I2C_ICR_NACKCF|I2C_ICR_STOPCF; return -3;}
        else if(I2C2->ISR & I2C_ISR_STOPF) {I2C2->ICR = I2C_ICR_STOPCF; break;}
        else if(I2C2->ISR & I2C_ISR_TCR){
            if(len>255){
                I2C2->CR2 = I2C_CR2_RELOAD | (0xFF << I2C_CR2_NBYTES_Pos);
                len-=255;
            }else I2C2->CR2 = I2C_CR2_AUTOEND | (len<< I2C_CR2_NBYTES_Pos);
        }else if(I2C2->ISR & I2C_ISR_TXIS) I2C2->TXDR = *src++;
    }
    return 0;
}