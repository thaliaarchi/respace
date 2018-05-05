#ifndef WS_CONSTANTS_H_
#define WS_CONSTANTS_H_

namespace WS {
    typedef long long integer_t;

    enum InstructionType {
        // Stack Manipulation [Space]
        PUSH,   // [Space]        # Push the number onto the stack
        DUP,    // [LF][Space]      Duplicate the top item on the stack
        COPY,   // [Tab][Space]   # Copy the nth item on the stack (given by the argument) onto the top of the stack
        SWAP,   // [LF][Tab]        Swap the top two items on the stack
        DROP,   // [LF][LF]         Discard the top item on the stack
        SLIDE,  // [Tab][LF]      # Slide n items off the stack, keeping the top item
        // Arithmetic [Tab][Space]
        ADD,    // [Space][Space]   Addition
        SUB,    // [Space][Tab]     Subtraction
        MUL,    // [Space][LF]      Multiplication
        DIV,    // [Tab][Space]     Integer division
        MOD,    // [Tab][Tab]       Modulo
        // Heap Access [Tab][Tab]
        STORE,  // [Space]          Store
        RETRIEVE, // [Tab]          Retrieve
        // Flow Control [LF]
        LABEL,  // [Space][Space] L Mark a location in the program
        CALL,   // [Space][Tab]   L Call a subroutine
        JMP,    // [Space][LF]    L Jump unconditionally to a label
        JZ,     // [Tab][Space]   L Jump to a label if the top of the stack is zero
        JN,     // [Tab][Tab]     L Jump to a label if the top of the stack is negative
        RET,    // [Tab][LF]        End a subroutine and transfer control back to the caller
        END,    // [LF][LF]         End the program
        // I/O [Tab][LF]
        PRINTC, // [Space][Space]   Output the character at the top of the stack
        PRINTI, // [Space][Tab]     Output the number at the top of the stack
        READC,  // [Tab][Space]     Read a character and place it in the location given by the top of the stack
        READI,  // [Tab][Tab]       Read a number and place it in the location given by the top of the stack
        // Invalid Instruction
        INVALID_INSTR = -1
    };

    struct Instruction {
        InstructionType type;
        integer_t value;

        Instruction(InstructionType type = INVALID_INSTR, integer_t value = 0) : type(type), value(value) {}
    };
}

#endif
