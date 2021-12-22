#pragma once

#include <raptordsp/defines.h>

template <class T>
class raptor_consumption_queue {

public:
	raptor_consumption_queue(size_t initial_count = 1024);
	~raptor_consumption_queue();
	raptor_consumption_queue(const raptor_consumption_queue&);

	void push(const T* ptr, size_t count);
	size_t get_available();
	void consume(size_t count);

	T* buffer;

private:
	size_t length;
	size_t available;

	void extend_buffer();

};

typedef raptor_consumption_queue<float> raptor_consumption_queue_float;
typedef raptor_consumption_queue<raptor_complex> raptor_consumption_queue_complex;