#ifndef WS_BUFFER_IO_H
#define WS_BUFFER_IO_H

#include <cstdio>
#include <cstdint>

namespace WS {
    typedef uint8_t block_t;
    const size_t WS_BUFFER_CAPACITY = 512;
    const size_t BIN_BUFFER_CAPACITY = WS_BUFFER_CAPACITY / 4;

    class BufferIO {
    protected:
        FILE * stream;
        block_t* buffer;
        const size_t buffer_capacity;
        size_t buffer_size = 0;
        int buffer_i = 0;
        block_t bit_block = 0;
        int bit_i = HIGH_BIT;
        static const int HIGH_BIT = 8 * sizeof(block_t) - 1;

        BufferIO(FILE* stream, size_t buffer_capacity) : stream(stream),
            buffer(new block_t[buffer_capacity]), buffer_capacity(buffer_capacity) {}

    public:
        ~BufferIO() {
            delete[] buffer;
        }
    };
}


#endif
