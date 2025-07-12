#include "IEC_61547_1.h"

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
) {
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

    return instance;
}