// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_GLOBAL_SEARCH_HPP_
#define OPTFRAME_GLOBAL_SEARCH_HPP_

// C++
#include <cstring>
#include <iostream>
#include <utility>
#include <vector>
//
#include <OptFrame/BaseConcepts.hpp>
#include <OptFrame/Component.hpp>
#include <OptFrame/Helper/VEPopulation.hpp>
#include <OptFrame/Hyper/ComponentBuilder.hpp>
#include <OptFrame/InitialSearch.hpp>
#include <OptFrame/SearchOutput.hpp>
#include <OptFrame/StopCriteria.hpp>

// using namespace std;

namespace optframe {

// Defaulting SearchSpace to XES, it means, <S,XEv> space (typically, single obj
// search)
// template<XESolution XES, XSearch<XES> XSH = XES, XSearchMethod XM =
// Component>
//
// template<XESolution XES, XEvaluation XEv = Evaluation<>, XSearch<XES> XSH =
// XES>
//
// 'XES' is the "base concept" for the primary search component.
// 'XSH' is the primary search type ('best' has type XSH)
// 'XES2' is the "base concept" for the secondary search component.
// 'XSH2' is the secondary search type ('incumbent' has type XSH2)
//
// We need to make all methods compatible, and too many templates is polluting
// this... 1) Explicitly renaming XSH to XBest 2) Keeping XSH2 as XIncumbent on
// specific searchBy methods (Trajectory? Populational?)
template <XESolution XES, XSearch<XES> XSH = XES>
class GlobalSearch : public Component {
  using XEv = typename XES::second_type;

 public:
  using BestType = XSH;

 public:
  // best known XSH object: solution/pareto/etc
  // std::optional<XSH> best;
  // current/working XSH2 object: population/etc
  // std::optional<XSH2> incumbent;
  // ----------
  //
  // callback action 'onBest' is triggered after best is updated (if
  // 'beforeUpdateBest' is required some day, think about it.. not now)
  // returning 'false' should lead to aborting execution (by target, for
  // instance)
  //
  bool (*onBest)(GlobalSearch<XES, BestType>& self, const BestType& best) =
      [](GlobalSearch<XES, BestType>& self, const BestType& best) {
        return true;
      };
  //
  // onIncumbent now will depend on Incumbent type (look on ITrajectory or
  // IPopulational)
  //
  // bool (*onIncumbent)(GlobalSearch<XES, XSH, XES2, XSH2>& self, const XSH2&
  // incumbent) =
  //  [](GlobalSearch<XES, XSH, XES2, XSH2>& self, const XSH2& incumbent) {
  //  return true; };
  //
  // strict or non-strict search
  bool strict{true};

  GlobalSearch() {}

  ~GlobalSearch() override = default;

  // Assuming method is not thread-safe.
  // Now, we can easily use flag SearchStatus::RUNNING.
  virtual SearchOutput<XES, BestType> search(const StopCriteria<XEv>& stop) {
    return searchBy(stop, std::nullopt);
  }
  // 'searchBy': Optionally start with some solution (best type XSH)
  // Note this is a copy, not reference, so solution will be moved to the
  // output or even destroyed. SearchOutput MUST hold a solution which is,
  // AT LEAST as good as input 'best', and never worsen it.
  virtual SearchOutput<XES, BestType> searchBy(const StopCriteria<XEv>& stop,
                                               std::optional<XSH> best) = 0;

  /*
   virtual SearchStatus searchBy(std::optional<XSH>& _best, std::optional<XSH2>&
   _inc, const StopCriteria<XEv>& stopCriteria)
   {
      best = _best;
      incumbent = _inc;
      return search(stopCriteria);
   }
*/

  virtual std::string log() const { return "Empty heuristic log."; }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (Component::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":GlobalSearch";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

// 'XES' is the "base concept" for the primary search component.
// 'XSH' is the primary search type ('best' has type XSH)
// 'XES2' is the "base concept" for the secondary search component.
// 'XSH2' is the secondary search type ('incumbent' has type XSH2)
template <XESolution XES, XSearch<XES> XSH = XES, XESolution XES2 = XES,
          XSearch<XES> XSH2 = XSH>
class Populational : public GlobalSearch<XES, XSH> {
  using XEv = typename XES::second_type;
  using BestType = XSH;

 public:
  using IncumbentType =
      optframe::VEPopulation<XES2>;  // this uses EPopulation, not "legacy
                                     // Population"

  virtual ~Populational() = default;

 public:
  // onIncumbent now will depend on Incumbent type (look on ITrajectory or
  // IPopulational)
  //
  bool (*onIncumbent)(GlobalSearch<XES, XSH>& self,
                      const IncumbentType& incumbent) =
      [](GlobalSearch<XES, XSH>& self, const IncumbentType& incumbent) {
        return true;
      };
  //

  // global search method (maybe, someday, create some abstract IGlobalSearch..
  // not now)
  // virtual SearchStatus search(const StopCriteria<XEv>& stopCriteria) = 0;
  //
  // SearchOutput<XES, BestType> search(
  //    const StopCriteria<XEv>& stopCriteria) override = 0;

  //
  // virtual method with search signature for populational methods
  //
  virtual SearchOutput<XES, BestType> searchPopulational(
      std::optional<BestType>& _best, IncumbentType& _inc,
      const StopCriteria<XEv>& stopCriteria) = 0;

  bool compatible(std::string s) override {
    return (s == idComponent()) || (GlobalSearch<XES, XSH>::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << GlobalSearch<XES, XSH>::idComponent() << ":Populational:";
    return ss.str();
  }

  virtual std::string id() const override { return idComponent(); }

  virtual std::string toString() const override { return id(); }
};

template <XESolution XES, XSearch<XES> XSH, XESolution XES2,
          X2ESolution<XES2> X2ES =
              MultiESolution<XES2>>  //, XESolution XES2, XSearch<XES> XSH2 =
                                     // XSH>
class GlobalSearchBuilder
    : public ComponentBuilder<typename XES::first_type,
                              typename XES::second_type, XSH> {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;

 public:
  virtual ~GlobalSearchBuilder() {}

  virtual GlobalSearch<XES, XSH>* build(Scanner& scanner,
                                        HeuristicFactory<S, XEv, XES, X2ES>& hf,
                                        string family = "") = 0;

  Component* buildComponent(Scanner& scanner,
                            HeuristicFactory<S, XEv, XES, X2ES>& hf,
                            string family = "") override {
    return build(scanner, hf, family);
  }

  vector<pair<string, string>> parameters() override = 0;

  bool canBuild(string) override = 0;

  std::string toString() const override { return id(); }

  static string idComponent() {
    stringstream ss;
    ss << ComponentBuilder<S, XEv, XSH>::idComponent() << "GlobalSearch:";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif /* OPTFRAME_GLOBAL_SEARCH_HPP_ */
