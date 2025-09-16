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

#define SAMPLING_FREQUENCY_HZ 10000

void startLEDErrorSignal() {
    while (1) {
        delay_ms(500);
        GPIOC->ODR ^= GPIO_ODR_OD6; // toogle PC6
    }
}

int main(void){ // Funcao principal
    BoardSetup_Init(); // Must always be the first initialization

    Timebase_Init(); // Must always be the second initialization

    // Now the peripherals may be initilized
    USART1_Init();
    ADC1_Init(SAMPLING_FREQUENCY_HZ); // Must be ran before initializing it's timer (TIM1)
    TIM1_Init();

    char dummystr[20];
    TIM1_Start();
    for(;;) { // Loop principal
        uint32_t value;
        if (!ADC1_QueueIsEmpty()) {
            value = ADC1_QueuePop();
            sprintf(dummystr, "%lu\n", value);
            USART1_sendStr(dummystr);
        }
    }
    TIM1_Stop();
}