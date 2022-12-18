// SPDX-License-Identifier:  MIT OR LGPL-3.0-or-later
// Copyright (C) 2007-2022 - OptFrame developers
// https://github.com/optframe/optframe

// OptFrame Demo TSP - BRKGA

// C++
#include <iostream>
//
#include "TSP-fcore.hpp"
// implementation of TSP
//
#include <OptFrame/Core.hpp>
#include <OptFrame/Heuristics/EA/RK/BRKGA.hpp>
#include <OptFrame/Heuristics/Heuristics.hpp>  // many metaheuristics here...
#include <OptFrame/InitialPopulation.hpp>
#include <OptFrame/LocalSearch.hpp>

// import everything on main()
using namespace std;        // NOLINT
using namespace optframe;   // NOLINT
using namespace scannerpp;  // NOLINT
// using namespace TSP_fcore;

class MyRandomKeysInitEPop
    : public InitialEPopulation<
          std::pair<std::vector<double>, Evaluation<int>>> {
  using RSK = std::vector<double>;

 private:
  int sz;
  sref<RandGen> rg;

 public:
  explicit MyRandomKeysInitEPop(int size, sref<RandGen> _rg = new RandGen)
      : sz{size}, rg{_rg} {}

  // copy constructor
  // MyRandomKeysInitEPop(const MyRandomKeysInitEPop& self)
  //     : sz{self.sz}, rg{self.rg} {}

  // this generator cannot evaluate solutions
  bool canEvaluate() const override { return false; }

  VEPopulation<std::pair<RSK, Evaluation<int>>> generateEPopulation(
      unsigned populationSize, double timelimit) override {
    VEPopulation<std::pair<RSK, Evaluation<int>>> pop;

    for (unsigned i = 0; i < populationSize; i++) {
      vector<double> vd(sz);
      for (int j = 0; j < sz; j++) vd[j] = (rg->rand() % 100000) / 100000.0;
      // assert(!this->canEvaluate());
      std::pair<RSK, Evaluation<int>> ind{vd, Evaluation<int>{}};
      pop.push_back(ind);
    }

    return pop;
  }
};


pair<Evaluation<int>, vector<int>> fDecodeEval(
    sref<Evaluator<typename ESolutionTSP::first_type,
                   typename ESolutionTSP::second_type, ESolutionTSP>>
        eval,
    const vector<double>& rk) {
  vector<pair<double, int>> v(rk.size());
  //
  for (unsigned i = 0; i < v.size(); i++) v[i] = pair<double, int>(rk[i], i);

  sort(v.begin(), v.end(),
       [](const pair<double, int>& i, const pair<double, int>& j) -> bool {
         return i.first < j.first;
       });

  // R = vector<int>
  vector<int> p(v.size());
  for (unsigned i = 0; i < v.size(); i++) p[i] = v[i].second;

  /*
  // ========== CHECKER ========
  vector<bool> vb(v.size(), false);
  for (unsigned i = 0; i < p.size(); i++)
     vb[p[i]] = true;
  for (unsigned i = 0; i < vb.size(); i++) {
     if (!vb[i]) {
        std::cout << "ERROR rk:" << rk << std::endl;
        std::cout << "ERROR v:" << v << std::endl;
        std::cout << "ERROR p:" << p << std::endl;
        std::cout << "ERROR vb:" << vb << std::endl;
     }
     assert(vb[i]);
  }
  // ===== end CHECKER =====
*/

  Evaluation<int> e = eval->evaluate(p);
  return make_pair(e, p);
}

// evaluator random keys (for TSP)
// FDecoderEvalRK<std::pair<std::vector<int>, Evaluation<int>>, double>
// decoder{fDecode};
int main() {
  sref<RandGen> rg = new RandGen;  // avoids weird windows OS interactions

  // load data into problem context 'pTSP'
  Scanner scanner{"5\n1 10 10\n2 20 20\n3 30 30\n4 40 40\n5 50 50\n"};
  sref<ProblemContext> pTSP{new ProblemContext{}};
  pTSP->load(scanner);
  std::cout << pTSP->dist << std::endl;

  OptFrameDemoTSP demo{pTSP};
  // setup decoder function
  demo.decoder = sptr<DecoderRandomKeys<ESolutionTSP, double>>{
      new FDecoderEvalRK<std::pair<std::vector<int>, Evaluation<int>>, double>{
          demo.eval, fDecodeEval}};

  // Parameters BRKGA
  // (C1): Evaluator<S, XEv>& _evaluator, int key_size, unsigned numGen,
  // unsigned _popSize, double fracTOP, double fracBOT, double _probElitism) :

  sref<DecoderRandomKeys<ESolutionTSP, double>> _decoder = demo.decoder;
  sref<InitialEPopulation<std::pair<vector<double>, ESolutionTSP::second_type>>>
      _initPop = new MyRandomKeysInitEPop(pTSP->n);  // passing key_size

  // eprk, pTSP.n, 1000, 30, 0.4, 0.3, 0.6
  BRKGA<ESolutionTSP, double> brkga(
      _decoder, MyRandomKeysInitEPop(pTSP->n, rg),  // key_size = pTSP.n
      30, 1000, 0.4, 0.3, 0.6, rg);

  auto searchOut = brkga.search(3.0);  // 3.0 seconds max
  ESolutionTSP best = *searchOut.best;
  // best solution value
  best.second.print();
  std::cout << "solution: " << best.first << std::endl;

  std::cout << "FINISHED" << std::endl;
  return 0;
}
