#include "Timebase.h"
#include <stm32g431xx.h>

static volatile uint32_t global_SysTickCounter = 0;

void Timebase_Init() {
    SystemCoreClockUpdate(); // This makes sure that SystemCoreClock is up to date after clock changes
    SysTick_Config(SystemCoreClock / 1000); // Equals to one interruption every 1 ms
}

void SysTick_Handler(void) {
    ++global_SysTickCounter; // 4e9 ms = 4e6 s = 46.3 days until it overflows, which is good enough for this project :)
}

uint32_t Timebase_GetTick() {
    return global_SysTickCounter;
}

