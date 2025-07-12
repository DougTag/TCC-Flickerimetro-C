#ifndef IEC_61547_1_HEADER_GUARD
#define IEC_61547_1_HEADER_GUARD

#include <stdlib.h>
#include "Filter.h"
#include "TDigest.h"

#define NETWORK_230V_50HZ

static float F_Ad_num[] = {4.9999750001250003943555934304843e-6, 4.9999750001250003943555934304843e-6};
static float F_Ad_den[] = {1.0, -0.99999000004999993862497831287328};

static float F_B_PAd_num[] = {0.99998429228346830122120536543662, -0.99998429228346830122120536543662};
static float F_B_PAd_den[] = {1.0, -0.99996858456693660244241073087323};

#if defined NETWORK_120V_60HZ
    static float F_B_PB1d_num[] = {};
    static float F_B_PB1d_den[] = {};

    static float F_B_PB2d_num[] = {};
    static float F_B_PB2d_den[] = {};

    static float F_B_PB3d_num[] = {};
    static float F_B_PB3d_den[] = {};

    static float F_B_ROC1d_num[] = {};
    static float F_B_ROC1d_den[] = {};

    static float F_B_ROC2d_num[] = {};
    static float F_B_ROC2d_den[] = {};
#elif defined NETWORK_230V_50HZ
    static float F_B_PB1d_num[] = {1.1920261655243667025822623672937e-4, 2.3912747285028898309305889213761e-4, 1.1992631369398782195147268314628e-4};
    static float F_B_PB1d_den[] = {1.0, -1.9579323230216028051131615939084, 0.95840579189736829768264669837663};

    static float F_B_PB2d_num[] = {1.1920261655243667025822623672937e-4, 2.384064733984661598516913150192e-4, 1.1920531805694794733312819046134e-4};
    static float F_B_PB2d_den[] = {1.0, -1.9689025211541926196900931245182, 0.96937868524911274814570560920401};

    static float F_B_PB3d_num[] = {1.1920261655243667025822623672937e-4, 2.376817530658640654529778490911e-4, 1.1848060143076954357603536305277e-4};
    static float F_B_PB3d_den[] = {1.0, -1.9882016421519090876302016113186, 0.98868246226584777236467971306411};

    static float F_B_ROC1d_num[] = {0.35163297992496200805589978699572, -0.66944172941026958145727121518576, 0.3178087494096303311508222577686};
    static float F_B_ROC1d_den[] = {1.0, -1.9855642294764010991059421940008, 0.9855747472225868666129144912702};

    static float F_B_ROC2d_num[] = {0.35163297992496200805589978699572, -0.70102706155250893438335424434626, 0.34939656593880558110853939979279};
    static float F_B_ROC2d_den[] = {1.0, -1.9948783165001813255656770706992, 0.99491132001847804033900501963217};
#else
    #error "System type not defined"
#endif

static float F_C_PBd_num[] = {1.6663889351774704215964005999e-4, 1.6663889351774704215964005999e-4};
static float F_C_PBd_den[] = {1.0, -0.99966672221296450591568071988002};


typedef struct IEC_61547_1 {
    // Block A
    Filter *F_Ad;
    // Block B
    Filter *F_B_PAd;
    Filter *F_B_PB1d, *F_B_PB2d, *F_B_PB3d;
    Filter *F_B_ROC1d, *F_B_ROC2d;
    // Block C
    Filter *F_C_PBd;
    float scaling_factor;
    // Block D
    TDigest* td;
} IEC_61547_1;


IEC_61547_1* IEC_61547_new(
    const float F_Ad_x[],       const float F_Ad_y[],
    const float F_B_PAd_x[],    const float F_B_PAd_y[],
    const float F_B_PB1d_x[],   const float F_B_PB1d_y[],
    const float F_B_PB2d_x[],   const float F_B_PB2d_y[],
    const float F_B_PB3d_x[],   const float F_B_PB3d_y[],
    const float F_B_ROC1d_x[],  const float F_B_ROC1d_y[],
    const float F_B_ROC2d_x[],  const float F_B_ROC2d_y[],
    const float F_C_PBd_x[],    const float F_C_PBd_y[],
    const float scaling_factor
);

#endif