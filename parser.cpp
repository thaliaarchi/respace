#include "parser.h"

namespace WS {
    Parser::Parser(FILE* stream) : stream_(stream) {}

    Instruction Parser::next() {
        char c;
        switch (c = nextChar()) {
        case ' ': return parseStack();     // [Space]      Stack Manipulation
        case '\t':
            switch (c = nextChar()) {
            case ' ': return parseArith(); // [Tab][Space] Arithmetic
            case '\t': return parseHeap(); // [Tab][Tab]   Heap Access
            case '\n': return parseIO();   // [Tab][LF]    I/O
            }
            break;
        case '\n': return parseFlow();     // [LF]         Flow Control
        }
        if (c == EOF) {
            isEOF_ = true;
            return INVALID_INSTR;
        }
        throw unexpectedException();
    }

    bool Parser::next(Instruction& instr) {
        instr = next();
        if (instr.type == INVALID_INSTR) {
            return false;
        }
        return true;
    }

    bool Parser::isEOF() const {
        return isEOF_;
    }

    // Private

    Instruction Parser::parseStack() {
        switch (nextChar()) {
        case ' ': return Instruction(PUSH, readInteger());
        case '\t':
            switch (nextChar()) {
            case ' ': return COPY;
            case '\t': return INVALID_INSTR;
            case '\n': return SLIDE;
            }
            break;
        case '\n':
            switch (nextChar()) {
            case ' ': return DUP;
            case '\t': return SWAP;
            case '\n': return DROP;
            }
        }
        throw unexpectedException();
    }

    Instruction Parser::parseArith() {
        switch (nextChar()) {
        case ' ':
            switch (nextChar()) {
            case ' ': return ADD;
            case '\t': return SUB;
            case '\n': return MUL;
            }
            break;
        case '\t':
            switch (nextChar()) {
            case ' ': return DIV;
            case '\t': return MOD;
            case '\n': return INVALID_INSTR;
            }
            break;
        case '\n': return INVALID_INSTR;
        }
        throw unexpectedException();
    }

    Instruction Parser::parseHeap() {
        switch (nextChar()) {
        case ' ': return STORE;
        case '\t': return RETRIEVE;
        case '\n': return INVALID_INSTR;
        }
        throw unexpectedException();
    }

    Instruction Parser::parseFlow() {
        switch (nextChar()) {
        case ' ':
            switch (nextChar()) {
            case ' ': return Instruction(LABEL, readUnsignedInteger());
            case '\t': return Instruction(CALL, readUnsignedInteger());
            case '\n': return Instruction(JMP, readUnsignedInteger());
            }
            break;
        case '\t':
            switch (nextChar()) {
            case ' ': return Instruction(JZ, readUnsignedInteger());
            case '\t': return Instruction(JN, readUnsignedInteger());
            case '\n': return RET;
            }
            break;
        case '\n':
            switch (nextChar()) {
            case ' ':
            case '\t': return INVALID_INSTR;
            case '\n': return END;
            }
        }
        throw unexpectedException();
    }

    Instruction Parser::parseIO() {
        switch (nextChar()) {
        case ' ':
            switch (nextChar()) {
            case ' ': return PRINTC;
            case '\t': return PRINTI;
            case '\n': return INVALID_INSTR;
            }
            break;
        case '\t':
            switch (nextChar()) {
            case ' ': return READC;
            case '\t': return READI;
            case '\n': return INVALID_INSTR;
            }
            break;
        case '\n': return INVALID_INSTR;
        }
        throw unexpectedException();
    }

    char Parser::nextChar() {
        char c;
        do {
            col_++;
            switch (c = getc(stream_)) {
            case ' ':
            case '\t':
                return c;
            case '\n':
                line_++;
                col_ = 0;
                return c;
            }
        } while (c != EOF);
        if (ferror(stream_)) {
            throw unexpectedException();
        }
        isEOF_ = true;
        return EOF;
    }

    integer_t Parser::readInteger() {
        integer_t sign;
        switch (nextChar()) {
        case ' ': sign = 1; break;
        case '\t': sign = -1; break;
        case '\n': throw badCharException();
        default: throw unexpectedException();
        }
        return sign * readUnsignedInteger();
    }

    integer_t Parser::readUnsignedInteger() {
        integer_t number = 0;
        while (true) {
            switch (nextChar()) {
            case ' ': number <<= 1; continue;
            case '\t': number = (number << 1) + 1; continue;
            case '\n': return number;
            case EOF: throw "Unterminated number";
            default: throw unexpectedException();
            }
        }
        throw unexpectedException();
    }

    ParseException Parser::unexpectedException() const {
        return ParseException("An unexpected error occurred.", line_, col_);
    }

    ParseException Parser::badCharException() const {
        return ParseException("Invalid character encountered.", line_, col_);
    }
}
