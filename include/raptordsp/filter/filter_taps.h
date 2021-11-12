#pragma once

#include <cstring>
#include <cassert>

/// <summary>
/// Generated filter taps.
/// </summary>
/// <typeparam name="T"></typeparam>
template <class T>
class raptor_filter_taps {

public:
	raptor_filter_taps(T* buffer, int ntaps) {
		this->buffer = buffer;
		this->ntaps = ntaps;
	}

	raptor_filter_taps(raptor_filter_taps const& src) {
		buffer = (T*)malloc(sizeof(T) * src.ntaps);
		ntaps = src.ntaps;

		//Deep copy buffer
		memcpy(buffer, src.buffer, sizeof(T) * src.ntaps);
	}

	~raptor_filter_taps() {
		assert(buffer != nullptr);
		buffer = nullptr;
		free(buffer);
	}

	int get_ntaps() {
		return ntaps;
	}

	void copy_to(T* output) {
		memcpy(output, buffer, sizeof(T) * ntaps);
	}

	T read(int index) {
		return buffer[index];
	}

private:
	T* buffer;
	int ntaps;

};