#define _CRT_SECURE_NO_DEPRECATE // To use fopen in VS
#include <cstdio>
#include <vector>
#include "constants.h"
#include "parser.h"
#include "vm.h"
#include "binary.h"

using namespace WS;

void assemble(const char* in, const char* out) {
    FILE* in_file = fopen(in, "r");
    FILE* out_file = fopen(out, "w");

    Parser parser(in_file);

    Instruction instr;
    while (!parser.isEOF()) {
        try {
            parser.next(instr);
        }
        catch (ParseException e) {
            fprintf(out_file, "\tL%d:C%d %s\n", e.line, e.col, e.what);
            continue;
        }
        switch (instr.type) {
        case PUSH: fprintf(out_file, "\tpush %lld", instr.value); break;
        case DUP: fprintf(out_file, "\tdup"); break;
        case SWAP: fprintf(out_file, "\tswap"); break;
        case DROP: fprintf(out_file, "\tdrop"); break;

        case ADD: fprintf(out_file, "\tadd"); break;
        case SUB: fprintf(out_file, "\tsub"); break;
        case MUL: fprintf(out_file, "\tmul"); break;
        case DIV: fprintf(out_file, "\tdiv"); break;
        case MOD: fprintf(out_file, "\tmod"); break;

        case STORE: fprintf(out_file, "\tstore"); break;
        case RETRIEVE: fprintf(out_file, "\tretrieve"); break;

        case LABEL: fprintf(out_file, "label_%lld:", instr.value); break;
        case CALL: fprintf(out_file, "\tcall label_%lld", instr.value); break;
        case JMP: fprintf(out_file, "\tjmp label_%lld", instr.value); break;
        case JZ: fprintf(out_file, "\tjz label_%lld", instr.value); break;
        case JN: fprintf(out_file, "\tjn label_%lld", instr.value); break;
        case RET: fprintf(out_file, "\tret"); break;
        case END: fprintf(out_file, "\tend"); break;

        case PRINTC: fprintf(out_file, "\tprintc"); break;
        case PRINTI: fprintf(out_file, "\tprinti"); break;
        case READC: fprintf(out_file, "\treadc"); break;
        case READI: fprintf(out_file, "\treadi"); break;

        case INVALID_INSTR: if (!parser.isEOF()) fprintf(out_file, "ERROR!"); break;
        }
        fputc('\n', out_file);
    }
}

void interpret(const char* in) {
    FILE* in_file = fopen(in, "r");
    Parser parser(in_file);
    std::vector<Instruction> instructions;
    Instruction instr;
    while (parser.next(instr)) {
        instructions.push_back(instr);
    }
    VM vm(instructions);
    vm.execute();
}

void bottles() {
    VM program({
        // Store " bottles\n" string in heap
        Instruction(PUSH, 0), Instruction(PUSH, ' '), STORE,
        Instruction(PUSH, 1), Instruction(PUSH, 'b'), STORE,
        Instruction(PUSH, 2), Instruction(PUSH, 'o'), STORE,
        Instruction(PUSH, 3), Instruction(PUSH, 't'), STORE,
        Instruction(PUSH, 4), Instruction(PUSH, 't'), STORE,
        Instruction(PUSH, 5), Instruction(PUSH, 'l'), STORE,
        Instruction(PUSH, 6), Instruction(PUSH, 'e'), STORE,
        Instruction(PUSH, 7), Instruction(PUSH, 's'), STORE,
        Instruction(PUSH, 8), Instruction(PUSH, '\n'), STORE,
        Instruction(PUSH, 9), Instruction(PUSH, 0), STORE,

        // While count >= 0
        Instruction(PUSH, 99),
        Instruction(LABEL, 0),
            // Print count
            DUP, PRINTI,

            // For each char of string until 0
            Instruction(PUSH, 0),
            Instruction(LABEL, 1),
                DUP, RETRIEVE,
                DUP, Instruction(JZ, 2),
                PRINTC,
                Instruction(PUSH, 1),
                ADD,
            Instruction(JMP, 1),

            // Loop cleanup
            Instruction(LABEL, 2),
            DROP,
            DROP,

            // Decrement count
            Instruction(PUSH, 1),
            SUB,
            DUP,
        Instruction(JN, 3),
        Instruction(JMP, 0),

        // Cleanup
        Instruction(LABEL, 3),
        DROP,
        END
    });
    program.execute();
}

void count(int min, int max) {
    // Count from 1 to 99
    VM program({
        Instruction(PUSH, min - 1),
        Instruction(LABEL, 0),
        Instruction(PUSH, 1),
        ADD,
        DUP,
        PRINTI,
        Instruction(PUSH, ' '),
        PRINTC,
        DUP,
        Instruction(PUSH, max),
        SUB,
        Instruction(JN, 0),
        DROP,
        Instruction(PUSH, '\n'),
        PRINTC,
        END
    });
    program.execute();
}

void toBinary(const char* in, const char* out) {
    FILE* in_file = fopen(in, "r");
    FILE* out_file = fopen(out, "wb");
    toBinary(in_file, out_file);
}

void fromBinary(const char* in, const char* out) {
    FILE* in_file = fopen(in, "rb");
    FILE* out_file = fopen(out, "w");
    fromBinary(in_file, out_file);
}

int main(int argc, char* argv[]) {
    assemble("hello-world.ws", "hello-world.wsa");
    toBinary("hello-world.ws", "hello-world.wsx");
    fromBinary("hello-world.wsx", "hello-world.wsx.ws");
    interpret("hello-world.ws");
    bottles();
    count(1, 10);
    system("pause");
    return 0;
}
