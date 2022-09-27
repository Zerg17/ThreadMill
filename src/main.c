#include "display.h"
#include "key.h"
#include "menu.h"
#include "servo.h"
#include "sysControl.h"
#include "system.h"
// #include "modbus.h"

uint32_t tim1 = 0, menuTim = 0;

uint8_t txBuf[15];

void printHexTable(uint8_t* buf, uint32_t s) {
    for (uint32_t i = 0; i < s; i++) {
        xprintf("%02X ", buf[i]);
        if (i % 32 == 31) xprintf("\n");
        if (i % 256 == 255) xprintf("\n");
        if (i % 32 == 15) xprintf(" ");
    }
    xprintf("\n");
}

uint8_t pos = 0;
int main(void) {
    sysInit();
    lcdClear();
    while (1) {
        // if (tim1 <= tick) {
        //     tim1 += 2;
        //     uint16_t regs[2];
        //     int8_t err = servoReadRegs(0x1BC, 2, regs);
        //     if (err)
        //         xprintf("err=%d\n", err);
        //     else
        //         xprintf("%d\n", regs[0] | (regs[1] << 16));
        // }
        if (tim1 <= tick) {
            tim1 += 2;
            // xprintf("pos: %d button: %s %d\n", encPos, key[0].isPressed ? "Pressed" : "Released");
            if (encPos > 0) {
                encPos = 0;
                lcdSetPos(0, pos);
                xfprintf(lcdChar, "            ");
                pos = (pos + 1) & 0b11;
                lcdSetPos(0, pos);
                xfprintf(lcdChar, "Привет, мир!");
                lcdChar(pos + '0');
            } else if (encPos < 0) {
                encPos = 0;
                lcdSetPos(0, pos);
                xfprintf(lcdChar, "            ");
                pos = (pos - 1) & 0b11;
                lcdSetPos(0, pos);
                xfprintf(lcdChar, "Привет, мир!");
                lcdChar(pos + '0');
            }
        }

        // if(tim1<=tick){
        //     tim1+=2;
        //     mbRequest_03_04_t* w = (mbRequest_03_04_t*)txBuf;
        //     w->adr=1;
        //     w->code=3;
        //     w->regAdr=0xBC01;
        //     w->regCnt=0x0200;
        //     mbTX(txBuf, sizeof(mbRequest_03_04_t));
        // }
        // if(mbRX.rxne){
        //     mbRX.rxne=0;
        //     mbResponse_03_04_t* r = (mbResponse_03_04_t*)mbRX.buf;
        //     int32_t pos = (__REV16(r->regData[0]))|(__REV16(r->regData[1])<<16);
        //     xprintf("%d\n", pos);
        //     // printHexTable(mbRX.buf, 7);
        // }
        // if(menuTim<=tick){
        //     menuTim+=10;
        //     procMenu();
        // }
    }
}

void USART2_IRQHandler() {
    if (USART2->ISR & USART_ISR_RXNE) {  // receive buffer is not empty
        uint8_t receivedChar = (uint8_t)(USART2->RDR);
        if (receivedChar == '@') {
            uart2Write('\n');
            lcdSetPos(0, 0);

        } else {
            lcdChar(receivedChar);
            uart2Write(receivedChar);
        }
    }
    if (USART2->ISR & USART_ISR_FE) {  // Framing error
        USART2->ICR = USART_ICR_FECF;
    }
    if (USART2->ISR & USART_ISR_ORE) {  // Overrun error
        USART2->ICR = USART_ICR_ORECF;
    }
    if (USART2->ISR & USART_ISR_NE) {  // Noise error
        USART2->ICR = USART_ICR_NCF;
    }
}
