

#include <iostream>
//
#include "BTSP-fcore.hpp"
// implementation of bi-objective TSP

// import everything on main()
using namespace std;
using namespace optframe;
using namespace scannerpp;
using namespace BTSP_fcore;

int main() {
  srand(0);  // using system random (weak... just an example!)

  std::string str_inst =
      "5\n"
      "0 100 100\n1 100 200\n2 2000 2000\n3 2000 100\n4 150 150\n"
      "0 10  20 \n1 30  40 \n2 10   40  \n3 20   30 \n4 40  20\n";
  // load data into problem context 'pBTSP'
  Scanner scanner{str_inst};
  pBTSP.load(scanner);
  std::cout << "dist1:" << pBTSP.dist1 << std::endl;
  std::cout << "dist2:" << pBTSP.dist2 << std::endl;

  // evaluator
  // TSPEval ev;
  //
  // create simple solution
  // TSPRandom crand;
  //
  std::vector<int> sol = *crand.generateSolution(0);
  std::cout << sol << std::endl;

  // evaluation value and store on ESolution pair
  ESolutionBTSP esol(sol, ev->evaluate(sol));
  esol.second.print();  // print evaluation

  // swap 0 with 1
  //MoveSwap move{ make_pair(0, 1), fApplySwap };

  MoveSwap move(0, 1);

  move.print();

  // NSSwap nsswap;
  // move for solution 'esol'
  auto m1 = nsswap.randomMove(esol);
  m1->print();

  std::cout << std::endl;
  std::cout << "begin listing NSSeqSwapFancy" << std::endl;
  //
  auto it1 = nsseq.getIterator(esol);
  for (it1->first(); !it1->isDone(); it1->next())
    it1->current()->print();
  std::cout << "end listing NSSeqSwapFancy" << std::endl;

  // Random number generator
  RandGen rg;                      // stack version
  sref<RandGen> rg2{new RandGen};  // heap version (safely shared)
  // testing simulated annealing
  BasicInitialSearch<ESolutionBTSP> initRand(crand, ev);

  /*
  BasicSimulatedAnnealing<ESolutionBTSP> sa(
      //_evaluator, _constructive, _neighbors, _alpha, _SAmax, _Ti, _rg
      ev,
      initRand,
      nsseq,
      0.98,
      100,
      99999,
      rg2);

  auto status = sa.search(
      StopCriteria<ESolutionBTSP::second_type>{10.0});  // 10.0 seconds max
  ESolutionBTSP best = *status.best;                    //*sa.getBestSolution();
  // best solution value
  best.second.print();
*/

  std::cout << "FINISHED" << std::endl;
  return 0;
}