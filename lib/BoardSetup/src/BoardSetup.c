#include "BoardSetup.h"
#include <stm32g431xx.h>

void _BoardSetup_CoreClock();
void _BoardSetup_GPIOC6();

void BoardSetup_Init() {
    _BoardSetup_CoreClock();

    _BoardSetup_GPIOC();

    _BoardSetup_GPIOA();

    GPIOC->ODR |= GPIO_ODR_OD6; // Turns PC6 on (on-board LED), to indicate the setup's end and proper initialization
}

void _BoardSetup_CoreClock(){
    // Use clock HSE (extern, 8 MHz)
    RCC->CR |= RCC_CR_HSEON;  // Starts reading the HSE signal
    while (!(RCC->CR & RCC_CR_HSERDY)); // Waits it stabilize

    // PLL configuration, with
    // - VCO clock = 128 MHz,
    // - P clock = 42.667 MHz (for ADC)
    // - R clock = 64 MHz (for SYSCLK)
    RCC->CR &= ~RCC_CR_PLLON; // Turns PLL off
    while (RCC->CR & RCC_CR_PLLRDY); // Waits it turn off
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM; // M = 1
    RCC->PLLCFGR = (RCC->PLLCFGR & ~RCC_PLLCFGR_PLLN) | (0b10000 << RCC_PLLCFGR_PLLN_Pos); // N = 16 -> 8 MHz * 16 = 128 MHz in VCO clock
    RCC->PLLCFGR |= 0b11 << RCC_PLLCFGR_PLLPDIV_Pos; // P = 3 -> 128 MHz / 3 = 42.667 MHz
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLR; // R = 2 -> 128 MHz / 2 = 64 MHz
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC; // Use HSE as PLL source clock

    RCC->PLLCFGR |= RCC_PLLCFGR_PLLPEN; // Enable P clock
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN; // Enable R clock

    RCC->CR |= RCC_CR_PLLON; // Turns PLL on
    while (!(RCC->CR & RCC_CR_PLLRDY)); // Waits it get ready :)

    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_PLL; // Use PLL as SYSCLK. Has to be done in one line, since writing 00 is not allowed
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);   // Waits the SYSCLK clock source change to R clock
    SystemCoreClockUpdate();
}

void _BoardSetup_GPIOC() {
    // --- BUILT-IN LED CONFIGURATION --- //
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN; // Enables AHB2 bus clock to GPIOC
    GPIOC->MODER &= ~GPIO_MODER_MODE6_1; // Configures PC6 as output. Its other configurations (speed, pull-up or down,
                                         // .etc) are the reset values.
}
void _BoardSetup_GPIOA() {
    // --- ANALOG INPUTS FOR ADC1 --- //
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN; // Enables AHB2 bus clock to GPIOA
    GPIOA->MODER |= GPIO_MODER_MODE2; // Configures PA2 as analog mode (ADC12_IN2). Check the datasheet.
    GPIOA->MODER |= GPIO_MODER_MODE3; // Configures PA3 as analog mode (ADC1_IN3). Check the datasheet.
}
