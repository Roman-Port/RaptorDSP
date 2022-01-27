#pragma once

#include "../window.h"
#include "../../defines.h"

class raptor_filter_builder_base {

public:
    raptor_filter_builder_base(float sampleRate);

    float sampleRate;
    win_type window_type;
    double param;

    int get_ntaps();

    void automatic_tap_count(float transitionWidth, float attenuation = 30);
    virtual void set_ntaps(int tap);
    int calculate_decimation(float* actualOutputSampleRate);

    void build_taps_real(float* output);
    void build_taps_complex(raptor_complex* output);

protected:
    float transitionWidth;

    

    virtual float get_max_filter_cutoff() = 0;

    virtual void build_taps_real_internal(float* buffer, int ntaps);
    virtual void build_taps_complex_internal(raptor_complex* buffer, int ntaps);

private:
    int ntaps;

};