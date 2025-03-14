// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_VNS_RANDVNS_HPP_
#define OPTFRAME_HEURISTICS_VNS_RANDVNS_HPP_

#include <math.h>
//
#include <string>
#include <utility>
#include <vector>
//
#include "../LocalSearches/RVND.hpp"
#include "VNS.h"
#include "VariableNeighborhoodSearch.hpp"

namespace optframe {

template <class R, class ADS = OPTFRAME_DEFAULT_ADS,
          class DS = OPTFRAME_DEFAULT_DS>
class RandVNS : public VariableNeighborhoodSearch<R, ADS, DS> {
 public:
  typedef VariableNeighborhoodSearch<R, ADS, DS> super;

  RandVNS(Evaluator<R, ADS, DS>& evaluator, Constructive<S>& constructive,
          vector<NS<R, ADS, DS>*> vshake, vector<NSSeq<R, ADS, DS>*> vsearch,
          RandGen& _rg)
      : VariableNeighborhoodSearch<R, ADS, DS>(evaluator, constructive, vshake,
                                               vsearch),
        rg(_rg) {}

  virtual ~RandVNS() = default;

  virtual LocalSearch<R, ADS, DS>& buildSearch(unsigned k_search) {
    vector<LocalSearch<R, ADS, DS>*> vls;
    for (unsigned i = 0; i < super::vsearch.size(); i++)
      vls.push_back(new BestImprovement<R, ADS, DS>(super::evaluator,
                                                    *super::vsearch.at(i)));

    return *new RVND<R, ADS, DS>(super::evaluator, vls, rg);
  }

  std::string id() const override { return idComponent(); }

  static std::string idComponent() {
    std::stringstream ss;
    ss << VariableNeighborhoodSearch<R, ADS, DS>::idComponent() << "RandVNS";
    return ss.str();
  }

 private:
  RandGen& rg;
};

template <class R, class ADS = OPTFRAME_DEFAULT_ADS,
          class DS = OPTFRAME_DEFAULT_DS>
class RandVNSBuilder : public ILS, public SingleObjSearchBuilder<R, ADS, DS> {
 public:
  virtual ~RandVNSBuilder() = default;

  SingleObjSearch<R, ADS, DS>* build(Scanner& scanner,
                                     HeuristicFactory<R, ADS, DS>& hf,
                                     string family = "") override {
    Evaluator<R, ADS, DS>* eval;
    std::string comp_id1 = scanner.next();
    int id1 = *scanner.nextInt();
    hf.assign(eval, id1, comp_id1);

    Constructive<S>* constructive;
    std::string comp_id2 = scanner.next();
    int id2 = *scanner.nextInt();
    hf.assign(constructive, id2, comp_id2);

    vector<NS<R, ADS, DS>*> shakelist;
    std::string comp_id3 = scanner.next();
    int id3 = *scanner.nextInt();
    hf.assignList(shakelist, id3, comp_id3);

    vector<NSSeq<R, ADS, DS>*> searchlist;
    std::string comp_id4 = scanner.next();
    int id4 = *scanner.nextInt();
    hf.assignList(searchlist, id4, comp_id4);

    return new RandVNS<R, ADS, DS>(*eval, *constructive, shakelist, searchlist,
                                   hf.getRandGen());
  }

  std::vector<std::pair<std::string, std::string>> parameters() override {
    std::vector<std::pair<std::string, std::string>> params;
    params.push_back(
        make_pair(Evaluator<R, ADS, DS>::idComponent(), "evaluation function"));
    // params.push_back(std::make_pair(Constructive<S>::idComponent(), "constructive
    // heuristic"));
    params.push_back(
        make_pair(InitialSearch<XES>::idComponent(), "constructive heuristic"));

    std::stringstream ss;
    ss << NS<R, ADS, DS>::idComponent() << "[]";
    params.push_back(std::make_pair(ss.str(), "list of NS"));

    stringstream ss2;
    ss2 << NSSeq<R, ADS, DS>::idComponent() << "[]";
    params.push_back(std::make_pair(ss2.str(), "list of NSSeq"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == RandVNS<R, ADS, DS>::idComponent();
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << SingleObjSearchBuilder<R, ADS, DS>::idComponent() << VNS::family()
       << "RandVNS";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_VNS_RANDVNS_HPP_
