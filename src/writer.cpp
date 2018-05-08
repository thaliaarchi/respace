#include "writer.h"

namespace WS {
    Writer::Writer(FILE* stream, size_t buffer_capacity) : BufferIO(stream, buffer_capacity) {}

    Writer::~Writer() {
        flush();
    }

    void Writer::write(block_t block) {
        if (buffer_i_ >= buffer_capacity_) {
            writeBuffer(buffer_capacity_);
        }
        bit_i_ = HIGH_BIT_;
        buffer_[buffer_i_++] = block;
    }

    void Writer::writeBit(block_t bit) {
        if (bit_i_ < 0) {
            write(bit_block_);
            bit_block_ = 0;
        }
        bit_block_ |= bit << bit_i_--;
    }

    void Writer::flush() {
        if (bit_i_ < HIGH_BIT_) {
            write(bit_block_);
        }
        writeBuffer(buffer_i_);
    }

    // Private
    void Writer::writeBuffer(size_t size) {
        if (size > 0) {
            fwrite(buffer_, sizeof(block_t), size, stream_);
            buffer_i_ = 0;
        }
    }
}
