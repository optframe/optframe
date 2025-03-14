// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef EXAMPLES_HFMVRP_INCLUDE_HFMVRP_NEIGHBORHOODS_INTER_DELTAITERATORVRPSHIFT10_HPP_
#define EXAMPLES_HFMVRP_INCLUDE_HFMVRP_NEIGHBORHOODS_INTER_DELTAITERATORVRPSHIFT10_HPP_

// C++
#include <cmath>
#include <string>
#include <utility>
#include <vector>

// optframe
#include "OptFrame/Util/NSAdapter/VRP/Inter/NSSeqVRPShift10.hpp"

// hfmvrp
#include "./DeltaMoveVRPShift10.hpp"

using namespace std;

namespace HFMVRP {

/*
template <class MOVE = DeltaMoveVRPShift10>
class DeltaNSIteratorVRPShift10
    : public NSIteratorVRPShift10<int, AdsHFMVRP, SolutionHFMVRP,
                                  DeltaMoveVRPShift10, ProblemInstance> {
  typedef NSIteratorVRPShift10<int, AdsHFMVRP, SolutionHFMVRP,
                               DeltaMoveVRPShift10, ProblemInstance>
      super;
      */

// personalized move
class DeltaIteratorVRPShift10 : public NSIterator<ESolutionHFMVRP> {
 private:
  const AdsHFMVRP& ads;  // TODO COULD BE A POINTER? WHAT IS THE BEST OPTION?
  const RepHFMVRP& r;
  ProblemInstance* p;

  uptr<Move<ESolutionHFMVRP>> m;
  vector<uptr<Move<ESolutionHFMVRP>>> moves;
  int index;  // index of moves

 public:
  using MoveType = DeltaMoveVRPShift10;
  /*
   DeltaNSIteratorVRPShift10(const RepHFMVRP& _rep, const AdsHFMVRP& _ads,
                             ProblemInstance* _hfmvrp)
       : super(_rep, _ads, _hfmvrp), ads(_ads) {
         */
  DeltaIteratorVRPShift10(const ESolutionHFMVRP& se, ProblemInstance* _hfmvrp)
      : r{se.first.getR()}, ads{se.first.getADS()}, p{_hfmvrp} {
    if (!_hfmvrp) {
      std::cout << "Error: hfmvrp problem is NULL!" << std::endl;
      exit(1);
    }
  }

  // virtual ~DeltaNSIteratorVRPShift10() {}

  void first() override {
    for (int r1 = 0; r1 < (int)r.size(); r1++)
      for (int r2 = 0; r2 < (int)r.size(); r2++)
        if (!(ads.neighborhoodStatus.find(idComponent())->second[r1] &&
              ads.neighborhoodStatus.find(idComponent())->second[r2]))
          if ((ads.minDemand[r1] + ads.sumDemand[r2] <= p->vehiclesCap[r2]) &&
              (r1 != r2)) {
            for (int cli = 1; cli < (int)(r.at(r1).size() - 1); cli++)
              if (p->demands[r.at(r1).at(cli)] + ads.sumDemand[r2] <=
                  p->vehiclesCap[r2]) {
                for (int pos = 1; pos < (int)(r.at(r2).size() - 1); pos++)
                  moves.push_back(uptr<Move<ESolutionHFMVRP>>(
                      new DeltaMoveVRPShift10(r1, r2, cli, pos, p)));
              }
          }

    if (moves.size() > 0)
      m = std::move(moves[index]);
    else
      m = NULL;
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
      std::cout << "There isnt any current element!" << std::endl;
      std::cout << "DeltaNSIteratorVRPShift10. Aborting." << std::endl;
      exit(1);
    }

    uptr<Move<ESolutionHFMVRP>> m2(std::move(m));
    m = nullptr;

    return m2;
  }
};

}  // namespace HFMVRP

#endif  // EXAMPLES_HFMVRP_INCLUDE_HFMVRP_NEIGHBORHOODS_INTER_DELTAITERATORVRPSHIFT10_HPP_
