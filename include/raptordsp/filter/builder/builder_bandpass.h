#pragma once

#include "builder.h"

class raptor_filter_builder_bandpass : public raptor_filter_builder_base {

public:
    raptor_filter_builder_bandpass(float sampleRate, float lowCutoffFreq, float highCutoffFreq);

    float lowCutoffFreq;
    float highCutoffFreq;
    float gain;

protected:
    float get_max_filter_cutoff() override;

    virtual void build_taps_real_internal(float* buffer, int ntaps) override;
    virtual void build_taps_complex_internal(raptor_complex* buffer, int ntaps) override;
    

};
