#include "modbus.h"
#include "system.h"

#define CRC_INIT 0xFFFF
#define CRC_POLY 0xA001 // Reversed 0x8005

/// @brief Шаги конечного автомата для приема
typedef enum{
    entry,
    getCmd,
    getLen,
    get4Bytes,
    get4BytesThenLen,
    checkCRCh,
    checkCRCl
} mbStep;

mbRX_t mbRX;

void mbInit(void){
    RCC->CFGR3|=RCC_CFGR3_USART1SW_0;
    DMA1_Channel2->CPAR = (uint32_t) (&(USART1->TDR));
    DMA1_Channel2->CCR = DMA_CCR_MINC | DMA_CCR_DIR;
    USART1->BRR = F_CPU/MB_BAUD;
    USART1->RTOR = 80;
    USART1->CR3|=USART_CR3_DEM|USART_CR3_DMAT|USART_CR3_EIE;
    USART1->CR2|=USART_CR2_RTOEN;
    USART1->CR1|=USART_CR1_RTOIE|USART_CR1_DEAT_Msk|USART_CR1_DEDT_Msk|USART_CR1_RXNEIE|USART_CR1_TE|USART_CR1_RE|USART_CR1_UE;
}

void mbTX(uint8_t* d, uint16_t l){
    uint16_t crc = CRC_INIT;
    for(uint16_t i=0; i<l; i++){
        crc ^= d[i];
        for(uint8_t i=0; i<8; i++)
        if(crc & 1){
            crc >>= 1;
            crc ^= CRC_POLY;
        }else crc >>= 1;
    }
    d[l]=crc;
    d[l+1]=crc>>8;

    while(DMA1_Channel2->CNDTR) continue;
    DMA1->IFCR=DMA_IFCR_CTEIF2|DMA_IFCR_CHTIF2|DMA_IFCR_CTCIF2|DMA_IFCR_CGIF2;
    DMA1_Channel2->CCR &=~DMA_CCR_EN;
    DMA1_Channel2->CMAR = (uint32_t)d;
    DMA1_Channel2->CNDTR = l+2;
    DMA1_Channel2->CCR |= DMA_CCR_EN;
}

void USART1_IRQHandler(void){
    if (USART1->ISR & USART_ISR_RXNE) {
        uint8_t d = (uint8_t)(USART1->RDR);
        switch(mbRX.step){
            case entry: 
                if(mbRX.rxne) break;
                mbRX.ptr=0;
                mbRX.crc=CRC_INIT;
                mbRX.step=getCmd;
                break;
            case getCmd:
                switch(d){
                    case 0x01:
                    case 0x02:
                    case 0x03:
                    case 0x04: mbRX.step=getLen; break; 
                    case 0x05:
                    case 0x06:
                    case 0x0F:
                    case 0x10: mbRX.cnt=4; mbRX.step=get4Bytes; break;
                    case 0x81:
                    case 0x82:
                    case 0x83:
                    case 0x84:
                    case 0x85:
                    case 0x86:
                    case 0x8F:
                    case 0x90: mbRX.cnt=1; mbRX.step=get4Bytes; break;
                    default: mbRX.step=entry; break;
                }
                break;
                case getLen: mbRX.cnt=d; mbRX.step=get4Bytes; break;
                case get4Bytes: if(!--mbRX.cnt)mbRX.step=checkCRCh; break;
                case get4BytesThenLen: if(!--mbRX.cnt)mbRX.step=getLen; break;
                case checkCRCh: mbRX.step=(mbRX.crc&0xff)?checkCRCl:entry; break;
                case checkCRCl: 
                    if(d==(mbRX.crc>>8))mbRX.rxne=1;
                    mbRX.step=entry;
                    break;
        }

        if(mbRX.step!=checkCRCl){
            if(mbRX.step!=entry){
                mbRX.buf[mbRX.ptr++]=d;
                if(mbRX.ptr==RX_BUF)mbRX.step=entry;
            }
            mbRX.crc^=d;
            for(uint8_t i=0; i<8; i++)
                if(mbRX.crc&1){
                    mbRX.crc>>=1;
                    mbRX.crc^=CRC_POLY;
                }else mbRX.crc>>=1;
        }
    }

    if (USART1->ISR & USART_ISR_RTOF){
        USART1->ICR = USART_ICR_RTOCF;
        mbRX.step=entry;
    }
    if (USART1->ISR & USART_ISR_FE) USART1->ICR = USART_ICR_FECF;
    if (USART1->ISR & USART_ISR_ORE) USART1->ICR = USART_ICR_ORECF;
    if (USART1->ISR & USART_ISR_NE) USART1->ICR = USART_ICR_NCF;
}