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

	inline size_t write(const float* ptr, size_t count) { return converter->write(ptr, count, file); }
	inline size_t write(const raptor_complex* ptr, size_t count) { return write((const float*)ptr, count * 2) / 2; }

private:
	int sampleRate;
	int channels;
	int bitsPerSample;
	size_t buffer_size;
	raptor_sample_converter* converter;
	FILE* file;

};
