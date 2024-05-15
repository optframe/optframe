// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef EXAMPLES_HFMVRP_INCLUDE_HFMVRP_NEIGHBORHOODS_INTRA_DELTAITERATORVRP2OPT_HPP_
#define EXAMPLES_HFMVRP_INCLUDE_HFMVRP_NEIGHBORHOODS_INTRA_DELTAITERATORVRP2OPT_HPP_

// C++
#include <cmath>
// hfmvrp
#include "DeltaMoveVRP2Opt.hpp"

using namespace std;

namespace HFMVRP {

template <class MOVE = DeltaMoveVRP2Opt>
// class DeltaNSIteratorVRP2Opt: public NSIteratorVRP2Opt<int, AdsHFMVRP,
// SolutionHFMVRP, DeltaMoveVRP2Opt, ProblemInstance>
class DeltaNSIteratorVRP2Opt
    : public NSIteratorVRP2Opt<ESolutionHFMVRP, ProblemInstance,
                               DeltaMoveVRP2Opt> {
  // typedef NSIteratorVRP2Opt<int, AdsHFMVRP, SolutionHFMVRP, DeltaMoveVRP2Opt,
  // ProblemInstance> super;
  typedef NSIteratorVRP2Opt<ESolutionHFMVRP, ProblemInstance, DeltaMoveVRP2Opt>
      super;

 private:
  const AdsHFMVRP& ads;  // TODO COULD BE A POINTER? WHAT IS THE BEST OPTION?

  using Routes = std::vector<vector<int>>;

 public:
  // const vector<vector<int>>& (*getRoutesX)(const ESolutionHFMVRP& s);

  // DeltaNSIteratorVRP2Opt(const RepHFMVRP& _rep, const AdsHFMVRP& _ads,
  // ProblemInstance* _hfmvrp) : 	super(_rep, _ads, _hfmvrp), ads(_ads)
  DeltaNSIteratorVRP2Opt(
      std::function<Routes&(const ESolutionHFMVRP&)> _getRoutes,
      const ESolutionHFMVRP& se, ProblemInstance* _hfmvrp)
      : super(_getRoutes, se, _hfmvrp), ads(se.first.getADS()) {
    if (!_hfmvrp) {
      cout << "Error: hfmvrp problem is NULL!" << endl;
      exit(1);
    }
  }

  virtual ~DeltaNSIteratorVRP2Opt() {}

  void first() override {
    for (int r = 0; r < (int)rep.size(); r++)
      if (!(ads.neighborhoodStatus.find(DeltaMoveVRP2Opt::idComponent())
                ->second[r])) {
        int tam = rep.at(r).size() - 2;
        for (int p1 = 0; p1 < tam; p1++) {
          for (int p2 = p1 + 2; p2 < (int)rep.at(r).size(); p2++) {
            uptr<Move<ESolutionHFMVRP>> m1(
                new MOVE(super::getRoutes, r, p1, p2, p));
            moves.push_back(std::move(m1));
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
      cout << "There isnt any current element!" << endl;
      cout << "DeltaNSSeqVRP2Opt. Aborting." << endl;
      exit(1);
    }

    uptr<Move<ESolutionHFMVRP>> m2(std::move(m));
    m = nullptr;
    return m2;
  }
};

}  // namespace HFMVRP

#endif  // EXAMPLES_HFMVRP_INCLUDE_HFMVRP_NEIGHBORHOODS_INTRA_DELTAITERATORVRP2OPT_HPP_
