#include <raptordsp/io/wav_reader.h>
#include <stdexcept>

raptor_wav_reader::raptor_wav_reader(FILE* file, size_t bufferSize) :
	file(file),
	converter(nullptr),
	buffer_size(bufferSize),
	bytes_per_sample(0)
{
	//Read the WAV header
	uint8_t header[WAV_HEADER_SIZE];
	if (fread(header, 1, sizeof(header), file) != sizeof(header))
		throw std::runtime_error("WAV file invalid: Failed to read header.");
	if (!read_wav_header(header, &info))
		throw std::runtime_error("WAV file invalid: Failed to parse WAV header. Is this a WAV file?");

	//Sanity check WAV file
	if (info.channels < 1)
		throw std::runtime_error("WAV file invalid: There must be at least one channel.");
	if (info.sample_rate <= 0)
		throw std::runtime_error("WAV file invalid: Sample rate must be greater than zero.");

	//Create the sample converter
	converter = raptor_sample_converter::create_converter(info.bits_per_sample, bufferSize * info.channels);
	if (converter == nullptr)
		throw std::runtime_error("WAV file invalid: Unknown sample format.");

	//Calculate
	bytes_per_sample = info.channels * (info.bits_per_sample / 8);
}

raptor_wav_reader::raptor_wav_reader(raptor_wav_reader const& src) :
	file(src.file),
	converter(raptor_sample_converter::create_converter(src.info.bits_per_sample, src.buffer_size * src.info.channels)),
	buffer_size(src.buffer_size),
	bytes_per_sample(src.bytes_per_sample)
{
}

raptor_wav_reader::~raptor_wav_reader() {
	//Free the converter
	if (converter != nullptr) {
		delete(converter);
		converter = nullptr;
	}
}

int64_t raptor_wav_reader::get_position() {
	int64_t pos = ftell(file);
	return (pos - WAV_HEADER_SIZE) / bytes_per_sample;
}

void raptor_wav_reader::set_position(int64_t pos) {
	pos = (pos * bytes_per_sample) + WAV_HEADER_SIZE;
	if (fseek(file, pos, SEEK_END) != 0)
		throw std::runtime_error("File IO error.");
}

int raptor_wav_reader::get_sample_rate()
{
	return info.sample_rate;
}

int raptor_wav_reader::get_channels()
{
	return info.channels;
}

int raptor_wav_reader::get_bits_per_sample()
{
	return info.bits_per_sample;
}

size_t raptor_wav_reader::read(float* ptr, size_t count)
{
	return converter->read(ptr, count, file);
}

size_t raptor_wav_reader::read(raptor_complex* ptr, size_t count)
{
	return read((float*)ptr, count * 2) / 2;
}