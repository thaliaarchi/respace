#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "../src/instruction.h"
#include "../src/parser.h"
#include "../src/vm.h"

using namespace WS;

std::string captureOutput(VM vm) {
    std::ostringstream oss;
    std::streambuf *cout_streambuf = std::cout.rdbuf();
    std::cout.rdbuf(oss.rdbuf());
    vm.execute();
    std::cout.rdbuf(cout_streambuf); // Restore
    return oss.str();
}

VM loadProgram(const char *path) {
    FILE* in_file = fopen(path, "r");
    Parser parser(in_file);
    std::vector<Instruction> instructions;
    Instruction instr;
    while (parser.next(instr)) {
        instructions.push_back(instr);
    }
    fclose(in_file);
    return VM(instructions);
}

TEST_CASE("VM executes simple stack instructions", "[vm]") {
    const std::vector<integer_t> stack{1, 2, 3, 4, 5};
    VM vm({
        Instruction(PUSH, 1),
        Instruction(PUSH, 2),
        Instruction(PUSH, 3),
        DUP,
        Instruction(PUSH, 1),
        ADD,
        Instruction(COPY, 1),
        Instruction(COPY, 3),
        ADD
    });
    vm.execute();

    SECTION("Simple stack instructions ouput correctly") {
        REQUIRE(vm.getStack() == stack);
    }

    SECTION("VM::getStack returns a copy of the stack") {
        vm.getStack().push_back(42);
        REQUIRE(vm.getStack() == stack);
    }
}

TEST_CASE("Verify bottles program", "[bottles]") {
    VM program({
        // Store " bottles\n" string in heap
        Instruction(PUSH, 0), Instruction(PUSH, ' '), STORE,
        Instruction(PUSH, 1), Instruction(PUSH, 'b'), STORE,
        Instruction(PUSH, 2), Instruction(PUSH, 'o'), STORE,
        Instruction(PUSH, 3), Instruction(PUSH, 't'), STORE,
        Instruction(PUSH, 4), Instruction(PUSH, 't'), STORE,
        Instruction(PUSH, 5), Instruction(PUSH, 'l'), STORE,
        Instruction(PUSH, 6), Instruction(PUSH, 'e'), STORE,
        Instruction(PUSH, 7), Instruction(PUSH, 's'), STORE,
        Instruction(PUSH, 8), Instruction(PUSH, '\n'), STORE,
        Instruction(PUSH, 9), Instruction(PUSH, 0), STORE,

        // While count >= 0
        Instruction(PUSH, 99),
        Instruction(LABEL, 0),
            // Print count
            DUP, PRINTI,

            // For each char of string until 0
            Instruction(PUSH, 0),
            Instruction(LABEL, 1),
                DUP, RETRIEVE,
                DUP, Instruction(JZ, 2),
                PRINTC,
                Instruction(PUSH, 1),
                ADD,
            Instruction(JMP, 1),

            // Loop cleanup
            Instruction(LABEL, 2),
            DROP,
            DROP,

            // Decrement count
            Instruction(PUSH, 1),
            SUB,
            DUP,
        Instruction(JN, 3),
        Instruction(JMP, 0),

        // Cleanup
        Instruction(LABEL, 3),
        DROP,
        END
    });

    REQUIRE(captureOutput(program) == captureOutput(loadProgram("programs/bottles.generated.ws")));
}
