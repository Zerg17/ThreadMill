#include "display.h"
#include "key.h"
#include "menu.h"
#include "servo.h"
#include "sysControl.h"
#include "system.h"
#include "key.h"
// #include "modbus.h"
#include "eeprom_i2c.h"
#include "rammap.h"

uint32_t tim1 = 0, tim2 = 0, menuTim = 0;

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
    while (tick < 200);

    setControlMode(3); //Гибридный режим Pos/Speed
    servoSaveSettings();  //Сохранить настройки
    setStandardExtendedMode(1);
    setDIConfig(2, SERVO_DI_CONF_C_MODE);   // Переключение в гибридном режиме
    setDIConfig(5, SERVO_DI_CONF_POS_LOAD); // Команда на загрузку позиции
    setDIConfig(6, SERVO_DI_CONF_ORG_SW);   // Команда на обнуление текущих координат
    setDOConfig(5, SERVO_STATUS_TLC);       // Флаг превышения крутящего момента
    setDOConfig(2, SERVO_STATUS_COIN);      // Флаг достижения заданной позиции

    setExtIO(0b11111111);
    // servoWriteReg(0x168, 0);
    // setSimDI(0b01000001);
    // setSimDI(0b00000001);
    // setSimDI(0b00000001);

    // servoWriteReg(0x094,0); // relative position setting
    // servoWriteReg(0x096,0); // pos control signal
    servoWriteReg(0x190, 300);
    setTorqueLimit(50);

    // while(setTargetPos(0));

    // setSpeedCommandType(3);
    /*
    setControlMode(0);
    setStandardExtendedMode(1);
    servoWriteReg(0x001,3);


    setTorqueLimit(50);
    setDIConfig(5, SERVO_DI_CONF_POS_LOAD);
    setExtIO(0b11111111);
    setSimDI(0b00000001);
    servoWriteReg(0x091,1); //Положение
    servoWriteReg(0x170,10000); //Положение
    servoWriteReg(0x191,5);     //Скорость в режиме pos
    servoWriteReg(0x094,0); // relative position setting
    servoWriteReg(0x096,0); // pos control signal
    */

    /*
    setDIConfig(0, SERVO_DI_CONF_SRV_ON);
    setDIConfig(1, SERVO_DI_CONF_A_CLR);
    setDIConfig(2, SERVO_DI_CONF_CWL);
    setDIConfig(3, SERVO_DI_CONF_CCWL);
    setDIConfig(4, SERVO_DI_CONF_EMG);
    setDIConfig(6, SERVO_DI_CONF_ORG_SW);
    setDIConfig(7, SERVO_DI_CONF_HOMING);

    servoWriteReg(0x091,0); // Communication location mode index = 0
    servoWriteReg(0x094,0); // relative position setting
    setSimDI(0b00000000);
    setExtIO(0b11111111);
    
    setCommSendPulse(0);
    setPositionControlMode(0);
	// servoWriteReg(0x58, 2500)  // Ускорение =  2500
	// servoWriteReg(0x59, 2500)  // Замедление = 2500

    setTorqueLimit(50);

    //PA_168 The 0th internal position command
    servoWriteReg(0x168,60000);
    //PA_190 Internal position 0 speed
    servoWriteReg(0x190,5);
    setSimDI(0b00000001);


    setSimDI(0b00100001);   // Position loading signal
    setSimDI(0b00000001);
    */
    // loadProfile();
    // setSpeed(5);

    while (1) {
        // int32_t p = getServoPos();
        // if(p>10000)setSpeed(-5);
        // if(p<(-10000))setSpeed(5);

        if (tim1 <= tick) {
            tim1 += 10;
            servoStateMachineHandler(&servoState);
        }

        if (tim2 <= tick) {
            tim2 += 100;
            if(servoState.state > 2){
                setSimDI(0b00100001);
                setSimDI(0b00000001);
            }
        }

        // //Blink в гибридном режиме
        // //Переключение между двумя режимами
        // if (tim1 <= tick) {
        //     tim1 += 100;
        //     if(tim1/100 %2)     
        //         setSimDI(0b00000001); //Pos Mode; Servo enabled
        //     else
        //         setSimDI(0b00000101); //Speed Mode; Servo enabled
        // }


        // if (tim1 <= tick) {
        //     tim1 += 2;
        //     uint16_t regs[2];
        //     int8_t err = servoReadRegs(0x1BC, 2, regs);
        //     if (err)
        //         xprintf("err=%d\n", err);
        //     else
        //         xprintf("%d\n", regs[0] | (regs[1] << 16));
        // }
        // if (tim1 <= tick) {
        //     tim1 += 2;
        //     xprintf("pos: %d button: %s %d\n", encPos, key[0].isPressed ? "Pressed" : "Released");

        //     lcdSetPos(0, (encPos +-1)%4);
        //     xfprintf(lcdChar, "            ");
        //     lcdSetPos(0, (encPos + 1)%4);
        //     xfprintf(lcdChar, "            ");
        //     lcdSetPos(0, encPos%4);
        //     xfprintf(lcdChar, "Hello, World!");
        //     lcdChar(pos + '0');

        // }

        // if(tick>=500) NVIC_SystemReset();

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
        
        if(menuTim<=tick){
            menuTim+=10;
            procMenu();
        }
    }
}

uint8_t counter = 32;
char buf[255];
void USART2_IRQHandler() {
    if (USART2->ISR & USART_ISR_RXNE) {  // receive buffer is not empty
        uint8_t receivedChar = (uint8_t)(USART2->RDR);
        if (receivedChar == '@') {
            uart2Write('\n');
            xprintf("%d\n", eepromRead(0,(uint8_t*)buf,255));
            // xprintf("%s",buf);
            printHexTable((uint8_t*)buf, 255);
            counter = 0;
            // lcdSetPos(0, 0);

        } else {
            // lcdChar(receivedChar);
            uart2Write(receivedChar);
            xprintf("%d\n", eepromWrite(counter,&receivedChar,1u));
            counter++;
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
