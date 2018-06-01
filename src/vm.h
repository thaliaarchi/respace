#ifndef WS_INTERPRETER_H_
#define WS_INTERPRETER_H_

#include <iostream>
#include <vector>
#include <stack>
#include <map>
#include "instruction.h"

namespace WS {

class VM {
public:
    VM(std::vector<Instruction> instructions, std::istream &in, std::ostream &out)
        : instructions_(instructions), pc_(0), in_(in), out_(out) {
        initLabels();
    }

    VM(std::vector<Instruction> instructions) : VM(instructions, std::cin, std::cout) {}

    void execute();

    void instrPush(integer_t value);
    void instrDup();
    void instrCopy(integer_t n);
    void instrSwap();
    void instrDrop();
    void instrSlide(integer_t count);

    void instrAdd();
    void instrSub();
    void instrMul();
    void instrDiv();
    void instrMod();

    void instrStore();
    void instrRetrieve();

    void instrLabel();
    void instrCall(integer_t label);
    void instrJmp(integer_t label);
    void instrJz(integer_t label);
    void instrJn(integer_t label);
    void instrRet();
    void instrEnd();

    void instrPrintC();
    void instrPrintI();
    void instrReadC();
    void instrReadI();

    void instrDebugPrintStack();
    void instrDebugPrintHeap();

    std::vector<integer_t> getStack() const;
    std::map<integer_t, integer_t> getHeap() const;

  private:
    std::vector<Instruction> instructions_;
    std::vector<integer_t> stack_;
    std::map<integer_t, integer_t> heap_;
    std::map<integer_t, size_t> labels_;
    std::stack<size_t> call_stack_;
    size_t pc_;
    std::istream &in_;
    std::ostream &out_;

    void initLabels();
    void push(integer_t value);
    void drop();
    integer_t pop();
    integer_t top();
};

} // namespace WS

#endif
