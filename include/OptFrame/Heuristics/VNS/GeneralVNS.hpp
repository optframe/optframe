// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_VNS_GENERALVNS_HPP_
#define OPTFRAME_HEURISTICS_VNS_GENERALVNS_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

#include <math.h>
//
#include <memory>
#include <string>
#include <utility>
#include <vector>
//

#include <OptFrame/Heuristics/LocalSearches/BI.hpp>
#include <OptFrame/Heuristics/LocalSearches/VND.hpp>

#include "FamilyVNS.h"
#include "MetaVNS.hpp"

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

// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
#ifdef OPTFRAME_USE_STD_CONCEPTS
MOD_EXPORT template <XESolution XES, XSearch<XES> XSH = XES>
#else
MOD_EXPORT template <typename XES, typename XSH = XES>
#endif
class GeneralVNS : public MetaVNS<XES> {
 public:
  typedef MetaVNS<XES> super;

  GeneralVNS(sref<GeneralEvaluator<XES>> evaluator,
             sref<InitialSearch<XES>> constructive, vsref<NS<XES, XSH>> vshake,
             vsref<NSSeq<XES>> vsearch)
      : MetaVNS<XES>(evaluator, constructive, vshake, vsearch) {}

  virtual ~GeneralVNS() {}

  sref<LocalSearch<XES>> buildSearch(unsigned k_search) override {
    vsref<LocalSearch<XES>> vls;
    sref<GeneralEvaluator<XES>> geval = super::evaluator;
    for (unsigned i = 0; i < super::vsearch.size(); i++) {
      sref<NSSeq<XES>> nsseq = super::vsearch.at(i);
      sptr<LocalSearch<XES>> bi{new BI<XES>{geval, nsseq}};
      vls.push_back(bi);
    }
    LocalSearch<XES>* ptr_vnd = new VND<XES>{geval, vls};
    sref<LocalSearch<XES>> vnd{ptr_vnd};
    return vnd;
  }

  std::string id() const override { return idComponent(); }

  static std::string idComponent() {
    std::stringstream ss;
    ss << MetaVNS<XES>::idComponent() << "GeneralVNS";
    return ss.str();
  }
};

// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
#ifdef OPTFRAME_USE_STD_CONCEPTS
MOD_EXPORT template <XESolution XES>
#else
MOD_EXPORT template <typename XES>
#endif
class BuilderGeneralVNS : public FamilyVNS, public SingleObjSearchBuilder<XES> {
  using XSH = XES;  // primary-based search type only (BestType)

 public:
  ~BuilderGeneralVNS() override = default;

  SingleObjSearch<XES>* build(Scanner& scanner, HeuristicFactory<XES>& hf,
                              std::string family = "") override {
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

    return new GeneralVNS<XES, XSH>(eval, constructive, shakelist, searchlist);
  }

  std::vector<std::pair<std::string, std::string>> parameters() override {
    std::vector<std::pair<std::string, std::string>> params;
    params.push_back(
        make_pair(GeneralEvaluator<XES>::idComponent(), "evaluation function"));
    // params.push_back(std::make_pair(Constructive<S>::idComponent(),
    // "constructive heuristic"));
    params.push_back(
        make_pair(InitialSearch<XES>::idComponent(), "constructive heuristic"));

    std::stringstream ss;
    ss << NS<XES, XSH>::idComponent() << "[]";
    params.push_back(std::make_pair(ss.str(), "list of NS"));

    std::stringstream ss2;
    ss2 << NSSeq<XES>::idComponent() << "[]";
    params.push_back(std::make_pair(ss2.str(), "list of NSSeq"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == BasicVNS<XES, XSH>::idComponent();
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << SingleObjSearchBuilder<XES>::idComponent() << FamilyVNS::family()
       << "GVNS";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_VNS_GENERALVNS_HPP_
