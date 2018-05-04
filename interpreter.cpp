#include <vector>
#include <stack>
#include <map>
#include "interpreter.h"

namespace WS {
    void VM::push(Instruction instr) {
        stack_.push(instr.value);
    }
    void VM::dup() {
        if (stack_.size() >= 1) {
            stack_.push(stack_.top());
        }
    }
    void VM::copy() {
    }
    void VM::swap() {
        if (stack_.size() >= 2) {
            integer_t a = stack_.top();
            stack_.pop();
            integer_t b = stack_.top();
            stack_.pop();
            stack_.push(a);
            stack_.push(b);
        }
    }
    void VM::drop() {
        if (stack_.size() >= 1) {
            stack_.pop();
        }
    }
    void VM::slide() {
    }

    void VM::add() {
        if (stack_.size() >= 2) {
            integer_t a = stack_.top();
            stack_.pop();
            integer_t b = stack_.top();
            stack_.pop();
            stack_.push(b + a);
        }
    }
    void VM::sub() {
        if (stack_.size() >= 2) {
            integer_t a = stack_.top();
            stack_.pop();
            integer_t b = stack_.top();
            stack_.pop();
            stack_.push(b - a);
        }
    }
    void VM::mul() {
        if (stack_.size() >= 2) {
            integer_t a = stack_.top();
            stack_.pop();
            integer_t b = stack_.top();
            stack_.pop();
            stack_.push(b * a);
        }
    }
    void VM::div() {
        if (stack_.size() >= 2) {
            integer_t a = stack_.top();
            stack_.pop();
            integer_t b = stack_.top();
            stack_.pop();
            stack_.push(b / a);
        }
    }
    void VM::mod() {
        if (stack_.size() >= 2) {
            integer_t a = stack_.top();
            stack_.pop();
            integer_t b = stack_.top();
            stack_.pop();
            stack_.push(b % a);
        }
    }

    void VM::store() {
        if (stack_.size() >= 2) {
            integer_t value = stack_.top();
            stack_.pop();
            integer_t address = stack_.top();
            stack_.pop();
            heap_[address] = value;
        }
    }
    void VM::retrieve() {
        if (stack_.size() >= 1) {
            integer_t address = stack_.top();
            stack_.pop();
            stack_.push(heap_[address]);
        }
    }

    void VM::label(Instruction instr) {}
    void VM::call(Instruction instr) {
        call_stack_.push(pc_);
        pc_ = labels_[instr.value];
    }
    void VM::jmp(Instruction instr) {
        pc_ = labels_[instr.value];
    }
    void VM::jz(Instruction instr) {
        if (stack_.size() >= 1 && stack_.top() == 0) {
            pc_ = labels_[instr.value];
        }
    }
    void VM::jn(Instruction instr) {
        if (stack_.size() >= 1 && stack_.top() < 0) {
            pc_ = labels_[instr.value];
        }
    }
    void VM::ret() {
        if (stack_.size() >= 1) {
            pc_ = call_stack_.top();
            call_stack_.pop();
        }
    }
    void VM::end() {
        pc_ = instructions_.size();
    }

    void VM::printc() {
        if (stack_.size() >= 1) {
            putchar(stack_.top());
            stack_.pop();
        }
    }
    void VM::printi() {
        if (stack_.size() >= 1) {
            printf("%lld", stack_.top());
            stack_.pop();
        }
    }
    void VM::readc() {
        stack_.push(getchar());
    }
    void VM::readi() {
        integer_t integer;
        scanf_s("%lld", &integer);
        stack_.push(integer);
    }

    // Private

    void VM::init() {
        for (size_t i = 0; i < instructions_.size(); i++) {
            Instruction instr = instructions_.at(i);
            if (instr.type == LABEL) {
                labels_[instr.value] = i;
            }
        }
    }
}
