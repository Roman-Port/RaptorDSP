#pragma once

#include <stdio.h>
#include <stdint.h>

class raptor_sample_converter {

public:
	virtual size_t write(const float* ptr, size_t count, FILE* dst) = 0;
	virtual size_t read(float* ptr, size_t count, FILE* src) = 0;

	static raptor_sample_converter* create_converter(int bitsPerSample, size_t bufferSize);

};

class raptor_sample_converter_float : public raptor_sample_converter {

public:
	raptor_sample_converter_float();
	~raptor_sample_converter_float();
	virtual size_t write(const float* ptr, size_t count, FILE* dst) override;
	virtual size_t read(float* ptr, size_t count, FILE* src) override;

};

class raptor_sample_converter_short : public raptor_sample_converter {

public:
	raptor_sample_converter_short(size_t bufferCount);
	~raptor_sample_converter_short();
	virtual size_t write(const float* ptr, size_t count, FILE* dst) override;
	virtual size_t read(float* ptr, size_t count, FILE* src) override;

private:
	int16_t* buffer;
	size_t buffer_size;

};

class raptor_sample_converter_byte : public raptor_sample_converter {

public:
	raptor_sample_converter_byte(size_t bufferCount);
	~raptor_sample_converter_byte();
	virtual size_t write(const float* ptr, size_t count, FILE* dst) override;
	virtual size_t read(float* ptr, size_t count, FILE* src) override;

private:
	int8_t* buffer;
	size_t buffer_size;

};