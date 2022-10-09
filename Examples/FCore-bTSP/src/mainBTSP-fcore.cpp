// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe
//
// Example: implementation of bi-objective TSP
//

// base
#include "BTSP-fcore.hpp"
// C++
#include <iostream>
//
#include <OptFrame/Helper/PopulationBasedMultiObjSearch.hpp>
// optframe extra
#include <OptFrame/Heuristics/EA/NSGAII.hpp>
#include <OptFrame/Heuristics/MultiObjective/ClassicNSGAII.hpp>

// import everything on main()
using namespace std;
using namespace optframe;
using namespace scannerpp;
using namespace BTSP_fcore;

class MyNSGAIIforBTSP : public NSGAII<ESolutionBTSP> {
  //
 public:
  using S = typename ESolutionBTSP::first_type;
  using XMEv = typename ESolutionBTSP::second_type;
  using XEv = typename XMEv::XEv;
  using XES = std::pair<S, XEv>;
  //
  MyNSGAIIforBTSP(vsref<Evaluator<S, XEv, XES>> _v_e,
                  sref<InitialMultiESolution<ESolutionBTSP>> _init_epop,
                  int _init_pop_size,
                  int _gMax,
                  sref<RandGen> _rg)
      : NSGAII(_v_e,
               _init_epop,
               _init_pop_size,
               _gMax,
               _rg) {
  }

  void basicGeneticOperators(VEPopulation<ESolutionBTSP>& p) override {
    // TODO: must implement this!!!!
    assert(false);
  }
};

/*

// base population 'p' and desired final population size 'q_size'
vector<IndividualNSGAII<RepCARP>*> MyDecoderNSGAII::basicGeneticOperators(const PS& p, int indMutation)
{
	PS q;

	//cout << "indMutation = " << indMutation << endl;

	for(int n = 1; n <= indMutation; n++)
	{
		int ind = rg.rand(p.size());

		Solution<RepCARP>& s_ind = p.at(ind)->s.clone();
		int neigh;
		Move<RepCARP>* move = NULL;
		if(neighbors.size() > 0)
		{
			neigh = rg.rand(neighbors.size());
			move = neighbors[neigh]->validMove(s_ind);
		}
		else
			neigh = -1;

		if(move)
		{

			delete &move->apply(s_ind);

			delete move;
		}
		else
		{
			cout << "WARNING: could not find a valid move for neighborhood " << neigh << " ";
			neighbors[neigh]->print();
			s_ind.print();
		}

		// do local search only in rank '0' individuals

		

IndividualNSGAII<RepCARP>* indiv = new IndividualNSGAII<RepCARP>(s_ind);
indiv->isMutation = true;
q.push_back(indiv);

//localSearch.exec(q.at(ind), 100000, 0);
}

int indMissing = indMutation - q.size();
if (indMissing > 0) {
  cout << "MISSING " << indMissing << endl;
  cout << "|P|=" << p.size() << " indMutation=" << indMutation << " |q|=" << q.size() << " |NS|=" << neighbors.size() << endl;
  exit(1);
}

for (int n = 1; n <= indMissing; n++) {
  cout << "Warning: n=" << n << "/" << indMissing << " MISSING MUTATION => CLONING ELEMENT!" << endl;
  int ind = rg.rand(p.size());
  Solution<RepCARP>& s_ind = p.at(ind)->s.clone();
  IndividualNSGAII<RepCARP>* indiv = new IndividualNSGAII<RepCARP>(s_ind);
  q.push_back(indiv);
}

return q;
}
*/

class CrossTSPRandomPoint : public GeneralCrossover<
                                typename ESolutionBTSP::first_type> {
  using S = typename ESolutionBTSP::first_type;
  sref<RandGen> rg;

 public:
  explicit CrossTSPRandomPoint(sref<RandGen> _rg) : rg{_rg} {}

  virtual ~CrossTSPRandomPoint() {}

  virtual pair<std::optional<S>, std::optional<S>>
  cross(const S& p1, const S& p2) {
    op<S> s1 = p1;
    op<S> s2 = p2;

    int k = (rg->rand() % (p1.size() - 1)) + 1;

    for (int i = 0; i < (int)k; i++) {
      (*s1)[i] = p1[i];
      (*s2)[i] = p2[i];
    }
    for (int j = k; j < (int)p1.size(); j++) {
      (*s1)[j] = p2[j];
      (*s2)[j] = p1[j];
    }
    return {s1, s2};
  }
};

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
  // RandGen rg;                      // stack version
  sref<RandGen> rg2{new RandGen};  // heap version (safely shared)
  // rg2->setSeed(0);
  //
  // testing simulated annealing
  BasicInitialSearch<ESolutionBTSP> initRand(crand, ev);

  sref<InitialMultiESolution<ESolutionBTSP>> init_epop{
      new BasicInitialMultiESolution<ESolutionBTSP>{crand, ev}};

  vsref<GeneralCrossover<typename ESolutionBTSP::first_type>> crossovers;
  crossovers.push_back(new CrossTSPRandomPoint{rg2});
  //
  sref<MOPopulationManagement<ESolutionBTSP>>
      popMan{
          new BasicPopulationManagement<ESolutionBTSP>(
              init_epop,
              nslist,  //vsref<NS<XMES2>> _mutations,
              0.5,     //double _mutationRate,
              crossovers,
              0.1,  //double _renewRate,
              rg2)};

  sref<MultiDirection<typename ESolutionBTSP::second_type::XEv>> mDir = {
      new MultiDirection<typename ESolutionBTSP::second_type::XEv>{
          ev->vDir}};

  ev->vDir[0]->setLimits(0, 100000);
  ev->vDir[1]->setLimits(0, 100000);

  ClassicNSGAII<ESolutionBTSP> classic_nsgaii{ev, mDir, popMan, 30, 100};

  // classic_nsgaii.setVerboseR();

  sref<NSGAII<ESolutionBTSP>> nsgaii{
      new MyNSGAIIforBTSP(ev_list,
                          init_epop,
                          30,
                          100,
                          rg2)};

  // testing 'classic_nsgaii' for now

  auto sout = classic_nsgaii.search(
      StopCriteria<ESolutionBTSP::second_type>{10.0});  // 10.0 seconds max
  std::cout << "finished classic_nsgaii with: ";
  std::cout << "status=" << sout.status << std::endl;
  Pareto<ESolutionBTSP> best = *sout.best;
  std::cout << "best pareto: ";
  // best pareto front
  best.print();

  std::cout << "FINISHED" << std::endl;
  return 0;
}