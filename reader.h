#ifndef WS_READER_H
#define WS_READER_H

#include "bufferio.h"

namespace WS {
    class Reader : public BufferIO {
    public:
        Reader(FILE* stream, size_t capacity);

        block_t readBlock();
        block_t readBit();
        bool canRead();
        void close();

    private:
        size_t readBuffer();
    };
}

#endif
