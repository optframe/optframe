//#include <OptFCore/FCore.hpp> // included src

#include<iostream>

#include<vector>

#include <OptFrame/myconcepts.h>
#include <OptFrame/Evaluation.hpp>
#include <OptFrame/BaseConcepts.hpp>
#include <OptFrame/EPopulation.hpp>

// simple test for package loading (on bazel)

using ESolutionTest = std::pair<int, Evaluation<>>;

static_assert(XESolution<ESolutionTest>);

using EPopulationTest = std::vector<ESolutionTest>;

static_assert(X2ESolution<EPopulationTest, ESolutionTest>);

using EPopulationRealTest = optframe::EPopulation<ESolutionTest>;

static_assert(X2ESolution<EPopulationRealTest, ESolutionTest>);

int main()
{
   EPopulationTest epop;
   std::cout << "A" << std::endl;
   //
   optframe::Evaluation<int> ev;
   //
   return 0;
}
