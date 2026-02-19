#include "Filter.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>


void Filter_init(Filter* f, const double num[], const double den[], const double x0[], const double y0[], 
                   const unsigned num_x_sz, const unsigned den_y_sz) 
{
    memcpy(f->num, num, num_x_sz*sizeof(double));
    memcpy(f->den, den, den_y_sz*sizeof(double));
    memcpy(f->x, x0, num_x_sz*sizeof(double));
    memcpy(f->y, y0, den_y_sz*sizeof(double));
    f->num_x_sz = num_x_sz;
    f->den_y_sz = den_y_sz;
}

double Filter_input(Filter *f, double point) {
    double *num = f->num, *den = f->den, *x = f->x, *y = f->y;
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
    double sum = 0;
    double c = 0;
    for (int i = 0; i<num_x_sz; ++i) {  // Difference equation sum for x
        double sum_aux = fma(num[i]/den[0], x[num_x_sz-1 - i], sum);

        if (fabs(sum) > fabs(num[i]/den[0] * x[num_x_sz-1 - i]))
            c += fma(num[i]/den[0], x[num_x_sz-1 - i], sum-sum_aux);
        else
            c += fma(num[i]/den[0], x[num_x_sz-1 - i], -sum_aux) + sum;

        sum = sum_aux;
    }

    for (int i = 1; i<den_y_sz; ++i) {  // Difference equation sum for y
        double sum_aux = fma(-den[i]/den[0], y[den_y_sz-1 - i], sum);

        if (fabs(sum) > fabs(den[i]/den[0] * y[den_y_sz-1 - i]))
            c += fma(-den[i]/den[0], y[den_y_sz-1 - i], sum-sum_aux);
        else
            c += fma(-den[i]/den[0], y[den_y_sz-1 - i], -sum_aux) + sum;

        sum = sum_aux;
    }
    y[den_y_sz-1] = sum + c;
    */

    for (int i = 0; i<num_x_sz; ++i) {  // Difference equation sum for x
        y[den_y_sz-1] = fma(num[i]/den[0], x[num_x_sz-1 - i], y[den_y_sz-1]);
    }

    for (int i = 1; i<den_y_sz; ++i) {  // Difference equation sum for y
        y[den_y_sz-1] = fma(-den[i]/den[0], y[den_y_sz-1 - i], y[den_y_sz-1]);
    }

    return y[den_y_sz-1];   // return the new output point (y_n)
}
