#ifndef WS_WRITER_H_
#define WS_WRITER_H_

#include "bufferio.h"

namespace WS {

class Writer : public BufferIO {
public:
    Writer(FILE* stream, size_t buffer_capacity);
    ~Writer();

    void write(block_t block);
    void writeBit(block_t bit);
    void flush();

private:
    void writeBuffer(size_t size);
};

} // namespace WS

#endif
