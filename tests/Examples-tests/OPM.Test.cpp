#include <gtest/gtest.h>

// system
#include <limits>

// core includes
#include <OPM/InitialSolutionGreedyRandomized.hpp>
#include <OPM.h>

using namespace std;
//using namespace optframe;

// basics

// =======================
// memory management tests
// =======================

TEST(ExamplesOPMTests, Matrix_Is_Zero)
{
   //BigInteger big;
   //EXPECT_EQ(big.Length(), 1);
   Matrix<int> m(1);
   EXPECT_EQ(m.getNumRows(), 1);
}
