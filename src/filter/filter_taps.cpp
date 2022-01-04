#include <raptordsp/filter/filter_taps.h>
#include <cstring>

template <class T>
raptor_filter_taps<T>::raptor_filter_taps(T* buffer, int ntaps) {
	this->buffer = buffer;
	this->ntaps = ntaps;
}

template <class T>
raptor_filter_taps<T>::raptor_filter_taps(raptor_filter_taps const& src) {
	buffer = (T*)malloc(sizeof(T) * src.ntaps);
	ntaps = src.ntaps;

	//Deep copy buffer
	memcpy(buffer, src.buffer, sizeof(T) * src.ntaps);
}

template <class T>
raptor_filter_taps<T>::~raptor_filter_taps() {
	if (buffer != nullptr) {
		free(buffer);
		buffer = nullptr;
	}
}

template <class T>
int raptor_filter_taps<T>::get_ntaps() {
	return ntaps;
}

template <class T>
void raptor_filter_taps<T>::copy_to(T* output) {
	memcpy(output, buffer, sizeof(T) * ntaps);
}

template <class T>
T raptor_filter_taps<T>::read(int index) {
	return buffer[index];
}

template class raptor_filter_taps<float>;
template class raptor_filter_taps<raptor_complex>;