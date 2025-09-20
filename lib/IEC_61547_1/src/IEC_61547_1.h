#ifndef IEC_61547_1_HEADER_GUARD
#define IEC_61547_1_HEADER_GUARD

#include "Filter.h"
#include "TDigest.h"

#define NETWORK_230V_50HZ

#define IEC_SCALING_FACTOR 1.101603155420234e+06
#define IEC_SAMPLING_FREQUENCY_HZ 2000

typedef struct IEC_61547_1 {
    float fs;
    unsigned sample_count;
    // BLOCO A
    Filter F_Ad;
    // BLOCO B
    Filter F_B_PAd;
    Filter F_B_PB1d, F_B_PB2d, F_B_PB3d;
    Filter F_B_ROC1d, F_B_ROC2d;
    // BLOCO C
    Filter F_C_PBd;
    float scaling_factor;
    // BLOCO D
    TDigest td;
} IEC_61547_1;

void IEC_61547_init(IEC_61547_1* IEC);

void IEC_61547_insert(IEC_61547_1* IEC, float data);

float IEC_61547_compute_P_st(IEC_61547_1* IEC);

#endif