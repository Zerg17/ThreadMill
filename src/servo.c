#include "servo.h"
#include "modbus.h"
#include "sysControl.h"
#include "system.h"

uint8_t txBuf[8];

int8_t servoWriteReg(uint16_t adr, uint16_t val){
    mbRequest_06_t* w = (mbRequest_06_t*)txBuf;
    w->adr=SERVO_ADR;
    w->code=writeSingleRegister;
    w->regAdr=__REV16(adr);
    w->regData=__REV16(val);
    mbTX(txBuf, sizeof(mbRequest_06_t));
    uint32_t t=tick;
    while(!mbRX.rxne && t+5>=tick); // 50ms wait
    if(mbRX.rxne){
        mbRX.rxne=0;
        mbHead_t* h = (mbHead_t*)mbRX.buf;
        if(h->adr!=SERVO_ADR) return -2;
        if((h->code&0x7F)!=writeSingleRegister) return -3;
        if(h->code&0x80){
            mbResponse_err_t* e = (mbResponse_err_t*)mbRX.buf;
            return e->err;
        }
        return 0;
    }
    return -1;
}

int8_t servoReadReg(uint16_t adr, uint16_t* dst){
    mbRequest_03_04_t* w = (mbRequest_03_04_t*)txBuf;
    w->adr=SERVO_ADR;
    w->code=readHoldingRegisters;
    w->regAdr=__REV16(adr);
    w->regCnt=0x0100;
    mbTX(txBuf, sizeof(mbRequest_03_04_t));
    uint32_t t=tick;
    while(!mbRX.rxne && t+20>=tick); // 200ms wait
    if(mbRX.rxne){
        mbRX.rxne=0;
        mbHead_t* h = (mbHead_t*)mbRX.buf;
        if(h->adr!=SERVO_ADR) return -2;
        if((h->code&0x7F)!=readHoldingRegisters) return -3;
        mbResponse_03_04_t* r = (mbResponse_03_04_t*)mbRX.buf;
        if(h->code&0x80){
            mbResponse_err_t* e = (mbResponse_err_t*)mbRX.buf;
            return e->err;
        }
        *dst=__REV16(r->regData[0]);
        return 0;
    }
    return -1;
}

int8_t servoReadRegs(uint16_t startAdr, uint8_t numRegs, uint16_t* dst){
    mbRequest_03_04_t* w = (mbRequest_03_04_t*)txBuf;
    w->adr=SERVO_ADR;
    w->code=readHoldingRegisters;
    w->regAdr=__REV16(startAdr);
    w->regCnt=__REV16(numRegs);
    mbTX(txBuf, sizeof(mbRequest_03_04_t));
    uint32_t t=tick;
    while(!mbRX.rxne && t+20>=tick); // 200ms wait
    if(mbRX.rxne){
        mbRX.rxne=0;
        mbHead_t* h = (mbHead_t*)mbRX.buf;
        if(h->adr!=SERVO_ADR) return -2;
        if((h->code&0x7F)!=readHoldingRegisters) return -3;
        mbResponse_03_04_t* r = (mbResponse_03_04_t*)mbRX.buf;
        if(h->code&0x80){
            mbResponse_err_t* e = (mbResponse_err_t*)mbRX.buf;
            return e->err;
        }
        for(uint8_t i=0; i<numRegs; i++)
            dst[i]=__REV16(r->regData[i]);
        return 0;
    }
    return -1;
}

int32_t getServoPos(void){
    uint16_t regs[2];
    int8_t err = servoReadRegs(0x1BC, 2, regs);
    if(err) return 0;
    else return regs[0] | (regs[1] << 16);
}

void setExtIO(uint8_t v){
    servoWriteReg(0x1A0, v);
}

void setSimIO(uint8_t v){
    servoWriteReg(0x1A4, v);
}

//0,1,2,3,21,20,17,19
void setDiConfig(uint8_t d, uint8_t conf){
    servoWriteReg(0x80+d, conf);
}

//0,1,2,3,4,5
void setDoConfig(uint8_t d, uint8_t conf){
    servoWriteReg(0x88+d, conf);
}

//Standart 0, Extendet 1
void setStandartExtendetMode(uint8_t m){
    servoWriteReg(0x90, m);
}


void setCommsndPulse(uint8_t v){
    servoWriteReg(0x43, v);
}

void setTrotleLimit(uint16_t t){
    if(t>3000) t=3000;
    servoWriteReg(0x5e, t);
}

void setSpeed(int16_t s){
    if(s<-3000)s=-3000;
    if(s>3000)s=3000;
    servoWriteReg(0x53, s);
}

void setControlMode(uint8_t v){
    servoWriteReg(0x2, v);
}

uint16_t getStatus(void){
    uint16_t regs;
    int8_t err = servoReadRegs(0x1BC, 1, &regs);
    if(err) return 0XFFFF;
    else return regs;
}