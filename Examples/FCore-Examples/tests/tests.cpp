// system
#include <functional>

// libs
#include <catch2/catch.hpp>

// core includes
#include <OptFCore/FMove.hpp>
#include <OptFrame/Util/CheckCommand.hpp>

// local src
#include "TSP-fcore.hpp"

using namespace std;
using namespace optframe;
using namespace TSP_fcore;

/*
                  ...                           2
                  ...
       1 
            4
       0                                        3
   */
constexpr std::string_view instance5 = "5\n0 100 100\n1 100 200\n2 2000 2000\n3 2000 100\n4 150 150";

TEST_CASE("FCore-Examples TSP fcore: Test Problem load")
{
   Scanner scanner{ "3\n1 10 10\n2 20 20\n3 30 30\n" };
   pTSP.load(scanner);

   REQUIRE(pTSP.n == 3);
}

TEST_CASE("FCore-Examples TSP fcore: Test Constructive")
{
   Scanner scanner{ std::string(instance5) };
   pTSP.load(scanner);

   REQUIRE(pTSP.n == 5);

   // set random seed for std::random_shuffle
   srand(1000000);

   BasicInitialSearch<ESolutionTSP> initRand(crand, ev);
   ESolutionTSP esol = *initRand.initialSearch({ 1.0 }).first; // 1.0 second max

   // check evaluation
   REQUIRE(esol.second.evaluation() == 9107);
   // check solution
   REQUIRE(esol.first.size() == 5);
   REQUIRE(esol.first == std::vector<int>{ 0, 3, 1, 2, 4 });
}

TEST_CASE("FCore-Examples TSP fcore: Test Check Command")
{
   Scanner scanner{ std::string(instance5) };
   pTSP.load(scanner);

   REQUIRE(pTSP.n == 5);

   // set random seed for std::random_shuffle
   srand(1000000);

   sref<InitialSearch<ESolutionTSP>> initRand{
      new BasicInitialSearch<ESolutionTSP>(crand, ev)
   };

   CheckCommand<ESolutionTSP> check(true); // verbose
   //
   check.addEvaluator(ev);
   check.add(initRand);
   check.addNS(nsswap);    // NS
   check.addNSSeq(nsseq2); // NSSeq

   // bool run(int iterMax, int nSolNSSeq)
   check.run(10, 10);
   //
   REQUIRE(0);
}
