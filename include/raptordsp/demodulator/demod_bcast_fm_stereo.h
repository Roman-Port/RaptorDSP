#pragma once

#include <raptordsp/demodulator/demod_bcast_fm.h>
#include <raptordsp/analog/pll.h>
#include <raptordsp/filter/fir/filter_complex.h>
#include <raptordsp/misc/delay_line.h>

class demod_bcast_fm_stereo : public demod_bcast_fm {

public:
	demod_bcast_fm_stereo(size_t buffer_size, float deemphasisTime = DEEMPHASIS_TIME_USA);
	~demod_bcast_fm_stereo();
	virtual float configure(float sampleRate);
	bool is_stereo_detected();

	bool stereo_enabled;

protected:
	virtual void baseband_demodulated(int count) override;
	virtual void audio_filtered(float* audioL, float* audioR, int mpxCount, int audioCount) override;

private:
	raptor_complex* pilot_buffer; //filtered to the stereo pilot
	raptor_complex* pll_buffer; //PLL'd

	raptor_delay_line<float>* delay;

	raptor_pll pll;
	raptor_filter_real filter_stereo;
	raptor_filter_complex pilot_filter;

};