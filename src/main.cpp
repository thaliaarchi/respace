#define _CRT_SECURE_NO_DEPRECATE // To use fopen in VS
#include <cstdio>
#include <vector>
#include "instruction.h"
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
            fprintf(out_file, "\tL%lu:C%lu %s\n", e.line, e.col, e.what);
            continue;
        }
        switch (instr.type) {
        case PUSH:     fprintf(out_file, "\tpush %lld", instr.value); break;
        case DUP:      fprintf(out_file, "\tdup"); break;
        case COPY:     fprintf(out_file, "\tcopy %lld", instr.value); break;
        case SWAP:     fprintf(out_file, "\tswap"); break;
        case DROP:     fprintf(out_file, "\tdrop"); break;
        case SLIDE:    fprintf(out_file, "\tslide %lld", instr.value); break;

        case ADD:      fprintf(out_file, "\tadd"); break;
        case SUB:      fprintf(out_file, "\tsub"); break;
        case MUL:      fprintf(out_file, "\tmul"); break;
        case DIV:      fprintf(out_file, "\tdiv"); break;
        case MOD:      fprintf(out_file, "\tmod"); break;

        case STORE:    fprintf(out_file, "\tstore"); break;
        case RETRIEVE: fprintf(out_file, "\tretrieve"); break;

        case LABEL:    fprintf(out_file, "label_%lld:", instr.value); break;
        case CALL:     fprintf(out_file, "\tcall label_%lld", instr.value); break;
        case JMP:      fprintf(out_file, "\tjmp label_%lld", instr.value); break;
        case JZ:       fprintf(out_file, "\tjz label_%lld", instr.value); break;
        case JN:       fprintf(out_file, "\tjn label_%lld", instr.value); break;
        case RET:      fprintf(out_file, "\tret"); break;
        case END:      fprintf(out_file, "\tend"); break;

        case PRINTC:   fprintf(out_file, "\tprintc"); break;
        case PRINTI:   fprintf(out_file, "\tprinti"); break;
        case READC:    fprintf(out_file, "\treadc"); break;
        case READI:    fprintf(out_file, "\treadi"); break;

        case DEBUG_PRINTSTACK: fprintf(out_file, "\tdebug_printstack"); break;
        case DEBUG_PRINTHEAP:  fprintf(out_file, "\tdebug_printheap"); break;

        case INVALID_INSTR: if (!parser.isEOF()) fprintf(out_file, "ERROR!"); break;
        }
        fputc('\n', out_file);
    }

    fclose(in_file);
    fclose(out_file);
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
    fclose(in_file);
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
    fclose(in_file);
    fclose(out_file);
}

void fromBinary(const char* in, const char* out) {
    FILE* in_file = fopen(in, "rb");
    FILE* out_file = fopen(out, "w");
    fromBinary(in_file, out_file);
    fclose(in_file);
    fclose(out_file);
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        try {
            interpret(argv[1]);
        }
        catch (const char* e) {
            printf("ERROR: %s", e);
        }
    }
    /*toBinary("programs/ws-assemble.generated.ws", "programs/ws-assemble.out.wsx");
    assemble("programs/hello-world.ws", "programs/hello-world.out.wsa");
    toBinary("programs/hello-world.ws", "programs/hello-world.out.wsx");
    fromBinary("programs/hello-world.out.wsx", "programs/hello-world.out.ws");
    interpret("programs/hello-world.ws");*/
    //count(1, 10);
#ifdef _WIN32
    system("pause");
#endif
    return 0;
}
