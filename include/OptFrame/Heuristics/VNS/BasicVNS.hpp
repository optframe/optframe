// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_VNS_BASICVNS_HPP_
#define OPTFRAME_HEURISTICS_VNS_BASICVNS_HPP_

// C
#include <math.h>
// C++
#include <string>
#include <utility>
#include <vector>
//
#include <OptFrame/Heuristics/LocalSearches/BestImprovement.hpp>
#include <OptFrame/Heuristics/VNS/VariableNeighborhoodSearch.hpp>
#include <OptFrame/Hyper/HeuristicFactory.hpp>

#include "VNS.h"

namespace optframe {

template <XESolution XES, XEvaluation XEv = Evaluation<>>
class BasicVNS : public VariableNeighborhoodSearch<XES, XEv> {
 public:
  typedef VariableNeighborhoodSearch<XES, XEv> super;

  // BasicVNS(Evaluator<XES>& evaluator, Constructive<S>& constructive,
  // vector<NS<XES, XEv>*> vshake, vector<NSSeq<S>*> vsearch) :
  BasicVNS(sref<GeneralEvaluator<XES>> evaluator,
           sref<InitialSearch<XES>> constructive, vsref<NS<XES, XEv>> vshake,
           vsref<NSSeq<XES>> vsearch)
      : VariableNeighborhoodSearch<XES, XEv>(evaluator, constructive, vshake,
                                             vsearch) {}

  virtual ~BasicVNS() = default;

  sref<LocalSearch<XES, XEv>> buildSearch(unsigned k_search) override {
    return *new BestImprovement<XES, XEv>(super::evaluator,
                                          super::vsearch.at(k_search));
  }

  std::string id() const override { return idComponent(); }

  static string idComponent() {
    stringstream ss;
    ss << VariableNeighborhoodSearch<XES, XEv>::idComponent() << "BVNS";
    return ss.str();
  }
};

template <XSolution S, XEvaluation XEv = Evaluation<>,
          XESolution XES = pair<S, XEv>,
          X2ESolution<XES> X2ES = MultiESolution<XES>>
class BasicVNSBuilder : public ILS, public SingleObjSearchBuilder<S, XEv, XES> {
 public:
  ~BasicVNSBuilder() override = default;

  // NOLINTNEXTLINE
  SingleObjSearch<XES>* build(Scanner& scanner,
                              HeuristicFactory<S, XEv, XES, X2ES>& hf,
                              string family = "") override {
    sptr<GeneralEvaluator<XES>> eval;
    std::string comp_id1 = scanner.next();
    int id1 = *scanner.nextInt();
    hf.assign(eval, id1, comp_id1);

    sptr<InitialSearch<XES>> constructive;
    std::string comp_id2 = scanner.next();
    int id2 = *scanner.nextInt();
    hf.assign(constructive, id2, comp_id2);

    vsptr<NS<XES, XEv>> _shakelist;
    std::string comp_id3 = scanner.next();
    int id3 = *scanner.nextInt();
    hf.assignList(_shakelist, id3, comp_id3);
    vsref<NS<XES, XEv>> shakelist;
    for (auto x : _shakelist) shakelist.push_back(x);

    vsptr<NSSeq<XES, XEv>> _searchlist;
    std::string comp_id4 = scanner.next();
    int id4 = *scanner.nextInt();
    hf.assignList(_searchlist, id4, comp_id4);
    vsref<NSSeq<XES, XEv>> searchlist;
    for (auto x : _searchlist) searchlist.push_back(x);

    // NOLINTNEXTLINE
    return new BasicVNS<XES, XEv>(eval, constructive, shakelist, searchlist);
  }

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(
        make_pair(GeneralEvaluator<XES>::idComponent(), "evaluation function"));
    // params.push_back(make_pair(Constructive<S>::idComponent(), "constructive
    // heuristic"));
    params.push_back(
        make_pair(InitialSearch<XES>::idComponent(), "constructive heuristic"));

    stringstream ss;
    ss << NS<XES, XEv>::idComponent() << "[]";
    params.push_back(make_pair(ss.str(), "list of NS"));

    stringstream ss2;
    ss2 << NSSeq<XES, XEv>::idComponent() << "[]";
    params.push_back(make_pair(ss2.str(), "list of NSSeq"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == BasicVNS<XES, XEv>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << SingleObjSearchBuilder<S, XEv>::idComponent() << VNS::family()
       << "BasicVNS";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_VNS_BASICVNS_HPP_
