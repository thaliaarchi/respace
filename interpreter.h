#ifndef WS_INTERPRETER_H_
#define WS_INTERPRETER_H_

#include <vector>
#include <stack>
#include <map>
#include "constants.h"

namespace WS {
    class VM {
    public:
        VM(std::vector<Instruction> instructions) : instructions_(instructions), pc_(0) {
            init();
        }

        void push(Instruction instr);
        void dup();
        void copy();
        void swap();
        void drop();
        void slide();

        void add();
        void sub();
        void mul();
        void div();
        void mod();

        void store();
        void retrieve();

        void label(Instruction instr);
        void call(Instruction instr);
        void jmp(Instruction instr);
        void jz(Instruction instr);
        void jn(Instruction instr);
        void ret();
        void end();

        void printc();
        void printi();
        void readc();
        void readi();

    private:
        std::vector<Instruction> instructions_;
        std::stack<integer_t> stack_;
        std::map<integer_t, integer_t> heap_;
        std::map<integer_t, size_t> labels_;
        std::stack<size_t> call_stack_;
        size_t pc_;

        void init();
    };
}

#endif
