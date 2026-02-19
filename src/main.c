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
    int T_short = 3;

    //const float f_m = 0.325,  d_E = 2.5386/100; // 1.008
    //const float f_m = 0.9167, d_E = 2.0473/100; // 1.009
    const float f_m = 8.8,    d_E = 0.6832/100; // 1.008
    //const float f_m = 13.5,   d_E = 0.7780/100; // 1.009
    //const float f_m = 33.3,   d_E = 2.0027/100; // 1.017
    char strBuff[40];
    sprintf(strBuff, "f_m: %g, d_E: %g\n", f_m, d_E);
    USART1_SendStr(strBuff);
    TIM1_Start();
    for (int sample_count = 0; sample_count < T_short*60*SAMPLING_FREQUENCY_HZ; ) {
        //float value;
        uint32_t value;
        if (!ADC1_QueueIsEmpty()) {
            value = ADC1_QueuePop();
            //ADC1_QueuePop();
            //float t = (float) sample_count / SAMPLING_FREQUENCY_HZ;
            //value = (1 - 0.11*cos(2*3.1415926536*100*t))*(1 + d_E/2*sgn(sin(2*3.1415926536*f_m*t)));
            if (value < 13107) { // 13107 is 20% of the maximum of an ADC reading (65536 aka 2**16)
                blueLEDTurnON();
                redLEDTurnOFF();
            }
            else if (value > 62259) { // 62259 is 95% of the maximum of an ADC reading (65536 aka 2**16)
                blueLEDTurnOFF();
                redLEDTurnON();
            }
            else {
                blueLEDTurnOFF();
                redLEDTurnOFF();
            }

            IEC_61547_insert(&IEC, (double) value);
            ++sample_count;
        }
    }
    TIM1_Stop();
    redLEDTurnON();
    blueLEDTurnON();

    return IEC_61547_compute_P_st(&IEC);
}

int main(void){
    // 1. BOARD AND PERIPHERICS INITIALIZATION
    BoardSetup_Init(); // Must always be the first initialization

    Timebase_Init(); // Must always be the second initialization

    // Now the peripherals may be initilized
    USART1_Init();
    ADC1_Init(SAMPLING_FREQUENCY_HZ); // Must be ran before initializing it's timer (TIM1)
    TIM1_Init();

    // 2. IEC 61547-1 
    char strbuff[40];
    USART1_SendStr("Computing P_st...\n"); // Make sure the PC's serial monitor is on 230400 baud rate
    float P_st = computePst();
    char strBuffer[40];
    sprintf(strBuffer, "P_st = %d.%.3d\n", (int) P_st, (int) ((P_st - (int) P_st) * 1000));
    USART1_SendStr(strBuffer);
    USART1_SendStr("End of computation.\n");

    startLEDErrorSignal();
}