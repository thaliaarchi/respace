#define _CRT_SECURE_NO_WARNINGS // To use fscanf in VS
#include <vector>
#include <stack>
#include <map>
#include "vm.h"

//#define DEBUG_STORE
//#define DEBUG_END

namespace WS {

void VM::execute() {
    while (pc_ < instructions_.size()) {
        Instruction instr = instructions_[pc_];
        switch (instr.type) {
        case PUSH:   instrPush(instr.value); break;
        case DUP:    instrDup(); break;
        case COPY:   instrCopy(instr.value); break;
        case SWAP:   instrSwap(); break;
        case DROP:   instrDrop(); break;
        case SLIDE:  instrSlide(instr.value); break;

        case ADD:    instrAdd(); break;
        case SUB:    instrSub(); break;
        case MUL:    instrMul(); break;
        case DIV:    instrDiv(); break;
        case MOD:    instrMod(); break;

        case STORE:  instrStore(); break;
        case RETRIEVE: instrRetrieve(); break;

        case LABEL:  instrLabel(); break;
        case CALL:   instrCall(instr.value); break;
        case JMP:    instrJmp(instr.value); break;
        case JZ:     instrJz(instr.value); break;
        case JN:     instrJn(instr.value); break;
        case RET:    instrRet(); break;
        case END:    instrEnd(); break;

        case PRINTC: instrPrintC(); break;
        case PRINTI: instrPrintI(); break;
        case READC:  instrReadC(); break;
        case READI:  instrReadI(); break;

        case DEBUG_PRINTSTACK: instrDebugPrintStack(); break;
        case DEBUG_PRINTHEAP:  instrDebugPrintHeap(); break;

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
void VM::instrCopy(integer_t n) {
    if (n < 0) {
        throw "Runtime Error: Index cannot be negative\n"; // Undefined behavior
    }
    if ((unsigned_t) n >= stack_.size()) {
        throw "Runtime Error: Stack underflow\n";
    }
    push(stack_.at(stack_.size() - n - 1));
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
void VM::instrSlide(integer_t count) {
    if (count < 0) {
        throw "Runtime Error: Count cannot be negative\n"; // Undefined behavior
    }
    if ((unsigned_t) count >= stack_.size()) {
        throw "Runtime Error: Stack underflow\n";
    }
    stack_.erase(stack_.end() - count - 1, stack_.end() - 1);
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
#ifdef DEBUG_STORE
    size_t pc_tmp = pc_;
    instrDebugPrintHeap();
    instrDebugPrintStack();
    pc_ = pc_tmp;
#endif
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
    pc_ = call_stack_.top() + 1;
    call_stack_.pop();
}
// End the program
void VM::instrEnd() {
#ifdef DEBUG_END
    out_.put('\n');
    instrDebugPrintHeap();
    instrDebugPrintStack();
#endif
    pc_ = instructions_.size();
}

// Output the character at the top of the stack
void VM::instrPrintC() {
    out_.put(pop()); // TODO: Integers larger than 7 bits will not display correctly
    pc_++;
}
// Output the number at the top of the stack
void VM::instrPrintI() {
    out_ << pop();
    pc_++;
}
// Read a character and place it in the location given by the top of the stack
void VM::instrReadC() {
    heap_[pop()] = (integer_t) in_.get();
    pc_++;
}
// 	Read a number and place it in the location given by the top of the stack
void VM::instrReadI() {
    integer_t integer;
    in_ >> integer;
    heap_[pop()] = integer;
    pc_++;
}

// Print contents of stack
void VM::instrDebugPrintStack() {
    out_.put('[');
    for (size_t i = 0; i < stack_.size(); i++) {
        out_ << ' ' << stack_.at(i);
    }
    out_ << " ]\n";
    pc_++;
}
// Print contents of heap
void VM::instrDebugPrintHeap() {
    std::map<integer_t, integer_t>::iterator iter = heap_.begin();
    out_.put('{');
    if (iter != heap_.end()) {
        out_ << ' ' << iter->first << ": " << iter->second;
        ++iter;
    }
    for (; iter != heap_.end(); ++iter) {
        out_ << ", " << iter->first << ": " << iter->second;
    }
    out_ << " }\n";
    pc_++;
}

std::vector<integer_t> VM::getStack() const {
    return stack_;
}

std::map<integer_t, integer_t> VM::getHeap() const {
    return heap_;
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
        throw "Runtime Error: Stack underflow\n";
    }
    return stack_.back();
}

} // namespace WS
