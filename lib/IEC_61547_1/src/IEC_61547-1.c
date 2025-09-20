#include "IEC_61547_1.h"

#include <stdlib.h>
#include <math.h>

static const float F_Ad_num[] = {2.49993750156246118241e-05, 2.49993750156246118241e-05};
static const float F_Ad_den[] = {1.00000000000000000000e+00, -9.99950001249968800643e-01};
static const float F_B_PAd_num[] = {9.99921466351678556173e-01, -9.99921466351678556173e-01};
static const float F_B_PAd_den[] = {1.00000000000000000000e+00, -9.99842932703357001323e-01};

#if defined NETWORK_120V_60HZ
static const  float F_B_PB1d_num[] = {6.39103544422572842693e-08, 1.28214235609465422611e-07, 6.43046907447774154932e-08};
static const float F_B_PB1d_den[] = {1.00000000000000000000e+00, -1.75940008582059936870e+00, 7.74782600169963031611e-01};
static const float F_B_PB2d_num[] = {1.00000000000000000000e+00, 1.99997162257658978035e+00, 9.99984202754113926659e-01};
static const float F_B_PB2d_den[] = {1.00000000000000000000e+00, -1.81413884089840027336e+00, 8.29999938712660201290e-01};
static const float F_B_PB3d_num[] = {1.00000000000000000000e+00, 1.99387089749101731329e+00, 9.93883390551590251150e-01};
static const float F_B_PB3d_den[] = {1.00000000000000000000e+00, -1.91746703645014937578e+00, 9.34231537318869098563e-01};

static const float F_B_ROC1d_num[] = {3.29182656277640164899e-02, -5.25586358418011420635e-02, 1.96403702140852398639e-02};
static const float F_B_ROC1d_den[] = {1.00000000000000000000e+00, -1.94267201520901866552e+00, 9.42903503574435863932e-01};
static const float F_B_ROC2d_num[] = {1.00000000000000000000e+00, -1.96636904429212222567e+00, 9.66593710330289623833e-01};
static const float F_B_ROC2d_den[] = {1.00000000000000000000e+00, -1.97335760337371812767e+00, 9.74159170947024755094e-01};

#elif defined NETWORK_230V_50HZ
static const float F_B_PB1d_num[] = {2.23310661894997042971e-08, 4.47974346401915448860e-08, 2.24666414749802995909e-08};
static const float F_B_PB1d_den[] = {1.00000000000000000000e+00, -1.79760003392422929025e+00, 8.08499709788220832785e-01};
static const float F_B_PB2d_num[] = {1.00000000000000000000e+00, 2.00001040491919290787e+00, 1.00002266313097321060e+00};
static const float F_B_PB2d_den[] = {1.00000000000000000000e+00, -1.84493398299358402070e+00, 8.56120666156118703682e-01};
static const float F_B_PB3d_num[] = {1.00000000000000000000e+00, 1.99393066981301525509e+00, 9.93942959118271351926e-01};
static const float F_B_PB3d_den[] = {1.00000000000000000000e+00, -1.93309859085714697002e+00, 9.44819856630583765345e-01};

static const float F_B_ROC1d_num[] = {5.05045849827754980699e-02, -8.06376654372727985631e-02, 3.01330804543759635250e-02};
static const float F_B_ROC1d_den[] = {1.00000000000000000000e+00, -1.92964503547832810781e+00, 9.29900588019778373550e-01};
static const float F_B_ROC2d_num[] = {1.00000000000000000000e+00, -1.96842594856799601999e+00, 9.68600349138119209869e-01};
static const float F_B_ROC2d_den[] = {1.00000000000000000000e+00, -1.97400123286491746555e+00, 9.74817849072579623027e-01};

#else
    #error "System type not defined. Use #define NETWORK_120V_60HZ or #define NETWORK_230V_50HZ."
#endif

static const float F_C_PBd_num[] = {8.32639467110741072868e-04, 8.32639467110741072868e-04};
static const float F_C_PBd_den[] = {1.00000000000000000000e+00, -9.98334721065778474269e-01};

void IEC_61547_init(IEC_61547_1* IEC) {
    float F_Ad_x[] = {1.f, 1.f}, F_Ad_y[] = {1.f, 1.f}; // 1 to avoid division by zero when doing E = u_E/u_E_avg
    float F_B_PAd_x[] = {0.f, 0.f}, F_B_PAd_y[] = {0.f, 0.f};
    float F_B_PB1d_x[] = {0.f, 0.f, 0.f}, F_B_PB1d_y[] = {0.f, 0.f, 0.f};
    float F_B_PB2d_x[] = {0.f, 0.f, 0.f}, F_B_PB2d_y[] = {0.f, 0.f, 0.f};
    float F_B_PB3d_x[] = {0.f, 0.f, 0.f}, F_B_PB3d_y[] = {0.f, 0.f, 0.f};
    float F_B_ROC1d_x[] = {0.f, 0.f, 0.f}, F_B_ROC1d_y[] = {0.f, 0.f, 0.f};
    float F_B_ROC2d_x[] = {0.f, 0.f, 0.f}, F_B_ROC2d_y[] = {0.f, 0.f, 0.f};
    float F_C_PBd_x[] = {0.f, 0.f}, F_C_PBd_y[] = {0.f, 0.f};
    float scaling_factor = IEC_SCALING_FACTOR;

    Filter_init(&IEC->F_Ad, F_Ad_num, F_Ad_den, F_Ad_x, F_Ad_y, 2, 2);
    Filter_init(&IEC->F_B_PAd, F_B_PAd_num, F_B_PAd_den, F_B_PAd_x, F_B_PAd_y, 2, 2);
    Filter_init(&IEC->F_B_PB1d, F_B_PB1d_num, F_B_PB1d_den, F_B_PB1d_x, F_B_PB1d_y, 3, 3);
    Filter_init(&IEC->F_B_PB2d, F_B_PB2d_num, F_B_PB2d_den, F_B_PB2d_x, F_B_PB2d_y, 3, 3);
    Filter_init(&IEC->F_B_PB3d, F_B_PB3d_num, F_B_PB3d_den, F_B_PB3d_x, F_B_PB3d_y, 3, 3);
    Filter_init(&IEC->F_B_ROC1d, F_B_ROC1d_num, F_B_ROC1d_den, F_B_ROC1d_x, F_B_ROC1d_y, 3, 3);
    Filter_init(&IEC->F_B_ROC2d, F_B_ROC2d_num, F_B_ROC2d_den, F_B_ROC2d_x, F_B_ROC2d_y, 3, 3);
    Filter_init(&IEC->F_C_PBd, F_C_PBd_num, F_C_PBd_den, F_C_PBd_x, F_C_PBd_y, 2, 2);
    IEC->scaling_factor = scaling_factor;
    TDigest_init(&IEC->td);
    IEC->fs = IEC_SAMPLING_FREQUENCY_HZ;
    IEC->sample_count = 0;
}

void IEC_61547_insert(IEC_61547_1* IEC, float u_E) {
        IEC->sample_count++;
        // BLOCO A - Normalização
        float u_E_avg = Filter_input(&IEC->F_Ad, u_E);
        float E = u_E/u_E_avg;

        // BLOCO B - Frequências de flicker visível e resposta olho-cérebro
        float Y_B1 = Filter_input(&IEC->F_B_PAd, E);

        float Y_B2_1 = Filter_input(&IEC->F_B_PB1d, Y_B1);
        float Y_B2_2 = Filter_input(&IEC->F_B_PB2d, Y_B2_1);
        float Y_B2_3 = Filter_input(&IEC->F_B_PB3d, Y_B2_2);

        float Y_B3_1 = Filter_input(&IEC->F_B_ROC1d, Y_B2_3);
        float Y_B3_2 = Filter_input(&IEC->F_B_ROC2d, Y_B3_1);

        // BLOCO C - Quadratura, média móvel e normalização
        float Y_Bquad = Y_B3_2 * Y_B3_2; // Quadratura
        float Y_C = Filter_input(&IEC->F_C_PBd, Y_Bquad);

        if (IEC->sample_count < 60 * IEC->fs) return; // Sempre descartar o primeiro 1 min, para acomodação dos filtros.

        const float P_inst = IEC->scaling_factor * Y_C;

        // BLOCO D.1 - Análise estatística (apenas inserção no T-Digest. O bloco termina no método seguinte)
        TDigest_insert(&IEC->td, P_inst);
}

float IEC_61547_compute_P_st(IEC_61547_1* IEC) {
    // BLOCO D.2 - Análise estatística (Cálculo dos percentis)
    float P_0p1 = TDigest_query(&IEC->td, 1 - 0.1f/100); float P_0p7 = TDigest_query(&IEC->td, 1 - 0.7f/100);
    float P_1   = TDigest_query(&IEC->td, 1 - 0.7f/100); float P_1p5 = TDigest_query(&IEC->td, 1 - 1.5f/100); 
    float P_2p2 = TDigest_query(&IEC->td, 1 - 2.2f/100); float P_3   = TDigest_query(&IEC->td, 1 - 3.f/100);
    float P_4   = TDigest_query(&IEC->td, 1 - 4.f/100);  float P_6   = TDigest_query(&IEC->td, 1 - 6.f/100);
    float P_8   = TDigest_query(&IEC->td, 1 - 8.f/100);  float P_10  = TDigest_query(&IEC->td, 1 - 10.f/100);
    float P_13  = TDigest_query(&IEC->td, 1 - 13.f/100); float P_17  = TDigest_query(&IEC->td, 1 - 17.f/100);
    float P_30  = TDigest_query(&IEC->td, 1 - 30.f/100); float P_50  = TDigest_query(&IEC->td, 1 - 50.f/100);
    float P_80  = TDigest_query(&IEC->td, 1 - 80.f/100);

    float P_1s  = (P_0p7 + P_1 + P_1p5) / 3.f;
    float P_3s  =  (P_2p2 + P_3 + P_4) / 3.f;
    float P_10s =  (P_6 + P_8 + P_10 + P_13 + P_17) / 5.f;
    float P_50s  =  (P_30 + P_50 + P_80) / 3.f;

    float P_st = sqrtf(0.0314f*P_0p1 + 0.052f*P_1s + 0.0657f*P_3s + 0.28f*P_10s + 0.08f*P_50s);
    return P_st;
}