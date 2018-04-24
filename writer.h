#ifndef WS_WRITER_H
#define WS_WRITER_H

#include "bufferio.h"

namespace WS {
    class Writer : public BufferIO {
    public:
        Writer(FILE* stream, size_t capacity);

        void writeBlock(block_t block);
        void writeBit(block_t bit);
        void flushBits();
        void close();

    private:
        void writeBuffer(size_t size);
    };
}

#endif
