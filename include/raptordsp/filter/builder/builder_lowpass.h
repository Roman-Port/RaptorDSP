#pragma once

#include "builder.h"

class raptor_filter_builder_lowpass : public raptor_filter_builder_base {

public:
    raptor_filter_builder_lowpass(float sampleRate, float cutoffFreq);

    float cutoffFreq;
    float gain;

protected:
    float get_max_filter_cutoff() override;
    virtual void build_taps_real_internal(float* buffer, int ntaps) override;

};
