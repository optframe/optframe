// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_LOCALSEARCHES_VARIABLENEIGHBORHOODDESCENTUPDATEADS_HPP_
#define OPTFRAME_HEURISTICS_LOCALSEARCHES_VARIABLENEIGHBORHOODDESCENTUPDATEADS_HPP_

#include <string>
#include <utility>
#include <vector>
//
#include <OptFrame/Core/Evaluator.hpp>
#include <OptFrame/Core/NSEnum.hpp>
#include <OptFrame/Helper/ADSManager.hpp>
#include <OptFrame/Search/LocalSearch.hpp>

#include "FamilyVND.hpp"

namespace optframe {

// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
#ifdef OPTFRAME_USE_STD_CONCEPTS
template <XRepresentation R, class ADS, XBaseSolution<R, ADS> S,
          XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>>
#else
template <typename R, typename ADS, typename S, typename XEv = Evaluation<>,
          typename XES = pair<S, XEv>>
#endif
class VariableNeighborhoodDescentUpdateADS : public LocalSearch<XES> {
 private:
  sref<GeneralEvaluator<XES>> ev;
  sref<ADSManager<R, ADS, S>> adsMan;
  vsref<LocalSearch<XES>> lsList;

 public:
  VariableNeighborhoodDescentUpdateADS(sref<GeneralEvaluator<XES>> _ev,
                                       sref<ADSManager<R, ADS, S>> _adsMan,
                                       vsref<LocalSearch<XES>> _lsList)
      : ev(_ev), adsMan(_adsMan), lsList(_lsList) {}

  virtual ~VariableNeighborhoodDescentUpdateADS() = default;

  SearchStatus searchFrom(XES& se, const StopCriteria<XEv>& sosc) override {
    // S& s = se.first;
    // XEv& e = se.second;
    //
    double timelimit = sosc.timelimit;

    // XEv target_f(sosc.target_f); // 'target_f' will break... removing

    long tini = time(nullptr);

    int r = lsList.size();

    int k = 1;

    long tnow = time(nullptr);
    while ((k <= r) &&
           ((tnow - tini) < timelimit))  //&& ev.betterThan(target_f, e))
    {
      // avoiding heap
      // S* s0 = &s.clone();
      // Evaluation<>* e0 = &e.clone();
      // S s0(s); // enough to clone?
      // XEv e0(e);
      pair<S, XEv> p0 = se;  // enough to clone?

      lsList[k - 1]->searchFrom(p0, sosc);

      // if (ev.betterThan(p0, se)) {
      // if (p0.second.betterStrict(se.second)) {
      if (ev->betterStrict(p0.second, se.second)) {
        se = p0;
        // delete s0; // no need
        // delete e0; // no need
        k = 1;
      } else {
        // Find move ID
        string localSearchID = lsList[k - 1]->toString();
        unsigned found = localSearchID.find("OptFrame");
        string moveID = localSearchID.substr(found);
        adsMan->setNeighLocalOptimum(se.first, moveID);

        // delete s0; // no need
        // delete e0; // no need

        k = k + 1;
      }
      //
      ev->reevaluate(se);

      tnow = time(nullptr);
    }
    return SearchStatus::NO_REPORT;
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (LocalSearch<XES>::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << LocalSearch<XES>::idComponent() << ":VNDUpdateADS";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  std::string toString() const override {
    std::stringstream ss;
    ss << "VND: [ ";
    for (unsigned i = 0; i < lsList.size(); i++) {
      ss << lsList[i]->toString();
      if (i != lsList.size() - 1) ss << ",";
    }
    ss << "]";

    return ss.str();
  }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_LOCALSEARCHES_VARIABLENEIGHBORHOODDESCENTUPDATEADS_HPP_
