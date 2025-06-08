// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_VNS_VARIABLENEIGHBORHOODSEARCH_HPP_
#define OPTFRAME_HEURISTICS_VNS_VARIABLENEIGHBORHOODSEARCH_HPP_

// This is MetaVNS, a template foi building VNS algorithms

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

// C
#include <math.h>
// C++
#include <string>
#include <vector>
//

#include <OptFrame/Core/Constructive.hpp>
#include <OptFrame/Core/Evaluator.hpp>
#include <OptFrame/Core/NS.hpp>
#include <OptFrame/Core/NSSeq.hpp>
#include <OptFrame/Search/LocalSearch.hpp>
#include <OptFrame/Search/SingleObjSearch.hpp>
#include <OptFrame/Timer.hpp>

#include "FamilyVNS.h"

#define MOD_EXPORT
#else

// CANNOT IMPORT HERE... Already part of optframe.core
/*
import std;
import optframe.component;
import optframe.concepts;
*/

// do NOT import/export modules on .hpp... only on .cppm

#define MOD_EXPORT export

#endif

namespace optframe {

MOD_EXPORT template <XESolution XES, XESolution XSH = XES>
class MetaVNS : public FamilyVNS, public SingleObjSearch<XES> {
  using XEv = typename XSH::second_type;

 protected:
  sref<GeneralEvaluator<XES>> evaluator;
  sref<InitialSearch<XES>> constructive;
  vsref<NS<XES, XSH>> vshake;
  vsref<NSSeq<XES, XSH>> vsearch;

 public:
  MetaVNS(sref<GeneralEvaluator<XES>> _evaluator,
          sref<InitialSearch<XES>> _constructive, vsref<NS<XES, XSH>> _vNS,
          vsref<NSSeq<XES, XSH>> _vNSSeq)
      : evaluator(_evaluator),
        constructive(_constructive),
        vshake(_vNS),
        vsearch(_vNSSeq) {}

  ~MetaVNS() override = default;

  virtual void shake(XES& se, unsigned int k_shake,
                     const StopCriteria<XEv>& sosc) {
    uptr<Move<XES, XSH>> move = vshake.at(k_shake)->validRandomMove(se);
    if (move) {
      move->applyUpdate(se);
      evaluator->reevaluate(se);  // refresh 'e'
    }
  }

  virtual std::pair<XES, unsigned int> neighborhoodChange(const XES& star,
                                                          const XES& p2,
                                                          unsigned int k) {
    const XEv& e2 = p2.second;

    const XEv& eStar = star.second;
    //

    if (evaluator->betterStrict(e2, eStar)) {
      // IMPROVEMENT!
      XES p = p2;  // copy! AVOID

      if (Component::information) {
        std::cout << "VNS: improvement at NS " << k << " => " << e2.evaluation()
                  << std::endl;
        // e2.print();
        // eStar.print();
      }
      return make_pair(p, 0);  // k <- 0
    } else {
      XES p = star;  // COPY: AVOID!
      return make_pair(p, k + 1);
    }
  }

  virtual sref<LocalSearch<XES>> buildSearch(unsigned k_search) = 0;

  SearchOutput<XES> searchBy(const StopCriteria<XEv>& sosc,
                             std::optional<XES> _best) override {
    // gets incoming solution
    op<XES> star;  // TODO: get on 'searchBy'
    //
    double timelimit = sosc.timelimit;

    std::cout << id() << " search("
              << "timelimit=" << timelimit << ")"
              << std::endl;  // TODO: 'stop'.toString()

    Timer tnow;

    if (!star) star = constructive->initialSearch(sosc).first;
    //
    auto& sStar = star->first;
    auto& eStar = star->second;

    if (Component::information)
      std::cout << "MetaVNS starts: " << eStar.evaluation() << std::endl;

    while (
        (tnow.now() < timelimit))  //  && evaluator.betterThan(target_f, eStar))
    {
      unsigned k = 0;

      while (k < vshake.size()) {
        XES p1 = *star;

        shake(p1, k, sosc);

        LocalSearch<XES>& improve = buildSearch(k);

        improve.searchFrom(p1, sosc);

        delete &improve;  // save trajectory history?

        std::pair<XES, unsigned int> nc = neighborhoodChange(*star, p1, k);

        sStar = nc.first.first;   // TODO: move?
        eStar = nc.first.second;  // TODO: move?

        if (k != nc.second) {
          // std::cout << "VNS k change from " << k << " to " << nc.second <<
          // std::endl;
        }
        k = nc.second;
      }
    }

    if (evaluator->betterStrict(star->second, sosc.target_f)) {
      std::cout << "MetaVNS exit by target_f: " << star->second.evaluation()
                << " better than " << sosc.target_f.evaluation() << std::endl;
      // std::cout << "isMin: " << evaluator.isMinimization() << std::endl;
      //  std::cout << "isMin: " << star->second.isMini << std::endl;
    }

    if (((tnow.now()) >= timelimit)) {
      std::cout << "MetaVNS exit by timelimit: " << timelimit << std::endl;
    }

    return {SearchStatus::NO_REPORT, star};
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << SingleObjSearch<XES>::idComponent() << FamilyVNS::family()
       << "MetaVNS";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_VNS_VARIABLENEIGHBORHOODSEARCH_HPP_
