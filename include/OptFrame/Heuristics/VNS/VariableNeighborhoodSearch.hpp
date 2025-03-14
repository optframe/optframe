// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_VNS_VARIABLENEIGHBORHOODSEARCH_HPP_
#define OPTFRAME_HEURISTICS_VNS_VARIABLENEIGHBORHOODSEARCH_HPP_

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

#include "VNS.h"

namespace optframe {

template <XESolution XES, XEvaluation XEv = Evaluation<>, XESolution XSH = XES>
class VariableNeighborhoodSearch : public VNS, public SingleObjSearch<XES> {
 protected:
  sref<GeneralEvaluator<XES>> evaluator;
  sref<InitialSearch<XES>> constructive;
  vsref<NS<XES, XSH>> vshake;
  vsref<NSSeq<XES, XSH>> vsearch;

 public:
  VariableNeighborhoodSearch(sref<GeneralEvaluator<XES>> _evaluator,
                             sref<InitialSearch<XES>> _constructive,
                             vsref<NS<XES, XSH>> _vNS,
                             vsref<NSSeq<XES, XSH>> _vNSSeq)
      : evaluator(_evaluator),
        constructive(_constructive),
        vshake(_vNS),
        vsearch(_vNSSeq) {}

  ~VariableNeighborhoodSearch() override = default;

  // virtual void improvement(XSolution AUTO_CONCEPTS& s, Evaluation<>& e,
  // double timelimit, double target_f) = 0;

  virtual void shake(XES& se, unsigned int k_shake,
                     const StopCriteria<XEv>& sosc) {
    // double timelimit = sosc.timelimit;
    // XEv target_f(sosc.target_f); // BROKEN
    //
    // XSolution AUTO_CONCEPTS& s = se.first;
    // Evaluation<>& e = se.second;
    uptr<Move<XES, XSH>> move = vshake.at(k_shake)->validRandomMove(se);
    if (move) {
      move->applyUpdate(se);
      evaluator->reevaluate(se);  // refresh 'e'
    }
  }

  virtual pair<XES, unsigned int> neighborhoodChange(const XES& star,
                                                     const XES& p2,
                                                     unsigned int k) {
    // const XSolution AUTO_CONCEPTS& s2 = p2.first;
    const XEv& e2 = p2.second;
    // const XSolution AUTO_CONCEPTS& sStar = star.first;
    const XEv& eStar = star.second;
    //
    // if (evaluator.betterThan(e2, eStar))
    // if (e2.betterStrict(eStar))
    if (evaluator->betterStrict(e2, eStar)) {
      // IMPROVEMENT!
      // XES p(s2.clone(), e2.clone()); // TODO: avoid leak!!
      XES p = p2;  // copy! AVOID

      if (Component::information) {
        std::cout << "VNS: improvement at NS " << k << " => " << e2.evaluation()
             << std::endl;
        // e2.print();
        // eStar.print();
      }
      return make_pair(p, 0);  // k <- 0
    } else {
      // XES p(sStar.clone(), eStar.clone()); // TODO: avoid leak!
      XES p = star;  // COPY: AVOID!
      return make_pair(p, k + 1);
    }
  }

  virtual sref<LocalSearch<XES>> buildSearch(unsigned k_search) = 0;

  /*
   XES genPair(double timelimit)
   {
      std::optional<S> sStar = constructive.generateSolution(timelimit);
                XEv eStar = evaluator.evaluate(*sStar);
      return make_pair(*sStar, eStar);
   }
*/

  // pair<S, Evaluation<>>* search(StopCriteria<XEv>& sosc,  const S* _s =
  // nullptr,  const Evaluation<>* _e = nullptr) override virtual
  // std::optional<XES> search(StopCriteria<XEv>& sosc) override
  //
  // SearchStatus search(const StopCriteria<XEv>& sosc) override
  SearchOutput<XES> searchBy(const StopCriteria<XEv>& sosc,
                             std::optional<XES> _best) override {
    // gets incoming solution
    // op<XES>& star = this->best;
    op<XES> star;  // TODO: get on 'searchBy'
    //
    double timelimit = sosc.timelimit;
    // XEv target_f(sosc.target_f); // BROKEN
    // std::cout << id() << " search(target=" << target_f << ", timelimit=" <<
    // timelimit << ")" << std::endl;
    std::cout << id() << " search("
         << "timelimit=" << timelimit << ")"
         << std::endl;  // TODO: 'stop'.toString()

    Timer tnow;

    // XSolution AUTO_CONCEPTS& sStar =
    // *constructive.generateSolution(sosc.timelimit); Evaluation<>   eStar =
    // evaluator.evaluate(sStar); XES star =
    // input?*input:genPair(sosc.timelimit); // elvis star =
    // star?:genPair(sosc.timelimit);
    //
    // star = star ?: constructive->initialSearch(sosc).first;
    if (!star) star = constructive->initialSearch(sosc).first;
    //
    auto& sStar = star->first;
    auto& eStar = star->second;

    if (Component::information)
      std::cout << "VNS starts: " << eStar.evaluation() << std::endl;

    while (
        (tnow.now() < timelimit))  //  && evaluator.betterThan(target_f, eStar))
    {
      unsigned k = 0;
      // std::cout << "VNS k=0 initial target = " << target_f << " timelimit = " <<
      // timelimit << std::endl; std::cout << eStar.evaluation() << std::endl; std::cout <<
      // evaluator.betterThan(target_f, eStar) << std::endl; std::cout <<
      // evaluator.betterThan(8000, 7962.15) << std::endl;

      while (k < vshake.size()) {
        XES p1 = *star;  // copy (how to automatically invoke clone?)
        ////XSolution AUTO_CONCEPTS& s = *new S(sStar); // implicit clone on
        /// copy constructor /Evaluation<>&   e = eStar.clone();

        shake(p1, k, sosc);

        LocalSearch<XES>& improve = buildSearch(k);

        improve.searchFrom(p1, sosc);

        delete &improve;  // save trajectory history?

        pair<XES, unsigned int> nc = neighborhoodChange(*star, p1, k);

        sStar = nc.first.first;   // TODO: move?
        eStar = nc.first.second;  // TODO: move?

        // delete& nc.first.first; // drop automatically?
        // delete& nc.first.second; // drop automatically?

        // delete& s; // drop automatically?
        // delete& e; // drop automatically?

        if (k != nc.second) {
          // std::cout << "VNS k change from " << k << " to " << nc.second << std::endl;
        }
        k = nc.second;
      }
    }

    // if (sosc.target && evaluator.betterThan(star->second,
    // sosc.target->second)) if (star->second.betterStrict(sosc.target_f))
    if (evaluator->betterStrict(star->second, sosc.target_f)) {
      std::cout << "VNS exit by target_f: " << star->second.evaluation()
           << " better than " << sosc.target_f.evaluation() << std::endl;
      // std::cout << "isMin: " << evaluator.isMinimization() << std::endl;
      //  std::cout << "isMin: " << star->second.isMini << std::endl;
    }

    if (((tnow.now()) >= timelimit)) {
      std::cout << "VNS exit by timelimit: " << timelimit << std::endl;
    }

    // updates method solution
    // this->best = star;
    //
    // return std::optional<XES> (star);
    return {SearchStatus::NO_REPORT, star};
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << SingleObjSearch<XES>::idComponent() << VNS::family();
    // ss << SingleObjSearch<XES>::idComponent() << VNS::family <<
    // "VariableNeighborhoodSearch:";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_VNS_VARIABLENEIGHBORHOODSEARCH_HPP_
