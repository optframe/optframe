#include <gtest/gtest.h>

// system
#include <limits>

// core includes
#include <HFM/ConstructiveRandom.hpp>
#include <HFM/Evaluator.hpp>
#include <HFM/MultiEvaluatorHFM.hpp>
#include <HFM/ProblemInstance.hpp>
#include <HFM/Solution.h>
#include <HFM/TreatForecasts.hpp>
//#include <HFM.h> // where?

using namespace std;
//using namespace optframe;

// basics

// =======================
// memory management tests
// =======================

TEST(ExamplesHFMTests, Matrix_Is_Zero)
{
   //BigInteger big;
   //EXPECT_EQ(big.Length(), 1);
   Matrix<int> m(1);
   EXPECT_EQ(m.getNumRows(), 1);
}
