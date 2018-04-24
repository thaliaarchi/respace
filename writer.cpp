#include "writer.h"

namespace WS {
    Writer::Writer(FILE* stream, size_t capacity) : BufferIO(stream, capacity) {}

    void Writer::writeBlock(block_t block) {
        if (buffer_i >= buffer_capacity) {
            writeBuffer(buffer_capacity);
        }
        bit_i = HIGH_BIT;
        buffer[buffer_i++] = block;
    }

    void Writer::writeBit(block_t bit) {
        if (bit_i < 0) {
            writeBlock(bit_block);
            bit_block = 0;
        }
        bit_block |= bit << bit_i--;
    }

    void Writer::flushBits() {
        buffer[buffer_i++] = bit_block;
    }

    void Writer::close() {
        writeBuffer(buffer_i);
        fclose(stream);
    }

    // Private
    void Writer::writeBuffer(size_t size) {
        fwrite(buffer, sizeof(block_t), size, stream);
        buffer_i = 0;
    }
}
