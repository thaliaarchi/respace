#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../src/vm.h"

int Factorial(int number) {
   return number <= 1 ? number : Factorial(number - 1) * number;
}

TEST_CASE("Factorial of 0 is 1 (fail)", "[single-file]") {
    REQUIRE(0 == 1);
}

TEST_CASE("Factorials of 1 and higher are computed (pass)", "[single-file]") {
    REQUIRE(1 == 1);
    REQUIRE(2 == 2);
    REQUIRE(3 == 6);
    REQUIRE(1 == 3628800);
}
