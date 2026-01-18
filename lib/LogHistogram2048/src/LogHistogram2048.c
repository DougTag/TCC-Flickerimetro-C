#include "LogHistogram2048.h"

LogHistogram2048* LogHistogram2048_new(double min, double max) {
    LogHistogram2048* lh = (LogHistogram2048*) malloc(sizeof(LogHistogram2048));
    lh->bins = (unsigned*) malloc(LOG_HISTOGRAM_BINS_QTY * sizeof(unsigned));
    memset(lh->bins, 0, LOG_HISTOGRAM_BINS_QTY * sizeof(unsigned));
    lh->min = min;
    lh->max = max;

    lh->bounds_values[0] = min;
    lh->bounds_values[LOG_HISTOGRAM_BINS_QTY] = max;
    for (int i = 1; i<LOG_HISTOGRAM_BINS_QTY; ++i) {
        lh->bounds_values[i] = lh->bounds_values[i-1] * lh->interval;
    }

    lh->interval = pow(max/min, 1.f / LOG_HISTOGRAM_BINS_QTY);

    return lh;
}

/* 
 * Binary search to find the bin index for a given sample value.
 */
static inline int get_bin_index(LogHistogram2048* lh, double sample) {
    
    int l = 0, r = LOG_HISTOGRAM_BINS_QTY;

    while (l < r) {
        int m = l + (r-l)/2;
        if (sample >= lh->bounds_values[m+1]) {
            l = m+1;
        }
        else if (sample < lh->bounds_values[m]) {
            r = m;
        }
        else {
            return m;
        }
    }
    return LOG_HISTOGRAM_BINS_QTY-1;
}

void LogHistogram2048_insert(LogHistogram2048* lh, double sample) {
    int pos = get_bin_index(lh, sample);
    lh->bins[pos] += 1;
}

double LogHistogram2048_getQuantile(LogHistogram2048* lh, double q) {
    int total_samples = 0;
    for (int i = 0; i<LOG_HISTOGRAM_BINS_QTY; ++i) total_samples += lh->bins[i];
    double q_samples = q * total_samples;

    unsigned qtd = 0;
    for (int i = 0; i < LOG_HISTOGRAM_BINS_QTY; ++i) {
        if (qtd + lh->bins[i] >= q_samples) {
            double interval = lh->interval;
            return lh->bounds_values[i] * pow(lh->interval, (double) (q_samples - qtd) / lh->bins[i]);
        }

        qtd += lh->bins[i];
    }
    return lh->bounds_values[LOG_HISTOGRAM_BINS_QTY];
}

void* LogHistogram2048_delete(LogHistogram2048* lh) {
    free(lh->bins);
    free(lh);
    return NULL;
}