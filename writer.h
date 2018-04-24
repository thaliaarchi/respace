#ifndef WS_WRITER_H_
#define WS_WRITER_H_

#include "bufferio.h"

namespace WS {
    class Writer : public BufferIO {
    public:
        Writer(FILE* stream, size_t buffer_capacity);

        void writeBlock(block_t block);
        void writeBit(block_t bit);
        void flushBits();
        void close();

    private:
        void writeBuffer(size_t size);
    };
}

#endif
