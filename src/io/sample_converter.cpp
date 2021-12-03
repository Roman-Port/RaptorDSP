#include <raptordsp/io/sample_converter.h>
#include <volk/volk.h>
#include <algorithm>

raptor_sample_converter* raptor_sample_converter::create_converter(int bitsPerSample, size_t bufferSize) {
	switch (bitsPerSample) {
	case 8: return new raptor_sample_converter_byte(bufferSize);
	case 16: return new raptor_sample_converter_short(bufferSize);
	case 32: return new raptor_sample_converter_float();
	default: return nullptr;
	}
}

/* ===== FLOAT ===== */

raptor_sample_converter_float::raptor_sample_converter_float() {

}

raptor_sample_converter_float::~raptor_sample_converter_float() {

}

size_t raptor_sample_converter_float::read(float* ptr, size_t count, FILE* src) {
	return fread(ptr, sizeof(float), count, src);
}

size_t raptor_sample_converter_float::write(const float* ptr, size_t count, FILE* dst) {
	return fwrite(ptr, sizeof(float), count, dst);
}

/* ===== SHORT ===== */

raptor_sample_converter_short::raptor_sample_converter_short(size_t bufferCount) :
	buffer((int16_t*)volk_malloc(sizeof(int16_t) * bufferCount, volk_get_alignment())),
	buffer_size(bufferCount)
{

}

raptor_sample_converter_short::~raptor_sample_converter_short() {
	if (buffer != nullptr) {
		volk_free(buffer);
		buffer = nullptr;
	}
}

size_t raptor_sample_converter_short::read(float* ptr, size_t count, FILE* src) {
	count = fread(buffer, sizeof(int16_t), std::min(count, buffer_size), src);
	volk_16i_s32f_convert_32f(ptr, buffer, 32768, count);
	return count;
}

size_t raptor_sample_converter_short::write(const float* ptr, size_t count, FILE* dst) {
	count = std::min(count, buffer_size);
	volk_32f_s32f_convert_16i(buffer, ptr, 32768, count);
	return fwrite(buffer, sizeof(int16_t), count, dst);
}

/* ===== BYTE ===== */

raptor_sample_converter_byte::raptor_sample_converter_byte(size_t bufferCount) :
	buffer((int8_t*)volk_malloc(sizeof(int8_t) * bufferCount, volk_get_alignment())),
	buffer_size(bufferCount)
{

}

raptor_sample_converter_byte::~raptor_sample_converter_byte() {
	if (buffer != nullptr) {
		volk_free(buffer);
		buffer = nullptr;
	}
}

size_t raptor_sample_converter_byte::read(float* ptr, size_t count, FILE* src) {
	count = fread(buffer, sizeof(int8_t), std::min(count, buffer_size), src);
	volk_8i_s32f_convert_32f(ptr, buffer, 128, count);
	return count;
}

size_t raptor_sample_converter_byte::write(const float* ptr, size_t count, FILE* dst) {
	count = std::min(count, buffer_size);
	volk_32f_s32f_convert_8i(buffer, ptr, 128, count);
	return fwrite(buffer, sizeof(int8_t), count, dst);
}