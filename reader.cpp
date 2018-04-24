#include "reader.h"

namespace WS {
    Reader::Reader(FILE* stream, size_t buffer_capacity) : BufferIO(stream, buffer_capacity) {
        readBuffer();
    }

    block_t Reader::readBlock() {
        if (buffer_i_ >= buffer_size_) {
            readBuffer();
        }
        bit_i_ = HIGH_BIT_;
        return buffer_[buffer_i_++];
    }

    block_t Reader::readBit() {
        if (bit_i_ < 0) { // All bits in current block read
            readBlock(); // Advance to next block
        }
        return (buffer_[buffer_i_] >> bit_i_--) & 1;
    }

    bool Reader::canRead() {
        if (buffer_i_ >= buffer_size_ && buffer_size_ > 0) {
            return readBuffer() > 0;
        }
        return buffer_size_ > 0;
    }

    void Reader::close() {
        fclose(stream_);
    }

    // Private
    size_t Reader::readBuffer() {
        buffer_size_ = fread(buffer_, sizeof(block_t), buffer_capacity_, stream_);
        buffer_i_ = 0;
        return buffer_size_;
    }
}
