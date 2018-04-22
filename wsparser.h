#ifndef WS_LEXER_H
#define WS_LEXER_H

#include <cstdio>

namespace WS {
    enum InstructionType {
        ERROR,
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
    };

    struct Instruction {
        InstructionType type;
        long long value;
        Instruction(InstructionType type = ERROR, long long value = NULL) : type(type), value(value) {}
        operator int() const { // To allow instruction to be used as condition in while
            return this->type;
        }
    };

    struct ParseException {
        const char* msg;
        const size_t line;
        const size_t col;
        ParseException(const char* msg, size_t line, size_t col) : msg(msg), line(line), col(col) {}
    };

    class Parser {
    private:
        FILE* stream;
        size_t line = 1;
        size_t col = 0;
        bool isEOF_ = false;

        Instruction parseStackInstr() {
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

        Instruction parseArithInstr() {
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

        Instruction parseHeapInstr() {
            switch (this->nextChar()) {
            case ' ': return Instruction(STORE);
            case '\t': return Instruction(RETRIEVE);
            case '\n': throw this->badCharException();
            }
            throw this->unexpectedException();
        }

        Instruction parseFlowInstr() {
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

        Instruction parseIOInstr() {
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
        Parser(FILE* stream) : stream(stream) {}

        Instruction next() {
            char c;
            switch (c = this->nextChar()) {
            case ' ': return this->parseStackInstr();     // [Space]      Stack Manipulation
            case '\t':
                switch (c = this->nextChar()) {
                case ' ': return this->parseArithInstr(); // [Tab][Space] Arithmetic
                case '\t': return this->parseHeapInstr(); // [Tab][Tab]   Heap Access
                case '\n': return this->parseIOInstr();   // [Tab][LF]    I/O
                }
                break;
            case '\n': return this->parseFlowInstr();     // [LF]         Flow Control
            }
            if (c == EOF) {
                this->isEOF_ = true;
                return Instruction(ERROR);
            }
            throw this->unexpectedException();
        }

        bool next(Instruction& instr) {
            instr = this->next();
            if (instr.type == ERROR) {
                return false;
            }
            return true;
        }

        bool isEOF() const {
            return this->isEOF_;
        }
    };
}

#endif
