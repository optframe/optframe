// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_MULTISTART_HPP_
#define OPTFRAME_HEURISTICS_MULTISTART_HPP_  // NOLINT

// C
#include <math.h>
// C++
#include <string>
#include <utility>
#include <vector>
//

#include <OptFrame/LocalSearch.hpp>

#include "../Search/ITrajectory.hpp"
#include "../Search/SingleObjSearch.hpp"

// Current MultiStart is a trajectory-based method and SingleObjSearch(SoS).
// This means:
// 1) Primary and secondary spaces are the same (XES=XES2)
// 2) Due to SoS nature, it requires XSH from SoS to be its basetype (=XES)

// DIRECT EXPLANATION: the way it is now, it will not support Populational
// local searches and pareto local search... it could be changed for that,
// in a near future

namespace optframe {
template <XESolution XES>
class MultiStart : public SingleObjSearch<XES>, public ITrajectory<XES> {
 public:
  using XEv = typename XES::second_type;
  using XSH = XES;  // XSearch
  //
  sref<GeneralEvaluator<XES>> evaluator;
  sref<InitialSearch<XES>> constructive;
  sref<LocalSearch<XES>> localSearch;
  int iterMax;

  MultiStart(sref<GeneralEvaluator<XES>> _evaluator,
             sref<InitialSearch<XES>> _constructive,
             sref<LocalSearch<XES>> _localSearch, int _iterMax)
      : evaluator{_evaluator},
        constructive{_constructive},
        localSearch{_localSearch},
        iterMax{_iterMax} {
    //
  }

  virtual ~MultiStart() = default;

  SearchOutput<XES, XSH> searchBy(const StopCriteria<XEv>& stop,
                                  std::optional<XSH> _best) override {
    if (Component::information)
      std::cout << "MS search(" << stop.timelimit << ")" << std::endl;

    // optionally received from searchBy
    std::optional<XSH> star = _best;

    // perform iterMax loop
    for (auto i = 0; i < iterMax; i++) {
      // early stop, maybe?
      if (star && stop.shouldStop(star->second))
        return {SearchStatus::NO_REPORT, star};
      // ignore search status from initialSearch
      std::optional<XSH> incumbent = constructive->initialSearch(stop).first;
      if (stop.shouldStop()) break;  // timeout already?
      if (!incumbent) continue;      // try again
      if (!star) star = incumbent;   // copy
      // ignore search status from localSearch
      localSearch->searchFrom(*incumbent, stop);
      // check best
      if (evaluator->betterStrict(incumbent->second, star->second)) {
        star = incumbent;
        if (Component::information)
          std::cout << "MS: new best solution! e=" << star->second.toString()
                    << std::endl;
      }
    }  // end-for: finish iterMax

    return {SearchStatus::NO_REPORT, star};
  }

  // =======================================
  //               Component
  // =======================================
  // set verbose level recursive: returns 'false' if not supported.
  bool setMessageLevelR(LogLevel ll) override {
    this->setMessageLevel(ll);

    evaluator->setMessageLevel(ll);
    constructive->setMessageLevel(ll);
    localSearch->setMessageLevel(ll);

    return true;
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (SingleObjSearch<XES>::compatible(s));
  }

  std::string id() const override { return idComponent(); }

  static string idComponent() {
    stringstream ss;
    ss << SingleObjSearch<XES>::idComponent() << ":MultiStart";
    return ss.str();
  }
};

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES, XESolution XES2,
          X2ESolution<XES2> X2ES = MultiESolution<XES2>>
#else
template <typename XES, XESolution XES2, typename X2ES = MultiESolution<XES2>>
#endif
class MultiStartBuilder : public GlobalSearchBuilder<XES> {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;

 public:
  virtual ~MultiStartBuilder() = default;

  // NOLINTNEXTLINE
  GlobalSearch<XES>* build(Scanner& scanner, HeuristicFactory<XES>& hf,
                           string family = "") override {
    if (Component::debug)
      std::cout << "MultiStart Builder Loading Parameter #0" << std::endl;
    if (!scanner.hasNext()) {
      if (Component::warning)
        std::cout << "no next1a! aborting..." << std::endl;
      return nullptr;
    }

    sptr<GeneralEvaluator<XES>> eval;
    std::string sid_0 = scanner.next();
    int id_0 = *scanner.nextInt();
    hf.assign(eval, id_0, sid_0);
    assert(eval);

    sptr<GeneralEvaluator<XES>> ge{eval};

    if (Component::debug)
      std::cout << "MultiStart Builder Loading Parameter #2" << std::endl;
    if (!scanner.hasNext()) {
      if (Component::warning)
        std::cout << "no next1b! aborting..." << std::endl;
      return nullptr;
    }

    sptr<InitialSearch<XES>> constructive;
    std::string sid_1 = scanner.next();
    int id_1 = *scanner.nextInt();
    hf.assign(constructive, id_1, sid_1);
    assert(constructive);

    if (Component::debug)
      std::cout << "MultiStart Builder Loading Parameter #3" << std::endl;
    if (!scanner.hasNext()) {
      if (Component::warning)
        std::cout << "no next1c! aborting..." << std::endl;
      return nullptr;
    }

    sptr<LocalSearch<XES>> localSearch;
    std::string sid_2 = scanner.next();
    int id_2 = *scanner.nextInt();
    hf.assign(localSearch, id_2, sid_2);
    assert(localSearch);

    if (Component::debug)
      std::cout << "BasicSA Builder Loading Parameter #4" << std::endl;
    if (!scanner.hasNext()) {
      if (Component::warning)
        std::cout << "no next iterMax! aborting..." << std::endl;
      return nullptr;
    }
    int iterMax = *scanner.nextInt();

    if (Component::debug) {
      std::cout << "MultiStart Builder: got all parameters!" << std::endl;
      std::cout << "MultiStart with:" << std::endl;
      std::cout << "\teval=" << ge->id() << std::endl;
      std::cout << "\tconstructive=" << constructive->id() << std::endl;
      std::cout << "\tls=" << localSearch->id() << std::endl;
      std::cout << "\titerMax=" << iterMax << std::endl;
    }

    // NOLINTNEXTLINE
    return new MultiStart<XES>(ge, constructive, localSearch, iterMax);
  }

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(
        make_pair(GeneralEvaluator<XES>::idComponent(), "evaluation function"));
    params.push_back(
        make_pair(InitialSearch<XES>::idComponent(), "constructive heuristic"));
    params.push_back(
        make_pair(LocalSearch<XES>::idComponent(), "local search heuristic"));

    params.push_back(make_pair("OptFrame:int", "number of iterations"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == MultiStart<XES>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << GlobalSearchBuilder<XES>::idComponent() << "MultiStart";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_MULTISTART_HPP_  // NOLINT
