// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef EXAMPLES_HFMVRP_INCLUDE_HFMVRP_NEIGHBORHOODS_INTRA_DELTAITERATORVRPOROPT2_HPP_  // NOLINT
#define EXAMPLES_HFMVRP_INCLUDE_HFMVRP_NEIGHBORHOODS_INTRA_DELTAITERATORVRPOROPT2_HPP_

// C++
#include <cmath>
#include <utility>
#include <vector>
// hfmvrp
#include "DeltaMoveVRPOrOpt2.hpp"

using namespace std;

namespace HFMVRP {

template <class MOVE = DeltaMoveVRPOrOpt2>
class DeltaNSIteratorVRPOrOpt2
    : public NSIteratorVRPOrOpt2<ESolutionHFMVRP, ProblemInstance,
                                 DeltaMoveVRPOrOpt2> {
  using super =
      NSIteratorVRPOrOpt2<ESolutionHFMVRP, ProblemInstance, DeltaMoveVRPOrOpt2>;

 private:
  const AdsHFMVRP& ads;  // TODO COULD BE A POINTER? WHAT IS THE BEST OPTION?

  using Routes = std::vector<vector<int>>;

 public:
  DeltaNSIteratorVRPOrOpt2(
      std::function<Routes&(const ESolutionHFMVRP&)> _getRoutes,
      const ESolutionHFMVRP& se, ProblemInstance* _hfmvrp)
      : super(_getRoutes, se, _hfmvrp), ads(se.first.getADS()) {
    if (!_hfmvrp) {
      cout << "Error: hfmvrp problem is NULL!" << endl;
      exit(1);
    }
  }

  virtual ~DeltaNSIteratorVRPOrOpt2() {}

  void first() override {
    for (int r = 0; r < (int)rep.size(); r++)
      if (!(ads.neighborhoodStatus.find(DeltaMoveVRPOrOpt2::idComponent())
                ->second[r])) {
        if (rep.at(r).size() > 1)
          for (int c = 0; c < (int)rep.at(r).size() - 1; c++) {
            for (int pos = 0; pos <= (int)rep.at(r).size(); pos++) {
              if ((c != pos) && (c + 1 != pos) && (c + 2 != pos)) {
                moves.push_back(uptr<Move<ESolutionHFMVRP>>(
                    new MOVE(getRoutes, r, c, pos, p)));
              }
            }
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
      m = std::move(moves[index]);  // stealing from vector...
    else
      m = NULL;
  }

  bool isDone() override { return m == NULL; }

  uptr<Move<ESolutionHFMVRP>> current() override {
    if (isDone()) {
      cout << "There isnt any current element!" << endl;
      cout << "DeltaNSSeqVRPOrOpt2. Aborting." << endl;
      exit(1);
    }

    uptr<Move<ESolutionHFMVRP>> m2 = std::move(m);
    m = nullptr;

    return m2;
  }
};

}  // namespace HFMVRP

#endif  // EXAMPLES_HFMVRP_INCLUDE_HFMVRP_NEIGHBORHOODS_INTRA_DELTAITERATORVRPOROPT2_HPP_
