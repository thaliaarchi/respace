#ifndef WS_COMPRESS_H
#define WS_COMPRESS_H

#include <cstdio>
#include <cstdint>

namespace WS {
    typedef uint8_t block_t;
    const size_t WS_BUFFER_CAPACITY = 512;
    const size_t BIN_BUFFER_CAPACITY = WS_BUFFER_CAPACITY / 4;

    // Manages reading and writing of binary files bit by bit
    class BitCursor {
    protected:
        FILE* stream;
        block_t buffer[BIN_BUFFER_CAPACITY];
        int buffer_i = 0;
        int bit_i = HIGH_BIT;
        static const int HIGH_BIT = 8 * sizeof(block_t) - 1;

        BitCursor(FILE* stream) : stream(stream) {}
    };

    class BitReader : public BitCursor {
    private:
        size_t buffer_size = 0;

    public:
        BitReader(FILE* stream) : BitCursor(stream) {
            buffer_size = fread(buffer, sizeof(block_t), BIN_BUFFER_CAPACITY, stream);
        }

        block_t readBit() {
            block_t bit = (buffer[buffer_i] >> bit_i) & 1;
            bit_i--;
            if (bit_i < 0) {
                buffer_i++;
                if (buffer_i >= buffer_size) {
                    buffer_size = fread(buffer, sizeof(block_t), BIN_BUFFER_CAPACITY, stream);
                    buffer_i = 0;
                }
                bit_i = HIGH_BIT;
            }
            return bit;
        }

        void close() {
            fclose(stream);
        }

        bool canRead() {
            return this->buffer_size > 0;
        }
    };

    class BitWriter : public BitCursor {
    private:
        block_t block = 0;

    public:
        BitWriter(FILE* stream) : BitCursor(stream) {}

        void writeBit(block_t bit) {
            if (bit_i < 0) {
                buffer[buffer_i] = block;
                buffer_i++;
                if (buffer_i >= BIN_BUFFER_CAPACITY) {
                    fwrite(buffer, sizeof(block_t), BIN_BUFFER_CAPACITY, stream);
                    buffer_i = 0;
                }
                block = 0;
                bit_i = HIGH_BIT;
            }
            block |= bit << bit_i--;
        }

        void close() {
            buffer[buffer_i] = block;
            fwrite(buffer, sizeof(block_t), buffer_i + 1, stream);
            fclose(stream);
        }
    };

    void toBinary(FILE* in, FILE* out) {
        if (!in || !out) {
            return;
        }
        char in_buffer[WS_BUFFER_CAPACITY];
        BitWriter cursor(out);

        size_t in_size;
        while (in_size = fread(in_buffer, sizeof(char), WS_BUFFER_CAPACITY, in)) {
            for (size_t i = 0; i < in_size; i++) {
                switch (in_buffer[i]) {
                case '\t': cursor.writeBit(1); // Insert 10
                case ' ': cursor.writeBit(0); break; // Insert 0
                case '\n': cursor.writeBit(1); cursor.writeBit(1); // Insert 11
                // Otherwise, ignore character
                }
            }
        }
        cursor.close();
    }

    void fromBinary(FILE* in, FILE* out) {
        if (!in || !out) {
            return;
        }
        BitReader cursor(in);
        char out_buffer[WS_BUFFER_CAPACITY];
        int out_i = 0;
        bool has_prefix = false;

        while (cursor.canRead()) {
            block_t bit = cursor.readBit();
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
        fwrite(out_buffer, sizeof(char), out_i, out);
    }
}

#endif
