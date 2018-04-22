#include <stdio.h>
#include "wsparser.h"

int main(int argc, char* argv[]) {
    FILE* inFile = nullptr;
    FILE* outFile = nullptr;
    fopen_s(&inFile, "hello-world.ws", "r");
    fopen_s(&outFile, "hello-world.wsa", "w");

    WS::Parser parser(inFile);
    
    WS::Instruction instr;
    while (!parser.isEOF()) {
        try {
            parser.next(instr);
            switch (instr.type) {
            case WS::PUSH: fprintf(outFile, "\tpush %lld", instr.value); break;
            case WS::DUP: fprintf(outFile, "\tdup"); break;
            case WS::SWAP: fprintf(outFile, "\tswap"); break;
            case WS::DROP: fprintf(outFile, "\tdrop"); break;

            case WS::ADD: fprintf(outFile, "\tadd"); break;
            case WS::SUB: fprintf(outFile, "\tsub"); break;
            case WS::MUL: fprintf(outFile, "\tmul"); break;
            case WS::DIV: fprintf(outFile, "\tdiv"); break;
            case WS::MOD: fprintf(outFile, "\tmod"); break;

            case WS::STORE: fprintf(outFile, "\tstore"); break;
            case WS::RETRIEVE: fprintf(outFile, "\tretrieve"); break;

            case WS::LABEL: fprintf(outFile, "label_%lld:", instr.value); break;
            case WS::CALL: fprintf(outFile, "\tcall label_%lld", instr.value); break;
            case WS::JMP: fprintf(outFile, "\tjmp label_%lld", instr.value); break;
            case WS::JZ: fprintf(outFile, "\tjz label_%lld", instr.value); break;
            case WS::JN: fprintf(outFile, "\tjn label_%lld", instr.value); break;
            case WS::RET: fprintf(outFile, "\tret"); break;
            case WS::END: fprintf(outFile, "\tend"); break;

            case WS::PRINTC: fprintf(outFile, "\tprintc"); break;
            case WS::PRINTI: fprintf(outFile, "\tprinti"); break;
            case WS::READC: fprintf(outFile, "\treadc"); break;
            case WS::READI: fprintf(outFile, "\treadi"); break;

            case WS::ERROR: fprintf(outFile, "\tERROR!"); break;
            }
            fputc('\n', outFile);
        }
        catch (WS::ParseException e) {
            fprintf(outFile, "\tL%d:C%d %s\n", e.line, e.col, e.msg);
        }
    }

    return 0;
}
