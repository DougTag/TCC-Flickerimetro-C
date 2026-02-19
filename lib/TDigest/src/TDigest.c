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

#include "TDigest.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <string.h>

void TDigest_init(TDigest *td) {
    // After merging & before compressing, it'll use 
    // 2*delta at max. Check TDigest_merge().
    td->clusters_size = td->buffer_size = td->count = 0;
}


void TDigest_clear(TDigest *td) {
    td->clusters_size = td->buffer_size = td->count = 0;
}


int TDigest_empty(TDigest *td) {
    return td->clusters_size == 0 && td->buffer_size == 0;
}


void TDigest_copy(TDigest* dst, TDigest* src) {
    memcpy(dst, src, sizeof(TDigest));
}

// That's k1(q) from [2]
// Because of the optimizations of _TDigest_cluster_max_q_r, is not being used
static double _TDigest_scale_function(double q) { 
    // k0(q)
    //return TDIGEST_DELTA/2.f * q;
    // k1(q)
    q = fmin(q, TDIGEST_COS2_PI_DELTA);
    return TDIGEST_DELTA / (2.0*M_PI) * asin(2.0*q - 1.0);
    // k2(q)
}

// Inverse of k1(q) from [2]
// Because of the optimizations of _TDigest_cluster_max_q_r, is not being used
static double _TDigest_inv_scale_function(double k) {
    // k0^-1(q)
    //return 2.f/TDIGEST_DELTA * k;
    // k1^-1(q)
    return (sin(2.f*M_PI*k/TDIGEST_DELTA) + 1.f) / 2.f; 
    // k2^-1(q)
}


static double _TDigest_cluster_max_q_r(double q_l) {
    // Since we need that k(q_r) - k(q_l) <= 1, then we need that,
    // at max, k(q_r) = k(q_l) + 1, thus
    // q_r_max = k^-1 (k(q_l) + 1)
    //return _TDigest_inv_scale_function(_TDigest_scale_function(q_l)+1.f);
    // I've expanded this function, and got this way faster equation:
    q_l = fminf(q_l, TDIGEST_COS2_PI_DELTA);
    return (q_l-0.5f)*TDIGEST_COS_2PI_DELTA + sqrtf(-q_l*q_l + q_l)*TDIGEST_SIN_2PI_DELTA + 0.5f;
    // Less operations, and just one sqrt - it's faster than sin and cos.
}



static void _TDigest_compress(TDigest *td) {
    double q_l = 0.0, q_r;
    double q_r_max = _TDigest_cluster_max_q_r(0.0);
    unsigned partial_count = td->clusters[0].count;
    const unsigned total_count = td->count;

    int i = 0;
    for (int j = 1; j < td->clusters_size; ++j) {
        partial_count += td->clusters[j].count;
        q_r = partial_count / (double) total_count;

        if (q_r <= q_r_max) { // Then merge
            td->clusters[i].count += td->clusters[j].count;
            td->clusters[i].mean += (td->clusters[j].mean - td->clusters[i].mean) * td->clusters[j].count
                                    / td->clusters[i].count;
        }
        else { // Otherwise go to the next cluster
            ++i;
            td->clusters[i].mean = td->clusters[j].mean;
            td->clusters[i].count = td->clusters[j].count;
            q_l = (partial_count - td->clusters[j].count) / (double) total_count;
            q_r_max = _TDigest_cluster_max_q_r(q_l);
        }
    }

    td->clusters_size = i+1;
}

// Merges b into a. The same algorithm used for merging in Merge Sort.
void TDigest_merge(TDigest *a, const TDigest *b) {
    Cluster result[TDIGEST_DELTA+1];

    int i = 0, j = 0;

    while (i < a->clusters_size && j < b->clusters_size) {
        if (a->clusters[i].mean <= b->clusters[j].mean) {
            result[i+j] = a->clusters[i];
            ++i;
        }
        else {
            result[i+j] = b->clusters[j];
            ++j;
        }
    }

    for (; i < a->clusters_size; ++i)
        result[i+j] = a->clusters[i];

    for (; j < b->clusters_size; ++j)
        result[i+j] = b->clusters[j];

    memcpy(a->clusters, result, (i+j)*sizeof(Cluster));
    a->clusters_size += b->clusters_size;
    a->count += b->count;

    _TDigest_compress(a);
}


static int _TDigest_data_comp(const void *a_ptr, const void *b_ptr) {
    const double a = *(const double*) a_ptr, b = *(const double*) b_ptr;
    return (a>b) - (a<b);
}
// O(size(td)+size(buffer))
// Conceived to have better constant than separating it between inserting into an ordered array and then compressing.
// Each data point is considered to be a cluster.
// Then, does the compressing logic while inserting!
void _TDigest_merge_buffer(TDigest *td) {
    if (td->buffer_size <= 0) return;

    qsort((void*) td->buffer, td->buffer_size, sizeof(double), _TDigest_data_comp);

    Cluster result[TDIGEST_DELTA+1];

    unsigned r_count;
    const unsigned total_count = td->count + td->buffer_size;

    int i = 0; // Index for the buffer (td->buffer)
    int j = 0; // Index for the cluster array td->clusters
    int k = 0; // Index for the result

    if (td->clusters_size <= 0 || td->buffer[0] < td->clusters[0].mean) {
        result[0].mean = td->buffer[0];
        result[0].count = 1;
        r_count = 1;
        ++i;
    }
    else {
        result[0] = td->clusters[0];
        r_count = td->clusters[0].count;
        ++j;
    }

    double q_r = 0.0;
    double q_r_max = _TDigest_cluster_max_q_r(0.0);
    while (i < td->buffer_size || j < td->clusters_size) {
        Cluster cur_cluster;
        if (i >= td->buffer_size) {
            cur_cluster = td->clusters[j++];
        }
        else if (j >= td->clusters_size || td->buffer[i] < td->clusters[j].mean) {
            cur_cluster.count = 1;
            cur_cluster.mean = td->buffer[i++];
        }
        else {
            cur_cluster = td->clusters[j++];
        }

        r_count += cur_cluster.count;
        q_r = r_count / (double) total_count;

        if (q_r <= q_r_max) {
            result[k].count += cur_cluster.count;
            result[k].mean += (cur_cluster.mean - result[k].mean) * cur_cluster.count / result[k].count;
        }
        else {
            ++k;
            result[k] = cur_cluster;
            const double q_l = (r_count - cur_cluster.count) / (double) total_count;
            q_r_max = _TDigest_cluster_max_q_r(q_l);
        }
    }

    td->count = total_count;
    td->clusters_size = k+1;
    td->buffer_size = 0;
    memcpy(td->clusters, result, (td->clusters_size)*sizeof(Cluster));
}


void TDigest_insert(TDigest *td, const double point) {
    td->buffer[td->buffer_size++] = point;

    if(td->buffer_size < TDIGEST_BUFFER_SIZE) 
        return;
    else
        _TDigest_merge_buffer(td);
}


double TDigest_query(TDigest *td, double q) {
    // See [1] for explanation. Nomenclature used (x, w and q) came from [2], p. 13-14.
    // I didn't do the little optimizations for clusters with 1 sample, or for the last
    // and first bins, proposed in [2] p. 14
    if (td->buffer_size > 0) _TDigest_merge_buffer(td);

    const unsigned total_count = td->count;
    const double quantile_count = q * total_count;
    double partial_count = td->clusters[0].count / 2.0;  // It may contain whole or half a sample (i.e. 12 or 12.5), so
                                                        // I used double

    if (quantile_count < partial_count) // If it fell on the left of the first cluster (the smallest samples)
        return td->clusters[0].mean;

    for (int i = 1; i < td->clusters_size; ++i) { // If it fell in the middle of two consecutive clusters
        const double x0 = td->clusters[i-1].mean,  x1 = td->clusters[i].mean;
        const unsigned w0 = td->clusters[i-1].count, w1 = td->clusters[i].count;
        const double interval_count = (w0 + w1)/2.0;

        if (partial_count + interval_count > quantile_count) {
            const double slope = (quantile_count - partial_count) / interval_count;
            return x0 + slope*(x1-x0);
        }

        partial_count += interval_count;
    }

    return td->clusters[td->clusters_size-1].mean; // Fell on the right of the last cluster (the biggest samples)
}