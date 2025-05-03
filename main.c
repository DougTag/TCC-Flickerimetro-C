// 15:32
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