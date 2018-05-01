#ifndef WS_BUFFER_IO_H_
#define WS_BUFFER_IO_H_

#include <cstdio>
#include <cstdint>

namespace WS {
    typedef uint8_t block_t;
    const size_t WS_BUFFER_CAPACITY = 512;
    const size_t BIN_BUFFER_CAPACITY = WS_BUFFER_CAPACITY / 4;

    class BufferIO {
    public:
        ~BufferIO() {
            delete[] buffer_;
        }

    protected:
        FILE* stream_;
        block_t* buffer_;
        const size_t buffer_capacity_;
        size_t buffer_size_ = 0;
        size_t buffer_i_ = 0;
        block_t bit_block_ = 0;
        int bit_i_ = HIGH_BIT_;
        static const int HIGH_BIT_ = 8 * sizeof(block_t) - 1;

        BufferIO(FILE* stream, size_t buffer_capacity) : stream_(stream),
            buffer_(new block_t[buffer_capacity]), buffer_capacity_(buffer_capacity) {}
    };
}

#endif
