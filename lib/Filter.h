#ifndef FILTER_HEADER_GUARD
#define FILTER_HEADER_GUARD

#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct Filter {
    float *x, *y;
    float *num, *den;
    unsigned num_x_sz, den_y_sz;
} Filter;


Filter* Filter_new(const float num[], const float den[], const float x0[], const float y0[],
                   const unsigned num_x_sz, const unsigned den_y_sz);

Filter* Filter_delete(Filter *f);

float Filter_input(Filter *f, float point);

#endif