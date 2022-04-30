

#include <iostream>
//
#include "TSP-fcore.hpp"
// implementation of TSP

#include <OptFrame/Core.hpp>
#include <OptFrame/Heuristics/Heuristics.hpp> // many metaheuristics here...
#include <OptFrame/Heuristics/ILS/IteratedLocalSearchLevels.hpp>
#include <OptFrame/Heuristics/LocalSearches/BestImprovement.hpp>
#include <OptFrame/Heuristics/LocalSearches/VariableNeighborhoodDescent.hpp>
#include <OptFrame/LocalSearch.hpp>

// import everything on main()
using namespace std;
using namespace optframe;
using namespace scannerpp;
//using namespace TSP_fcore;
int
main()
{
   srand(0); // using system random (weak... just an example!)

   // load data into problem context 'pTSP'
   Scanner scanner{ "5\n1 10 10\n2 20 20\n3 30 30\n4 40 40\n5 50 50\n" };
   pTSP.load(scanner);
   std::cout << pTSP.dist << std::endl;

   // evaluator
   // TSPEval ev;
   //
   // create simple solution
   // TSPRandom crand;
   //
   std::vector<int> sol = *crand.generateSolution(0);
   std::cout << sol << std::endl;

   // evaluation value and store on ESolution pair
   ESolutionTSP esol(sol, eval.evaluate(sol));
   esol.second.print(); // print evaluation

   // swap 0 with 1
   MoveSwap move{ make_pair(0, 1), fApplySwap };
   move.print();

   // NSSwap nsswap;
   // move for solution 'esol'
   auto m1 = nsswap.randomMove(esol);
   m1->print();

   std::cout << std::endl;
   std::cout << "begin listing NSSeqSwapFancy" << std::endl;
   //
   auto it1 = nsseq2->getIterator(esol);
   for (it1->first(); !it1->isDone(); it1->next())
      it1->current()->print();
   std::cout << "end listing NSSeqSwapFancy" << std::endl;

   // Random number generator
   RandGen rg;                       // stack version
   sref<RandGen> rg2{ new RandGen }; // heap version (safely shared)
   // testing simulated annealing
   BasicInitialSearch<ESolutionTSP> initRand(crand, eval);

   vsref<LocalSearch<ESolutionTSP>> ns_list;
   Evaluator<ESolutionTSP::first_type, ESolutionTSP::second_type>* ev2 = new FEvaluator<ESolutionTSP, MinOrMax::MINIMIZE>{ fevaluate };
   GeneralEvaluator<ESolutionTSP>* gev2 = (GeneralEvaluator<ESolutionTSP>*)ev2;
   sref<GeneralEvaluator<ESolutionTSP>> eval2(gev2);
   ns_list.push_back(new BestImprovement<ESolutionTSP>(eval2, nsseq2));

   VariableNeighborhoodDescent<ESolutionTSP> VND(eval, ns_list);
   // VND.setVerbose();//
ILSLPerturbationLPlus2<ESolutionTSP> pert(eval, nsswap, rg2);

IteratedLocalSearchLevels<ESolutionTSP> ils(eval, initRand, VND, pert, 10, 5);
// ils.setVerbose();

std::cout << "will start ILS for 3 seconds" << std::endl;

auto status = ils.search(
  StopCriteria<ESolutionTSP::second_type>{ 3.0 }); // 3.0 seconds max
ESolutionTSP best = *status.best;
// best solution value
best.second.print();
std::cout << "solution: " << best.first << std::endl;

std::cout << "FINISHED" << std::endl;
return 0;
}