#ifndef ADC_HEADER_GUARD
#define ADC_HEADER_GUARD
#include <inttypes.h>

#define ADC1_QUEUE_MAX_SIZE 1536

extern volatile uint32_t ADC1_queue[ADC1_QUEUE_MAX_SIZE];
extern volatile uint32_t ADC1_queue_sz;

void ADC1_Init();

void ADC1_2_IRQHandler();

uint32_t ADC1_QueuePop();

int ADC1_QueueIsEmpty();
#endif