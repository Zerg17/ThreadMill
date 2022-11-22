#include "system.h"

#include "display.h"
#include "modbus.h"

void rccInit() {
    RCC->AHBENR |= RCC_AHBENR_GPIOFEN | RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOAEN | RCC_AHBENR_DMAEN;
    RCC->APB2ENR |= RCC_APB2ENR_TIM17EN | RCC_APB2ENR_TIM16EN | RCC_APB2ENR_USART1EN | RCC_APB2ENR_SPI1EN | RCC_APB1ENR_SPI2EN | RCC_APB2ENR_SYSCFGEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C2EN | RCC_APB1ENR_USART2EN | RCC_APB1ENR_SPI2EN | RCC_APB1ENR_TIM14EN;

}

void gpioInit() {
    // PA15 - KEY7              - GPIO_IN
    // PA12 - RS485_REDE        - USART1_RTS
    // PA11 - KEY10             - GPIO_IN
    // PA10 - RS485_RX          - USART1_RX
    // PA9  - RS485_TX          - USART1_TX
    // PA8  - KEY11             - GPIO_IN
    // PA7  - KEY1              - GPIO_IN
    // PA6  - KEY2              - GPIO_IN
    // PA5  - KEY3              - GPIO_IN
    // PA4  - KEY4              - GPIO_IN
    // PA3  - DEBUG_TX          - USART2_TX
    // PA2  - DEBUG_RX          - USART2_RX
    // PA1  - KEY5              - GPIO_IN
    // PA0  - KEY6              - GPIO_IN
    // PB15 - MOSI_KEYS_BKL     - SPI2_MOSI
    // PB14 - LATCH_KEYS_BKL    - GPIO_OUT
    // PB13 - SCK_KEYS_BKL      - SPI2_SCK
    // PB12 - OE_KEYS_BKL       - GPIO_OUT
    // PB11 - SDA_MEM           - I2C2_SDA
    // PB10 - SCL_MEM           - I2C2_SCL
    // PB9  - BUZZER            - TIM17
    // PB8  - OE_DISPLAY        - GPIO_OUT(?TIM16)
    // PB7  - SDA_DISPLAY       - /NC/(I2C1_SDA)
    // PB6  - SCL_DISPLAY       - /NC/(I2C1_SCL)
    // PB5  - MOSI_DISPLAY      - SPI1_MOSI
    // PB4  - LATCH_DISPLAY     - GPIO_OUT
    // PB3  - SCK_DISPLAY       - SPI1_SCK
    // PB2  - ENC_SW            - GPIO_IN
    // PB1  - ENC_B             - ?GPIO_IN
    // PB0  - ENC_A             - ?GPIO_IN
    // PF7  - KEY8              - GPIO_IN
    // PF6  - KEY9              - GPIO_IN

    // 0 - GPIO_OUT
    // 1 - Alternate function
    // Msk - Analog mode

    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR10_0;
    GPIOB->PUPDR |= GPIO_PUPDR_PUPDR2_0;

    GPIOA->MODER |= GPIO_MODER_MODER12_1 | GPIO_MODER_MODER10_1 | GPIO_MODER_MODER9_1 | GPIO_MODER_MODER3_1 | GPIO_MODER_MODER2_1;
    GPIOB->MODER |= GPIO_MODER_MODER15_1 | GPIO_MODER_MODER14_0 | GPIO_MODER_MODER13_1 | GPIO_MODER_MODER12_0 | GPIO_MODER_MODER11_1 | GPIO_MODER_MODER10_1 | GPIO_MODER_MODER9_1 | GPIO_MODER_MODER8_0 | GPIO_MODER_MODER5_1 | GPIO_MODER_MODER4_0 | GPIO_MODER_MODER3_1;
    GPIOA->AFR[1] |= 0x00010110;
    GPIOA->AFR[0] |= 0x00001100;
    GPIOB->AFR[1] |= 0x00001122;
    GPIOB->AFR[0] |= 0x11000000;

    // GPIOB->ODR |= GPIO_ODR_8;
}

void uart2Write(uint8_t d) {
    while (!(USART2->ISR & USART_ISR_TXE))
        ;
    USART2->TDR = d;
}

void uart2Init() {
    USART2->BRR = F_CPU / BAUD2;
    USART2->CR1 = USART_CR1_TE | USART_CR1_RXNEIE | USART_CR1_RE;
    USART2->CR1 |= USART_CR1_UE;
}

void tim17Init() {
    TIM17->CCMR1 = (0x06 << TIM_CCMR1_OC1M_Pos);
    TIM17->CCER = TIM_CCER_CC1E;
    TIM17->BDTR |= TIM_BDTR_MOE;
    TIM17->PSC = 0;
    TIM17->CR1 = TIM_CR1_ARPE | TIM_CR1_CEN;
}

void tim17pwm(uint16_t p, uint16_t a) {
    TIM17->ARR = a;
    TIM17->CCR1 = p;
}

void tim14Init() {
    TIM14->ARR = F_CPU/1000-1;
    TIM14->CR1 |= TIM_CR1_CEN;
    TIM14->DIER |= TIM_DIER_UIE;
}

void spi1Init(void) {
    SPI1->CR1 = SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_MSTR | SPI_CR1_SPE;
}

void spi2Init(void) {
    SPI2->CR2=SPI_CR2_DS_Msk;
    SPI2->CR1 = SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_MSTR | SPI_CR1_SPE;
}

void i2c2Init() {
    I2C2->TIMINGR = 0x00310309;
    I2C2->CR1 = I2C_CR1_PE;
}

void extiInit(){
    SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI1_PB | SYSCFG_EXTICR1_EXTI0_PB;
    EXTI->RTSR = (1<<0);
    EXTI->IMR =  (1<<0);
    NVIC_SetPriority(EXTI0_1_IRQn, 1);
    NVIC_EnableIRQ(EXTI0_1_IRQn);
}

void nvicInit() {
    NVIC_EnableIRQ(USART1_IRQn);
    NVIC_EnableIRQ(USART2_IRQn);
    NVIC_EnableIRQ(TIM14_IRQn);
    NVIC_SetPriority(SysTick_IRQn, 3);
    SysTick_Config(F_CPU/100-1);
}

void sysInit() {
    rccInit();
    gpioInit();
    uart2Init();
    xdev_out(uart2Write);
    tim17Init();
    tim14Init();
    spi1Init();
    i2c2Init();
    mbInit();
    nvicInit();
    lcdInit();
    spi2Init();
    // extiInit();
}