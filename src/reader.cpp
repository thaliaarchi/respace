#include "reader.h"

namespace WS {

Reader::Reader(FILE* stream, size_t buffer_capacity) : BufferIO(stream, buffer_capacity) {
    readBuffer();
}

block_t Reader::next() {
    block_t block = buffer_[buffer_i_++];
    if (buffer_i_ >= buffer_size_) {
        readBuffer();
    }
    bit_i_ = HIGH_BIT_;
    return block;
}

block_t Reader::nextBit() {
    block_t bit = (buffer_[buffer_i_] >> bit_i_--) & 1;
    if (bit_i_ < 0) { // All bits in current block read
        next(); // Advance to next block
    }
    return bit;
}

bool Reader::hasNext() {
    return buffer_size_ > 0;
}

// Private
void Reader::readBuffer() {
    buffer_size_ = fread(buffer_, sizeof(block_t), buffer_capacity_, stream_);
    buffer_i_ = 0;
}

} // namespace WS
