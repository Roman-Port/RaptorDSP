#ifndef RAPTORDSP_FILTER_BUILDER_ROOTRAISED
#define RAPTORDSP_FILTER_BUILDER_ROOTRAISED

#include "builder.h"

class raptor_filter_builder_root_raised_cosine : public raptor_filter_builder_base {

public:
    raptor_filter_builder_root_raised_cosine(float sampleRate, float symbolRate, float alpha);

    float symbolRate;
    float gain;
    float alpha;

    float* build_taps_real() override;

protected:
    float get_max_filter_cutoff() override;

};

#endif
