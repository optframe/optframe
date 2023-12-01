// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_LOCALSEARCHES_VARIABLENEIGHBORHOODDESCENTUPDATEADS_HPP_
#define OPTFRAME_HEURISTICS_LOCALSEARCHES_VARIABLENEIGHBORHOODDESCENTUPDATEADS_HPP_

#include <string>
#include <utility>
#include <vector>
//
#include <OptFrame/Evaluator.hpp>
#include <OptFrame/Helper/ADSManager.hpp>
#include <OptFrame/LocalSearch.hpp>
#include <OptFrame/NSEnum.hpp>

#include "VND.h"

namespace optframe {

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
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

  static string idComponent() {
    stringstream ss;
    ss << LocalSearch<XES>::idComponent() << ":VNDUpdateADS";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  std::string toString() const override {
    stringstream ss;
    ss << "VND: [ ";
    for (unsigned i = 0; i < lsList.size(); i++) {
      ss << lsList[i]->toString();
      if (i != lsList.size() - 1) ss << ",";
    }
    ss << "]";

    return ss.str();
  }
};

/// template<XESolution XES, XEvaluation XEv = Evaluation<>>
// template<XRepresentation R, class ADS, XBaseSolution<R,ADS> S =
// CopySolution<R,ADS>, XEvaluation XEv = Evaluation<>>
//  passing 'S' manually, for safety
//
//
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XRepresentation R, class ADS, XBaseSolution<R, ADS> S,
          XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>,
          X2ESolution<XES> X2ES = MultiESolution<XES>>
#else
template <typename R, class ADS, typename S, typename XEv = Evaluation<>,
          typename XES = pair<S, XEv>, typename X2ES = MultiESolution<XES>>
#endif
class VariableNeighborhoodDescentUpdateADSBuilder
    : public LocalSearchBuilder<XES> {
 public:
  virtual ~VariableNeighborhoodDescentUpdateADSBuilder() = default;

  // NOLINTNEXTLINE
  LocalSearch<XES>* build(Scanner& scanner, HeuristicFactory<XES>& hf,
                          string family = "") override {
    sptr<GeneralEvaluator<XES>> eval;
    std::string comp_id1 = scanner.next();
    int id1 = *scanner.nextInt();
    hf.assign(eval, id1, comp_id1);

    sptr<ADSManager<R, ADS, S>> adsMan;
    std::string comp_id2 = scanner.next();
    int id2 = *scanner.nextInt();
    hf.assign(adsMan, id2, comp_id2);

    vsptr<LocalSearch<XES>> _hlist;
    std::string comp_id3 = scanner.next();
    int id3 = *scanner.nextInt();
    hf.assignList(_hlist, id3, comp_id3);
    vsref<LocalSearch<XES>> hlist;
    for (auto x : _hlist) hlist.push_back(x);

    // NOLINTNEXTLINE
    return new VariableNeighborhoodDescentUpdateADS<R, ADS, S, XEv>(
        eval, adsMan, hlist);
  }

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(make_pair(Evaluator<S, XEv, XES>::idComponent(),
                               "evaluation function"));

    params.push_back(
        make_pair(ADSManager<R, ADS, S>::idComponent(), "ADSManager function"));

    stringstream ss;
    ss << LocalSearch<XES>::idComponent() << "[]";
    params.push_back(make_pair(ss.str(), "list of local searches"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component ==
           VariableNeighborhoodDescentUpdateADS<R, ADS, S, XEv>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << LocalSearchBuilder<XES>::idComponent() << ":VNDUpdateADS";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_LOCALSEARCHES_VARIABLENEIGHBORHOODDESCENTUPDATEADS_HPP_
