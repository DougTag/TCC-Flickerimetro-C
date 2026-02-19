#include "LogHistogram1024.h"

LogHistogram1024* LogHistogram1024_new(double min, double max) {
    LogHistogram1024* lh = (LogHistogram1024*) malloc(sizeof(LogHistogram1024));
    lh->bins = (unsigned*) malloc(LOG_HISTOGRAM_BINS_QTY * sizeof(unsigned));
    memset(lh->bins, 0, LOG_HISTOGRAM_BINS_QTY * sizeof(unsigned));
    lh->min = min;
    lh->max = max;

    //lh->interval = pow(max/min, 1. / LOG_HISTOGRAM_BINS_QTY); // This was giving error for LOG_HISTOGRAM_BINS_QTY > 1024
    lh->interval = pow(10, (log10(max)-log10(min))/LOG_HISTOGRAM_BINS_QTY);

    lh->bounds_values[0] = min;
    for (int i = 1; i<LOG_HISTOGRAM_BINS_QTY; ++i) {
        lh->bounds_values[i] = lh->bounds_values[i-1] * lh->interval;
    }
    lh->bounds_values[LOG_HISTOGRAM_BINS_QTY] = max;

    return lh;
}

/* 
 * Binary search to find the bin index for a given sample value.
 */
static inline int get_bin_index(LogHistogram1024* lh, double sample) {
    
    int l = 0, r = LOG_HISTOGRAM_BINS_QTY+1;
    int ans = 0;

    while (l < r) {
        int m = l + (r-l)/2;
        if (sample >= lh->bounds_values[m]) {
            ans = l = m+1;
        }
        else {
            r = m;
        }
    }
    return ans < LOG_HISTOGRAM_BINS_QTY ? ans : LOG_HISTOGRAM_BINS_QTY-1;
}

void LogHistogram1024_insert(LogHistogram1024* lh, double sample) {
    int pos = get_bin_index(lh, sample);
    lh->bins[pos] += 1;
}

double LogHistogram1024_getQuantile(LogHistogram1024* lh, double q) {
    int total_samples = 0;
    for (int i = 0; i<LOG_HISTOGRAM_BINS_QTY; ++i) total_samples += lh->bins[i];
    double q_samples = q * total_samples;

    unsigned qtd = 0;
    for (int i = 0; i < LOG_HISTOGRAM_BINS_QTY; ++i) {
        if (qtd + lh->bins[i] >= q_samples) {
            return lh->bounds_values[i] * pow(lh->interval, (double) (q_samples - qtd) / lh->bins[i]);
        }

        qtd += lh->bins[i];
    }
    return lh->bounds_values[LOG_HISTOGRAM_BINS_QTY];
}

void* LogHistogram1024_delete(LogHistogram1024* lh) {
    free(lh->bins);
    free(lh);
    return NULL;
}