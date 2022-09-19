#include "system.h"
#include "sysControl.h"
#include "display.h"
#include "menu.h"
#include "key.h"
#include "servo.h"
// #include "modbus.h"

uint32_t tim1 = 0, menuTim=0;

uint8_t txBuf[15];

void printHexTable(uint8_t* buf, uint32_t s){
	for(uint32_t i=0; i<s; i++){
		xprintf("%02X ", buf[i]);
		if(i%32==31)xprintf("\n");
		if(i%256==255)xprintf("\n");
		if(i%32==15)xprintf(" ");
	}
	xprintf("\n");
}

int main(void){ 
    sysInit();

    while(1){
        if(tim1<=tick){
            tim1+=2;
            uint16_t regs[2];
            int8_t err = servoReadRegs(0x1BC, 2, regs);
            if(err) xprintf("err=%d\n", err);
            else xprintf("%d\n", regs[0]|(regs[1]<<16));
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