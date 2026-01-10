#include "OPAMP.h"
#include <stm32g431xx.h>

void OPAMP1_Init(){
    // Used for adjusting the signal gain
    // PA1 -> VINP; ADC1_IN3 -> VOUT; VINM is internally connected to gain resistors
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    OPAMP1->CSR |= OPAMP_CSR_HIGHSPEEDEN_Msk; // High speed mode (better slew rate)
    OPAMP1->CSR |= OPAMP_CSR_VMSEL_1;    // PGA mode
    OPAMP1->CSR &= ~OPAMP_CSR_PGGAIN;    // Gain = +2. Will be changed by software to find best gain.
    OPAMP1->CSR &= ~OPAMP_CSR_OPAMPINTEN;// Output connected to pin PA2 (that is connected to channel ADC1_IN3)
    OPAMP1->CSR |= OPAMP_CSR_OPAMPxEN;   // Turn OPAMP on
}

void OPAMP2_Init(){
    // Used for the Sallen-Key filter (active 2nd order with Butterworth response)
    // PA7 -> VINP; PA6 -> VOUT; VINM is internally connected to VOUT
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    OPAMP2->CSR |= OPAMP_CSR_HIGHSPEEDEN_Msk; // High speed mode (better slew rate)
    OPAMP2->CSR |= OPAMP_CSR_VMSEL; // Follower mode
    OPAMP2->CSR &= ~OPAMP_CSR_OPAMPINTEN;// Output connected to pin PA6
    OPAMP2->CSR |= OPAMP_CSR_OPAMPxEN;   // Turn OPAMP on
}
