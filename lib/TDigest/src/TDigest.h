#ifndef TDIGEST_HEADER_GUARD
#define TDIGEST_HEADER_GUARD
/*
    T-digest implementation by Douglas Sonntag Carvalho, 2025
    Implemented for the Computer Engineering course's final thesis:
        Designing of a flickermeter for rating flicker of artificial lighting systems

    I used those first two references. [1] is way more understandable. Although [2] is the original paper, it doesn't
    say much about the implementation. Still, it is more rigorous and gives the insights of the structure. This way,
    I opted to use the nomenclature from [2].

    Check [3] if you need another explanation or another way of seeing this data structure, but I've not read it.

    [1] T-Digest in Python
        https://www.kuniga.me/blog/2021/11/29/t-digest.html
    [2] Computing Extremely Accurate Quantiles Using t-Digests
        https://arxiv.org/abs/1902.04023
    [3] Approximate percentiles with t-digests
        https://www.gresearch.com/news/approximate-percentiles-with-t-digests/
*/
#ifndef TDIGEST_DELTA // Increase up to ~200 for better precision at the expense of processing resources
#define TDIGEST_DELTA 100  
#define TDIGEST_COS_2PI_DELTA 0.99802672842827156195 // cos(2*pi/TDIGEST_DELTA). Recalc it if TDIGEST_DELTA changes!
#define TDIGEST_SIN_2PI_DELTA 0.06279051952931337607 // sin(2*pi/TDIGEST_DELTA). Recalc it if TDIGEST_DELTA changes!
#define TDIGEST_COS2_PI_DELTA 0.99901336421413578097 // cos^2(pi/TDIGEST_DELTA). Recalc it if TDIGEST_DELTA changes!

#endif

#ifndef TDIGEST_BUFFER_SIZE // The bigger, the better
#define TDIGEST_BUFFER_SIZE 2048 
#endif

typedef struct Cluster {
    float mean;
    unsigned count; // Num. of samples inside it
} Cluster;

typedef struct TDigest {
    unsigned count; // Total number of data samples

    Cluster clusters[2*TDIGEST_DELTA+4];    // Array of Clusters, or bins
    unsigned clusters_size;

    float buffer[TDIGEST_BUFFER_SIZE+4];
    unsigned buffer_size;
} TDigest;


void TDigest_init(TDigest *td);

void TDigest_clear(TDigest *td);

int TDigest_empty(TDigest *td);

void TDigest_copy(TDigest* dst, TDigest* src);

void TDigest_merge(TDigest *a, const TDigest *b);

void TDigest_insert(TDigest *td, const float point);

float TDigest_query(TDigest *td, float q);

#endif // TDIGEST_HEADER_GUARD