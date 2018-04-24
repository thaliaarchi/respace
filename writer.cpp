#include "writer.h"

namespace WS {
    Writer::Writer(FILE* stream, size_t buffer_capacity) : BufferIO(stream, buffer_capacity) {}

    void Writer::writeBlock(block_t block) {
        if (buffer_i_ >= buffer_capacity_) {
            writeBuffer(buffer_capacity_);
        }
        bit_i_ = HIGH_BIT_;
        buffer_[buffer_i_++] = block;
    }

    void Writer::writeBit(block_t bit) {
        if (bit_i_ < 0) {
            writeBlock(bit_block_);
            bit_block_ = 0;
        }
        bit_block_ |= bit << bit_i_--;
    }

    void Writer::flushBits() {
        buffer_[buffer_i_++] = bit_block_;
    }

    void Writer::close() {
        writeBuffer(buffer_i_);
        fclose(stream_);
    }

    // Private
    void Writer::writeBuffer(size_t size) {
        fwrite(buffer_, sizeof(block_t), size, stream_);
        buffer_i_ = 0;
    }
}
