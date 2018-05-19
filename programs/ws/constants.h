// Array start addresses
#define INSTR_STACK_START 100
#define STACK_START 2000
#define HEAP_START 2200
#define CALL_STACK_START 2400
#define LABELS_START 2600

// Variables
#define INSTR_PTR 1
#define STACK_TOP 2
#define HEAP_TOP 3
#define CALL_STACK_TOP 4
#define LABELS_TOP 5

// Instruction enum

#define INSTR_PUSH 1
#define INSTR_DUP 2
#define INSTR_COPY 3
#define INSTR_SWAP 4
#define INSTR_DROP 5
#define INSTR_SLIDE 6

#define INSTR_ADD 7
#define INSTR_SUB 8
#define INSTR_MUL 9
#define INSTR_DIV 10
#define INSTR_MOD 11

#define INSTR_STORE 12
#define INSTR_RETRIEVE 13

#define INSTR_LABEL 14
#define INSTR_CALL 15
#define INSTR_JMP 16
#define INSTR_JZ 17
#define INSTR_JN 18
#define INSTR_RET 19
#define INSTR_END 20

#define INSTR_PRINTC 21
#define INSTR_PRINTI 22
#define INSTR_READC 23
#define INSTR_READI 24
