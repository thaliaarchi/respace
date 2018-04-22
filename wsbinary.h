#ifndef WS_COMPRESS_H
#define WS_COMPRESS_H

#include <cstdio>
#include <cstdint>

namespace WS {
    typedef uint8_t block_t;
    const size_t WS_BUFFER_CAPACITY = 512;
    const size_t BIN_BUFFER_CAPACITY = WS_BUFFER_CAPACITY / 4;

    class Cursor {
    protected:
        FILE* stream;
        block_t* buffer;
        const size_t buffer_capacity;
        int buffer_i = 0;

    public:
        Cursor(FILE* stream, size_t buffer_capacity) : stream(stream),
            buffer_capacity(buffer_capacity), buffer(new block_t[buffer_capacity]) {}

        ~Cursor() {
            delete[] buffer;
        }
    };

    class CharReader : public Cursor {
    private:
        size_t buffer_size = 0;

    public:
        CharReader(FILE* stream) : Cursor(stream, WS_BUFFER_CAPACITY) {
            buffer_size = fread(buffer, sizeof(char), buffer_capacity, stream);
        }

        char readChar() {
            if (buffer_i >= buffer_size) {
                buffer_size = fread(buffer, sizeof(block_t), buffer_capacity, stream);
                buffer_i = 0;
            }
            return buffer[buffer_i++];
        }

        void close() {
            fclose(stream);
        }

        bool canRead() {
            return this->buffer_size > 0;
        }
    };

    class CharWriter : public Cursor {
    public:
        CharWriter(FILE* stream) : Cursor(stream, WS_BUFFER_CAPACITY) {}

        void writeChar(char c) {
            if (buffer_i >= buffer_capacity) {
                fwrite(buffer, sizeof(block_t), buffer_capacity, stream);
                buffer_i = 0;
            }
            buffer[buffer_i++] = c;
        }

        void close() {
            fwrite(buffer, sizeof(block_t), buffer_i, stream);
            fclose(stream);
        }
    };

    // Manages reading and writing of binary files bit by bit
    class BitCursor : public Cursor {
    protected:
        static const int HIGH_BIT = 8 * sizeof(block_t) - 1;
        int bit_i = HIGH_BIT;

        BitCursor(FILE* stream) : Cursor(stream, BIN_BUFFER_CAPACITY) {}
    };

    class BitReader : public BitCursor {
    private:
        size_t buffer_size = 0;

    public:
        BitReader(FILE* stream) : BitCursor(stream) {
            buffer_size = fread(buffer, sizeof(block_t), buffer_capacity, stream);
        }

        block_t readBit() {
            block_t bit = (buffer[buffer_i] >> bit_i) & 1;
            bit_i--;
            if (bit_i < 0) {
                buffer_i++;
                if (buffer_i >= buffer_size) {
                    buffer_size = fread(buffer, sizeof(block_t), buffer_capacity, stream);
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
                if (buffer_i >= buffer_capacity) {
                    fwrite(buffer, sizeof(block_t), buffer_capacity, stream);
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
        CharReader reader(in);
        BitWriter writer(out);

        while (reader.canRead()) {
            switch (reader.readChar()) {
            case '\t': writer.writeBit(1); // Insert 10
            case ' ': writer.writeBit(0); break; // Insert 0
            case '\n': writer.writeBit(1); writer.writeBit(1); // Insert 11
            // Otherwise, ignore character
            }
        }

        reader.close();
        writer.close();
    }

    void fromBinary(FILE* in, FILE* out) {
        if (!in || !out) {
            return;
        }
        BitReader reader(in);
        CharWriter writer(out);

        while (reader.canRead()) {
            if (reader.readBit()) {
                if (!reader.canRead())
                    break;
                if (reader.readBit())
                    writer.writeChar('\n'); // [LF] is 11
                else
                    writer.writeChar('\t'); // [TAB] is 10
            }
            else {
                writer.writeChar(' '); // [SPACE] is 0
            }
        }
        
        reader.close();
        writer.close();
    }
}

#endif
