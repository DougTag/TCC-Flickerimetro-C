/*      This header is responsible for setting up the board, so everything is available for use by other libraries.
 *      That means setting the clock sources and prescalers, core system configurations like debugging and voltage
 *  scaling, and so on.
 */

#ifndef BOARDSETUP_HEADER_GUARD
#define BOARDSETUP_HEADER_GUARD

#define SAMPLING_FREQUENCY_HZ 2000
#define SAMPLING_PERIOD_US 500

void BoardSetup_Init();

#endif