#include <catch2/catch.hpp>
// Some instructions for Catch2
// https://github.com/catchorg/Catch2/blob/master/docs/test-cases-and-sections.md

// system
#include <limits>

// core includes
#include <HFM/src/ConstructiveRandom.hpp>
#include <HFM/src/Evaluator.hpp>
#include <HFM/src/MultiEvaluatorHFM.hpp>
#include <HFM/src/ProblemInstance.hpp>
#include <HFM/src/Solution.h>
#include <HFM/src/TreatForecasts.hpp>
//#include <HFM.h> // where?

#include <OptFrame/Util/Matrix.hpp>

using namespace optframe;

// basics

// =======================
// memory management tests
// =======================

TEST_CASE("ExamplesHFMTests : Matrix_Is_Zero")
{
   //BigInteger big;
   //EXPECT_EQ(big.Length(), 1);
   Matrix<int> m(1);
   REQUIRE(m.getNumRows() == 1);
}
