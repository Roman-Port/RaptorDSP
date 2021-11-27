#pragma once

#include <raptordsp/defines.h>
#include <raptordsp/misc/consumption_queue.h>

class raptor_clock_recovery_mm_complex {

public:
    void configure(float omega, float gain_omega, float mu, float gain_mu, float omega_relative_limit);
    void set_omega(float omega);
    int process(const raptor_complex* in, int count, raptor_complex* out, int outputCount);

private:
    float d_mu;                   // fractional sample position [0.0, 1.0]
    float d_omega;                // nominal frequency
    float d_gain_omega;           // gain for adjusting omega
    float d_omega_relative_limit; // used to compute min and max omega
    float d_omega_mid;            // average omega
    float d_omega_lim;            // actual omega clipping limit
    float d_gain_mu;              // gain for adjusting mu

    raptor_complex d_last_sample;

    raptor_complex d_p_2T, d_p_1T, d_p_0T;
    raptor_complex d_c_2T, d_c_1T, d_c_0T;

    raptor_consumption_queue<raptor_complex> queue;

};