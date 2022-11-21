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

uint32_t tim1 = 200, tim2 = 0, menuTim = 0;

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

    setControlMode(1); //Режим Speed
    // servoSaveSettings();  //Сохранить настройки
    setStandardExtendedMode(1);
    // setDIConfig(2, SERVO_DI_CONF_C_MODE);   // Переключение в гибридном режиме
    // setDIConfig(5, SERVO_DI_CONF_POS_LOAD); // Команда на загрузку позиции
    // setDIConfig(6, SERVO_DI_CONF_ORG_SW);   // Команда на обнуление текущих координат
    setDOConfig(5, SERVO_STATUS_TLC);       // Флаг превышения крутящего момента
    // setDOConfig(2, SERVO_STATUS_COIN);      // Флаг достижения заданной позиции

    setExtIO(0b11111111);
    servoWriteReg(0x190, 300);
    setTorqueLimit(50);

    while (1) {

        if (tim1 <= tick) {
            tim1 += 10;
            servoStateMachineHandler(&servoState);
        }

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
