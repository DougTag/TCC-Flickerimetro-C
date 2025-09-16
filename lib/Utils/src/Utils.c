#include "Utils.h"

#include <stm32g431xx.h>
#include <stdint.h>
#include "Timebase.h"

/*** Utils ***/

void delay_ms(unsigned ms) { // Waits AT LEAST the specified milliseconds
    uint32_t initial_counter = Timebase_GetTick();

    while (Timebase_GetTick() - initial_counter <= ms) { // Note it may wait an extra 1 ms because of the <=
        __NOP();
    }
}
