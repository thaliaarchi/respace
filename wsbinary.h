#ifndef WS_COMPRESS_H
#define WS_COMPRESS_H

#include <cstdio>
#include <cstdint>

namespace WS {
    typedef uint8_t block_t;
    const size_t WS_BUFFER_CAPACITY = 512;
    const size_t BIN_BUFFER_CAPACITY = WS_BUFFER_CAPACITY / 4;

    /**
     * @param bit - must be 1 or 0
     * @param bit_i - bit position to insert bit
     * @param block_i - block to insert bit into
     * @param buffer - buffer containing output
     * @param out - stream for output
     */
    void setBit(block_t bit, int& bit_i, int& buffer_i, block_t buffer[BIN_BUFFER_CAPACITY], FILE* out) {
        if (bit_i < 0) {
            buffer_i++;
            if (buffer_i >= BIN_BUFFER_CAPACITY) {
                fwrite(buffer, sizeof(block_t), BIN_BUFFER_CAPACITY, out);
                buffer_i = 0;
                buffer[0] = 0;
            }
            buffer[buffer_i] = 0;
            bit_i = 8 * sizeof(block_t) - 1;
        }
        buffer[buffer_i] |= bit << bit_i--;
    }

    block_t getBit(int& bit_i, int& buffer_i, block_t buffer[BIN_BUFFER_CAPACITY], FILE* in) {
        if (bit_i < 0) {
            buffer_i++;
            if (buffer_i >= BIN_BUFFER_CAPACITY) {
                fread(buffer, sizeof(block_t), BIN_BUFFER_CAPACITY, in);
                buffer_i = 0;
            }
            bit_i = 8 * sizeof(block_t) - 1;
        }
        return (buffer[buffer_i] >> bit_i--) & 1;
    }

    void toBinary(FILE* in, FILE* out) {
        if (!in || !out) {
            return;
        }
        char in_buffer[WS_BUFFER_CAPACITY];
        block_t out_buffer[BIN_BUFFER_CAPACITY];
        int out_i = 0;
        int bit_i = 8 * sizeof(block_t) - 1;

        out_buffer[0] = 0;
        size_t in_size;
        while (in_size = fread(in_buffer, sizeof(char), WS_BUFFER_CAPACITY, in)) {
            for (size_t i = 0; i < in_size; i++) {
                switch (in_buffer[i]) {
                case '\t': setBit(1, bit_i, out_i, out_buffer, out); // Insert 10
                case ' ': setBit(0, bit_i, out_i, out_buffer, out); break; // Insert 0
                case '\n': setBit(1, bit_i, out_i, out_buffer, out); setBit(1, bit_i, out_i, out_buffer, out); // Insert 11
                // Otherwise, ignore character
                }
            }
        }
        fwrite(out_buffer, sizeof(block_t), out_i + 1, out);
    }

    void fromBinary(FILE* in, FILE* out) {
        if (!in || !out) {
            return;
        }
        block_t in_buffer[BIN_BUFFER_CAPACITY];
        char out_buffer[WS_BUFFER_CAPACITY];
        int in_i = 0;
        int out_i = 0;
        int bit_i = 8 * sizeof(block_t) - 1;
        bool has_prefix = false;

        size_t in_size;
        while (in_size = fread(in_buffer, sizeof(block_t), BIN_BUFFER_CAPACITY, in)) {
            while (in_i < in_size) {
                block_t bit = getBit(bit_i, in_i, in_buffer, out);
                char token;
                if (has_prefix) { // [TAB] is 10, [LF] is 11
                    token = bit ? '\n' : '\t';
                }
                else if (bit) {
                    has_prefix = true;
                    continue;
                }
                else { // [SPACE] is 0
                    token = ' ';
                }
                has_prefix = false;
                if (out_i >= WS_BUFFER_CAPACITY) {
                    fwrite(out_buffer, sizeof(char), WS_BUFFER_CAPACITY, out);
                    out_i = 0;
                }
                out_buffer[out_i] = token;
                out_i++;
            }
        }
        fwrite(out_buffer, sizeof(char), out_i, out);
    }
}

#endif
