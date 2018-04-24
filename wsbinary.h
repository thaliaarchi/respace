#ifndef WS_BINARY_H
#define WS_BINARY_H

#include "reader.h"
#include "writer.h"

namespace WS {
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
