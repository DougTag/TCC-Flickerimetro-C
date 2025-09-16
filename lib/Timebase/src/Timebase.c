#include "Timebase.h"
#include <stm32g431xx.h>

static volatile uint32_t global_SysTickCounter = 0;

void Timebase_Init() {
    SystemCoreClockUpdate(); // This makes sure that SystemCoreClock is up to date after clock changes
    SysTick_Config(SystemCoreClock / 1000); // Equals to one interruption every 1 ms
}

void SysTick_Handler(void) {
    ++global_SysTickCounter;
}

uint32_t Timebase_GetTick() {
    return global_SysTickCounter;
}

