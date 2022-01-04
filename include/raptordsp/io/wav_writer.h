#pragma once

#include <raptordsp/misc/wav.h>
#include <raptordsp/io/sample_converter.h>
#include <raptordsp/defines.h>
#include <stdint.h>

class raptor_wav_writer {

public:
	raptor_wav_writer(FILE* file, size_t bufferSize, int sampleRate, int channels, int bitsPerSample);
	raptor_wav_writer(raptor_wav_writer const& src);
	~raptor_wav_writer();

	size_t write(const float* ptr, size_t count);
	size_t write(const raptor_complex* ptr, size_t count);

private:
	int sampleRate;
	int channels;
	int bitsPerSample;
	size_t buffer_size;
	raptor_sample_converter* converter;
	FILE* file;

};
