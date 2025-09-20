#include "TIM.h"

#include <stm32g431xx.h>

void TIM1_Init() {
    // TIM1 configuration for 100 us (10 kHz) interruptions
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;      // Enables APB2 bus as TIM1's clock
    // By default it's in continuous mode (after the update event, it goes back to 0 and reinitiates the counting)
    TIM1->PSC = SystemCoreClock/1000000 - 1; // Prescaler for making the counting period equal to 1 us
    TIM1->ARR = 500-1;                      // Auto-reload register
    TIM1->CNT = 0;                           // Initial counting value
    TIM1->CR2 |= TIM_CR2_MMS_1;              // Send trigger output when the update event occurs
    TIM1->EGR |= TIM_EGR_UG;                 // Manual update event, to load the new values to the timer
}

void TIM1_Start() {
    TIM1->CR1 |= TIM_CR1_CEN;
}

void TIM1_Stop() {
    TIM1->CR1 &= ~TIM_CR1_CEN;
}