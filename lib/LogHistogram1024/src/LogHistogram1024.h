#ifndef LOGHISTOGRAM_HEADER_GUARD
#define LOGHISTOGRAM_HEADER_GUARD

#include <stdlib.h> // malloc(), free()
#include <string.h> // memset()
#include <stdio.h>
#include <math.h> // log(), pow()
#define LOG_HISTOGRAM_BINS_QTY 1024


typedef struct LogHistogram1024
{
    unsigned* bins; 
    double min, max;

    // Pre-computed constants
    double bounds_values[LOG_HISTOGRAM_BINS_QTY+1];
    double interval;
} LogHistogram1024;

LogHistogram1024* LogHistogram1024_new(double min, double max);

void LogHistogram1024_insert(LogHistogram1024* log_hist, double sample);

double LogHistogram1024_getQuantile(LogHistogram1024* lh, double q); // q must be between 0 and 1

void* LogHistogram1024_delete(LogHistogram1024* h);

#endif  // LOGHISTOGRAM_HEADER_GUARD