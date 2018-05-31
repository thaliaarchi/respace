#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include <vector>
#include "../src/instruction.h"
#include "../src/vm.h"

using namespace WS;

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
