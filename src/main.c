#include <stm32g431xx.h>
#include <stdio.h>
#include <stdlib.h>
#include "BoardSetup.h"
#include "Timebase.h"
#include "USART.h"
#include "ADC.h"
#include "OPAMP.h"
#include "TIM.h"
#include "Utils.h"
#include "IEC_61547_1.h"

void startLEDErrorSignal() {
    while (1) {
        delay_ms(500);
        GPIOC->ODR ^= GPIO_ODR_OD6; // toogle PC6
    }
}

void redLEDTurnON() {
    GPIOC->ODR |= GPIO_ODR_OD10;
}

void redLEDTurnOFF() {
    GPIOC->ODR &= ~GPIO_ODR_OD10;
}

void blueLEDTurnON() {
    GPIOC->ODR |= GPIO_ODR_OD11;
}

void blueLEDTurnOFF() {
    GPIOC->ODR &= ~GPIO_ODR_OD11;
}

int sgn(float x) {
    return x<0 ? -1 : x>0;
}

float computePst() {
    IEC_61547_1 IEC;
    IEC_61547_init(&IEC);
    int T_short = 5;

    int a = 0;
    const float f_m = 0.9167, d_E = 0.020473;
    TIM1_Start();
    for (int sample_count = 0; sample_count < T_short*60*SAMPLING_FREQUENCY_HZ; ) {
        uint32_t value;
        if (!ADC1_QueueIsEmpty()) {
            value = ADC1_QueuePop();
            if (value < 13107) // 13107 is 20% of the maximum of an ADC reading (65536 aka 2**16)
                blueLEDTurnON();
            else
                blueLEDTurnOFF();
            if (value > 62259) // 62259 is 95% of the maximum of an ADC reading (65536 aka 2**16)
                redLEDTurnON();
            else
                redLEDTurnOFF();
            IEC_61547_insert(&IEC, value);
            ++sample_count;
        }
    }
    TIM1_Stop();

    return IEC_61547_compute_P_st(&IEC);
}

int main(void){ // Funcao principal
    // 1. BOARD AND PERIPHERICS INITIALIZATION
    BoardSetup_Init(); // Must always be the first initialization

    Timebase_Init(); // Must always be the second initialization

    // Now the peripherals may be initilized
    USART1_Init();
    ADC1_Init(SAMPLING_FREQUENCY_HZ); // Must be ran before initializing it's timer (TIM1)
    TIM1_Init();

    // 2. IEC 61547-1 
    USART1_SendStr("Computing P_st...\n");
    float P_st = computePst();
    char strBuffer[40];
    sprintf(strBuffer, "P_st = %d.%.3d\n", (int) P_st, (int) ((P_st - (int) P_st) * 1000));
    USART1_SendStr(strBuffer);
    USART1_SendStr("End of computation.\n");

    startLEDErrorSignal();
}