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

        size_t in_size;
        while (in_size = fread(in_buffer, sizeof(block_t), BIN_BUFFER_CAPACITY, in)) {
            while (in_i < in_size) {
                char token;
                if (getBit(bit_i, in_i, in_buffer, out)) {
                    if (getBit(bit_i, in_i, in_buffer, out))
                        token = '\n'; // Read 11
                    else
                        token = '\t'; // Read 10
                }
                else {
                    token = ' '; // Read 0
                }
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

    /**
     * @param bit - must be 1 or 0
     * @param blocks - buffer containing output
     * @param bit_i - bit position to insert bit
     * @param block_i - block to insert bit into
     */
    inline void setBit(block_t bit, block_t* blocks, int& bit_i, int& block_i) {
        if (bit_i < 0) {
            blocks[++block_i] = 0;
            bit_i = 8 * sizeof(block_t) - 1;
        }
        blocks[block_i] |= bit << bit_i--;
    }

    // Simple version without file I/O or buffer resizing
    void toBinary(const char* ws, block_t*& blocks, size_t& size) {
        const int BUFFER_SIZE = 128; // Dummy value, must be large enough to fit entire output
        blocks = new block_t[BUFFER_SIZE];
        int block_i = 0;
        int bit_i = 8 * sizeof(block_t) - 1;

        blocks[0] = 0;
        for (size_t i = 0; ws[i]; i++) {
            switch (ws[i]) {
            case '\t': setBit(1, blocks, bit_i, block_i); // Insert 10
            case ' ': setBit(0, blocks, bit_i, block_i); break; // Insert 0
            case '\n': setBit(1, blocks, bit_i, block_i); setBit(1, blocks, bit_i, block_i); // Insert 11
            // Otherwise, ignore character
            }
        }
        size = block_i + 1;
    }
}

#endif
