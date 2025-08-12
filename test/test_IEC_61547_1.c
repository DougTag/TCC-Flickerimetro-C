// Escolha do tipo de rede:
#define NETWORK_230V_50HZ
//#define NETWORK_120V_60HZ

#define _USE_MATH_DEFINES

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "src/IEC_61547_1.h"

int sgn(float x) {
    return x<0 ? -1 : x>0;
}


int main() {
    const unsigned fs = 10000; // Não mude, se não será preciso recalcular os coeficientes dos filtros.
    const float Ts = 1.0l/fs;  // TODO: Fazer os filtros serem calculados a partir de fs.

    /*** Declaração dos filtros, determinados pelo código IEC_61547_1.m ***/

    IEC_61547_1* IEC = IEC_61547_new(fs);

    unsigned T_short = 5; // Em minutos. A norma diz que pode ser de 1 a 15 min.

    // Constants for the standardized illuminance waveform
    //float f_m = 0.325f, d_E = 2.5386/100.f;   // deu P_st = 1.000007987022400, 0.999508798122406
    float f_m = 0.9167f, d_E = 2.0473/100.f;  // deu P_st = 0.999999761581421, 1.000000000000000
    //float f_m = 8.8f,   d_E = 0.6832/100.f;   // deu P_st = 1.003168225288391, 1.000446319580078
    //float f_m = 13.5f,  d_E = 0.7780/100.f;   // deu P_st = 1.004138350486755, 1.001269340515137
    //float f_m = 33.3f,  d_E = 2.0027/100.f;   // deu P_st = 1.009302139282227, 1.006463170051575

    double P_inst_max = 0;

    for (int i = 0; i<T_short*60*fs; ++i) {
        // ENTRADA - Equação do sinal
        const float t = i*Ts;
        const float u_E = (1.f - 0.22f/2.f * cos(2*M_PI*100*t)) * (1 + d_E/2.f * sgn(sin(2*M_PI*f_m*t)));
        //const float u_E = 1 + 0.00630l * sin(2*M_PI*8.8*t);

        IEC_61547_insert(IEC, u_E);
    }

    //printf("P_inst_max: %.32le\n", P_inst_max);

    float P_st;
    P_st = IEC_61547_compute_P_st(IEC);

    printf("%f\n", P_st);

    IEC = IEC_61547_delete(IEC);

    return 0;
}
