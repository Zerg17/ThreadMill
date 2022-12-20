#include "servo.h"
#include "modbus.h"
#include "sysControl.h"
#include "system.h"
#include "key.h"
#include "menu.h"

#define PRE_TAP_TORQUE_LIMIT            50 //Ограничение крутящего момента до нарезания резьбы

static inline int32_t constrain(int32_t v,int32_t min,int32_t max){
    return (v < min) ? min : ((v > max) ? max : v);
}

static inline int32_t abs(int32_t v){
    return (v < 0) ? -v : v;
}

uint8_t txBuf[13];

int8_t servoWriteReg(uint16_t adr, uint16_t val){
    mbRequest_06_t* w = (mbRequest_06_t*)txBuf;
    w->adr=SERVO_ADR;
    w->code=writeSingleRegister;
    w->regAdr=__REV16(adr);
    w->regData=__REV16(val);
    mbTX(txBuf, sizeof(mbRequest_06_t));
    uint32_t t=tick;
    while(!mbRX.rxne && t+10>=tick); // 50ms wait
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

int8_t servoWriteReg32(uint16_t adr, uint32_t val){
    uint16_t tmp1 = val;
    uint16_t tmp2 = val>>16;
    mbRequest_10_t* w = (mbRequest_10_t*)txBuf;
    w->adr=SERVO_ADR;
    w->code=writeMultipleRegisters;
    w->regAdr=__REV16(adr);
    w->regCnt=0x0200;
    w->dataLen=4;
    w->regData[0]=__REV16(tmp1);
    w->regData[1]=__REV16(tmp2);
    mbTX(txBuf, 11);
    uint32_t t=tick;
    while(!mbRX.rxne && t+10>=tick); // 50ms wait
    if(mbRX.rxne){
        mbRX.rxne=0;
        mbHead_t* h = (mbHead_t*)mbRX.buf;
        if(h->adr!=SERVO_ADR) return -2;
        if((h->code&0x7F)!=writeMultipleRegisters) return -3;
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
    while(!mbRX.rxne && t+10>=tick); // 200ms wait
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
    while(!mbRX.rxne && t+10>=tick); // 200ms wait
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
    while(servoReadRegs(0x1BC, 2, regs));
    return regs[0] | (regs[1] << 16);
}

void setSpeedCommandType(uint8_t v){
    servoWriteReg(0x05, v);
}

void setExtIO(uint8_t v){
    servoWriteReg(0x1A0, v);
}

void setSimDI(uint8_t v){
    servoWriteReg(0x1A4, v);
}

uint16_t getSimDO(void){
    uint16_t regs;
    int8_t err = servoReadRegs(0x1D3, 1, &regs);
    xprintf("getSimDO() = %08b,ret%d\n", regs, err);
    if(err) return 0xFFFF;
    else return regs & 0xFF;
}

//0,1,2,3,21,20,17,19
void setDIConfig(uint8_t d, DI_configuration_t conf){
    if(d>8) return;
    servoWriteReg(0x80+d, conf);
}

//0,1,2,3,4,5
void setDOConfig(uint8_t d, servoStatus_t conf){
    if(d>6) return;
    servoWriteReg(0x88+d, conf);
}

//Standard 0, Extended 1
void setStandardExtendedMode(uint8_t m){
    servoWriteReg(0x90, m);
}


void setCommSendPulse(uint8_t v){
    servoWriteReg(0x43, v);
}

void setTorqueLimit(uint16_t t){
    if(t>3000) t=3000;
    while(servoWriteReg(0x5e, t));
}

void setOverloadLevel(uint16_t t){
    if(t>3000) t=3000;
    while(servoWriteReg(0x72, t));
}

void setSpeed(int16_t s){
    if(s<-3000)s=-3000;
    if(s>3000)s=3000;
    // servoWriteReg(0x53, s);
    while(servoWriteReg(0x092, 0));
    while(servoWriteReg(0x140, s));
}

int8_t setTargetPos(int32_t p){
    servoWriteReg(0x091,0);
    int8_t err=servoWriteReg32(0x168, p);
    xprintf("setTargetPos(%ld)=%d\n", p, err);
    return err;
}

void setPosSpeed(uint16_t s){
    if(s>3000)s=3000;
    servoWriteReg(0x190, s);
}

void setControlMode(uint8_t v){
    servoWriteReg(0x002, v);
}

void setPositionControlMode(uint8_t v){
	servoWriteReg(0x94, v);
}

servoStatus_t getStatus(void){
    uint16_t regs;
    int8_t err = servoReadRegs(0x1D0, 1, &regs);
    if(err) return SERVO_STATUS_UNKNOWN;
    else return regs;
}

void servoSaveSettings(void){
    servoWriteReg(0x1A7,0x801);
}

servoState_t servoState;
int8_t servoStateMachineHandler(servoState_t* servoState){
    switch(servoState->state){
        case 1:                        //Подготовка
            //Записать настройки скорости и крутящего момента в регистры контроллера
            setSpeed(servoState->speed);
            setTorqueLimit(PRE_TAP_TORQUE_LIMIT);
            setSimDI(0b00000101); // Сервопривод активен, режим Speed
            servoState->state = 2;   //Ожидать касания
            keyBklSet(SAVE_TO_EEPROM_KEY,3);
            break;
        case 2:                        //Холостой ход, ожидать начала нарезки
        #ifndef NO_PRE_TAP
            if((getSimDO() & (0xFF20)) == 0x20){ //Если превышен момент (0b00100000 == 0x20), перейти к нарезке резьбы
        #else
                setSimDI(0b00000101); // Сервопривод активен, режим Speed
        #endif
                setSpeed(0);
                setTorqueLimit(servoState->torque);
                servoState->initialPos = getServoPos();
                //TODO Добавить дополнительную переменную для хранения относительной целевой позиции
                servoState->finalPos+=servoState->initialPos;
                servoState->targetPos=servoState->finalPos; //Вкрутиться на всю глубину
                setSpeed(servoState->speed);
                servoState->state = 3;  //Перейти к нарезке
                keyBklSet(SAVE_TO_EEPROM_KEY,2);
        #ifndef NO_PRE_TAP
            }
        #endif
            break;
        case 3:                       //Ожидать превышение крутящего момента или глубины
            if(getServoPos() > servoState->targetPos){   //Если достигнута глубина
                setSpeed(0);
                servoState->targetPos=servoState->initialPos-EXTRA_UNSCREW_ANGLE; //Выкрутиться на всю глубину + запас EXTRA_UNSCREW_ANGLE
                setSpeed(-servoState->speed);
                servoState->state=5;   //Перейти к выкручиванию метчика
            }
            else if((getSimDO() & (0xFF20)) == 0x20){ //Если превышен момент (0b00100000 == 0x20), делаем реверс
                setSpeed(0);
                servoState->targetPos=getServoPos() - REVERSE_ANGLE; //Выкрутиться на REVERSE_ANGLE
                setSpeed(-servoState->speed);
                servoState->state=4;   //Ждем окончания выкручивания
            }
            break;
        case 4:{                       //Возврат обратно к нарезке резьбы
            int32_t cur = getServoPos();
            xprintf("cur: %d, tar: %d\n",cur,servoState->targetPos);
            if(((getSimDO() & (0xFF20)) == 0x20) || (cur < servoState->targetPos)){ //Если достигнута глубина или превышен момент (0b00100100 == 0x24)
                setSpeed(0);
                servoState->targetPos=servoState->finalPos; //Вкрутиться на всю глубину
                setSpeed(servoState->speed);
                servoState->state = 3;  //Продолжаем нарезку
            }
            break;
        }
        case 5:                       //Ожидать превышение крутящего момента или полного выкручивания
            if(getServoPos() < servoState->targetPos){   //Если Выкрутились
                setSpeed(0);
                servoState->state=0;   //Завершаем
                setSimDI(0b00000000); //Сервопривод не активен
            }
            else if((getSimDO() & (0xFF20)) == 0x20){ //Если превышен момент (0b00100000 == 0x20), делаем реверс
                setSpeed(0);
                servoState->targetPos=getServoPos() + REVERSE_ANGLE; //Выкрутиться на REVERSE_ANGLE
                setSpeed(servoState->speed);
                servoState->state=6;   //Ждем окончания выкручивания
            }
            break;
        case 6:{                       //Возврат обратно к выкручиванию
            if(((getSimDO() & (0xFF20)) == 0x20) || (getServoPos() > servoState->targetPos)){ //Если достигнута глубина или превышен момент (0b00100100 == 0x24)
                setSpeed(0);
                servoState->targetPos=servoState->initialPos-EXTRA_UNSCREW_ANGLE; //Выкрутиться на всю глубину + запас EXTRA_UNSCREW_ANGLE
                setSpeed(-servoState->speed);
                servoState->state = 5;  //Продолжаем выкручивание
            }
            break;
        }
        default:
            setSimDI(0b00000000); //Сервопривод не активен
            keyBklSet(SAVE_TO_EEPROM_KEY,0);
            break;
    }
    return 0;
}
