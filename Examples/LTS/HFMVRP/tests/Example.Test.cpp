
#ifdef MAKE
#include <catch2/catch.hpp>
#else
#include <catch2/catch_test_macros.hpp>
#endif
// Some instructions for Catch2
// https://github.com/catchorg/Catch2/blob/master/docs/test-cases-and-sections.md

// system
//#include <limits>

//
#include <OptFrame/printable/printable.hpp>

// core includes
#include <HFMVRP/ConstructiveSavings.hpp>

// using namespace std;
// using namespace optframe;

// basics

// =======================
// memory management tests
// =======================

TEST_CASE("ExamplesEtIITests: Matrix_Is_Zero") {
  // BigInteger big;
  // EXPECT_EQ(big.Length(), 1);
  // Matrix<int> m(1);
  // REQUIRE(m.getNumRows() == 1);
  REQUIRE(true);
}
