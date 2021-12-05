#include <raptordsp/digital/clock_recovery_mm.h>
#include <volk/volk.h>
#include "interpolator_taps.h"
#include <cassert>
#include <cstring>

raptor_clock_recovery_mm::raptor_clock_recovery_mm() {
    //Clear
    d_p_2T = 0;
    d_p_1T = 0;
    d_p_0T = 0;
    d_c_2T = 0;
    d_c_1T = 0;
    d_c_0T = 0;

    //Create buffer
    bufferLen = NTAPS;
    bufferUse = 0;
    buffer = (raptor_complex*)malloc(bufferLen * sizeof(raptor_complex));
}

void raptor_clock_recovery_mm::configure(float omega, float gain_omega, float mu, float gain_mu, float omega_relative_limit) {
    d_mu = mu;
    d_omega = omega;
    d_gain_omega = gain_omega;
    d_omega_relative_limit = omega_relative_limit;
    d_gain_mu = gain_mu;
    d_last_sample = 0;

    set_omega(omega);
}

void raptor_clock_recovery_mm::set_omega(float omega)
{
    d_omega = omega;
    d_omega_mid = omega;
    d_omega_lim = d_omega_relative_limit * omega;
}

static void fast_cc_multiply(raptor_complex& out, const raptor_complex cc1, const raptor_complex cc2)
{
    float o_r, o_i;

    o_r = (cc1.real() * cc2.real()) - (cc1.imag() * cc2.imag());
    o_i = (cc1.real() * cc2.imag()) + (cc1.imag() * cc2.real());

    out.real(o_r);
    out.imag(o_i);
}

static inline float branchless_clip(float x, float clip)
{
    return 0.5f * (std::abs(x + clip) - std::abs(x - clip));
}

int raptor_clock_recovery_mm::process(raptor_complex* in, raptor_complex* out, int count)
{
    int inputIndex = 0;
    int outputIndex = 0;

    assert(d_mu >= 0.0);
    assert(d_mu <= 1.0);

    while (count > 0) {
        //Write as much to the buffer as possible
        int writable = std::min(bufferLen - bufferUse, count);
        memcpy(&buffer[bufferUse], &in[inputIndex], writable * sizeof(raptor_complex));
        inputIndex += writable;
        bufferUse += writable;
        count -= writable;

        //Check if we have an entire block
        if (bufferUse == bufferLen) {
            //Process
            int consumed = process_block(&out[outputIndex]);
            outputIndex++;
            assert(consumed <= bufferLen);

            //Move buffer
            bufferUse -= consumed;
            memcpy(buffer, &buffer[bufferLen - bufferUse], bufferUse * sizeof(raptor_complex));
        }
    }
    return outputIndex;
}

int raptor_clock_recovery_mm::process_block(raptor_complex* out) {
    raptor_complex u, x, y;
    d_p_2T = d_p_1T;
    d_p_1T = d_p_0T;
    interpolate(buffer, &d_p_0T);

    d_c_2T = d_c_1T;
    d_c_1T = d_c_0T;
    d_c_0T = slicer_0deg(d_p_0T);

    fast_cc_multiply(x, d_c_0T - d_c_2T, conj(d_p_1T));
    fast_cc_multiply(y, d_p_0T - d_p_2T, conj(d_c_1T));
    u = y - x;
    float mm_val = u.real();
    *out = d_p_0T;

    // limit mm_val
    mm_val = branchless_clip(mm_val, 1.0);

    d_omega = d_omega + d_gain_omega * mm_val;
    d_omega =
            d_omega_mid + branchless_clip(d_omega - d_omega_mid, d_omega_lim);

    d_mu = d_mu + d_omega + d_gain_mu * mm_val;
    int consumed = (int)floor(d_mu);
    d_mu -= floor(d_mu);

    //clamp consumed, although this shouldn't happen
    if (consumed < 0)
        consumed = 0;
    return consumed;
}

void raptor_clock_recovery_mm::interpolate(raptor_complex* input, raptor_complex* output) {
    int imu = (int)rint(d_mu * NSTEPS);

    if ((imu < 0) || (imu > NSTEPS)) {
        throw std::runtime_error("imu out of bounds.");
    }

    //Filter
    volk_32fc_32f_dot_prod_32fc(output, input, taps[imu], NTAPS);
}