#include "BoardSetup.h"
#include <stm32g431xx.h>

void _BoardSetup_CoreClock();
void _BoardSetup_GPIOC();
void _BoardSetup_GPIOA();

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

    // FLASH configuration with 4 wait states, for up to 150 MHz in normal mode
    FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY) | FLASH_ACR_LATENCY_4WS;

    // Configure Voltage Scaling (Range 1 - High Performance)
    // We shouldn't need it for 140 MHz, but it may help. Better safe than sorry
    PWR->CR1 &= ~PWR_CR1_VOS; 
    PWR->CR1 |= PWR_CR1_VOS_0; // Range 1

    // PLL configuration, with
    // - VCO clock = 280 MHz,
    // - P clock = 46.666 MHz (for ADC). I need at least 10k * 256(oversampling) * 15 (cycles for 1 smp) = 38.4 MHz
    // - R clock = 140 MHz (for SYSCLK). With 64MHz I managed to proccess 5.7ksps, but I need at least 10ksps
    RCC->CR &= ~RCC_CR_PLLON; // Turns PLL off
    while (RCC->CR & RCC_CR_PLLRDY); // Waits it turn off
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM; // M = 1
    RCC->PLLCFGR = (RCC->PLLCFGR & ~RCC_PLLCFGR_PLLN) | (0b100011 << RCC_PLLCFGR_PLLN_Pos); // N = 35 -> 8 MHz * 35 = 280 MHz in VCO clock
    RCC->PLLCFGR |= 0b110 << RCC_PLLCFGR_PLLPDIV_Pos; // P = 6 -> 280 MHz / 6 = 46.667 MHz
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLR; // R = 2 -> 280 MHz / 2 = 140 MHz
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC; // Use HSE as PLL source clock

    RCC->PLLCFGR |= RCC_PLLCFGR_PLLPEN; // Enable P clock
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN; // Enable R clock

    RCC->CR |= RCC_CR_PLLON; // Turns PLL on
    while (!(RCC->CR & RCC_CR_PLLRDY)); // Waits it get ready :)

    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_HPRE) | RCC_CFGR_HPRE_DIV2;  // AHB prescaler = 2 (70 MHz)
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_PLL; // Use PLL as SYSCLK. Has to be done in one line, since writing 00 is not allowed
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);   // Waits the SYSCLK clock source change to R clock
    SystemCoreClockUpdate();

    for (int i = 70000000/1000; i >= 0; --i) // Just a 1ms delay to let the chip stabilize with the medium speed (70 MHz). Must be at least 1 us.
        __NOP(); 

    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_HPRE) | RCC_CFGR_HPRE_DIV1;  // AHB prescaler = 1 (140 MHz)

    SystemCoreClockUpdate();
}

void _BoardSetup_GPIOC() {
    // --- BUILT-IN LED CONFIGURATION --- //
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN; // Enables AHB2 bus clock to GPIOC
    GPIOC->MODER &= ~GPIO_MODER_MODE6_1; // Configures PC6 as output. Its other configurations (speed, pull-up or down,
                                         // .etc) are the reset values.
    GPIOC->MODER &= ~GPIO_MODER_MODE10_1;
    GPIOC->MODER &= ~GPIO_MODER_MODE11_1;
}
void _BoardSetup_GPIOA() {
    // --- ANALOG INPUTS FOR ADC1 --- //
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN; // Enables AHB2 bus clock to GPIOA
    GPIOA->MODER |= GPIO_MODER_MODE2; // Configures PA2 as analog mode (ADC12_IN2). Check the datasheet.
    GPIOA->MODER |= GPIO_MODER_MODE3; // Configures PA3 as analog mode (ADC1_IN3). Check the datasheet.
}
