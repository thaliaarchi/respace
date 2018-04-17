#ifndef WS_LEXER_H
#define WS_LEXER_H

#include <stdio.h>

namespace WS {
    enum TokenType {
        // Stack Manipulation
        PUSH,   // Push the number onto the stack
        DUP,    // Duplicate the top item on the stack
        SWAP,   // Swap the top two items on the stack
        DROP,   // Discard the top item on the stack
        // Arithmetic
        ADD,    // Addition
        SUB,    // Subtraction
        MUL,    // Multiplication
        DIV,    // Integer division
        MOD,    // Modulo
        // Heap Access
        STORE,  // Store
        RETRIEVE, // Retrieve
        // Flow Control
        LABEL,  // Mark a location in the program
        CALL,   // Call a subroutine
        JMP,    // Jump unconditionally to a label
        JZ,     // Jump to a label if the top of the stack is zero
        JN,     // Jump to a label if the top of the stack is negative
        RET,    // End a subroutine and transfer control back to the caller
        END,    // End the program
        // I/O
        PRINTC, // Output the character at the top of the stack
        PRINTI, // Output the number at the top of the stack
        READC,  // Read a character and place it in the location given by the top of the stack
        READI,  // Read a number and place it in the location given by the top of the stack
        // Error
        FILEEOF, ERROR = -1
    };

    struct Token {
        const TokenType type;
        const long long value;
        Token(TokenType type = ERROR, long long value = NULL) : type(type), value(value) {}
    };

    struct ParseException {
        const char* msg;
        const size_t line;
        const size_t col;
        ParseException(const char* msg, size_t line, size_t col) : msg(msg), line(line), col(col) {}
    };

    class Lexer {
    private:
        FILE* stream;
        size_t line = 1;
        size_t col = 0;
        bool isEOF_ = false;

        char nextChar() {
            char c;
            do {
                this->col++;
                switch (c = getc(this->stream)) {
                case ' ':
                case '\t':
                    return c;
                case '\n':
                    this->line++;
                    this->col = 0;
                    return c;
                }
            } while (c != EOF);
            if (ferror(this->stream)) {
                throw this->unexpectedException();
            }
            this->isEOF_ = true;
            return EOF;
        }

        int readNumber() {
            int sign;
            switch (this->nextChar()) {
            case ' ': sign = 1; break;
            case '\t': sign = -1; break;
            case '\n': throw this->badCharException();
            default: throw this->unexpectedException();
            }
            return sign * this->readLabel();
        }

        unsigned readLabel() {
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

        ParseException unexpectedException() const {
            return ParseException("An unexpected error occurred.", this->line, this->col);
        }

        ParseException badCharException() const {
            return ParseException("Invalid character encountered.", this->line, this->col);
        }
    public:
        Lexer(FILE* stream) : stream(stream) {}

        Token nextToken() {
            char c;
            switch (c = this->nextChar()) {
            case ' ':
                // Stack Manipulation
                switch (c = this->nextChar()) {
                case ' ': return Token(PUSH, this->readNumber());
                case '\t': throw this->badCharException();
                case '\n':
                    switch (c = this->nextChar()) {
                    case ' ': return Token(DUP);
                    case '\t': return Token(SWAP);
                    case '\n': return Token(DROP);
                    }
                }
                break;
            case '\t':
                switch (c = this->nextChar()) {
                // Arithmetic
                case ' ':
                    switch (c = this->nextChar()) {
                    case ' ':
                        switch (c = this->nextChar()) {
                        case ' ': return Token(ADD);
                        case '\t': return Token(SUB);
                        case '\n': return Token(MUL);
                        }
                        break;
                    case '\t':
                        switch (c = this->nextChar()) {
                        case ' ': return Token(DIV);
                        case '\t': return Token(MOD);
                        case '\n': throw this->badCharException();
                        }
                        break;
                    case '\n': throw this->badCharException();
                    }
                    break;
                // Heap Access
                case '\t':
                    switch (c = this->nextChar()) {
                    case ' ': return Token(STORE);
                    case '\t': return Token(RETRIEVE);
                    case '\n': throw this->badCharException();
                    }
                    break;
                // I/O
                case '\n':
                    switch (c = this->nextChar()) {
                    case ' ':
                        switch (c = this->nextChar()) {
                        case ' ': return Token(PRINTC);
                        case '\t': return Token(PRINTI);
                        case '\n': throw this->badCharException();
                        }
                        break;
                    case '\t':
                        switch (c = this->nextChar()) {
                        case ' ': return Token(READC);
                        case '\t': return Token(READI);
                        case '\n': throw this->badCharException();
                        }
                        break;
                    case '\n': throw this->badCharException();
                    }
                }
                break;
            case '\n':
                // Flow Control
                switch (c = this->nextChar()) {
                case ' ':
                    switch (c = this->nextChar()) {
                    case ' ': return Token(LABEL, this->readLabel());
                    case '\t': return Token(CALL, this->readLabel());
                    case '\n': return Token(JMP, this->readLabel());
                    }
                    break;
                case '\t':
                    switch (c = this->nextChar()) {
                    case ' ': return Token(JZ, this->readLabel());
                    case '\t': return Token(JN, this->readLabel());
                    case '\n': return Token(RET);
                    }
                    break;
                case '\n':
                    switch (c = this->nextChar()) {
                    case ' ':
                    case '\t': throw this->badCharException();
                    case '\n': return Token(END);
                    }
                }
            }
            if (c == EOF) {
                this->isEOF_ = true;
                return Token(FILEEOF);
            }
            throw this->unexpectedException();
        }

        bool isEOF() const {
            return this->isEOF_;
        }
    };
}

#endif
