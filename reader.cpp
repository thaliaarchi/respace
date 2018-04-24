#include "reader.h"

namespace WS {
    Reader::Reader(FILE* stream, size_t capacity) : BufferIO(stream, capacity) {
        readBuffer();
    }

    block_t Reader::readBlock() {
        if (buffer_i >= buffer_size) {
            readBuffer();
        }
        bit_i = HIGH_BIT;
        return buffer[buffer_i++];
    }

    block_t Reader::readBit() {
        if (bit_i < 0) { // All bits in current block read
            readBlock(); // Advance to next block
        }
        return (buffer[buffer_i] >> bit_i--) & 1;
    }

    bool Reader::canRead() {
        if (buffer_i >= buffer_size && buffer_size > 0) {
            return readBuffer() > 0;
        }
        return buffer_size > 0;
    }

    void Reader::close() {
        fclose(stream);
    }

    // Private
    size_t Reader::readBuffer() {
        buffer_size = fread(buffer, sizeof(block_t), buffer_capacity, stream);
        buffer_i = 0;
        return buffer_size;
    }
}
