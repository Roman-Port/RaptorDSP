#include <raptordsp/digital/clock_recovery_mm_complex.h>
#include <volk/volk.h>
#include <cassert>
#include "interpolator_taps.h"

static float branchless_clip(float x, float clip)
{
	float x1 = fabsf(x + clip);
	float x2 = fabsf(x - clip);
	x1 -= x2;
	return 0.5 * x1;
}

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

void fast_cc_multiply(raptor_complex& out, const raptor_complex cc1, const raptor_complex cc2)
{
	// The built-in complex.h multiply has significant NaN/INF checking that
	// considerably slows down performance.  While on some compilers the
	// -fcx-limit-range flag can be used, this fast function makes the math consistent
	// in terms of performance for the Costas loop.
	float o_r, o_i;

	o_r = (cc1.real() * cc2.real()) - (cc1.imag() * cc2.imag());
	o_i = (cc1.real() * cc2.imag()) + (cc1.imag() * cc2.real());

	out.real(o_r);
	out.imag(o_i);
}

void raptor_clock_recovery_mm_complex::configure(float omega, float gain_omega, float mu, float gain_mu, float omega_relative_limit) {
	this->d_mu = mu;
	this->d_omega = omega;
	this->d_gain_omega = gain_omega;
	this->d_omega_relative_limit = omega_relative_limit;
	this->d_gain_mu = gain_mu;
	this->d_last_sample = 0;
	this->d_p_2T = 0;
	this->d_p_1T = 0;
	this->d_p_0T = 0;
	this->d_c_2T = 0;
	this->d_c_1T = 0;
	this->d_c_0T = 0;

	set_omega(omega); // also sets min and max omega
}

void raptor_clock_recovery_mm_complex::set_omega(float omega)
{
	d_omega = omega;
	d_omega_mid = omega;
	d_omega_lim = d_omega_relative_limit * omega;
}

void interpolate(const raptor_complex* input, float mu, raptor_complex* output) {
	//Get the index
	int step = (int)rint((1 - mu) * NSTEPS);

	//Apply interpolation
	volk_32fc_32f_dot_prod_32fc(output, input, taps[step], NTAPS);
}

int raptor_clock_recovery_mm_complex::process(const raptor_complex* inp, int count, raptor_complex* out, int outputCount) {
	//Push into the consumption queue
	queue.push(inp, count);

	//Declare
	int ii = 0; //Input index
	int oo = 0; //Output index
	int ni = queue.get_available() - NTAPS; //Don't consume more than this
	float mm_val = 0;
	raptor_complex u, x, y;

	//Sanity check
	assert(d_mu >= 0.0);
	assert(d_mu <= 1.0);

	//Work
	while (ii < ni && oo < outputCount) {
		d_p_2T = d_p_1T;
		d_p_1T = d_p_0T;
		interpolate(&queue.buffer[ii], d_mu, &d_p_0T);

		d_c_2T = d_c_1T;
		d_c_1T = d_c_0T;
		d_c_0T = slicer_0deg(d_p_0T);

		fast_cc_multiply(x, d_c_0T - d_c_2T, conj(d_p_1T));
		fast_cc_multiply(y, d_p_0T - d_p_2T, conj(d_c_1T));
		u = y - x;
		mm_val = u.real();
		out[oo++] = d_p_0T;

		// limit mm_val
		mm_val = branchless_clip(mm_val, 1.0);

		d_omega = d_omega + d_gain_omega * mm_val;
		d_omega = d_omega_mid + branchless_clip(d_omega - d_omega_mid, d_omega_lim);

		d_mu = d_mu + d_omega + d_gain_mu * mm_val;
		ii += (int)floor(d_mu);
		d_mu -= floor(d_mu);

		if (ii < 0) // clamp it.  This should only happen with bogus input
			ii = 0;
	}

	//Consume
	queue.consume(ii);

	//Sanity check
	assert(queue.get_available() <= NTAPS);

	return oo;
}