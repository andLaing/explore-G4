#include "nain4.hh"

#include <catch2/catch.hpp>

TEST_CASE("nain4", "[dummy functions]") {

  SECTION("two") {
    REQUIRE(nain4::two  () == 2);
  }

  SECTION("three") {
    REQUIRE(nain4::three() == 3);
  }
}
