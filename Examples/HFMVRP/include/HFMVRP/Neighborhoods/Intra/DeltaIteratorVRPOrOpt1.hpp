// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef EXAMPLES_HFMVRP_INCLUDE_HFMVRP_NEIGHBORHOODS_INTRA_DELTAITERATORVRPOROPT1_HPP_
#define EXAMPLES_HFMVRP_INCLUDE_HFMVRP_NEIGHBORHOODS_INTRA_DELTAITERATORVRPOROPT1_HPP_

// C++
#include <cmath>
// hfmvrp
#include "DeltaMoveVRPOrOpt1.hpp"

using namespace std;

namespace HFMVRP {

template <class MOVE = DeltaMoveVRPOrOpt1>
class DeltaNSIteratorVRPOrOpt1
    : public NSIteratorVRPOrOpt1<
          ESolutionHFMVRP, ProblemInstance,
          DeltaMoveVRPOrOpt1>  //<int, AdsHFMVRP, SolutionHFMVRP,
                               // DeltaMoveVRPOrOpt1, ProblemInstance>
{
  typedef NSIteratorVRPOrOpt1<ESolutionHFMVRP, ProblemInstance,
                              DeltaMoveVRPOrOpt1>
      super;  //<int, AdsHFMVRP, SolutionHFMVRP, DeltaMoveVRPOrOpt1,
              // ProblemInstance> super;

 private:
  const AdsHFMVRP& ads;  // TODO COULD BE A POINTER? WHAT IS THE BEST OPTION?

 public:
  DeltaNSIteratorVRPOrOpt1(
      vector<vector<int>>& (*_getRoutes)(const ESolutionHFMVRP& s),
      const ESolutionHFMVRP& se, ProblemInstance* _hfmvrp)
      : super(_getRoutes, se, _hfmvrp), ads(se.first.getADS()) {
    if (!_hfmvrp) {
      cout << "Error: hfmvrp problem is NULL!" << endl;
      exit(1);
    }
  }

  virtual ~DeltaNSIteratorVRPOrOpt1() {}

  void first() override {
    for (int r = 0; r < (int)rep.size(); r++)
      if (!(ads.neighborhoodStatus.find(DeltaMoveVRPOrOpt1::idComponent())
                ->second[r])) {
        for (int c = 0; c < (int)rep.at(r).size(); c++) {
          for (int pos = 0; pos <= (int)rep.at(r).size(); pos++) {
            if ((c != pos) && (c + 1 != pos)) {
              moves.push_back(uptr<Move<ESolutionHFMVRP>>(
                  new MOVE(getRoutes, r, c, pos, p)));
            }
          }
        }
      }

    if (moves.size() > 0)
      m = std::move(moves[index]);  // stealing from vector..
    else
      m = NULL;
  }

  void next() override {
    index++;
    if (index < (int)moves.size())
      m = std::move(moves[index]);  // stealing from vector...
    else
      m = NULL;
  }

  bool isDone() override { return m == NULL; }

  uptr<Move<ESolutionHFMVRP>> current() override {
    if (isDone()) {
      cout << "There isnt any current element!" << endl;
      cout << "DeltaNSSeqVRPOrOpt1. Aborting." << endl;
      exit(1);
    }

    uptr<Move<ESolutionHFMVRP>> m2 = std::move(m);
    m = nullptr;

    return m2;
  }
};

}  // namespace HFMVRP

#endif  // EXAMPLES_HFMVRP_INCLUDE_HFMVRP_NEIGHBORHOODS_INTRA_DELTAITERATORVRPOROPT1_HPP_
