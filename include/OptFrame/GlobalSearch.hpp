// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_GLOBALSEARCH_HPP_
#define OPTFRAME_GLOBALSEARCH_HPP_

// C++
#include <cstring>
#include <iostream>
#include <string>
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

namespace optframe {
// REMEMBER: XES = (S, E)
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
  // AVAILABLE HERE
  // best known XSH object: solution/pareto/etc
  // std::optional<XSH> best;
  //
  // NOT AVAILABLE HERE
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

  virtual std::string log() const { return "Empty heuristic log."; }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (Component::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":GlobalSearch"
       << Domain::getAlternativeDomain<XES>("<XESf64>");
    // NOTE THAT: PRIMARY/BEST IS TYPICALLY XESf64
    // WE IGNORE SECONDARY TYPE FOR NOW...
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
    ss << GlobalSearch<XES, XSH>::idComponent() << ":Populational"
       << Domain::getAlternativeDomain<XES>("<XESf64>") << ":";
    // NOTE THAT:
    // - PRIMARY/BEST IS TYPICALLY XESf64
    // - SECONDARY/INCUMBENT IS TYPICALLY X2ESf64
    // FOR NOW, WE DONT FLAG SECONDARY TYPES... BUT WE COULD IN THE FUTURE (IF
    // NECESSARY!) A NOTATION COULD BE: Populational<XESf64, X2ESf64>
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  std::string toString() const override { return id(); }
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

#endif  // OPTFRAME_GLOBALSEARCH_HPP_
