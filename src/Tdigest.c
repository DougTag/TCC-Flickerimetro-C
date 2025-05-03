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

#include "tdigest.h"

Tdigest* tdigest_new() {
    Tdigest* td = (Tdigest*) malloc(sizeof(Tdigest));
    // After merging & before compressing, it'll use 
    // 2*delta at max. Check tdigest_merge().
    td->C_sz = td->count = 0;
    return td;
}


void tdigest_delete(Tdigest *td) {
    free(td);
}


void tdigest_clear(Tdigest *td) {
    td->C_sz = td->count = 0;
}


int tdigest_empty(Tdigest *td) {
    return td->C_sz >= 0;
}


void tdigest_copy(Tdigest* dst, Tdigest* src) {
    memcpy(dst, src, sizeof(Tdigest));
}

// That's k1(q) from [2]
float _tdigest_scale_function(float q) { 
    return TDIGEST_DELTA / (2*M_PI) * asinf(2*q - 1);
}

// Inverse of k1(q)
float _tdigest_inv_scale_function(float k) {
    return (sinf(2*M_PI*k/TDIGEST_DELTA) + 1) / 2.;
}


float _tdigest_cluster_max_q_r(float q_l) {
    // Since we need that k(q_r) - k(q_l) <= 1, then we need that,
    // at max, k(q_r) = k(q_l) + 1, thus
    // q_r_max = k^-1 (k(q_l) + 1)
    return _tdigest_inv_scale_function(_tdigest_scale_function(q_l)+1);
}


void _tdigest_compress(Tdigest *td) {
    float q_l = 0.0f, q_r;
    float q_r_max = _tdigest_cluster_max_q_r(0.0f);
    unsigned partial_count = td->C[0].count;
    const unsigned total_count = td->count;

    int i = 0;
    for (int j = 1; j<td->C_sz; ++j) {
        partial_count += td->C[j].count;
        q_r = partial_count / (float) total_count;

        if (q_r <= q_r_max) { // Then merge
            td->C[i].count += td->C[j].count;
            td->C[i].mean += (td->C[j].mean - td->C[i].mean) * td->C[j].count / td->C[i].count;
        }
        else { // Otherwise go to the next cluster
            ++i;
            td->C[i].mean = td->C[j].mean;
            td->C[i].count = td->C[j].count;
            q_l = (partial_count - td->C[j].count) / (float) total_count;
            q_r_max = _tdigest_cluster_max_q_r(q_l);
        }
    }

    td->C_sz = i;
}

// Merges b into a. The same algorithm used for merging in Merge Sort.
void tdigest_merge(Tdigest *a, const Tdigest *b) {
    Cluster result[2*TDIGEST_DELTA+1];

    int i = 0, j = 0;

    while (i < a->C_sz && j < b->C_sz) {
        if (a->C[i].mean <= b->C[j].mean) {
            result[i+j] = a->C[i];
            ++i;
        }
        else {
            result[i+j] = b->C[j];
            ++j;
        }
    }

    for (; i < a->C_sz; ++i)
        result[i+j] = a->C[i];

    for (; j < b->C_sz; ++j)
        result[i+j] = b->C[j];

    memcpy(a->C, result, (i+j)*sizeof(Cluster));
    a->C_sz += b->C_sz;
    a->count += b->count;

    _tdigest_compress(a);
}

// O(N)
void tdigest_insert(Tdigest *td, const float const sorted_data[], unsigned sorted_data_sz) {
    if (sorted_data_sz <= 0) return; 

    Cluster result[TDIGEST_DELTA+1];

    float q_l = 0.0f, q_r = 0.0f;
    float q_r_max = _tdigest_cluster_max_q_r(0.0f);
    const unsigned total_count = td->count + sorted_data_sz;
    unsigned partial_count; 

    int i = 0, j = 0, k = 0;

    if (td->C_sz <= 0 || sorted_data[0] < td->C[0].mean) {
        result[0].mean = sorted_data[0];
        result[0].count = 1;
        partial_count = 1;
        ++i;
    }
    else {
        result[0] = td->C[0];
        partial_count = td->C[0].count;
        ++j;
    }

    while (i < sorted_data_sz || j < td->C_sz) {
        Cluster cur_cluster;
        if (i >= sorted_data_sz) {
            cur_cluster = td->C[j++];
        }
        else if (j >= td->C_sz || sorted_data[i] < td->C[j].mean) {
            cur_cluster.count = 1;
            cur_cluster.mean = sorted_data[i++];
        }
        else {
            cur_cluster = td->C[j++];
        }

        partial_count += cur_cluster.count;
        q_r = (float) partial_count / total_count;

        if (q_r < q_r_max) {
            result[k].count += cur_cluster.count;
            result[k].mean += (cur_cluster.mean - result[k].mean) * cur_cluster.count / result[k].count;
        }
        else {
            ++k;
            result[k] = cur_cluster;
            q_l = (float) (partial_count - cur_cluster.count) / total_count;
            q_r_max = _tdigest_cluster_max_q_r(q_l);
        }
    }

    td->count = total_count;
    td->C_sz = k+1;
    memcpy(td->C, result, (td->C_sz)*sizeof(Cluster));
}


float tdigest_query(Tdigest *td, float q) {
    // See [1] for explanation. Nomenclature used (x, w and q) came from [2], p. 13-14.
    // I didn't do the little optimizations for clusters with 1 sample, or for the last
    // and first bins, proposed in [2] p. 14
    const unsigned total_count = td->count;
    const float quantile_count = q * td->count;
    float partial_count = td->C[0].count / 2.f; // It may contain whole or half a sample (i.e. 12 or 12.5), so I used float

    if (quantile_count < partial_count) // If it fell on the first samples
        return td->C[0].mean;

    for (int i = 1; i < td->C_sz; ++i) {
        const float x0 = td->C[i-1].mean,  x1 = td->C[i].mean;
        const unsigned w0 = td->C[i-1].count, w1 = td->C[i].count;
        const float interval_count = (w0 + w1)/2.0f;

        if (partial_count + interval_count > quantile_count) {
            const float slope = (quantile_count - partial_count) / interval_count;
            return x0 + slope*(x1-x0);
        }

        partial_count += interval_count;
    }

    return td->C[td->C_sz-1].mean; // Fell on the last samples
}