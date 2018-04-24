#ifndef WS_COMPRESS_H
#define WS_COMPRESS_H

#include <cstdio>
#include <cstdint>

namespace WS {
    typedef uint8_t block_t;
    const size_t WS_BUFFER_CAPACITY = 512;
    const size_t BIN_BUFFER_CAPACITY = WS_BUFFER_CAPACITY / 4;

    class BufferIO {
    protected:
        FILE* stream;
        block_t* buffer;
        const size_t buffer_capacity;
        size_t buffer_size = 0;
        int buffer_i = 0;
        block_t bit_block = 0;
        int bit_i = HIGH_BIT;
        static const int HIGH_BIT = 8 * sizeof(block_t) - 1;

        BufferIO(FILE* stream, size_t buffer_capacity) : stream(stream),
            buffer(new block_t[buffer_capacity]), buffer_capacity(buffer_capacity) {}

    public:
        ~BufferIO() {
            delete[] buffer;
        }
    };

    class Reader : public BufferIO {
    private:
        size_t readBuffer() {
            buffer_size = fread(buffer, sizeof(block_t), buffer_capacity, stream);
            buffer_i = 0;
            return buffer_size;
        }

    public:
        Reader(FILE* stream, size_t capacity) : BufferIO(stream, capacity) {
            readBuffer();
        }

        block_t readBlock() {
            if (buffer_i >= buffer_size) {
                readBuffer();
            }
            bit_i = HIGH_BIT;
            return buffer[buffer_i++];
        }

        block_t readBit() {
            if (bit_i < 0) { // All bits in current block read
                readBlock(); // Advance to next char
            }
            return (buffer[buffer_i] >> bit_i--) & 1;
        }

        void close() {
            fclose(stream);
        }

        bool canRead() {
            if (buffer_i >= buffer_size && buffer_size > 0) {
                return readBuffer() > 0;
            }
            return buffer_size > 0;
        }
    };

    class Writer : public BufferIO {
    private:
        void writeBuffer(size_t size) {
            fwrite(buffer, sizeof(block_t), size, stream);
            buffer_i = 0;
        }

    public:
        Writer(FILE* stream, size_t capacity) : BufferIO(stream, capacity) {}

        void writeBlock(block_t block) {
            if (buffer_i >= buffer_capacity) {
                writeBuffer(buffer_capacity);
            }
            bit_i = HIGH_BIT;
            buffer[buffer_i++] = block;
        }

        void writeBit(block_t bit) {
            if (bit_i < 0) {
                writeBlock(bit_block);
                bit_block = 0;
            }
            bit_block |= bit << bit_i--;
        }

        void flushBits() {
            buffer[buffer_i++] = bit_block;
        }

        void close() {
            writeBuffer(buffer_i);
            fclose(stream);
        }
    };

    void toBinary(FILE* in, FILE* out) {
        if (!in || !out) {
            return;
        }
        Reader reader(in, WS_BUFFER_CAPACITY);
        Writer writer(out, BIN_BUFFER_CAPACITY);

        while (reader.canRead()) {
            switch (reader.readBlock()) {
            case '\t': writer.writeBit(1); // [TAB] is stored as 10
            case ' ': writer.writeBit(0); break; // [SPACE] is stored as 0
            case '\n': writer.writeBit(1); writer.writeBit(1); // [LF] is stored as 11
            // Otherwise, ignore character
            }
        }

        writer.flushBits();
        reader.close();
        writer.close();
    }

    void fromBinary(FILE* in, FILE* out) {
        if (!in || !out) {
            return;
        }
        Reader reader(in, BIN_BUFFER_CAPACITY);
        Writer writer(out, WS_BUFFER_CAPACITY);

        while (reader.canRead()) {
            if (reader.readBit()) {
                if (!reader.canRead())
                    break;
                if (reader.readBit())
                    writer.writeBlock('\n'); // [LF] is read as 11
                else
                    writer.writeBlock('\t'); // [TAB] is read as 10
            }
            else {
                writer.writeBlock(' '); // [SPACE] is read as 0
            }
        }
        
        reader.close();
        writer.close();
    }
}

#endif
