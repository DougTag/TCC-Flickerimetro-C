#ifndef TIM_HEADER_GUARD
#define TIM_HEADER_GUARD

// TIM1 is dedicated to 10kHz counting, for ADC1 acquisitions
void TIM1_Init();
void TIM1_Start();
void TIM1_Stop();

#endif