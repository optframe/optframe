// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_VARIABLENEIGHBORHOODDESCENT_HPP_
#define OPTFRAME_VARIABLENEIGHBORHOODDESCENT_HPP_

#include <vector>
//
#include <OptFrame/Core/NSEnum.hpp>
#include <OptFrame/Core/RandGen.hpp>
#include <OptFrame/Evaluator.hpp>
#include <OptFrame/LocalSearch.hpp>

#include "VND.h"

namespace optframe {

// When RandGen is given as parameter it performs RVND
template <XESolution XES, XEvaluation XEv = typename XES::second_type>
class VariableNeighborhoodDescent : public LocalSearch<XES> {
 private:
  sref<GeneralEvaluator<XES>> ev;
  vsref<LocalSearch<XES>> lsList;
  sptr<RandGen> rg;

 public:
  VariableNeighborhoodDescent(sref<GeneralEvaluator<XES>> _ev,
                              vsref<LocalSearch<XES>> _lsList,
                              sptr<RandGen> _rg = nullptr)
      : ev(_ev), lsList(_lsList), rg(_rg) {}

  virtual ~VariableNeighborhoodDescent() {}

  // DEPRECATED
  // virtual void exec(S& s, const StopCriteria<XEv>& stopCriteria)
  //{
  //	Evaluation<> e = std::move(ev.evaluate(s));
  //	exec(s, e, stopCriteria);
  //}

  SearchStatus searchFrom(XES& se,
                          const StopCriteria<XEv>& stopCriteria) override {
    // S& s = se.first;
    // XEv& e = se.second;

    if (Component::information) cout << "VND::starts" << endl;

    Timer tNow;

    if (rg) rg->shuffle(lsList);  // shuffle elements

    int r = lsList.size();

    int k = 1;

    XES current(se);  // full backup! TODO: remove this copy
    // Evaluation<> eCurrent(e);
    //'target_f' will crash if not provided... removing
    while ((k <= r) &&
           !stopCriteria
                .shouldStop())  // && (ev.betterThan(stopCriteria.target_f, e))
    {
      // eCurrent = e; // backup
      current = se;  // TODO: remove this copy

      StopCriteria<XEv> stopCriteriaNextLS = stopCriteria;
      stopCriteriaNextLS.updateTimeLimit(tNow.now());
      lsList[k - 1]->searchFrom(se, stopCriteriaNextLS);

      // if (ev.betterThan(e, eCurrent))
      // if (ev.betterThan(se, current))
      // if (se.second.betterStrict(current.second))
      if (Component::debug)
        std::cout << "VND will compare(" << se.second.isOutdated() << ";"
                  << current.second.isOutdated() << ")" << std::endl;
      if (ev->betterStrict(se.second, current.second)) {
        // improvement
        k = 1;
        if (Component::information)
          // e.print();
          se.second.print();
      } else {
        k = k + 1;

        if (Component::information) cout << "VND::k=" << k << endl;
      }
    }
    return SearchStatus::NO_REPORT;
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (LocalSearch<XES>::compatible(s));
  }

  static string idComponent() {
    stringstream ss;
    ss << LocalSearch<XES>::idComponent() << ":VND";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  std::string toString() const override {
    stringstream ss;
    ss << "VND: [ ";
    for (unsigned i = 0; i < lsList.size(); i++) {
      auto& x = const_cast<sref<LocalSearch<XES>>&>(lsList[i]);
      // ss << lsList[i]->toString();
      ss << x->toString();
      if (i != lsList.size() - 1) ss << ",";
    }
    ss << "]";

    return ss.str();
  }
};

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES>
#else
template <typename XES>
#endif
class VariableNeighborhoodDescentBuilder : public LocalSearchBuilder<XES> {
 public:
  virtual ~VariableNeighborhoodDescentBuilder() {}

  LocalSearch<XES>* build(Scanner& scanner, HeuristicFactory<XES>& hf,
                          string family = "") override {
    if (this->verbose)
      std::cout << "Debug: VariableNeighborhoodDescentBuilder::build()"
                << std::endl;

    sptr<GeneralEvaluator<XES>> eval;
    std::string comp_ev_id = scanner.next();
    int ev_id = *scanner.nextInt();
    hf.assign(eval, ev_id, comp_ev_id);

    vsptr<LocalSearch<XES>> _hlist;
    std::string comp_list_id = scanner.next();
    int list_id = *scanner.nextInt();
    hf.assignList(_hlist, list_id, comp_list_id);
    vsref<LocalSearch<XES>> hlist;
    for (auto x : _hlist) hlist.push_back(x);

    return new VariableNeighborhoodDescent<XES>(eval, hlist);
  }

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(
        make_pair(GeneralEvaluator<XES>::idComponent(), "evaluation function"));
    stringstream ss;
    ss << LocalSearch<XES>::idComponent() << "[]";
    params.push_back(make_pair(ss.str(), "list of local searches"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == VariableNeighborhoodDescent<XES>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << LocalSearchBuilder<XES>::idComponent() << ":VND";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif /*OPTFRAME_VARIABLENEIGHBORHOODDESCENT_HPP_*/
