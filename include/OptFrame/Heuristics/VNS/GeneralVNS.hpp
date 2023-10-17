// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_VNS_GENERALVNS_HPP_
#define OPTFRAME_HEURISTICS_VNS_GENERALVNS_HPP_

#include <math.h>
//
#include <memory>
#include <string>
#include <utility>
#include <vector>
//

#include "VNS.h"
#include "VariableNeighborhoodSearch.hpp"

namespace optframe {

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES, XSearch<XES> XSH = XES>
#else
template <typename XES, typename XSH = XES>
#endif
class GeneralVNS : public VariableNeighborhoodSearch<XES> {
 public:
  typedef VariableNeighborhoodSearch<XES> super;

  // GeneralVNS(Evaluator<S>& evaluator, Constructive<S>& constructive,
  // vector<NS<XES, XSH>*> vshake, vector<NSSeq<S>*> vsearch) :
  GeneralVNS(sref<GeneralEvaluator<XES>> evaluator,
             sref<InitialSearch<XES>> constructive, vsref<NS<XES, XSH>> vshake,
             vsref<NSSeq<XES>> vsearch)
      : VariableNeighborhoodSearch<XES>(evaluator, constructive, vshake,
                                        vsearch) {}

  virtual ~GeneralVNS() {}

  sref<LocalSearch<XES>> buildSearch(unsigned k_search) override {
    vector<LocalSearch<XES>*> vls;
    for (unsigned i = 0; i < super::vsearch.size(); i++)
      vls.push_back(
          new BestImprovement<XES>(super::evaluator, *super::vsearch.at(i)));

    return *new VariableNeighborhoodDescent<XES>(super::evaluator, vls);
  }

  std::string id() const override { return idComponent(); }

  static string idComponent() {
    stringstream ss;
    ss << VariableNeighborhoodSearch<XES>::idComponent() << "GVNS";
    return ss.str();
  }
};

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XSolution S, XEvaluation XEv = Evaluation<>,
          XESolution XES = pair<S, XEv>,
          X2ESolution<XES> X2ES = MultiESolution<XES>>
#else
template <typename S, typename XEv = Evaluation<>, typename XES = pair<S, XEv>,
          typename X2ES = MultiESolution<XES>>
#endif
class GeneralVNSBuilder : public ILS,
                          public SingleObjSearchBuilder<S, XEv, XES> {
  using XSH = XES;  // primary-based search type only (BestType)

 public:
  virtual ~GeneralVNSBuilder() {}

  SingleObjSearch<XES>* build(Scanner& scanner,
                              HeuristicFactory<S, XEv, XES, X2ES>& hf,
                              string family = "") override {
    std::shared_ptr<GeneralEvaluator<XES>> eval;
    std::string comp_id1 = scanner.next();
    int id1 = *scanner.nextInt();
    hf.assign(eval, id1, comp_id1);

    std::shared_ptr<InitialSearch<XES>> constructive;
    std::string comp_id2 = scanner.next();
    int id2 = *scanner.nextInt();
    hf.assign(constructive, id2, comp_id2);

    vsptr<NS<XES, XSH>> _shakelist;
    std::string comp_id3 = scanner.next();
    int id3 = *scanner.nextInt();
    hf.assignList(_shakelist, id3, comp_id3);
    vsref<NS<XES, XSH>> shakelist;
    for (auto x : _shakelist) shakelist.push_back(x);

    vsptr<NSSeq<XES>> _searchlist;
    std::string comp_id4 = scanner.next();
    int id4 = *scanner.nextInt();
    hf.assignList(_searchlist, id4, comp_id4);
    vsref<NSSeq<XES>> searchlist;
    for (auto x : _searchlist) searchlist.push_back(x);

    return new BasicVNS<XES, XSH>(eval, constructive, shakelist, searchlist);
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
    ss << NS<XES, XSH>::idComponent() << "[]";
    params.push_back(make_pair(ss.str(), "list of NS"));

    stringstream ss2;
    ss2 << NSSeq<XES>::idComponent() << "[]";
    params.push_back(make_pair(ss2.str(), "list of NSSeq"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == BasicVNS<XES, XSH>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << SingleObjSearchBuilder<S, XEv>::idComponent() << VNS::family()
       << "GVNS";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_VNS_GENERALVNS_HPP_
