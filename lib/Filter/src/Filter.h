#ifndef FILTER_HEADER_GUARD
#define FILTER_HEADER_GUARD

typedef struct Filter {
    double x[16], y[16];
    double num[16], den[16];
    unsigned num_x_sz, den_y_sz;
} Filter;


void Filter_init(Filter* f, const double num[], const double den[], const double x0[], const double y0[],
                   const unsigned num_x_sz, const unsigned den_y_sz);

double Filter_input(Filter *f, double point);

#endif