#ifndef RAPTORDSP_FILTER_BUILDER
#define RAPTORDSP_FILTER_BUILDER

#include "../window.h"
#include "../../defines.h"

class raptor_filter_builder_base {

public:
    raptor_filter_builder_base(float sampleRate);

    float sampleRate;
    int ntaps;
    win_type window_type;
    double param;

    void automatic_tap_count(float transitionWidth, float attenuation = 30);
    void manual_tap_count(int taps);
    int calculate_decimation(float* actualOutputSampleRate);
    virtual float* build_taps_real() = 0;
    virtual raptor_complex* build_taps_complex();

protected:
    float transitionWidth;
    virtual float get_max_filter_cutoff() = 0;

};

#endif
