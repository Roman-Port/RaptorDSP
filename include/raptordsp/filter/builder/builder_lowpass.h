#ifndef RAPTORDSP_FILTER_BUILDER_LOWPASS
#define RAPTORDSP_FILTER_BUILDER_LOWPASS

#include "builder.h"

class raptor_filter_builder_lowpass : public raptor_filter_builder_base {

public:
    raptor_filter_builder_lowpass(float sampleRate, float cutoffFreq);

    float cutoffFreq;
    float gain;

    float* build_taps_real() override;

protected:
    float get_max_filter_cutoff() override;

};

#endif
