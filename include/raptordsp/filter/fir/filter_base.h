#pragma once

#include <raptordsp/defines.h>
#include <raptordsp/filter/filter_taps.h>
#include <raptordsp/filter/builder/builder.h>

template <class IN_T, class OUT_T, class TAP_T>
class raptor_filter {

public:
    raptor_filter();
    raptor_filter(raptor_filter const& src);
    ~raptor_filter();
    void configure(raptor_filter_builder_base* builder, int decimation);
    int process(const IN_T* input, OUT_T* output, int count);

private:
    void set_taps_from_builder(raptor_filter_builder_base* builder);
    void compute(OUT_T* result, const IN_T* input, const TAP_T* taps, unsigned int num_points);

    TAP_T* taps_buffer;
    IN_T* input_buffer;
    int ntaps;
    int decimation;

    int decimation_index;
    int offset;

};

//Filter definitions

typedef raptor_filter<float, float, float> raptor_filter_fff;
typedef raptor_filter<raptor_complex, raptor_complex, float> raptor_filter_ccf;
typedef raptor_filter<float, raptor_complex, raptor_complex> raptor_filter_fcc;
typedef raptor_filter<raptor_complex, raptor_complex, raptor_complex> raptor_filter_ccc;

// Backwards compatibility

typedef raptor_filter_fff raptor_filter_real;
typedef raptor_filter_ccc raptor_filter_complex;