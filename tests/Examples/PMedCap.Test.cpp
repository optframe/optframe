#include <gtest/gtest.h>

// system
#include <limits>

// core includes
#include <PMedCap/InitialSolutionRandom.h>
#include <PMedCap.h>

using namespace std;
//using namespace optframe;

using namespace pmedcap;

// basics

// =======================
// memory management tests
// =======================

TEST(ExamplesPMedCapTests, Matrix_Is_Zero)
{
   //BigInteger big;
   //EXPECT_EQ(big.Length(), 1);
   Matrix<int> m(1);
   EXPECT_EQ(m.getNumRows(), 1);
}
