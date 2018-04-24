#include "parser.h"

namespace WS {
    Parser::Parser(FILE* stream) : stream_(stream) {}

    Instruction Parser::next() {
        char c;
        switch (c = this->nextChar()) {
        case ' ': return this->parseStack();     // [Space]      Stack Manipulation
        case '\t':
            switch (c = this->nextChar()) {
            case ' ': return this->parseArith(); // [Tab][Space] Arithmetic
            case '\t': return this->parseHeap(); // [Tab][Tab]   Heap Access
            case '\n': return this->parseIO();   // [Tab][LF]    I/O
            }
            break;
        case '\n': return this->parseFlow();     // [LF]         Flow Control
        }
        if (c == EOF) {
            this->isEOF_ = true;
            return Instruction(ERROR);
        }
        throw this->unexpectedException();
    }

    bool Parser::next(Instruction& instr) {
        instr = this->next();
        if (instr.type == ERROR) {
            return false;
        }
        return true;
    }

    bool Parser::isEOF() const {
        return this->isEOF_;
    }

    // Private

    Instruction Parser::parseStack() {
        switch (this->nextChar()) {
        case ' ': return Instruction(PUSH, this->readNumber());
        case '\t': throw this->badCharException();
        case '\n':
            switch (this->nextChar()) {
            case ' ': return Instruction(DUP);
            case '\t': return Instruction(SWAP);
            case '\n': return Instruction(DROP);
            }
        }
        throw this->unexpectedException();
    }

    Instruction Parser::parseArith() {
        switch (this->nextChar()) {
        case ' ':
            switch (this->nextChar()) {
            case ' ': return Instruction(ADD);
            case '\t': return Instruction(SUB);
            case '\n': return Instruction(MUL);
            }
            break;
        case '\t':
            switch (this->nextChar()) {
            case ' ': return Instruction(DIV);
            case '\t': return Instruction(MOD);
            case '\n': throw this->badCharException();
            }
            break;
        case '\n': throw this->badCharException();
        }
        throw this->unexpectedException();
    }

    Instruction Parser::parseHeap() {
        switch (this->nextChar()) {
        case ' ': return Instruction(STORE);
        case '\t': return Instruction(RETRIEVE);
        case '\n': throw this->badCharException();
        }
        throw this->unexpectedException();
    }

    Instruction Parser::parseFlow() {
        switch (this->nextChar()) {
        case ' ':
            switch (this->nextChar()) {
            case ' ': return Instruction(LABEL, this->readLabel());
            case '\t': return Instruction(CALL, this->readLabel());
            case '\n': return Instruction(JMP, this->readLabel());
            }
            break;
        case '\t':
            switch (this->nextChar()) {
            case ' ': return Instruction(JZ, this->readLabel());
            case '\t': return Instruction(JN, this->readLabel());
            case '\n': return Instruction(RET);
            }
            break;
        case '\n':
            switch (this->nextChar()) {
            case ' ':
            case '\t': throw this->badCharException();
            case '\n': return Instruction(END);
            }
        }
        throw this->unexpectedException();
    }

    Instruction Parser::parseIO() {
        switch (this->nextChar()) {
        case ' ':
            switch (this->nextChar()) {
            case ' ': return Instruction(PRINTC);
            case '\t': return Instruction(PRINTI);
            case '\n': throw this->badCharException();
            }
            break;
        case '\t':
            switch (this->nextChar()) {
            case ' ': return Instruction(READC);
            case '\t': return Instruction(READI);
            case '\n': throw this->badCharException();
            }
            break;
        case '\n': throw this->badCharException();
        }
        throw this->unexpectedException();
    }

    char Parser::nextChar() {
        char c;
        do {
            this->col_++;
            switch (c = getc(this->stream_)) {
            case ' ':
            case '\t':
                return c;
            case '\n':
                this->line_++;
                this->col_ = 0;
                return c;
            }
        } while (c != EOF);
        if (ferror(this->stream_)) {
            throw this->unexpectedException();
        }
        this->isEOF_ = true;
        return EOF;
    }

    int Parser::readNumber() {
        int sign;
        switch (this->nextChar()) {
        case ' ': sign = 1; break;
        case '\t': sign = -1; break;
        case '\n': throw this->badCharException();
        default: throw this->unexpectedException();
        }
        return sign * this->readLabel();
    }

    unsigned Parser::readLabel() {
        int number = 0;
        while (true) {
            switch (this->nextChar()) {
            case ' ': number <<= 1; continue;
            case '\t': number = (number << 1) + 1; continue;
            case '\n': return number;
            case EOF: throw "Unterminated number";
            default: throw this->unexpectedException();
            }
        }
        throw this->unexpectedException();
    }

    ParseException Parser::unexpectedException() const {
        return ParseException("An unexpected error occurred.", this->line_, this->col_);
    }

    ParseException Parser::badCharException() const {
        return ParseException("Invalid character encountered.", this->line_, this->col_);
    }
}
