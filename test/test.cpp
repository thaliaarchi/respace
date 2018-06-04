#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include <sstream>
#include <string>
#include <vector>
#include <map>
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

void compareProgramOutput(std::vector<Instruction> program_a, std::vector<Instruction> program_b, std::string input) {
    std::istringstream in(input);
    std::ostringstream out_a, out_b;
    VM(program_a, in, out_a).execute();
    VM(program_b, in, out_b).execute();
    REQUIRE(out_a.str() == out_b.str());
}

void testProgramOutput(std::vector<Instruction> program, std::string input, std::string output) {
    std::istringstream in(input);
    std::ostringstream out;
    VM(program, in, out).execute();
    REQUIRE(output == out.str());
}

void testProgram(std::vector<Instruction> program, std::vector<integer_t> stack, std::map<integer_t, integer_t> heap) {
    VM vm(program);
    vm.execute();
    REQUIRE(vm.getStack() == stack);
    REQUIRE(vm.getHeap() == heap);
}

TEST_CASE("VM executes simple stack instructions", "[vm]") {
    const std::vector<integer_t> stack{1, 2, 3, 4, 5};
    std::istringstream in;
    std::ostringstream out;
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
    }, in, out);
    vm.execute();

    SECTION("Simple stack instructions ouput correctly") {
        REQUIRE(vm.getStack() == stack);
    }

    SECTION("VM::getStack returns a copy of the stack") {
        vm.getStack().push_back(42);
        REQUIRE(vm.getStack() == stack);
    }
}

TEST_CASE("Bottles instructions execute identically to parsed WS program", "[bottles]") {
    compareProgramOutput({
#include "../programs/bottles.instr"
    }, parseProgram("programs/bottles.generated.ws"), "");
}

TEST_CASE("Copy and slide run as expected", "[instructions]") {
    testProgram({
        Instruction(PUSH, 1),
        Instruction(PUSH, 2),
        Instruction(PUSH, 3),
        Instruction(PUSH, 4),
        Instruction(PUSH, 5),
        Instruction(PUSH, 6),
        Instruction(COPY, 3), // Should push 3
        Instruction(SLIDE, 2)
    }, { 1, 2, 3, 4, 3 }, {});
}

TEST_CASE("Test programs", "[programs]") {
    SECTION("hello-world.ws") {
        testProgramOutput(parseProgram("programs/hello-world.ws"), "", "Hello, World!\n");
    }
}
