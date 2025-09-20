#ifndef FILTER_HEADER_GUARD
#define FILTER_HEADER_GUARD

typedef struct Filter {
    float x[16], y[16];
    float num[16], den[16];
    unsigned num_x_sz, den_y_sz;
} Filter;


void Filter_init(Filter* f, const float num[], const float den[], const float x0[], const float y0[],
                   const unsigned num_x_sz, const unsigned den_y_sz);

float Filter_input(Filter *f, float point);

#endif