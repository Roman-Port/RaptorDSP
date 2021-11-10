#pragma once

#include <raptordsp/demodulator/demod.h>
#include <raptordsp/filter/fir/filter_real.h>

#define DEEMPHASIS_TIME_USA 75.0f

/// <summary>
/// A mono broadcast FM demodulator
/// </summary>
class demod_bcast_fm : public demod {

public:
	demod_bcast_fm(size_t bufferSize, float deemphasisTime = DEEMPHASIS_TIME_USA);
	~demod_bcast_fm();
	virtual float configure(float sampleRate) override;
	virtual int process(raptor_complex* iq, int count, float* audioL, float* audioR) override;

protected:
	/// <summary>
	/// Called after the baseband is demodulated but before we begin filtering L+R
	/// </summary>
	virtual void baseband_demodulated(int count);

	/// <summary>
	/// Called after audio has been filtered but before deemphasis is applied
	/// </summary>
	virtual void audio_filtered(float* audioL, float* audioR, int mpxCount, int audioCount);

	/// <summary>
	/// Creates a new audio filter. Put into it's own function for consistency between L and R filters.
	/// </summary>
	/// <param name="filter">The filter to create.</param>
	/// <param name="sampleRate">The sample rate of the input.</param>
	/// <returns>Sample rate of the output.</returns>
	float create_audio_filter(raptor_filter_real* filter, float sampleRate);

	float* mpx_buffer;

private:
	/// <summary>
	/// Processes deemphasis on an audio channel.
	/// </summary>
	/// <param name="state"></param>
	/// <param name="audio"></param>
	/// <param name="count"></param>
	void process_deemphasis(float* state, float* audio, int count);

	float gain;
	raptor_complex last_sample;

	float deemphasis_time;
	float deemphasis_alpha;
	float deemphasis_state_l;
	float deemphasis_state_r;

	raptor_filter_real filter_mono;

};