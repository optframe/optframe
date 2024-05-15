// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef EXAMPLES_HFMVRP_INCLUDE_HFMVRP_NEIGHBORHOODS_INTER_DELTAITERATORVRPSWAP1_1_HPP_
#define EXAMPLES_HFMVRP_INCLUDE_HFMVRP_NEIGHBORHOODS_INTER_DELTAITERATORVRPSWAP1_1_HPP_

// C++
#include <cmath>
#include <string>
#include <utility>
#include <vector>
// framework
// #include <OptFrame/Util/NSAdapter/VRP/Inter/NSSeqVRPSwap1_1.hpp>
// hfmvrp
#include "./DeltaMoveVRPSwap1_1.hpp"

using namespace std;

namespace HFMVRP {

/*
template <class MOVE = DeltaMoveVRPSwap1_1>
class DeltaNSIteratorVRPSwap1_1
    : public NSIteratorVRPSwap1_1<int, AdsHFMVRP, SolutionHFMVRP,
                                  DeltaMoveVRPSwap1_1, ProblemInstance> {
  typedef NSIteratorVRPSwap1_1<int, AdsHFMVRP, SolutionHFMVRP,
                               DeltaMoveVRPSwap1_1, ProblemInstance>
      super;
      */
class DeltaIteratorVRPSwap1_1 : public NSIterator<ESolutionHFMVRP> {
 private:
  const AdsHFMVRP& ads;  // TODO COULD BE A POINTER? WHAT IS THE BEST OPTION?
  const RepHFMVRP& r;
  ProblemInstance* p;

  uptr<Move<ESolutionHFMVRP>> m;
  vector<uptr<Move<ESolutionHFMVRP>>> moves;
  int index;  // index of moves

 public:
  using MoveType = DeltaMoveVRPSwap1_1;
  DeltaIteratorVRPSwap1_1(const ESolutionHFMVRP& se, ProblemInstance* _hfmvrp)
      : r{se.first.getR()}, ads{se.first.getADS()}, p{_hfmvrp} {
    if (!_hfmvrp) {
      cout << "Error: hfmvrp problem is NULL!" << endl;
      exit(1);
    }
  }

  virtual ~DeltaIteratorVRPSwap1_1() {}

  void first() override {
    for (int r1 = 0; r1 < (int)r.size() - 1; r1++)
      for (int r2 = r1 + 1; r2 < (int)r.size(); r2++)
        if (!(ads.neighborhoodStatus.find(idComponent())->second[r1] &&
              ads.neighborhoodStatus.find(idComponent())->second[r2])) {
          bool teste1 =
              (ads.minDemand[r1] - ads.maxDemand[r2] + ads.sumDemand[r2] <=
               p->vehiclesCap[r2]);
          bool teste2 =
              (ads.minDemand[r2] - ads.maxDemand[r1] + ads.sumDemand[r1] <=
               p->vehiclesCap[r1]);

          /*
           if (teste1 != teste2)
           {
           cout << "Bug SWAP 11!!!!" << endl;
           cout << r[r1] << endl;
           cout << r[r2] << endl;
           cout << "ads.minDemand[r1] = " << ads.minDemand[r1] << endl;
           cout << "ads.maxDemand[r1] = " << ads.maxDemand[r1] << endl;
           cout << "ads.sumDemand[r1] = " << ads.sumDemand[r1] << endl;
           cout << "ads.minDemand[r2] = " << ads.minDemand[r2] << endl;
           cout << "ads.maxDemand[r2] = " << ads.maxDemand[r2] << endl;
           cout << "ads.sumDemand[r2] = " << ads.sumDemand[r2] << endl;
           cout << "ads.cumulativeDemand" << ads.cumulativeDemand << endl;
           cout << "ads.maxDemand" << ads.maxDemand << endl;
           cout << "ads.minDemand" << ads.minDemand << endl;
           cout << "ads.maxPairDemand" << ads.maxPairDemand << endl;
           cout << "ads.minPairDemand" << ads.minPairDemand << endl;
           cout << "ads.sumDemand" << ads.sumDemand << endl;

           getchar();
           }
           */
          if (ads.minDemand[r1] - ads.maxDemand[r2] + ads.sumDemand[r2] <=
              p->vehiclesCap[r2]) {
            // TODO ERRO - Falta verificar a insercao do
            // cliente r2 na rota r1
            for (int c1 = 1; c1 < (int)(r.at(r1).size() - 1); c1++)
              if (p->demands[r.at(r1).at(c1)] - ads.maxDemand[r2] +
                      ads.sumDemand[r2] <=
                  p->vehiclesCap[r2]) {
                for (int c2 = 1; c2 < (int)(r.at(r2).size() - 1); c2++)
                  moves.push_back(uptr<Move<ESolutionHFMVRP>>(
                      new DeltaMoveVRPSwap1_1(r1, r2, c1, c2, p)));
              }
          }
        }

    if (moves.size() > 0) {
      m = std::move(moves[index]);
    } else {
      m = NULL;
    }
  }

  void next() override {
    index++;
    if (index < (int)moves.size())
      m = std::move(moves[index]);
    else
      m = NULL;
  }

  bool isDone() override { return m == NULL; }

  uptr<Move<ESolutionHFMVRP>> current() override {
    if (isDone()) {
      cout << "There isnt any current element!" << endl;
      cout << "DeltaNSSeqVRPSwap1_1. Aborting." << endl;
      exit(1);
    }

    uptr<Move<ESolutionHFMVRP>> m2 = std::move(m);
    m = nullptr;

    return m2;
  }
};

}  // namespace HFMVRP

#endif  // EXAMPLES_HFMVRP_INCLUDE_HFMVRP_NEIGHBORHOODS_INTER_DELTAITERATORVRPSWAP1_1_HPP_
