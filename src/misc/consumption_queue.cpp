#include <raptordsp/defines.h>
#include <raptordsp/misc/consumption_queue.h>
#include <malloc.h>
#include <cassert>
#include <cstring>

template <class T>
raptor_consumption_queue<T>::raptor_consumption_queue(size_t initial_count) :
	length(initial_count),
	available(0),
	buffer((T*)malloc(sizeof(T) * initial_count))
{
	assert(buffer != nullptr);
}

template <class T>
raptor_consumption_queue<T>::raptor_consumption_queue(const raptor_consumption_queue& src) :
	length(src.length),
	available(src.available),
	buffer((T*)malloc(sizeof(T) * src.length))
{
	assert(buffer != nullptr);
	memcpy(buffer, src.buffer, sizeof(T) * src.length);
}

template <class T>
raptor_consumption_queue<T>::~raptor_consumption_queue() {
	free(buffer);
	buffer = nullptr;
}

template <class T>
void raptor_consumption_queue<T>::push(const T* ptr, size_t count) {
	//Ensure we have enough space
	while (available + count >= length)
		extend_buffer();

	//Copy
	memcpy(&buffer[available], ptr, count * sizeof(T));
	available += count;
}

template <class T>
size_t raptor_consumption_queue<T>::get_available() {
	return available;
}

template <class T>
void raptor_consumption_queue<T>::consume(size_t count) {
	//Make sure we aren't consuming more than we have
	assert(count <= available);
	assert(count >= 0);

	//Update state
	available -= count;

	//Transfer all items to the beginning
	memcpy(buffer, &buffer[count], sizeof(T) * available);
}

template <class T>
void raptor_consumption_queue<T>::extend_buffer() {
	//Create a new buffer
	size_t extLength = length * 2;
	T* ext = (T*)malloc(sizeof(T) * extLength);
	assert(ext != nullptr);

	//Transfer old buffer over
	memcpy(ext, buffer, sizeof(T) * length);

	//Free old buffer
	free(buffer);

	//Replace
	buffer = ext;
	length = extLength;
}

template class raptor_consumption_queue<float>;
template class raptor_consumption_queue<raptor_complex>;