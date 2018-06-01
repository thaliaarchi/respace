#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include <sstream>
#include <string>
#include <vector>
#include "../src/instruction.h"
#include "../src/parser.h"
#include "../src/vm.h"

using namespace WS;

std::vector<Instruction> parseProgram(const char *path) {
    FILE* in_file = fopen(path, "r");
    Parser parser(in_file);
    std::vector<Instruction> instructions;
    Instruction instr;
    while (parser.next(instr)) {
        instructions.push_back(instr);
    }
    fclose(in_file);
    return instructions;
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
    std::istringstream instr_in, ws_in;
    std::ostringstream instr_out, ws_out;

    VM({
#include "../programs/bottles.instr"
    }, instr_in, instr_out).execute();

    VM(parseProgram("programs/bottles.generated.ws"), ws_in, ws_out).execute();

    REQUIRE(instr_out.str() == ws_out.str());
}
