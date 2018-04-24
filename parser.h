#ifndef WS_PARSER_H_
#define WS_PARSER_H_

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
        FILE* stream_;
        size_t line_ = 1;
        size_t col_ = 0;
        bool isEOF_ = false;

        Instruction parseStack();
        Instruction parseArith();
        Instruction parseHeap();
        Instruction parseFlow();
        Instruction parseIO();

        char nextChar();
        int readNumber();
        unsigned readLabel();

        ParseException unexpectedException() const;
        ParseException badCharException() const;
    public:
        Parser(FILE* stream);

        Instruction next();
        bool next(Instruction& instr);
        bool isEOF() const;
    };
}

#endif
