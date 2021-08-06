
#include "TSP-fxcore.hpp" // implementation of TSP
#include <iostream>

// import everything on main()
using namespace std;
using namespace optframe;
using namespace scannerpp;
using namespace TSP_fxcore;

int
main()
{
   srand(0); // using system random (weak... just an example!)

   // load data into problem context 'pTSP'
   Scanner scanner{ "3\n1 10 10\n2 20 20\n3 30 30\n" };
   pTSP.load(scanner);
   std::cout << pTSP.dist << std::endl;

   // evaluator
   //TSPEval ev;
   sref<Evaluator<ESolutionTSP::first_type, ESolutionTSP::second_type>> ev = new TSPEval();

   // create simple solution
   //TSPRandom crand;
   sref<Constructive<ESolutionTSP::first_type>> crand = new TSPRandom();
   std::vector<int> sol = *crand->generateSolution(0);
   std::cout << sol << std::endl;

   // evaluation value and store on ESolution pair
   ESolutionTSP esol(sol, ev->evaluate(sol));
   esol.second.print(); // print evaluation

   // swap 0 with 1
   MoveSwap move{ make_pair(0, 1) };
   move.print();

   //NSSwap nsswap;
   sref<NS<ESolutionTSP>> nsswap = new NSSwap();
   // move for solution 'esol'
   auto m1 = nsswap->randomMove(esol);
   m1->print();

   //NS<ESolutionTSP>& r_nsswap = nsswap;
   //sref<NS<ESolutionTSP>> nsswap_ref(std::move(r_nsswap));

   //NSSeqSwapFancy swapFancy;
   sref<NSSeq<ESolutionTSP>> swapFancy = new NSSeqSwapFancy();

   {
      std::cout << std::endl;
      std::cout << "begin listing NSSeqSwapFancy" << std::endl;
      auto it1 = swapFancy->getIterator(esol);
      for (it1->first(); !it1->isDone(); it1->next())
         it1->current()->print();
      std::cout << "end listing NSSeqSwapFancy" << std::endl;
   }

   // ====================

   // Random number generator
   sref<RandGen> rg = new RandGen(60000);

   // ====================

   sref<NSSeq<ESolutionTSP>> rswap = new RNSSeqSwap(rg);

   {
      std::cout << std::endl;
      std::cout << "begin listing RNSSeqSwap" << std::endl;
      auto it1 = rswap->getIterator(esol);
      for (it1->first(); !it1->isDone(); it1->next())
         it1->current()->print();
      std::cout << "end listing RNSSeqSwap" << std::endl;
   }
   // ====================

   {
      std::cout << std::endl;
      std::cout << "begin listing multiple random RNSSeqSwap" << std::endl;
      for (unsigned i = 0; i < 10; i++)
         rswap->randomMove(esol)->print();
      std::cout << "end listing RNSSeqSwap" << std::endl;
   }

   rg->setSeed(100000);
   sref<NSSeq<ESolutionTSP>> rswap_enum = new RNSEnumSwap(rg);
   {
      std::cout << std::endl;
      std::cout << "begin listing RNSEnumSwap" << std::endl;
      auto it1 = rswap_enum->getIterator(esol);
      for (it1->first(); !it1->isDone(); it1->next())
         it1->current()->print();
      std::cout << "end listing RNSEnumSwap" << std::endl;
   }

   rg->setSeed(0);
   sref<NSSeq<ESolutionTSP>> rswap_enum_shuffle = new RNSEnumSwapShuffle(rg);
   {
      std::cout << std::endl;
      std::cout << "begin listing RNSEnumSwapShuffle" << std::endl;
      auto it1 = rswap_enum_shuffle->getIterator(esol);
      for (it1->first(); !it1->isDone(); it1->next())
         it1->current()->print();
      std::cout << "end listing RNSEnumSwapShuffle" << std::endl;
   }

   // testing simulated annealing
   //BasicInitialSearch<ESolutionTSP> initRand(crand, ev);
   //
   sref<GeneralEvaluator<ESolutionTSP>>
     gev = ev;
   //
   sref<InitialSearch<ESolutionTSP>> initRand = new BasicInitialSearch<ESolutionTSP>(crand, ev);
   BasicSimulatedAnnealing<ESolutionTSP> sa(
     gev, initRand, swapFancy, 0.98, 100, 99999, rg);
   auto status = sa.search(StopCriteria<ESolutionTSP::second_type>{ 10.0 }); // 10.0 seconds max
   ESolutionTSP best = *status.best;                                         //*sa.getBestSolution();
   // best solution value
   best.second.print();

   std::cout << "FINISHED" << std::endl;
   return 0;
}