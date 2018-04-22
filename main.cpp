#include <cstdio>
#include <bitset>
#include "wsparser.h"
#include "wsbinary.h"

void assemble(const char* in, const char* out) {
    FILE* in_file = nullptr;
    FILE* out_file = nullptr;
    fopen_s(&in_file, in, "r");
    fopen_s(&out_file, out, "w");

    WS::Parser parser(in_file);

    WS::Instruction instr;
    while (!parser.isEOF()) {
        try {
            parser.next(instr);
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

            case WS::ERROR: if (!parser.isEOF()) fprintf(out_file, "\tERROR!"); break;
            }
            fputc('\n', out_file);
        }
        catch (WS::ParseException e) {
            fprintf(out_file, "\tL%d:C%d %s\n", e.line, e.col, e.msg);
        }
    }
}

void toBinary(const char* in, const char* out) {
    FILE* in_file = nullptr;
    FILE* out_file = nullptr;
    fopen_s(&in_file, in, "r");
    fopen_s(&out_file, out, "wb");

    WS::toBinary(in_file, out_file);
    fclose(in_file);
    fclose(out_file);
}

void fromBinary(const char* in, const char* out) {
    FILE* in_file = nullptr;
    FILE* out_file = nullptr;
    fopen_s(&in_file, in, "rb");
    fopen_s(&out_file, out, "w");

    WS::fromBinary(in_file, out_file);
    fclose(in_file);
    fclose(out_file);
}

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

void toBinaryASCII(const char* out, const char* ws) {
    FILE* out_file = nullptr;
    fopen_s(&out_file, out, "wb");
    WS::block_t* binary = nullptr;
    size_t size = 0;

    WS::toBinary(ws, binary, size);

    for (size_t i = 0; i < size; i++) {
        fprintf(out_file, BYTE_TO_BINARY_PATTERN " ", BYTE_TO_BINARY(binary[i]));
        if ((i + 1) % 6 == 0) {
            fprintf(out_file, "\n");
        }
    }
    fclose(out_file);
}

int main(int argc, char* argv[]) {
    assemble("hello-world.ws", "hello-world.wsa");
    toBinary("hello-world.ws", "hello-world.wsx");
    fromBinary("hello-world.wsx", "hello-world.wsx.ws");
    toBinaryASCII("hello-world.wsb",
        "   	 	 \n   	    	\n   		  	  \n   		 		  \n   			  	 \n   "
        "		 				\n   	 	 			\n   	     \n   	 		  \n   		 "
        "				\n   		 		  \n   		 		  \n   		  	 	\n   	  	   "
        "\n	\n  	\n  	\n  	\n  	\n  	\n  	\n  	\n  	\n  	\n  	\n  "
        "	\n  	\n  	\n  \n\n\n");
    return 0;
}
