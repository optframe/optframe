#include <catch2/catch.hpp>
// Some instructions for Catch2
// https://github.com/catchorg/Catch2/blob/master/docs/test-cases-and-sections.md

// system
#include <limits>

// core includes
#include <PMedCap/src/InitialSolutionRandom.h>
#include <PMedCap/PMedCap.h>

using namespace std;
//using namespace optframe;

using namespace pmedcap;

// basics

// =======================
// memory management tests
// =======================

TEST_CASE("ExamplesPMedCapTests : Matrix_Is_Zero")
{
   //BigInteger big;
   //EXPECT_EQ(big.Length(), 1);
   Matrix<int> m(1);
   REQUIRE(m.getNumRows() == 1);
}
