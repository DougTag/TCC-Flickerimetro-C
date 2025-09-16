#include "OPAMP.h"
#include <stm32g431xx.h>

void OPAMP1_Init(){
    // PA1 -> VINP; PA2 e ADC1_IN3 -> VOUT
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    OPAMP1->CSR |= OPAMP_CSR_VMSEL_1;    // Modo PGA
    OPAMP1->CSR &= ~OPAMP_CSR_PGGAIN;    // Ganho = +2
    OPAMP1->CSR &= ~OPAMP_CSR_OPAMPINTEN;// Saída fica conectada ao pino PA2 e canal ADC1_IN3
    OPAMP1->CSR |= OPAMP_CSR_OPAMPxEN;   // Liga o OPAMP
}

void OPAMP2_Init(){
    // PA7 -> VINP; PA6 -> VOUT; PA5 -> VINM
    // Modo standard
    OPAMP2->CSR |= OPAMP_CSR_OPAMPxEN;   // Liga o OPAMP
}
