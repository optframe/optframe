#include <gtest/gtest.h>

// system
#include <limits>

// core includes
#include <OptFrame/Util/CheckCommand.hpp>
#include <OptFrame/Loader.hpp>
#include <OptFrame/Util/RandGenMersenneTwister.hpp>
#include <OptFrame/Heuristics/NSearch/FirstImprovingNeighbor.hpp>

// example includes
#include <TSP/NearestNeighborConstructive.h>
#include <TSP.h>



using namespace std;
using namespace optframe;

using namespace TSP;

// basics

// =======================
// memory management tests
// =======================




uptr<Scanner> getTSPProblemInstance(int n, int seed = 0)
{
   RandGenMersenneTwister rg(seed);
   //
   stringstream ss;
   ss << n << endl;
   for(unsigned i=0; i<n; i++)
   {
      ss << i << " " << rg.rand(1000) << " " << rg.rand(1000) << endl;
   }
   uptr<Scanner> scanner(new Scanner(ss.str()));
   return scanner;
}

TEST(ExamplesTSPTest, CheckCommand_Is_True)
{
   using ProblemInstance = TSP::ProblemInstance;
   Loader<RepTSP, OPTFRAME_DEFAULT_ADS, SolutionTSP, EvaluationTSP, ESolutionTSP> optframe;
   TSPProblemCommand tsp;

   uptr<Scanner> uptrTSP = getTSPProblemInstance(30);
   //tsp.load(scanner, optframe.factory, optframe.dictionary, optframe.ldictionary);
   tsp.load(*uptrTSP.get(), optframe.factory, optframe.dictionary, optframe.ldictionary);

   //
   bool check_verbose = false;
   CheckCommand<RepTSP, OPTFRAME_DEFAULT_ADS, SolutionTSP> check(check_verbose);

   RandGenMersenneTwister rg(0);
   TSPEvaluator eval1(tsp.p); // Should not be Specific to TSP!! Won't work on Decoder..
   Evaluator<SolutionTSP>& eval = eval1;

   RandomInitialSolutionTSP randomTSP(tsp.p, eval, rg);
   NearestNeighborConstructive cnn(tsp.p, eval, rg);
   ConstructiveBestInsertion cbi(tsp.p, eval, rg);
   NSEnumSwap enumswap(tsp.p, rg);


   // Basic test for Neighborhood Exploration
   FirstImprovingNeighbor<ESolutionTSP> fin(eval, enumswap);
   // =======================================

   NSSeqTSP2Opt<int, OPTFRAME_DEFAULT_ADS, SolutionTSP, DeltaMoveTSP2Opt, ProblemInstance> nsseq_delta_2opt(tsp.p);
   NSSeqTSP2Opt<int, OPTFRAME_DEFAULT_ADS, SolutionTSP> tsp2opt;
   NSSeqTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, SolutionTSP, DeltaMoveTSPOrOptk, ProblemInstance> nsseq_delta_or1(1, tsp.p);
   NSSeqTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, SolutionTSP> tspor1(1);
   NSSeqTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, SolutionTSP> tspor2(2);
   NSSeqTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, SolutionTSP> tspor3(3);

   // TODO: we need to try NSSeqTSPOrOpt , because it requires adapters...
   NSSeqTSPOrOpt<int, OPTFRAME_DEFAULT_ADS, SolutionTSP> tspor_adapt;
   // Maybe S& should be the Representation itself over there.... no getR() inside there.
   // It makes more sense to pass RepTSP + ESolutionTSP... than SolutionTSP + ESolutionTSP
   // Then, should adapters just work for R,ADS pair on XBaseSolution concept?? TODO: think... 



   NSSeqTSPSwap<int, OPTFRAME_DEFAULT_ADS, SolutionTSP> tspswap;

   check.add(randomTSP);
   check.add(cnn);
   check.add(cbi);
   check.add(eval);
   check.add(enumswap);
   check.add(nsseq_delta_2opt);
   check.add(tsp2opt);
   check.add(nsseq_delta_or1);
   check.add(tspor1);
   check.add(tspor2);
   check.add(tspor3);
   check.add(tspswap);

   EXPECT_EQ(check.run(100, 10), true);
}