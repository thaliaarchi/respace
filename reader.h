#ifndef WS_READER_H_
#define WS_READER_H_

#include "bufferio.h"

namespace WS {
    class Reader : public BufferIO {
    public:
        Reader(FILE* stream, size_t buffer_capacity);

        block_t readBlock();
        block_t readBit();
        char readBinaryToken();
        bool canRead();
        void close();

    private:
        size_t readBuffer();
    };
}

#endif
