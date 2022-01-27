#include <raptordsp/filter/fir/filter_base.h>
#include <volk/volk.h>

#define DESTROY_BUFFER(name) if (name != nullptr) { volk_free(name); name = nullptr; }
#define RECREATE_BUFFER(name, type, count, alignment) DESTROY_BUFFER(name) name = (type*)volk_malloc(sizeof(type) * count, alignment); assert(name != nullptr);
#define DUPLICATE_BUFFER(dstName, srcName, type, count, alignment) if (srcName != nullptr) { dstName = (type*)volk_malloc(sizeof(type) * count, alignment); assert(dstName != nullptr); memcpy(dstName, srcName, sizeof(type) * count); } else { dstName = nullptr; }

/* CORE IMPLEMENTATION */

template <class IN_T, class OUT_T, class TAP_T>
raptor_filter<IN_T, OUT_T, TAP_T>::raptor_filter() :
    taps_buffer(nullptr),
    input_buffer(nullptr),
    ntaps(0),
    decimation(0),
    decimation_index(0),
    offset(0)
{

}

template <class IN_T, class OUT_T, class TAP_T>
raptor_filter<IN_T, OUT_T, TAP_T>::raptor_filter(raptor_filter const& src) {
    //Copy properties
    ntaps = src.ntaps;
    decimation = src.ntaps;
    decimation_index = src.decimation_index;
    offset = src.offset;

    //Deep copy buffers
    size_t alignment = volk_get_alignment();
    DUPLICATE_BUFFER(taps_buffer, src.taps_buffer, TAP_T, ntaps, alignment);
    DUPLICATE_BUFFER(input_buffer, src.input_buffer, IN_T, ntaps * 2, alignment);
}

template <class IN_T, class OUT_T, class TAP_T>
raptor_filter<IN_T, OUT_T, TAP_T>::~raptor_filter() {
    DESTROY_BUFFER(taps_buffer)
    DESTROY_BUFFER(input_buffer)
}

template <class IN_T, class OUT_T, class TAP_T>
void raptor_filter<IN_T, OUT_T, TAP_T>::configure(raptor_filter_builder_base* builder, int decimation) {
    //Apply settings
    this->ntaps = builder->get_ntaps();
    this->decimation = decimation;

    //Reset
    decimation_index = 0;
    offset = 0;

    //Create aligned buffers
    size_t alignment = volk_get_alignment();
    RECREATE_BUFFER(taps_buffer, TAP_T, ntaps, alignment);
    RECREATE_BUFFER(input_buffer, IN_T, ntaps * 2, alignment);

    //Apply taps
    set_taps_from_builder(builder);

    //Clear input
    memset(input_buffer, 0, this->ntaps * 2 * sizeof(IN_T));
}

template <class IN_T, class OUT_T, class TAP_T>
int raptor_filter<IN_T, OUT_T, TAP_T>::process(const IN_T* input, OUT_T* output, int count) {
    assert(taps_buffer != nullptr && input_buffer != nullptr);
    int read = 0;
    for (int i = 0; i < count; i++)
    {
        //Write to both the real position as well as an offset value
        this->input_buffer[this->offset] = input[i];
        this->input_buffer[this->offset + ntaps] = input[i];
        offset = (offset + 1) % ntaps;

        //Process (if needed)
        if (this->decimation_index++ == 0)
            compute(&output[read++], &input_buffer[offset], taps_buffer, ntaps);
        this->decimation_index %= this->decimation;
    }
    return read;
}

/* DECLARE IMPLEMENTATIONS */

template class raptor_filter<raptor_complex, raptor_complex, raptor_complex>;
template class raptor_filter<float, raptor_complex, raptor_complex>;
template class raptor_filter<raptor_complex, raptor_complex, float>;
template class raptor_filter<float, float, float>;