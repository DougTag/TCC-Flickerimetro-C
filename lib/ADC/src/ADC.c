#include "adc.h"

#include <stm32g431xx.h>
#include <stdint.h>
#include <stdlib.h>
#include "Utils.h"
#include <math.h>

volatile uint32_t ADC1_queue[ADC1_QUEUE_MAX_SIZE];
volatile uint32_t ADC1_queue_sz = 0;

// Must be ran before initializing it's timer (TIM1)
void ADC1_Init() {
    // --- CONFIGURAÇÃO DO ADC1 --- //

    // 1. LIGAR E CALIBRAR
    RCC->AHB2ENR |= RCC_AHB2ENR_ADC12EN;  // Liga o ADC1
    RCC->CCIPR |= RCC_CCIPR_ADC12SEL_0;   // Seleciona o P clock

    ADC1->CR &= ~ADC_CR_DEEPPWD; // Sai do modo deep-power-down, que é padrão
    ADC1->CR |= ADC_CR_ADVREGEN; // Libera a regulação de tensão
    delay_ms(1); // Agora é preciso esperar pelo menos 20 microssegundos

    // Aqui poderia colocar o channel 3 no modo single-ended, mas ele já vem por padrão. Seria com essa linha:
    //ADC1->DIFSEL &= ~ADC_DIFSEL_DIFSEL_3;

    ADC1->CR &= ~ADC_CR_ADCALDIF; // Configurar calibração para modo single-ended
    ADC1->CR |= ADC_CR_ADCAL; // Iniciar calibração
    while (ADC1->CR & ADC_CR_ADCAL); // Esperar acabar a calibração
    delay_ms(1); // Agora é preciso esperar pelo menos 4 ciclos de clock do ADC para dar enable
    // Agora pode dar enable, mas tenha certeza que o clock do AHB esteja dentro do máximo para o ADC.
    // Se não, é preciso mexer no prescaler do ADC para diminuir o clock.

    ADC1->ISR |= ADC_ISR_ADRDY;           // Desativa a flag. É invertido mesmo: escreve 1 que o hardware coverte para 0
    ADC1->CR |= ADC_CR_ADEN;              // Habilita o ADC
    while (!(ADC1->ISR & ADC_ISR_ADRDY)); // Espera o ADC ligar

    // 2. AMOSTRAGEM
    ADC1->SMPR1 &= ~ADC_SMPR1_SMP3;  // 2.5 ciclos de clock por amostragem (é o mais rápido) no channel 3
    ADC1->CFGR &= ~ADC_CFGR_RES;    // 12 bits para cada amostra
    ADC1->CFGR &= ~ADC_CFGR_CONT;   // Modo single conversion (depois configuro para iniciar cada conversão pelo timer)

    ADC1->CFGR2 |= ADC_CFGR2_ROVSE; // Habilita o oversampling
    ADC1->CFGR2 = ((ADC1->CFGR2 & ~ADC_CFGR2_OVSS) | (0b0100 << ADC_CFGR2_OVSS_Pos)); // Dá rshift em 4 bits no resultado do oversampler
    ADC1->CFGR2 = ((ADC1->CFGR2 & ~ADC_CFGR2_OVSR) | (0b111 << ADC_CFGR2_OVSR_Pos));  // Razão de oversampling igual a 256x

    ADC1->SQR1 &= ~ADC_SQR1_L;           // Apenas 1 conversão
    ADC1->SQR1 |= 3 << ADC_SQR1_SQ1_Pos; // Conversão no canal 3 (ADC12_IN3)

    // 3. TIMER 
    ADC1->CFGR |= ADC_CFGR_EXTEN_0;      // Habilita trigger interno. Nesse caso, na borda de subida
        // Seleção do sinal tim1_trgo para dar o trigger
    //ADC1->CFGR &= ~ADC_CFGR_EXTSEL_Msk;
    ADC1->CFGR |= (ADC_CFGR_EXTSEL_3 | ADC_CFGR_EXTSEL_0);

    // 4. INTERRUPÇÃO AO FINALIZAR
    ADC1->IER |= ADC_IER_EOCIE;          // Habilita geração de interrupção quando termina uma conversão 
    NVIC_EnableIRQ(ADC1_2_IRQn);         // Habilitar no NVIC
    
    // 5. ESCUTAR SINAL DE DISPARO
    ADC1->CR |= ADC_CR_ADSTART;
}

void ADC1_2_IRQHandler() {
    if (ADC1->ISR & ADC_ISR_EOC) { // Se a interrupção for por final de conversão
        ADC1->ISR |= ADC_ISR_EOC;  // Desarma a flag

        ADC1_queue[ADC1_queue_sz] = ADC1->DR;
        ++ADC1_queue_sz;
        if (ADC1_queue_sz > sizeof(ADC1_queue)/sizeof(uint32_t)) { // Se o buffer estourar...
            GPIOC->ODR ^= GPIO_ODR_OD6; // toogle PC6
            SystemCoreClockUpdate();
            exit(-1);
        }
    }
}

uint32_t ADC1_QueuePop() {
    --ADC1_queue_sz;
    return ADC1_queue[ADC1_queue_sz];
}

int ADC1_QueueIsEmpty() {
    return ADC1_queue_sz <= 0;
}
