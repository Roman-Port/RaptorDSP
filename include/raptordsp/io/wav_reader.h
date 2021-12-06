#pragma once

#include <raptordsp/misc/wav.h>
#include <raptordsp/io/sample_converter.h>
#include <raptordsp/defines.h>
#include <stdint.h>

class raptor_wav_reader {

public:
	raptor_wav_reader(FILE* file, size_t bufferSize);
	raptor_wav_reader(raptor_wav_reader const& src);
	~raptor_wav_reader();

	int get_sample_rate() { return info.sample_rate; }
	int get_channels() { return info.channels; }
	int get_bits_per_sample() { return info.bits_per_sample; }

	int64_t get_position();
	void set_position(int64_t pos);

	inline size_t read(float* ptr, size_t count) { return converter->read(ptr, count, file); }
	inline size_t read(raptor_complex* ptr, size_t count) { return read((float*)ptr, count * 2) / 2; }

private:
	wav_header_data info;
	size_t buffer_size;
	int bytes_per_sample;
	raptor_sample_converter* converter;
	FILE* file;

};
