#include <catch2/catch_amalgamated.hpp>
// Some instructions for Catch2
// https://github.com/catchorg/Catch2/blob/master/docs/test-cases-and-sections.md
// Now v3: ...

// system
#include <limits>

// core includes
#include <OptFrame/Util/Matrix.hpp>

using namespace std;
using namespace optframe;

// basics

// =======================
// memory management tests
// =======================

TEST_CASE("OptFrameUtilsTests: Matrix_Is_Zero")
{
   //BigInteger big;
   //EXPECT_EQ(big.Length(), 1);
   Matrix<int> m(2);
   REQUIRE(m.getNumRows() == 2);
}
