#include <catch2/catch.hpp>
// Some instructions for Catch2
// https://github.com/catchorg/Catch2/blob/master/docs/test-cases-and-sections.md

// system
#include <limits>

// core includes
#include <OPM/src/InitialSolutionGreedyRandomized.hpp>
#include <OPM/OPM.h>

using namespace std;
//using namespace optframe;

// basics

// =======================
// memory management tests
// =======================

TEST_CASE("ExamplesOPMTests : Matrix_Is_Zero")
{
   //BigInteger big;
   //EXPECT_EQ(big.Length(), 1);
   Matrix<int> m(1);
   REQUIRE(m.getNumRows() == 1);
}