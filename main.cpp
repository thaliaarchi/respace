#include <cstdio>
#include <vector>
#include <stack>
#include <map>
#include "constants.h"
#include "parser.h"
#include "binary.h"

void assemble(const char* in, const char* out) {
    FILE* in_file = fopen(in, "r");
    FILE* out_file = fopen(out, "w");

    WS::Parser parser(in_file);

    std::vector<WS::Instruction> instructions;
    std::stack<WS::integer_t> stack;
    std::map<WS::integer_t, WS::integer_t> heap;
    std::map<WS::integer_t, size_t> labels;
    std::stack<size_t> callers;

    WS::Instruction instr;
    size_t size = 0;
    while (!parser.isEOF()) {
        try {
            parser.next(instr);
            instructions.push_back(instr);
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
        case WS::JMP: fprintf(out_file, "\tjmp label_%lld", instr.value); labels[instr.value] = size; break;
        case WS::JZ: fprintf(out_file, "\tjz label_%lld", instr.value); labels[instr.value] = size; break;
        case WS::JN: fprintf(out_file, "\tjn label_%lld", instr.value); labels[instr.value] = size; break;
        case WS::RET: fprintf(out_file, "\tret"); break;
        case WS::END: fprintf(out_file, "\tend"); break;

        case WS::PRINTC: fprintf(out_file, "\tprintc"); break;
        case WS::PRINTI: fprintf(out_file, "\tprinti"); break;
        case WS::READC: fprintf(out_file, "\treadc"); break;
        case WS::READI: fprintf(out_file, "\treadi"); break;

        case WS::INVALID_INSTR: if (!parser.isEOF()) fprintf(out_file, "ERROR!"); break;
        }
        fputc('\n', out_file);
        size++;
    }
    size_t pc = 0; // Program counter
    while (pc < size) {
        WS::Instruction instr = instructions[pc++];
        switch (instr.type) {
        case WS::PUSH:
            stack.push(instr.value);
            break;
        case WS::DUP:
            if (stack.size() >= 1) {
                stack.push(stack.top());
            }
            break;
        case WS::SWAP:
            if (stack.size() >= 2) {
                WS::integer_t a = stack.top();
                stack.pop();
                WS::integer_t b = stack.top();
                stack.pop();
                stack.push(a);
                stack.push(b);
            }
            break;
        case WS::DROP:
            if (stack.size() >= 1) {
                stack.pop();
            }
            break;

        case WS::ADD:
            if (stack.size() >= 2) {
                WS::integer_t a = stack.top();
                stack.pop();
                WS::integer_t b = stack.top();
                stack.pop();
                stack.push(b + a);
            }
            break;
        case WS::SUB:
            if (stack.size() >= 2) {
                WS::integer_t a = stack.top();
                stack.pop();
                WS::integer_t b = stack.top();
                stack.pop();
                stack.push(b - a);
            }
            break;
        case WS::MUL:
            if (stack.size() >= 2) {
                WS::integer_t a = stack.top();
                stack.pop();
                WS::integer_t b = stack.top();
                stack.pop();
                stack.push(b * a);
            }
            break;
        case WS::DIV:
            if (stack.size() >= 2) {
                WS::integer_t a = stack.top();
                stack.pop();
                WS::integer_t b = stack.top();
                stack.pop();
                stack.push(b / a);
            }
            break;
        case WS::MOD:
            if (stack.size() >= 2) {
                WS::integer_t a = stack.top();
                stack.pop();
                WS::integer_t b = stack.top();
                stack.pop();
                stack.push(b % a);
            }
            break;

        case WS::STORE:
            if (stack.size() >= 2) {
                WS::integer_t value = stack.top();
                stack.pop();
                WS::integer_t address = stack.top();
                stack.pop();
                heap[address] = value;
            }
            break;
        case WS::RETRIEVE:
            if (stack.size() >= 1) {
                WS::integer_t address = stack.top();
                stack.pop();
                stack.push(heap[address]);
            }
            break;

        case WS::LABEL:
            break;
        case WS::CALL:
            callers.push(pc);
            pc = labels[instr.value];
            break;
        case WS::JMP:
            pc = labels[instr.value];
            break;
        case WS::JZ:
            if (stack.size() >= 1 && stack.top() == 0) {
                pc = labels[instr.value];
            }
            break;
        case WS::JN:
            if (stack.size() >= 1 && stack.top() < 0) {
                pc = labels[instr.value];
            }
            break;
        case WS::RET:
            if (stack.size() >= 1) {
                pc = callers.top();
                callers.pop();
            }
            break;
        case WS::END:
            pc = size;
            break;

        case WS::PRINTC:
            if (stack.size() >= 1) {
                putchar(stack.top());
                stack.pop();
            }
            break;
        case WS::PRINTI:
            if (stack.size() >= 1) {
                printf("%lld", stack.top());
                stack.pop();
            }
            break;
        case WS::READC:
            stack.push(getchar());
            break;
        case WS::READI:
            WS::integer_t integer;
            scanf_s("%lld", &integer);
            stack.push(integer);
            break;

        case WS::INVALID_INSTR:
            printf("Invalid Instruction!\n");
            break;
        }
    }
    system("pause");
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
