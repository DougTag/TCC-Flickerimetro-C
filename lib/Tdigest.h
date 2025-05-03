/*
    T-digest implementation by Douglas Sonntag Carvalho, 2025

    I used those first two references. [1] is way more understandable. Although
    [2] is the original paper, it doesn't say much about the implementation. 
    Still, it is more rigorous and gives the insights of the structure. This way,
    I opted to use the nomenclature from [2].

    Check [3] if you need another explanation, but I've not read it.

    [1] T-Digest in Python
        https://www.kuniga.me/blog/2021/11/29/t-digest.html
    [2] Computing Extremely Accurate Quantiles Using t-Digests
        https://arxiv.org/abs/1902.04023
    [3] Approximate percentiles with t-digests
        https://www.gresearch.com/news/approximate-percentiles-with-t-digests/
*/
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifndef TDIGEST_DELTA
#define TDIGEST_DELTA 200
#endif

typedef struct Cluster {
    float mean;
    unsigned count; // Num. of samples inside it
} Cluster;

typedef struct Tdigest {
    Cluster C[2*TDIGEST_DELTA+1];     // Array of Clusters, or bins
    unsigned C_sz;  // Number of clusters
    unsigned count; // Number of data samples
} Tdigest;


Tdigest* tdigest_new();


void tdigest_delete(Tdigest *td);


void tdigest_clear(Tdigest *td);


int tdigest_empty(Tdigest *td);


void tdigest_copy(Tdigest* dst, Tdigest* src);


void tdigest_merge(Tdigest *a, const Tdigest *b);


void tdigest_insert(Tdigest *td, const float const sorted_data[], unsigned sorted_data_sz);


float tdigest_query(Tdigest *td, float q);