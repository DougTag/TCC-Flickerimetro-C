#include "lib/TDigest.h"
#include <stdlib.h>
#include <stdio.h>

#include <direct.h>   // For _getcwd on Windows

int main() {
    float percentiles[] = {0.1, 0.7, 1, 1.5, 2.2, 3, 4, 6, 8, 10, 13, 17, 30, 50, 80};

    FILE* normal_file = fopen("data/normal_data.txt", "r");
    TDigest* normal_td = TDigest_new();
    printf("Normal distribution percentiles:\n");

    for (double value; fscanf(normal_file, " %lf", &value) == 1; ) {
        TDigest_insert(normal_td, value);
    }

    for (int i = 0; i<sizeof(percentiles)/sizeof(float); ++i) {
        printf("%.1f-th;%f\n", percentiles[i], TDigest_query(normal_td, percentiles[i]/100));
    }

    FILE* uniform_file = fopen("data/uniform_data.txt", "r");
    TDigest* uniform_td = TDigest_new();
    printf("\nUniform distribution percentiles:\n");

    for (float value; fscanf(uniform_file, " %f", &value) == 1; ) {
        TDigest_insert(uniform_td, value);
    }

    for (int i = 0; i<sizeof(percentiles)/sizeof(float); ++i) {
        printf("%.1f-th;%f\n", percentiles[i], TDigest_query(uniform_td, percentiles[i]/100));
    }

    FILE* exponential_file = fopen("data/exponential_data.txt", "r");
    TDigest* exponential_td = TDigest_new();
    printf("\nExponential distribution percentiles:\n");

    for (float value; fscanf(exponential_file, " %f", &value) == 1; ) {
        TDigest_insert(exponential_td, value);
    }

    for (int i = 0; i<sizeof(percentiles)/sizeof(float); ++i) {
        printf("%.1f-th;%f\n", percentiles[i], TDigest_query(exponential_td, percentiles[i]/100));
    }

    float compliment_percentiles[] = {99.9, 99.3, 99, 98.5, 97.8, 97, 96, 94, 92, 90, 87, 83, 70, 50, 20};
    FILE* signal_file = fopen("data/signal.txt", "r");
    TDigest* signal_td = TDigest_new();
    printf("\nSignal percentiles:\n");

    for (float value; fscanf(signal_file, " %f", &value) == 1; ) {
        TDigest_insert(signal_td, value);
    }

    for (int i = 0; i<sizeof(compliment_percentiles)/sizeof(float); ++i) {
        printf("%.1f-th;%f\n", compliment_percentiles[i], TDigest_query(signal_td, compliment_percentiles[i]/100));
    }

    TDigest_delete(normal_td);
    TDigest_delete(uniform_td);
    TDigest_delete(exponential_td);
    TDigest_delete(signal_td);

    fclose(normal_file);
    fclose(uniform_file);
    fclose(exponential_file);
    fclose(signal_file);

    return 0;
}