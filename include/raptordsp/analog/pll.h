#ifndef RAPTORDSP_ANALOG_PLL
#define RAPTORDSP_ANALOG_PLL

#include "../defines.h"

class raptor_pll {

public:
    void configure(float loop_bw, float max_freq, float min_freq);
    void process(raptor_complex* iptr, raptor_complex* optr, int count);
    float get_avg_error();

private:

    float phase_detector(raptor_complex sample, float ref_phase);
    float mod_2pi(float in);
    void frequency_limit();
    void phase_wrap();
    void advance_loop(float error);

    float d_phase, d_freq;
    float d_max_freq, d_min_freq;
    float d_damping, d_loop_bw;
    float d_alpha, d_beta;

    float d_alpha_inverse, d_avg_error;

};

#endif
