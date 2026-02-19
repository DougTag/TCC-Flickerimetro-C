#include "IEC_61547_1.h"

#include <stdlib.h>
#include <math.h>

static const float F_Ad_num[] = {4.99997500012500039436e-06, 4.99997500012500039436e-06};
static const float F_Ad_den[] = {1.00000000000000000000e+00, -9.99990000049999938625e-01};
static const float F_B_PAd_num[] = {9.99984292283468301221e-01, -9.99984292283468301221e-01};
static const float F_B_PAd_den[] = {1.00000000000000000000e+00, -9.99968584566936602442e-01};

#if defined NETWORK_120V_60HZ
static const float F_B_PB1d_num[] = {5.01481548257883157377e-12, 1.00605095908563379194e-11, 5.04575763291548024617e-12};
static const float F_B_PB1d_den[] = {1.00000000000000000000e+00, -1.94961383760800233134e+00, 9.50292901538568601794e-01};
static const float F_B_PB2d_num[] = {1.00000000000000000000e+00, 1.99997162257658978035e+00, 9.99984202754113926659e-01};
static const float F_B_PB2d_den[] = {1.00000000000000000000e+00, -1.96268899065199309284e+00, 9.63372464234605874189e-01};
static const float F_B_PB3d_num[] = {1.00000000000000000000e+00, 1.99387089749101731329e+00, 9.93883390551590251150e-01};
static const float F_B_PB3d_den[] = {1.00000000000000000000e+00, -1.98574294023442865509e+00, 9.86434481412803765288e-01};
static const float F_B_ROC1d_num[] = {2.81715000601174755945e-02, -5.36331311034866226040e-02, 2.54616310394136964590e-02};
static const float F_B_ROC1d_den[] = {1.00000000000000000000e+00, -1.98830366154924820243e+00, 9.88313138068227514665e-01};
static const float F_B_ROC2d_num[] = {1.00000000000000000000e+00, -1.99321877410352987781e+00, 9.93227882963352848300e-01};
static const float F_B_ROC2d_den[] = {1.00000000000000000000e+00, -1.99474443908076937149e+00, 9.94776842938627336999e-01};

#elif defined NETWORK_230V_50HZ
static const float F_B_PB1d_num[] = {1.69378142340311014997e-12, 3.39782534187058204511e-12, 1.70406462699361226356e-12};
static const float F_B_PB1d_den[] = {1.00000000000000000000e+00, -1.95793232302160280511e+00, 9.58405791897368297683e-01};
static const float F_B_PB2d_num[] = {1.00000000000000000000e+00, 2.00001040491919290787e+00, 1.00002266313097321060e+00};
static const float F_B_PB2d_den[] = {1.00000000000000000000e+00, -1.96890252115419261969e+00, 9.69378685249112748146e-01};
static const float F_B_PB3d_num[] = {1.00000000000000000000e+00, 1.99393066981301525509e+00, 9.93942959118271351926e-01};
static const float F_B_PB3d_den[] = {1.00000000000000000000e+00, -1.98820164215190908763e+00, 9.88682462265847772365e-01};
static const float F_B_ROC1d_num[] = {4.34779244315731741577e-02, -8.27736264353034489893e-02, 3.92957019943730931355e-02};
static const float F_B_ROC1d_den[] = {1.00000000000000000000e+00, -1.98556422947640109911e+00, 9.85574747222586866613e-01};
static const float F_B_ROC2d_num[] = {1.00000000000000000000e+00, -1.99363285463754613680e+00, 9.93639919706525653886e-01};
static const float F_B_ROC2d_den[] = {1.00000000000000000000e+00, -1.99487831650018132557e+00, 9.94911320018478040339e-01};

#else
    #error "System type not defined. Use #define NETWORK_120V_60HZ or #define NETWORK_230V_50HZ."
#endif

static const float F_C_PBd_num[] = {1.66638893517747054820e-04, 1.66638893517747054820e-04};
static const float F_C_PBd_den[] = {1.00000000000000000000e+00, -9.99666722212964597993e-01};

void IEC_61547_init(IEC_61547_1* IEC) {
    double F_Ad_x[] = {1.0, 1.0}, F_Ad_y[] = {1.0, 1.0}; // 1 to avoid division by zero when doing E = u_E/u_E_avg
    double F_B_PAd_x[] = {0.0, 0.0}, F_B_PAd_y[] = {0.0, 0.0};
    double F_B_PB1d_x[] = {0.0, 0.0, 0.0}, F_B_PB1d_y[] = {0.0, 0.0, 0.0};
    double F_B_PB2d_x[] = {0.0, 0.0, 0.0}, F_B_PB2d_y[] = {0.0, 0.0, 0.0};
    double F_B_PB3d_x[] = {0.0, 0.0, 0.0}, F_B_PB3d_y[] = {0.0, 0.0, 0.0};
    double F_B_ROC1d_x[] = {0.0, 0.0, 0.0}, F_B_ROC1d_y[] = {0.0, 0.0, 0.0};
    double F_B_ROC2d_x[] = {0.0, 0.0, 0.0}, F_B_ROC2d_y[] = {0.0, 0.0, 0.0};
    double F_C_PBd_x[] = {0.0, 0.0}, F_C_PBd_y[] = {0.0, 0.0};
    double scaling_factor = IEC_SCALING_FACTOR;

    Filter_init(&IEC->F_Ad, F_Ad_num, F_Ad_den, F_Ad_x, F_Ad_y, 2, 2);
    Filter_init(&IEC->F_B_PAd, F_B_PAd_num, F_B_PAd_den, F_B_PAd_x, F_B_PAd_y, 2, 2);
    Filter_init(&IEC->F_B_PB1d, F_B_PB1d_num, F_B_PB1d_den, F_B_PB1d_x, F_B_PB1d_y, 3, 3);
    Filter_init(&IEC->F_B_PB2d, F_B_PB2d_num, F_B_PB2d_den, F_B_PB2d_x, F_B_PB2d_y, 3, 3);
    Filter_init(&IEC->F_B_PB3d, F_B_PB3d_num, F_B_PB3d_den, F_B_PB3d_x, F_B_PB3d_y, 3, 3);
    Filter_init(&IEC->F_B_ROC1d, F_B_ROC1d_num, F_B_ROC1d_den, F_B_ROC1d_x, F_B_ROC1d_y, 3, 3);
    Filter_init(&IEC->F_B_ROC2d, F_B_ROC2d_num, F_B_ROC2d_den, F_B_ROC2d_x, F_B_ROC2d_y, 3, 3);
    Filter_init(&IEC->F_C_PBd, F_C_PBd_num, F_C_PBd_den, F_C_PBd_x, F_C_PBd_y, 2, 2);
    IEC->scaling_factor = scaling_factor;
    IEC->lh = LogHistogram1024_new(IEC_MIN_P_INST, IEC_MAX_P_INST);
    IEC->fs = IEC_SAMPLING_FREQUENCY_HZ;
    IEC->sample_count = 0;
}

void IEC_61547_insert(IEC_61547_1* IEC, double u_E) {
        IEC->sample_count++;
        // BLOCO A - Normalização
        double u_E_avg = Filter_input(&IEC->F_Ad, u_E);
        double E = u_E/u_E_avg;

        // BLOCO B - Frequências de flicker visível e resposta olho-cérebro
        double Y_B1 = Filter_input(&IEC->F_B_PAd, E);

        double Y_B2_1 = Filter_input(&IEC->F_B_PB1d, Y_B1);
        double Y_B2_2 = Filter_input(&IEC->F_B_PB2d, Y_B2_1);
        double Y_B2_3 = Filter_input(&IEC->F_B_PB3d, Y_B2_2);

        double Y_B3_1 = Filter_input(&IEC->F_B_ROC1d, Y_B2_3);
        double Y_B3_2 = Filter_input(&IEC->F_B_ROC2d, Y_B3_1);

        // BLOCO C - Quadratura, média móvel e normalização
        double Y_Bquad = Y_B3_2 * Y_B3_2; // Quadratura
        double Y_C = Filter_input(&IEC->F_C_PBd, Y_Bquad);

        if (IEC->sample_count < 60 * IEC->fs) return; // Sempre descartar o primeiro 1 min, para acomodação dos filtros.

        const double P_inst = IEC->scaling_factor * Y_C;

        // BLOCO D.1 - Análise estatística (apenas inserção no histograma. O bloco termina no método seguinte)
        LogHistogram1024_insert(IEC->lh, P_inst);
}

double IEC_61547_compute_P_st(IEC_61547_1* IEC) {
    // BLOCO D.2 - Análise estatística (Cálculo dos percentis)
    double P_0p1 = LogHistogram1024_getQuantile(IEC->lh, 1 - 0.1/100);
    double P_0p7 = LogHistogram1024_getQuantile(IEC->lh, 1 - 0.7/100);
    double P_1   = LogHistogram1024_getQuantile(IEC->lh, 1 - 0.7/100);
    double P_1p5 = LogHistogram1024_getQuantile(IEC->lh, 1 - 1.5/100); 
    double P_2p2 = LogHistogram1024_getQuantile(IEC->lh, 1 - 2.2/100);
    double P_3   = LogHistogram1024_getQuantile(IEC->lh, 1 - 3.0/100);
    double P_4   = LogHistogram1024_getQuantile(IEC->lh, 1 - 4.0/100);
    double P_6   = LogHistogram1024_getQuantile(IEC->lh, 1 - 6.0/100);
    double P_8   = LogHistogram1024_getQuantile(IEC->lh, 1 - 8.0/100);
    double P_10  = LogHistogram1024_getQuantile(IEC->lh, 1 - 10.0/100);
    double P_13  = LogHistogram1024_getQuantile(IEC->lh, 1 - 13.0/100);
    double P_17  = LogHistogram1024_getQuantile(IEC->lh, 1 - 17.0/100);
    double P_30  = LogHistogram1024_getQuantile(IEC->lh, 1 - 30.0/100);
    double P_50  = LogHistogram1024_getQuantile(IEC->lh, 1 - 50.0/100);
    double P_80  = LogHistogram1024_getQuantile(IEC->lh, 1 - 80.0/100);

    double P_1s  = (P_0p7 + P_1 + P_1p5) / 3.0;
    double P_3s  = (P_2p2 + P_3 + P_4) / 3.0;
    double P_10s = (P_6 + P_8 + P_10 + P_13 + P_17) / 5.0;
    double P_50s = (P_30 + P_50 + P_80) / 3.0;

    double P_st = sqrtf(0.0314*P_0p1 + 0.052*P_1s + 0.0657*P_3s + 0.28*P_10s + 0.08*P_50s);
    return P_st;
}