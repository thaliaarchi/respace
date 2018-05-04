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
            initLabels();
        }

        void execute();

        void instrPush(Instruction instr);
        void instrDup();
        void instrCopy();
        void instrSwap();
        void instrDrop();
        void instrSlide();

        void instrAdd();
        void instrSub();
        void instrMul();
        void instrDiv();
        void instrMod();

        void instrStore();
        void instrRetrieve();

        void instrLabel();
        void instrCall(Instruction instr);
        void instrJmp(Instruction instr);
        void instrJz(Instruction instr);
        void instrJn(Instruction instr);
        void instrRet();
        void instrEnd();

        void instrPrintC();
        void instrPrintI();
        void instrReadC();
        void instrReadI();

    private:
        std::vector<Instruction> instructions_;
        std::vector<integer_t> stack_;
        std::map<integer_t, integer_t> heap_;
        std::map<integer_t, size_t> labels_;
        std::stack<size_t> call_stack_;
        size_t pc_;

        void initLabels();
        void push(integer_t value);
        void drop();
        integer_t pop();
        integer_t top();
    };
}

#endif
