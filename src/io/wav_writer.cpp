#include <raptordsp/io/wav_writer.h>
#include <stdexcept>

raptor_wav_writer::raptor_wav_writer(FILE* file, size_t bufferSize, int sampleRate, int channels, int bitsPerSample) {
	throw std::runtime_error("Not yet implemented.");
}

raptor_wav_writer::raptor_wav_writer(raptor_wav_writer const& src) {
	throw std::runtime_error("Not yet implemented.");
}

raptor_wav_writer::~raptor_wav_writer() {
	throw std::runtime_error("Not yet implemented.");
}

size_t raptor_wav_writer::write(const float* ptr, size_t count)
{
	return converter->write(ptr, count, file);
}

size_t raptor_wav_writer::write(const raptor_complex* ptr, size_t count)
{
	return write((const float*)ptr, count * 2) / 2;
}