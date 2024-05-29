// SPDX-License-Identifier:  MIT OR LGPL-3.0-or-later
// Copyright (C) 2007-2022 - OptFrame developers
// https://github.com/optframe/optframe

// OptFrame Demo TSP - Iterated Local Search

// C++
#include <iostream>
//
#include "TSP-fcore.hpp"
// implementation of TSP

#include <OptFrame/Core.hpp>
#include <OptFrame/Heuristics/Heuristics.hpp>  // many metaheuristics here...
#include <OptFrame/Heuristics/ILS/IteratedLocalSearchLevels.hpp>
#include <OptFrame/Heuristics/LocalSearches/BestImprovement.hpp>
#include <OptFrame/Heuristics/LocalSearches/VariableNeighborhoodDescent.hpp>
#include <OptFrame/LocalSearch.hpp>

// import everything on main()
using namespace std;        // NOLINT
using namespace optframe;   // NOLINT
using namespace scannerpp;  // NOLINT
// using namespace TSP_fcore;
int main() {
  srand(0);  // using system random (weak... just an example!)

  // load data into problem context 'pTSP'
  Scanner scanner{"5\n1 10 10\n2 20 20\n3 30 30\n4 40 40\n5 50 50\n"};
  sref<ProblemContext> pTSP{new ProblemContext{}};
  pTSP->load(scanner);
  std::cout << pTSP->dist << std::endl;

  OptFrameDemoTSP demo{pTSP};

  // evaluator
  // TSPEval ev;
  //
  // create simple solution
  // TSPRandom crand;
  //
  std::vector<int> sol = *demo.randomConstructive->generateSolution(0);
  std::cout << sol << std::endl;

  // evaluation value and store on ESolution pair
  ESolutionTSP esol(sol, demo.eval->evaluate(sol));
  esol.second.print();  // print evaluation

  // swap 0 with 1
  MoveSwap move{demo.pTSP, make_pair(0, 1), fApplySwap};
  move.print();

  // NSSwap nsswap;
  // move for solution 'esol'
  auto m1 = demo.nsSwap->randomMove(esol);
  m1->print();

  std::cout << std::endl;
  std::cout << "begin listing NSSeqSwapFancy" << std::endl;
  //
  auto it1 = demo.nsseqSwap->getIterator(esol);
  for (it1->first(); !it1->isDone(); it1->next()) it1->current()->print();
  std::cout << "end listing NSSeqSwapFancy" << std::endl;

  // Random number generator
  RandGen rg;                      // stack version
  sref<RandGen> rg2{new RandGen};  // heap version (safely shared)
  // testing simulated annealing
  BasicInitialSearch<ESolutionTSP> initRand(demo.randomConstructive, demo.eval);

  vsref<LocalSearch<ESolutionTSP>> ns_list;
  Evaluator<ESolutionTSP::first_type, ESolutionTSP::second_type>* ev2 =
      new FEvaluator<ESolutionTSP, MinOrMax::MINIMIZE, ProblemContext>{
          pTSP, fevaluate};
  GeneralEvaluator<ESolutionTSP>* gev2 = (GeneralEvaluator<ESolutionTSP>*)ev2;
  sref<GeneralEvaluator<ESolutionTSP>> eval2(gev2);
  ns_list.push_back(new BestImprovement<ESolutionTSP>(eval2, demo.nsseqSwap));

  VariableNeighborhoodDescent<ESolutionTSP> VND(demo.eval, ns_list);
  // VND.setVerbose();//
ILSLPerturbationLPlus2<ESolutionTSP> pert(demo.eval, demo.nsSwap, rg2);

IteratedLocalSearchLevels<ESolutionTSP> ils(demo.eval, nnptr::copy(initRand),
                                            nnptr::copy(VND), nnptr::copy(pert),
                                            10, 5);
// ils.setVerbose();

std::cout << "will start ILS for 10 seconds" << std::endl;

optframe::Timer t;
ils.setVerbose();
auto status = ils.search(
    StopCriteria<ESolutionTSP::second_type>{10.0});  // 3.0 seconds max
ESolutionTSP best = *status.best;
std::cout << "spent time: " << t.now() << "s" << std::endl;
// best solution value
best.second.print();
std::cout << "solution: " << best.first << std::endl;

std::cout << "FINISHED" << std::endl;
return 0;
}