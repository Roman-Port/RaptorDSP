#include <raptordsp/io/wav_reader.h>
#include <stdexcept>

raptor_wav_reader::raptor_wav_reader(const char* filename, size_t bufferSize) :
	file(nullptr),
	converter(nullptr),
	buffer_size(bufferSize),
	bytes_per_sample(0)
{
	//Open the file
	file = fopen(filename, "rb");
	if (file == nullptr)
		throw std::runtime_error("Unable to open WAV file.");

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

raptor_wav_reader::raptor_wav_reader(raptor_wav_reader const& src)
{
	throw std::runtime_error("Cloning of the WAV file reader is not permitted.");
}

raptor_wav_reader::~raptor_wav_reader() {
	//Close file
	if (file != nullptr) {
		fclose(file);
		file = nullptr;
	}

	//Free the converter
	if (converter != nullptr) {
		delete(converter);
		converter = nullptr;
	}
}

fpos_t raptor_wav_reader::get_position() {
	fpos_t pos;
	if (fgetpos(file, &pos) != 0)
		throw std::runtime_error("File IO error.");
	return (pos - WAV_HEADER_SIZE) / bytes_per_sample;
}

void raptor_wav_reader::set_position(fpos_t pos) {
	pos = (pos * bytes_per_sample) + WAV_HEADER_SIZE;
	if (fsetpos(file, &pos) != 0)
		throw std::runtime_error("File IO error.");
}