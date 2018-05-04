#include <cstdio>
#include "constants.h"
#include "parser.h"
#include "binary.h"

void assemble(const char* in, const char* out) {
    FILE* in_file = fopen(in, "r");
    FILE* out_file = fopen(out, "w");

    WS::Parser parser(in_file);

    WS::Instruction instr;
    while (!parser.isEOF()) {
        try {
            parser.next(instr);
        }
        catch (WS::ParseException e) {
            fprintf(out_file, "\tL%d:C%d %s\n", e.line, e.col, e.what);
            continue;
        }
        switch (instr.type) {
        case WS::PUSH: fprintf(out_file, "\tpush %lld", instr.value); break;
        case WS::DUP: fprintf(out_file, "\tdup"); break;
        case WS::SWAP: fprintf(out_file, "\tswap"); break;
        case WS::DROP: fprintf(out_file, "\tdrop"); break;

        case WS::ADD: fprintf(out_file, "\tadd"); break;
        case WS::SUB: fprintf(out_file, "\tsub"); break;
        case WS::MUL: fprintf(out_file, "\tmul"); break;
        case WS::DIV: fprintf(out_file, "\tdiv"); break;
        case WS::MOD: fprintf(out_file, "\tmod"); break;

        case WS::STORE: fprintf(out_file, "\tstore"); break;
        case WS::RETRIEVE: fprintf(out_file, "\tretrieve"); break;

        case WS::LABEL: fprintf(out_file, "label_%lld:", instr.value); break;
        case WS::CALL: fprintf(out_file, "\tcall label_%lld", instr.value); break;
        case WS::JMP: fprintf(out_file, "\tjmp label_%lld", instr.value); break;
        case WS::JZ: fprintf(out_file, "\tjz label_%lld", instr.value); break;
        case WS::JN: fprintf(out_file, "\tjn label_%lld", instr.value); break;
        case WS::RET: fprintf(out_file, "\tret"); break;
        case WS::END: fprintf(out_file, "\tend"); break;

        case WS::PRINTC: fprintf(out_file, "\tprintc"); break;
        case WS::PRINTI: fprintf(out_file, "\tprinti"); break;
        case WS::READC: fprintf(out_file, "\treadc"); break;
        case WS::READI: fprintf(out_file, "\treadi"); break;

        case WS::INVALID_INSTR: if (!parser.isEOF()) fprintf(out_file, "ERROR!"); break;
        }
        fputc('\n', out_file);
    }
}

void toBinary(const char* in, const char* out) {
    FILE* in_file = fopen(in, "r");
    FILE* out_file = fopen(out, "wb");
    WS::toBinary(in_file, out_file);
}

void fromBinary(const char* in, const char* out) {
    FILE* in_file = fopen(in, "rb");
    FILE* out_file = fopen(out, "w");
    WS::fromBinary(in_file, out_file);
}

int main(int argc, char* argv[]) {
    assemble("hello-world.ws", "hello-world.wsa");
    toBinary("hello-world.ws", "hello-world.wsx");
    fromBinary("hello-world.wsx", "hello-world.wsx.ws");
    return 0;
}
