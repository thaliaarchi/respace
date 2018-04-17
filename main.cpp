#include <stdio.h>
#include "wslexer.h"

int main(int argc, char* argv[]) {
    FILE* inFile = nullptr;
    FILE* outFile = nullptr;
    fopen_s(&inFile, "hello-world.ws", "r");
    fopen_s(&outFile, "hello-world.wsa", "w");

    WS::Lexer lexer(inFile);
    
    while (!lexer.isEOF()) {
        try {
            WS::Token token = lexer.nextToken();
            if (lexer.isEOF()) {
                break;
            }

            switch (token.type) {
            case WS::PUSH: fprintf(outFile, "  push %d", token.value); break;
            case WS::DUP: fprintf(outFile, "  dup"); break;
            case WS::SWAP: fprintf(outFile, "  swap"); break;
            case WS::DROP: fprintf(outFile, "  drop"); break;

            case WS::ADD: fprintf(outFile, "  add"); break;
            case WS::SUB: fprintf(outFile, "  sub"); break;
            case WS::MUL: fprintf(outFile, "  mul"); break;
            case WS::DIV: fprintf(outFile, "  div"); break;
            case WS::MOD: fprintf(outFile, "  mod"); break;

            case WS::STORE: fprintf(outFile, "  store"); break;
            case WS::RETRIEVE: fprintf(outFile, "  retrieve"); break;

            case WS::LABEL: fprintf(outFile, "label_%d:", token.value); break;
            case WS::CALL: fprintf(outFile, "  call %d", token.value); break;
            case WS::JMP: fprintf(outFile, "  jmp %d", token.value); break;
            case WS::JZ: fprintf(outFile, "  jz %d", token.value); break;
            case WS::JN: fprintf(outFile, "  jn %d", token.value); break;
            case WS::RET: fprintf(outFile, "  ret"); break;
            case WS::END: fprintf(outFile, "  end"); break;

            case WS::PRINTC: fprintf(outFile, "  printc"); break;
            case WS::PRINTI: fprintf(outFile, "  printi"); break;
            case WS::READC: fprintf(outFile, "  readc"); break;
            case WS::READI: fprintf(outFile, "  readi"); break;

            case WS::ERROR: fprintf(outFile, "  ERROR!"); break;
            }
            fputc('\n', outFile);
        }
        catch (WS::ParseException e) {
            fprintf(outFile, "  L%d:C%d %s\n", e.line, e.col, e.msg);
        }
    }

    return 0;
}
