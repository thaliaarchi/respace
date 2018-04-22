#ifndef WS_COMPRESS_H
#define WS_COMPRESS_H

#include <cstdio>
#include <cstdint>

namespace WS {
    typedef uint8_t block_t;
    const size_t IN_BUFFER_CAPACITY = 512;
    const size_t OUT_BUFFER_CAPACITY = 128;

    /**
     * @param bit - must be 1 or 0
     * @param bit_i - bit position to insert bit
     * @param block_i - block to insert bit into
     * @param buffer - buffer containing output
     * @param out - stream for output
     */
    void insertBit(block_t bit, int& bit_i, int& buffer_i, block_t buffer[OUT_BUFFER_CAPACITY], FILE* out) {
        if (bit_i < 0) {
            buffer_i++;
            if (buffer_i >= OUT_BUFFER_CAPACITY) {
                fwrite(buffer, sizeof(block_t), OUT_BUFFER_CAPACITY, out);
                buffer_i = 0;
                buffer[0] = 0;
            }
            buffer[buffer_i] = 0;
            bit_i = 8 * sizeof(block_t) - 1;
        }
        buffer[buffer_i] |= bit << bit_i--;
    }

    void toBinary(FILE* in, FILE* out) {
        if (!in || !out) {
            return;
        }
        char in_buffer[IN_BUFFER_CAPACITY];
        block_t out_buffer[OUT_BUFFER_CAPACITY];
        size_t in_buffer_size;
        int block_i = 0;
        int bit_i = 8 * sizeof(block_t) - 1;

        out_buffer[0] = 0;
        while (in_buffer_size = fread(in_buffer, sizeof(char), IN_BUFFER_CAPACITY, in)) {
            for (size_t i = 0; i < in_buffer_size; i++) {
                switch (in_buffer[i]) {
                case '\t': insertBit(1, bit_i, block_i, out_buffer, out); // Insert 10
                case ' ': insertBit(0, bit_i, block_i, out_buffer, out); break; // Insert 0
                case '\n': insertBit(1, bit_i, block_i, out_buffer, out); insertBit(1, bit_i, block_i, out_buffer, out); // Insert 11
                // Otherwise, ignore character
                }
            }
        }
        fwrite(out_buffer, sizeof(block_t), block_i + 1, out);
    }

    /**
     * @param bit - must be 1 or 0
     * @param blocks - buffer containing output
     * @param bit_i - bit position to insert bit
     * @param block_i - block to insert bit into
     */
    inline void insertBit(block_t bit, block_t* blocks, int& bit_i, int& block_i) {
        if (bit_i < 0) {
            blocks[++block_i] = 0;
            bit_i = 8 * sizeof(block_t) - 1;
        }
        blocks[block_i] |= bit << bit_i--;
    }

    void toBinary(const char* ws, block_t*& blocks, size_t& size) {
        const int BUFFER_SIZE = 128; // Dummy value
        blocks = new block_t[BUFFER_SIZE];
        int block_i = 0;
        int bit_i = 8 * sizeof(block_t) - 1;

        blocks[0] = 0;
        for (size_t i = 0; ws[i]; i++) {
            switch (ws[i]) {
            case '\t': insertBit(1, blocks, bit_i, block_i); // Insert 10
            case ' ': insertBit(0, blocks, bit_i, block_i); break; // Insert 0
            case '\n': insertBit(1, blocks, bit_i, block_i); insertBit(1, blocks, bit_i, block_i); // Insert 11
            // Otherwise, ignore character
            }
        }
        size = block_i + 1;
    }
}

#endif
