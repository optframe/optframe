// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef EXAMPLES_HFMVRP_INCLUDE_HFMVRP_NEIGHBORHOODS_INTRA_DELTAITERATORVRPEXCHANGE_HPP_
#define EXAMPLES_HFMVRP_INCLUDE_HFMVRP_NEIGHBORHOODS_INTRA_DELTAITERATORVRPEXCHANGE_HPP_

// C++
#include <cmath>
// hfmvrp
#include "DeltaMoveVRPExchange.hpp"

using namespace std;

namespace HFMVRP {

template <class MOVE = DeltaMoveVRPExchange>
class DeltaNSIteratorVRPExchange
    : public NSIteratorVRPExchange<
          ESolutionHFMVRP, ProblemInstance,
          DeltaMoveVRPExchange>  //<int, AdsHFMVRP, SolutionHFMVRP,
                                 // DeltaMoveVRPExchange, ProblemInstance>
{
  typedef NSIteratorVRPExchange<ESolutionHFMVRP, ProblemInstance,
                                DeltaMoveVRPExchange>
      super;  //<int, AdsHFMVRP, SolutionHFMVRP, DeltaMoveVRPExchange,
              // ProblemInstance> super;

 private:
  const AdsHFMVRP& ads;  // TODO COULD BE A POINTER? WHAT IS THE BEST OPTION?

  using Routes = std::vector<vector<int>>;

 public:
  DeltaNSIteratorVRPExchange(
      std::function<Routes&(const ESolutionHFMVRP&)> _getRoutes,
      const ESolutionHFMVRP& se, ProblemInstance* _hfmvrp)
      : super(_getRoutes, se, _hfmvrp), ads(se.first.getADS()) {
    if (!_hfmvrp) {
      std::cout << "Error: hfmvrp problem is NULL!" << std::endl;
      exit(1);
    }
  }

  virtual ~DeltaNSIteratorVRPExchange() {}

  void first() override {
    for (int r = 0; r < (int)rep.size(); r++)
      if (!(ads.neighborhoodStatus.find(DeltaMoveVRPExchange::idComponent())
                ->second[r])) {
        for (int c1 = 0; c1 < (int)rep.at(r).size(); c1++) {
          for (int c2 = 0; c2 < (int)rep.at(r).size(); c2++) {
            if (c1 != c2)
              moves.push_back(uptr<Move<ESolutionHFMVRP>>(
                  new MOVE(getRoutes, r, c1, c2, p)));
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
      m = std::move(moves[index]);
    else
      m = NULL;
  }

  bool isDone() override { return m == NULL; }

  uptr<Move<ESolutionHFMVRP>> current() override {
    if (isDone()) {
      std::cout << "There isnt any current element!" << std::endl;
      std::cout << "DeltaNSSeqVRPExchange. Aborting." << std::endl;
      exit(1);
    }

    uptr<Move<ESolutionHFMVRP>> m2 = std::move(m);
    m = nullptr;

    return m2;
  }
};

}  // namespace HFMVRP

#endif  // EXAMPLES_HFMVRP_INCLUDE_HFMVRP_NEIGHBORHOODS_INTRA_DELTAITERATORVRPEXCHANGE_HPP_
