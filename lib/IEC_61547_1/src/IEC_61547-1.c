#include "IEC_61547_1.h"

static const float F_Ad_num[] = {4.99997500012500039436e-06, 4.99997500012500039436e-06};
static const float F_Ad_den[] = {1.00000000000000000000e+00, -9.99990000049999938625e-01};
static const float F_B_PAd_num[] = {9.99984292283468301221e-01, -9.99984292283468301221e-01};
static const float F_B_PAd_den[] = {1.00000000000000000000e+00, -9.99968584566936602442e-01};

#if defined NETWORK_120V_60HZ
    static const float F_B_PB1d_num[] = {};
    static const float F_B_PB1d_den[] = {};

    static const float F_B_PB2d_num[] = {};
    static const float F_B_PB2d_den[] = {};

    static const float F_B_PB3d_num[] = {};
    static const float F_B_PB3d_den[] = {};

    static const float F_B_ROC1d_num[] = {};
    static const float F_B_ROC1d_den[] = {};

    static const float F_B_ROC2d_num[] = {};
    static const float F_B_ROC2d_den[] = {};
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

IEC_61547_1* IEC_61547_new(float fs) {
    float F_Ad_x[] = {0.f, 0.f}, F_Ad_y[] = {0.f, 0.f};
    float F_B_PAd_x[] = {0.f, 0.f}, F_B_PAd_y[] = {0.f, 0.f};
    float F_B_PB1d_x[] = {0.f, 0.f, 0.f}, F_B_PB1d_y[] = {0.f, 0.f, 0.f};
    float F_B_PB2d_x[] = {0.f, 0.f, 0.f}, F_B_PB2d_y[] = {0.f, 0.f, 0.f};
    float F_B_PB3d_x[] = {0.f, 0.f, 0.f}, F_B_PB3d_y[] = {0.f, 0.f, 0.f};
    float F_B_ROC1d_x[] = {0.f, 0.f, 0.f}, F_B_ROC1d_y[] = {0.f, 0.f, 0.f};
    float F_B_ROC2d_x[] = {0.f, 0.f, 0.f}, F_B_ROC2d_y[] = {0.f, 0.f, 0.f};
    float F_C_PBd_x[] = {0.f, 0.f}, F_C_PBd_y[] = {0.f, 0.f};
    float scaling_factor = IEC_SCALING_FACTOR;

    IEC_61547_1* instance = (IEC_61547_1*) malloc(sizeof(IEC_61547_1));

    instance->F_Ad = Filter_new(F_Ad_num, F_Ad_den, F_Ad_x, F_Ad_y, 2, 2);
    instance->F_B_PAd = Filter_new(F_B_PAd_num, F_B_PAd_den, F_B_PAd_x, F_B_PAd_y, 2, 2);
    instance->F_B_PB1d = Filter_new(F_B_PB1d_num, F_B_PB1d_den, F_B_PB1d_x, F_B_PB1d_y, 3, 3);
    instance->F_B_PB2d = Filter_new(F_B_PB2d_num, F_B_PB2d_den, F_B_PB2d_x, F_B_PB2d_y, 3, 3);
    instance->F_B_PB3d = Filter_new(F_B_PB3d_num, F_B_PB3d_den, F_B_PB3d_x, F_B_PB3d_y, 3, 3);
    instance->F_B_ROC1d = Filter_new(F_B_ROC1d_num, F_B_ROC1d_den, F_B_ROC1d_x, F_B_ROC1d_y, 3, 3);
    instance->F_B_ROC2d = Filter_new(F_B_ROC2d_num, F_B_ROC2d_den, F_B_ROC2d_x, F_B_ROC2d_y, 3, 3);
    instance->F_C_PBd = Filter_new(F_C_PBd_num, F_C_PBd_den, F_C_PBd_x, F_C_PBd_y, 2, 2);
    instance->scaling_factor = scaling_factor;
    instance->td = TDigest_new();
    instance->fs = fs;
    instance->sample_count = 0;

    return instance;
}

void IEC_61547_insert(IEC_61547_1* IEC, float u_E) {
        IEC->sample_count++;
        // BLOCO A - Normalização
        float u_E_avg = Filter_input(IEC->F_Ad, u_E);
        float E = u_E/u_E_avg;

        // BLOCO B - Frequências de flicker visível e resposta olho-cérebro
        float Y_B1 = Filter_input(IEC->F_B_PAd, E);

        float Y_B2_1 = Filter_input(IEC->F_B_PB1d, Y_B1);
        float Y_B2_2 = Filter_input(IEC->F_B_PB2d, Y_B2_1);
        float Y_B2_3 = Filter_input(IEC->F_B_PB3d, Y_B2_2);

        float Y_B3_1 = Filter_input(IEC->F_B_ROC1d, Y_B2_3);
        float Y_B3_2 = Filter_input(IEC->F_B_ROC2d, Y_B3_1);

        // BLOCO C - Quadratura, média móvel e normalização
        float Y_Bquad = Y_B3_2 * Y_B3_2; // Quadratura
        float Y_C = Filter_input(IEC->F_C_PBd, Y_Bquad);

        if (IEC->sample_count < 60 * IEC->fs) return; // Sempre descartar o primeiro 1 min, para acomodação dos filtros.

        const float P_inst = IEC->scaling_factor * Y_C;

        // BLOCO D.1 - Análise estatística (apenas inserção no T-Digest. O bloco termina no método seguinte)
        TDigest_insert(IEC->td, P_inst);

        //P_inst_max = P_inst_max < P_inst ? P_inst : P_inst_max;
}

float IEC_61547_compute_P_st(IEC_61547_1* IEC) {
    // BLOCO D.2 - Análise estatística (Cálculo dos percentis)
    float P_0p1 = TDigest_query(IEC->td, 1 - 0.1f/100); float P_0p7 = TDigest_query(IEC->td, 1 - 0.7f/100);
    float P_1   = TDigest_query(IEC->td, 1 - 0.7f/100); float P_1p5 = TDigest_query(IEC->td, 1 - 1.5f/100); 
    float P_2p2 = TDigest_query(IEC->td, 1 - 2.2f/100); float P_3   = TDigest_query(IEC->td, 1 - 3.f/100);
    float P_4   = TDigest_query(IEC->td, 1 - 4.f/100);  float P_6   = TDigest_query(IEC->td, 1 - 6.f/100);
    float P_8   = TDigest_query(IEC->td, 1 - 8.f/100);  float P_10  = TDigest_query(IEC->td, 1 - 10.f/100);
    float P_13  = TDigest_query(IEC->td, 1 - 13.f/100); float P_17  = TDigest_query(IEC->td, 1 - 17.f/100);
    float P_30  = TDigest_query(IEC->td, 1 - 30.f/100); float P_50  = TDigest_query(IEC->td, 1 - 50.f/100);
    float P_80  = TDigest_query(IEC->td, 1 - 80.f/100);

    float P_1s  = (P_0p7 + P_1 + P_1p5) / 3.f;
    float P_3s  =  (P_2p2 + P_3 + P_4) / 3.f;
    float P_10s =  (P_6 + P_8 + P_10 + P_13 + P_17) / 5.f;
    float P_50s  =  (P_30 + P_50 + P_80) / 3.f;

    float P_st = sqrtf(0.0314f*P_0p1 + 0.052f*P_1s + 0.0657f*P_3s + 0.28f*P_10s + 0.08f*P_50s);
    return P_st;
}

IEC_61547_1* IEC_61547_delete(IEC_61547_1* instance) {
    Filter_delete(instance->F_Ad);
    Filter_delete(instance->F_B_PAd);
    Filter_delete(instance->F_B_PB1d);
    Filter_delete(instance->F_B_PB2d);
    Filter_delete(instance->F_B_PB3d);
    Filter_delete(instance->F_B_ROC1d);
    Filter_delete(instance->F_B_ROC2d);
    Filter_delete(instance->F_C_PBd);
    TDigest_delete(instance->td);
    free(instance);
    return NULL;
}