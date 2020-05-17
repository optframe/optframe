
#include<iostream>
#include "TSP-fcore.hpp" // implementation of TSP

// import everything on main()
using namespace std;
using namespace optframe;
using namespace scannerpp;
using namespace TSP_fcore; 

int
main()
{
   srand(0); // using system random (weak... just an example!)

   // load data into problem context 'pTSP'
   Scanner scanner{ "3\n1 10 10\n2 20 20\n3 30 30\n" };
   pTSP.load(scanner);
   std::cout << pTSP.dist << std::endl;

   // evaluator
   TSPEval ev;

   // create simple solution
   TSPRandom crand;
   std::vector<int> sol = *crand.generateSolution(0);
   std::cout << sol << std::endl;

   // evaluation value and store on ESolution pair
   ESolutionTSP esol(sol, ev.evaluate(sol));
   esol.second.print(); // print evaluation

   // swap 0 with 1
   MoveSwap move{ make_pair(0, 1) };
   move.print();

   NSSwap nsswap;
   // move for solution 'esol'
   auto m1 = nsswap.randomMove(esol);
   m1->print();

   std::cout << std::endl;
   std::cout << "begin listing NSSeqSwapFancy" << std::endl;
   NSSeqSwapFancy swapFancy;
   auto it1 = swapFancy.getIterator(esol);
   for (it1->first(); !it1->isDone(); it1->next())
      it1->current()->print();
   std::cout << "end listing NSSeqSwapFancy" << std::endl;

   // Random number generator
   RandGen rg;
   // testing simulated annealing
   BasicInitialSearch<ESolutionTSP> initRand(crand, ev);
   BasicSimulatedAnnealing<ESolutionTSP> sa{
      ev, initRand, swapFancy, 0.98, 100, 99999, rg
   };
   SearchStatus status = sa.search(StopCriteria<ESolutionTSP::second_type>{ 10.0 }); // 10.0 seconds max
   ESolutionTSP best = *sa.getBestSolution();
   // best solution value
   best.second.print();

   std::cout << "FINISHED" << std::endl;
   return 0;
}