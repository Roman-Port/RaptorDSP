#ifndef RAPTORDSP_DIGITAL_CLOCKRECOVERYMM
#define RAPTORDSP_DIGITAL_CLOCKRECOVERYMM

#include "../defines.h"

class raptor_clock_recovery_mm {

public:
    raptor_clock_recovery_mm();
    void configure(float omega, float gain_omega, float mu, float gain_mu, float omega_relative_limit);
    void set_omega(float omega);
    int process(raptor_complex* in, raptor_complex* out, int count);

private:
    float d_mu;                   // fractional sample position [0.0, 1.0]
    float d_omega;                // nominal frequency
    float d_gain_omega;           // gain for adjusting omega
    float d_omega_relative_limit; // used to compute min and max omega
    float d_omega_mid;            // average omega
    float d_omega_lim;            // actual omega clipping limit
    float d_gain_mu;              // gain for adjusting mu

    int process_block(raptor_complex* out);
    void interpolate(raptor_complex* input, raptor_complex* output);

    raptor_complex d_last_sample;

    raptor_complex d_p_2T, d_p_1T, d_p_0T;
    raptor_complex d_c_2T, d_c_1T, d_c_0T;

    raptor_complex* buffer;
    int bufferLen;
    int bufferUse;

    raptor_complex slicer_0deg(raptor_complex sample)
    {
        float real = 0.0f, imag = 0.0f;

        if (sample.real() > 0.0f)
            real = 1.0f;
        if (sample.imag() > 0.0f)
            imag = 1.0f;
        return raptor_complex(real, imag);
    }

    raptor_complex slicer_45deg(raptor_complex sample)
    {
        float real = -1.0f, imag = -1.0f;
        if (sample.real() > 0.0f)
            real = 1.0f;
        if (sample.imag() > 0.0f)
            imag = 1.0f;
        return raptor_complex(real, imag);
    }

};

#endif
