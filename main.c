// Escolha do tipo de rede:
#define NETWORK_230V_50HZ
//#define NETWORK_120V_60HZ

#define _USE_MATH_DEFINES

#define S 3.896784172398884e+04f

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "IEC_61547_1.h"
#include <x86intrin.h>


int sgn(float x) {
    return x<0 ? -1 : x>0;
}

int main() {
    const unsigned fs = 10000; // Não mude, se não será preciso recalcular os coeficientes dos filtros.
    const float Ts = 1.0l/fs;

    /*** Declaração dos filtros, determinados pelo código IEC_61547_1.m ***/

    float F_Ad_num[] = {4.9999750001250003943555934304843e-6, 4.9999750001250003943555934304843e-6};
    float F_Ad_den[] = {1.0, -0.99999000004999993862497831287328};
    float F_Ad_x[] = {32768, 32768}, F_Ad_y[] = {32768, 32768}; // Chute inicial
    Filter* F_Ad = Filter_new(F_Ad_num, F_Ad_den, F_Ad_x, F_Ad_y, 2, 2);

    float F_B_PAd_num[] = {0.99998429228346830122120536543662, -0.99998429228346830122120536543662};
    float F_B_PAd_den[] = {1.0, -0.99996858456693660244241073087323};
    float F_B_PAd_x[] = {1, 1}, F_B_PAd_y[] = {0, 0}; // Chute inicial
    Filter* F_B_PAd = Filter_new(F_B_PAd_num, F_B_PAd_den, F_B_PAd_x, F_B_PAd_y, 2, 2);

#if defined NETWORK_120V_60HZ
    float F_B_PBd_num[] = {5.0148154825788315737748496392234e-12, 3.0088892895472989442649097835341e-11, 7.5222232238682470375448476803087e-11, 
                           1.0029630965157663147549699278447e-10, 7.5222232238682470375448476803087e-11, 3.0088892895472989442649097835341e-11,
                           5.0148154825788315737748496392234e-12};
    float F_B_PBd_den[] = {1.0, -5.8980457684944251894876288133673, 14.495413183558468972478294745088, -19.001028400452028677136695478112, 
                           14.011067343595261291966380667873, -5.5104733296428891620166723441798, 0.90306697175656369669383138898411};
    float F_B_PBd_x[] = {0, 0, 0, 0, 0, 0, 0}, F_B_PBd_y[] = {0, 0, 0, 0, 0, 0, 0}; // Chute inicial
    Filter* F_B_PBd = Filter_new(F_B_PBd_num, F_B_PBd_den, F_B_PBd_x, F_B_PBd_y, 7, 7);

    float F_B_ROCd_num[] = {0.043477924431573174157694694486054, -0.16945264503353579810251972048718, 0.24751732449448274331338382125978,
                            -0.16058838206702027995298465157248, 0.039045778174500146706638048499372};
    float F_B_ROCd_den[] = {1.0, -3.9804425459765844230730635899818, 5.941445094641930602108459424926,
                            -3.9415620210543300672156874497887, 0.98055947273610355452433395839762};
    float F_B_ROCd_x[] = {0, 0, 0, 0, 0}, F_B_ROCd_y[] = {0, 0, 0, 0, 0}; // Chute inicial
    Filter* F_B_ROCd = Filter_new(F_B_ROCd_num, F_B_ROCd_den, F_B_ROCd_x, F_B_ROCd_y, 5, 5);
#elif defined NETWORK_230V_50HZ
    float F_B_PB1d_num[] = {1.1920261655243667025822623672937e-4, 2.3912747285028898309305889213761e-4, 1.1992631369398782195147268314628e-4};
    float F_B_PB1d_den[] = {1.0, -1.9579323230216028051131615939084, 0.95840579189736829768264669837663};
    float F_B_PB1d_x[] = {0, 0, 0}, F_B_PB1d_y[] = {0, 0, 0}; // Chute inicial
    Filter* F_B_PB1d = Filter_new(F_B_PB1d_num, F_B_PB1d_den, F_B_PB1d_x, F_B_PB1d_y, 3, 3);

    float F_B_PB2d_num[] = {1.1920261655243667025822623672937e-4, 2.384064733984661598516913150192e-4, 1.1920531805694794733312819046134e-4};
    float F_B_PB2d_den[] = {1.0, -1.9689025211541926196900931245182, 0.96937868524911274814570560920401};
    float F_B_PB2d_x[] = {0, 0, 0}, F_B_PB2d_y[] = {0, 0, 0}; // Chute inicial
    Filter* F_B_PB2d = Filter_new(F_B_PB2d_num, F_B_PB2d_den, F_B_PB2d_x, F_B_PB2d_y, 3, 3);

    float F_B_PB3d_num[] = {1.1920261655243667025822623672937e-4, 2.376817530658640654529778490911e-4, 1.1848060143076954357603536305277e-4};
    float F_B_PB3d_den[] = {1.0, -1.9882016421519090876302016113186, 0.98868246226584777236467971306411};
    float F_B_PB3d_x[] = {0, 0, 0}, F_B_PB3d_y[] = {0, 0, 0}; // Chute inicial
    Filter* F_B_PB3d = Filter_new(F_B_PB3d_num, F_B_PB3d_den, F_B_PB3d_x, F_B_PB3d_y, 3, 3);

    float F_B_ROC1d_num[] = {0.35163297992496200805589978699572, -0.66944172941026958145727121518576, 0.3178087494096303311508222577686};
    float F_B_ROC1d_den[] = {1.0, -1.9855642294764010991059421940008, 0.9855747472225868666129144912702};
    float F_B_ROC1d_x[] = {0, 0, 0}, F_B_ROC1d_y[] = {0, 0, 0}; // Chute inicial
    Filter* F_B_ROC1d = Filter_new(F_B_ROC1d_num, F_B_ROC1d_den, F_B_ROC1d_x, F_B_ROC1d_y, 3, 3);

    float F_B_ROC2d_num[] = {0.35163297992496200805589978699572, -0.70102706155250893438335424434626, 0.34939656593880558110853939979279};
    float F_B_ROC2d_den[] = {1.0, -1.9948783165001813255656770706992, 0.99491132001847804033900501963217};
    float F_B_ROC2d_x[] = {0, 0, 0}, F_B_ROC2d_y[] = {0, 0, 0}; // Chute inicial
    Filter* F_B_ROC2d = Filter_new(F_B_ROC2d_num, F_B_ROC2d_den, F_B_ROC2d_x, F_B_ROC2d_y, 3, 3);
#else
    #error "System type not defined"
#endif

    float F_C_PBd_num[] = {1.6663889351774704215964005999e-4, 1.6663889351774704215964005999e-4};
    float F_C_PBd_den[] = {1.0, -0.99966672221296450591568071988002};
    float F_C_PBd_x[] = {1, 1}, F_C_PBd_y[] = {1, 1}; // Chute inicial
    Filter* F_C_PBd = Filter_new(F_C_PBd_num, F_C_PBd_den, F_C_PBd_x, F_C_PBd_y, 2, 2);

    unsigned T_short = 10; // 10 minutos. A norma diz que pode ser de 1 a 15 min.
    float f_m = 0.9167, d_E = 0.020473;
    double P_inst_mean = 0;
    TDigest *td = tdigest_new();

    long long totalClocks = 0;
    unsigned bufferSize = 0, bufferMaxSize = 1000;
    float buffer[bufferMaxSize];

    for (int i = 0; i<T_short*60*fs; ++i) {
        // ENTRADA - Equação do sinal
        float u_E, t = i*Ts;
        u_E = (1-0.22f/2 * cos(2*M_PI*100*t)) * (1 + d_E/2 * sgn(sin(2*M_PI*f_m*t)));
        //u_E = 1 + 0.00630l * sin(2*M_PI*8.8*t);

        unsigned long long clk1 = __rdtsc();

        // BLOCO A - Normalização
        float u_E_avg = Filter_input(F_Ad, u_E);
        float E = u_E/u_E_avg;


        // BLOCO B - Frequências de flicker visível e resposta olho-cérebro
        float Y_B1 = Filter_input(F_B_PAd, E);
        float Y_B2_1 = Filter_input(F_B_PB1d, Y_B1);
        float Y_B2_2 = Filter_input(F_B_PB2d, Y_B2_1);
        float Y_B2_3 = Filter_input(F_B_PB3d, Y_B2_2);
        float Y_B3_1 = Filter_input(F_B_ROC1d, Y_B2_3);
        float Y_B3_2 = Filter_input(F_B_ROC2d, Y_B3_1);


        // BLOCO C - Quadratura, média móvel e normalização
        float Y_Bquad = Y_B3_2 * Y_B3_2; // Quadratura
        float Y_C = Filter_input(F_C_PBd, Y_Bquad); // It's P_inst

        if (i < 60*fs) continue; // Descartar primeiro 1 min

        const float P_inst = S*Y_C;


        // BLOCO D - Análise estatística
        buffer[bufferSize] = P_inst;
        ++bufferSize;

        if (bufferSize == bufferMaxSize) {
            tdigest_insert(td, buffer, bufferSize);
            bufferSize = 0;
        }
        unsigned long long clk2 = __rdtsc();
        totalClocks += clk2 - clk1;
        
        P_inst_mean += Y_C;
    }

    printf("Media clocks/Sa: %lf\n", (double) totalClocks/((T_short-1)*60*fs));

    P_inst_mean *= S / (float) ( (T_short-1)*60*fs );

    printf("P_inst_mean: %.32le\n", P_inst_mean);

    float P_0p1 = tdigest_query(td, 1 - 0.1/100); float P_0p7 = tdigest_query(td, 1 - 0.7/100);
    float P_1   = tdigest_query(td, 1 - 0.7/100); float P_1p5 = tdigest_query(td, 1 - 1.5/100); 
    float P_2p2 = tdigest_query(td, 1 - 2.2/100); float P_3   = tdigest_query(td, 1 - 3.0/100);
    float P_4   = tdigest_query(td, 1 - 4.0/100); float P_6   = tdigest_query(td, 1 - 6.0/100);
    float P_8   = tdigest_query(td, 1 - 8.0/100); float P_10  = tdigest_query(td, 1 - 10./100);
    float P_13  = tdigest_query(td, 1 - 13./100); float P_17  = tdigest_query(td, 1 - 17./100);
    float P_30  = tdigest_query(td, 1 - 30./100); float P_50  = tdigest_query(td, 1 - 50./100);
    float P_80  = tdigest_query(td, 1 - 80./100);

    float P_1s  = (P_0p7 + P_1 + P_1p5) / 3;
    float P_3s  =  (P_2p2 + P_3 + P_4) / 3;
    float P_10s =  (P_6 + P_8 + P_10 + P_13 + P_17) / 5;
    float P_50s  =  (P_30 + P_50 + P_80) / 3;

    float P_st = sqrt(0.0314*P_0p1 + 0.052*P_1s + 0.0657*P_3s + 0.28*P_10s + 0.08*P_50s);

    printf("P_st: %f\n", P_st);

    F_Ad = Filter_delete(F_Ad);
    F_B_PAd = Filter_delete(F_B_PAd);
    F_B_PB1d = Filter_delete(F_B_PB1d);
    F_B_PB2d = Filter_delete(F_B_PB2d);
    F_B_PB3d = Filter_delete(F_B_PB3d);
    F_B_ROC1d = Filter_delete(F_B_ROC1d);
    F_B_ROC2d = Filter_delete(F_B_ROC2d);
    F_C_PBd = Filter_delete(F_C_PBd);

    return 0;
}

/*
#include "tdigest.h"
#include <stdio.h>

int comp(const void* a, const void* b) {
    float aa = *(const float*) a;
    float bb = *(const float*) b;
    return (aa > bb) - (aa < bb);
}

int main() {
    Tdigest* td1 = tdigest_new();
    Tdigest* td2 = tdigest_new();

    FILE* file = fopen("sinal.txt", "r");

    float aux[10000];
    unsigned counter = 0;
    while (fscanf(file, " %f", &aux[counter]) == 1) {
        counter++;
        if (counter == 10000) {
            qsort(aux, counter, sizeof(float), comp);
            tdigest_insert(td1, aux, counter);
            counter = 0;
        }
    }
    qsort(aux, counter, sizeof(float), comp);
    tdigest_insert(td1, aux, counter);
    fclose(file);

    printf(" -- Termino insercao\n");

    //tdigest_copy(td2, td1);

    printf(" -- Termino copia\n");

    //tdigest_merge(td1, td2);

    printf(" -- Termino merge\n");

    printf("Count: %d\n", td1->count);
    printf("Cluster count: %d\n", td1->C_sz);
    int p_count = 0;
    for (int i = 0; i<td1->C_sz; ++i) {
        p_count += td1->C[i].count;
        printf(" %f: %7d (%d)\n", td1->C[i].mean, td1->C[i].count, p_count);
    }

    float P[] = {0.1, 0.7, 1, 1.5, 2.2, 3, 4, 6, 8, 10, 13, 17, 30, 50, 80};
    
    for (int i = 0; i<sizeof(P)/sizeof(float); ++i) {
        printf("%f\n", tdigest_query(td1, (100-P[i])/100.));
    }

    tdigest_clear(td1);

    tdigest_delete(td1);
    tdigest_delete(td2);
}
*/