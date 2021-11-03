#ifndef RAPTORDSP_FILTER_BUILDER_BANDPASS
#define RAPTORDSP_FILTER_BUILDER_BANDPASS

#include "builder.h"

class raptor_filter_builder_bandpass : public raptor_filter_builder_base {

public:
    raptor_filter_builder_bandpass(float sampleRate, float lowCutoffFreq, float highCutoffFreq);

    float lowCutoffFreq;
    float highCutoffFreq;
    float gain;

    float* build_taps_real() override;
    raptor_complex* build_taps_complex() override;

protected:
    float get_max_filter_cutoff() override;

};

#endif
