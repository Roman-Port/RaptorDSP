#pragma once

#include <raptordsp/defines.h>
#include <cstring>
#include <cassert>

/// <summary>
/// Generated filter taps.
/// </summary>
/// <typeparam name="T"></typeparam>
template <class T>
class raptor_filter_taps {

public:
	raptor_filter_taps(T* buffer, int ntaps);
	raptor_filter_taps(raptor_filter_taps const& src);
	~raptor_filter_taps();

	int get_ntaps();
	void copy_to(T* output);
	T read(int index);

private:
	T* buffer;
	int ntaps;

};

typedef raptor_filter_taps<float> raptor_filter_taps_float;
typedef raptor_filter_taps<raptor_complex> raptor_filter_taps_complex;