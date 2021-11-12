#ifndef ANDROIDSDRTUNER_DELAY_LINE_H
#define ANDROIDSDRTUNER_DELAY_LINE_H

template<typename T>
class raptor_delay_line {

private:
    T* buffer;
    int len;
    int pos;

public:
    raptor_delay_line(int delay, T defaultValue) {
        //Allocate the buffer
        buffer = (T*)malloc(sizeof(T) * delay);

        //Zero out buffer
        for (int i = 0; i < delay; i++)
            buffer[i] = defaultValue;

        //Set up vars
        pos = 0;
        len = delay;
    }

    raptor_delay_line(raptor_delay_line const& src) {
        len = src->len;
        pos = src->pos;
        buffer = (T*)malloc(sizeof(T) * len);
        memcpy(buffer, src->buffer, sizeof(T) * len);
    }

    ~raptor_delay_line() {
        free(buffer);
        buffer = nullptr;
    }

    void process_one(T input, T* output) {
        //Read output
        *output = buffer[pos];

        //Write value in its place
        buffer[pos] = input;

        //Advance pointer
        pos = (pos + 1) % len;
    }

    void process(T* ptr, int count) {
        T temp;
        for (int i = 0; i < count; i++) {
            process_one(ptr[i], &temp);
            ptr[i] = temp;
        }
    }

};

#endif //ANDROIDSDRTUNER_DELAY_LINE_H
