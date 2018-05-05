#include <vector>
#include <stack>
#include <map>
#include "vm.h"

namespace WS {
    void VM::execute() {
        while (pc_ < instructions_.size()) {
            Instruction instr = instructions_[pc_];
            switch (instr.type) {
            case PUSH:  instrPush(instr.value); break;
            case DUP:   instrDup(); break;
            case COPY:  instrCopy(); break;
            case SWAP:  instrSwap(); break;
            case DROP:  instrDrop(); break;
            case SLIDE: instrSlide(); break;

            case ADD: instrAdd(); break;
            case SUB: instrSub(); break;
            case MUL: instrMul(); break;
            case DIV: instrDiv(); break;
            case MOD: instrMod(); break;

            case STORE:    instrStore(); break;
            case RETRIEVE: instrRetrieve(); break;

            case LABEL: instrLabel(); break;
            case CALL:  instrCall(instr.value); break;
            case JMP:   instrJmp(instr.value); break;
            case JZ:    instrJz(instr.value); break;
            case JN:    instrJn(instr.value); break;
            case RET:   instrRet(); break;
            case END:   instrEnd(); break;

            case PRINTC: instrPrintC(); break;
            case PRINTI: instrPrintI(); break;
            case READC:  instrReadC(); break;
            case READI:  instrReadI(); break;

            case INVALID_INSTR: throw "Invalid instruction!";
            }
        }
    }

    // Push the number onto the stack
    void VM::instrPush(integer_t value) {
        stack_.push_back(value);
        pc_++;
    }
    // Duplicate the top item on the stack
    void VM::instrDup() {
        push(top());
        pc_++;
    }
    // Copy the nth item on the stack (given by the argument) onto the top of the stack
    void VM::instrCopy() {
        pc_++;
    }
    // Swap the top two items on the stack
    void VM::instrSwap() {
        integer_t a = pop();
        integer_t b = pop();
        push(a);
        push(b);
        pc_++;
    }
    // Discard the top item on the stack
    void VM::instrDrop() {
        drop();
        pc_++;
    }
    // Slide n items off the stack, keeping the top item
    void VM::instrSlide() {
        pc_++;
    }

    // Addition
    void VM::instrAdd() {
        push(pop() + pop());
        pc_++;
    }
    // Subtraction
    void VM::instrSub() {
        integer_t a = pop();
        integer_t b = pop();
        push(b - a);
        pc_++;
    }
    // Multiplication
    void VM::instrMul() {
        push(pop() * pop());
        pc_++;
    }
    // Integer Division
    void VM::instrDiv() {
        integer_t a = pop();
        integer_t b = pop();
        push(b / a);
        pc_++;
    }
    // Modulo
    void VM::instrMod() {
        integer_t a = pop();
        integer_t b = pop();
        push(b % a);
        pc_++;
    }

    // Store
    void VM::instrStore() {
        integer_t value = pop();
        integer_t address = pop();
        heap_[address] = value;
        pc_++;
    }
    // Retrieve
    void VM::instrRetrieve() {
        integer_t address = pop();
        push(heap_[address]);
        pc_++;
    }

    // Mark a location in the program
    void VM::instrLabel() {
        pc_++;
    }
    // Call a subroutine
    void VM::instrCall(integer_t label) {
        call_stack_.push(pc_);
        pc_ = labels_[label];
    }
    // Jump unconditionally to a label
    void VM::instrJmp(integer_t label) {
        pc_ = labels_[label];
    }
    // Jump to a label if the top of the stack is zero
    void VM::instrJz(integer_t label) {
        if (pop() == 0) {
            instrJmp(label);
        }
        else {
            pc_++;
        }
    }
    // Jump to a label if the top of the stack is negative
    void VM::instrJn(integer_t label) {
        if (pop() < 0) {
            instrJmp(label);
        }
        else {
            pc_++;
        }
    }
    // End a subroutine and transfer control back to the caller
    void VM::instrRet() {
        pc_ = call_stack_.top();
        call_stack_.pop();
    }
    // End the program
    void VM::instrEnd() {
        pc_ = instructions_.size();
    }

    // Output the character at the top of the stack
    void VM::instrPrintC() {
        putchar((char) pop()); // Integers larger than 7 bits will not display correctly
        pc_++;
    }
    // Output the number at the top of the stack
    void VM::instrPrintI() {
        printf("%lld", pop());
        pc_++;
    }
    // Read a character and place it in the location given by the top of the stack
    void VM::instrReadC() {
        push(getchar());
        pc_++;
    }
    // 	Read a number and place it in the location given by the top of the stack
    void VM::instrReadI() {
        integer_t integer;
        scanf("%lld", &integer);
        push(integer);
        pc_++;
    }

    // Private

    void VM::initLabels() {
        for (size_t i = 0; i < instructions_.size(); i++) {
            Instruction instr = instructions_.at(i);
            if (instr.type == LABEL) {
                labels_[instr.value] = i;
            }
        }
    }

    void VM::push(integer_t value) {
        stack_.push_back(value);
    }

    void VM::drop() {
        if (stack_.size() >= 1) {
            stack_.pop_back();
        }
    }

    integer_t VM::pop() {
        integer_t topValue = top();
        stack_.pop_back();
        return topValue;
    }

    integer_t VM::top() {
        if (stack_.size() < 1) {
            throw "Stack is empty!";
        }
        return stack_.back();
    }
}
