#include "USART.h"

#include <stdlib.h>
#include <stm32g431xx.h>

void USART1_Init() {
    // Configuração da USART1 para Tx sem interrupção
    // PC4 como saída push-pull em função alt. (Tx da USART1)
    GPIOC->MODER &= ~GPIO_MODER_MODE4; 
    GPIOC->MODER |= GPIO_MODER_MODE4_1; 
    GPIOC->AFR[0] |= 0b0111 << GPIO_AFRL_AFSEL4_Pos; // Escolha da função 7

    RCC->APB2ENR |= RCC_APB2ENR_USART1EN; // Hab. clock para USART1
    if (SystemCoreClock > 65536000) // Será preciso mudar o prescaler do APB2, pois o BRR só vai até 65536
        exit(-1);
    // Define baudrate = 230400 baud/s (APB2_clock = 64 MHz)
    USART1->BRR  = SystemCoreClock/230400;          
    USART1->CR1 |= USART_CR1_TE; // Habilita TX
    USART1->CR1 |= USART_CR1_UE; // Habilita USART1 
}

void USART1_SendByte(char data)  {
    while (!(USART1->ISR & USART_ISR_TXE)); // Aguarda buffer de Tx estar vazio
    USART1->TDR = data;
}

void USART1_SendStr(char *data)  {
    for(int i = 0; data[i]; ++i) {
       USART1_SendByte(data[i]);
    }
}