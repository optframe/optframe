

// C++
#include <iostream>
#include <vector>
//
//#include <OptFCore/FCore.hpp> // included src
#include <OptFrame/BaseConcepts.hpp>
#include <OptFrame/Evaluation.hpp>
#include <OptFrame/Helper/EPopulation.hpp>
#include <OptFrame/MyConcepts.hpp>

// simple test for package loading (on bazel)

using optframe::Evaluation;
using optframe::X2ESolution;
using optframe::XESolution;

using ESolutionTest = std::pair<int, Evaluation<>>;

static_assert(XESolution<ESolutionTest>);

using EPopulationTest = std::vector<ESolutionTest>;

static_assert(X2ESolution<EPopulationTest, ESolutionTest>);

using EPopulationRealTest = optframe::EPopulation<ESolutionTest>;

static_assert(X2ESolution<EPopulationRealTest, ESolutionTest>);

int main() {
  EPopulationTest epop;
  std::cout << "Hello" << std::endl;
  //
  optframe::Evaluation<int> ev;
  //
  return 0;
}
