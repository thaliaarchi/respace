#ifndef WS_BINARY_H_
#define WS_BINARY_H_

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
        
        char c;
        while (reader.canRead()) {
            if ((c = reader.readBinaryToken()) != EOF) {
                writer.writeBlock(c);
            }
        }
        
        reader.close();
        writer.close();
    }
}

#endif
