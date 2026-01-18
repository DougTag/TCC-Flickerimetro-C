#ifndef LOGHISTOGRAM_HEADER_GUARD
#define LOGHISTOGRAM_HEADER_GUARD

#include <stdlib.h> // malloc(), free()
#include <string.h> // memset()
#include <stdio.h>
#include <math.h> // log(), pow()
#define LOG_HISTOGRAM_BINS_QTY 2048


typedef struct LogHistogram2048
{
    unsigned* bins; 
    double min, max;

    // Pre-computed constants
    double bounds_values[LOG_HISTOGRAM_BINS_QTY+1];
    double interval;
} LogHistogram2048;

LogHistogram2048* LogHistogram2048_new(double min, double max);

void LogHistogram2048_insert(LogHistogram2048* log_hist, double sample);

double LogHistogram2048_getQuantile(LogHistogram2048* lh, double q); // q must be between 0 and 1

void* LogHistogram2048_delete(LogHistogram2048* h);

#endif  // LOGHISTOGRAM_HEADER_GUARD