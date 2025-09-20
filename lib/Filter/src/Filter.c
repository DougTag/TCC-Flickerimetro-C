#include "Filter.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>


void Filter_init(Filter* f, const float num[], const float den[], const float x0[], const float y0[], 
                   const unsigned num_x_sz, const unsigned den_y_sz) 
{
    memcpy(f->num, num, num_x_sz*sizeof(float));
    memcpy(f->den, den, den_y_sz*sizeof(float));
    memcpy(f->x, x0, num_x_sz*sizeof(float));
    memcpy(f->y, y0, den_y_sz*sizeof(float));
    f->num_x_sz = num_x_sz;
    f->den_y_sz = den_y_sz;
}

float Filter_input(Filter *f, float point) {
    float *num = f->num, *den = f->den, *x = f->x, *y = f->y;
    unsigned num_x_sz = f->num_x_sz, den_y_sz = f->den_y_sz;

    for (int i = 1; i<num_x_sz; ++i) {  // pop front and ...
        x[i-1] = x[i];
    }
    x[num_x_sz-1] = point;              // ... push back

    for (int i = 1; i<den_y_sz; ++i) {  // pop front and ...
        y[i-1] = y[i];
    }
    y[den_y_sz-1] = 0;                  // ... push back

    // For the summation, this uses the Kahan-Babushka-Neumaier Sum 
    // for reducing the sum error of large values with small values.
    // Also, I'm using fma (fused multiply-add), so it does just one
    // rounding instead of two when multiplying and adding.
    // https://en.wikipedia.org/wiki/Kahan_summation_algorithm#Further_enhancements
    /*
    // This was not used because the processing cost was too much for keeping Fs >= 2kHz.
    float sum = 0;
    float c = 0;
    for (int i = 0; i<num_x_sz; ++i) {  // Difference equation sum for x
        float sum_aux = fmal(num[i]/den[0], x[num_x_sz-1 - i], sum);

        if (fabsl(sum) > fabsl(num[i]/den[0] * x[num_x_sz-1 - i]))
            c += fmal(num[i]/den[0], x[num_x_sz-1 - i], sum-sum_aux);
        else
            c += fmal(num[i]/den[0], x[num_x_sz-1 - i], -sum_aux) + sum;

        sum = sum_aux;
    }

    for (int i = 1; i<den_y_sz; ++i) {  // Difference equation sum for y
        float sum_aux = fmal(-den[i]/den[0], y[den_y_sz-1 - i], sum);

        if (fabsl(sum) > fabsl(den[i]/den[0] * y[den_y_sz-1 - i]))
            c += fmal(-den[i]/den[0], y[den_y_sz-1 - i], sum-sum_aux);
        else
            c += fmal(-den[i]/den[0], y[den_y_sz-1 - i], -sum_aux) + sum;

        sum = sum_aux;
    }
    y[den_y_sz-1] = sum + c;
    */

    for (int i = 0; i<num_x_sz; ++i) {  // Difference equation sum for x
        y[den_y_sz-1] = fmaf(num[i]/den[0], x[num_x_sz-1 - i], y[den_y_sz-1]);
    }

    for (int i = 1; i<den_y_sz; ++i) {  // Difference equation sum for y
        y[den_y_sz-1] = fmaf(-den[i]/den[0], y[den_y_sz-1 - i], y[den_y_sz-1]);
    }

    return y[den_y_sz-1];   // return the new output point (y_n)
}
