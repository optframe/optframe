// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_INITIALPARETO_HPP_
#define OPTFRAME_INITIALPARETO_HPP_

//
#include <string>
#include <utility>
//
#include "Component.hpp"
#include "Constructive.hpp"
#include "Heuristics/GRASP/GRConstructive.hpp"
#include "MultiEvaluator.hpp"
#include "Pareto.hpp"
#include "RandGen.hpp"
// #include "MultiEvaluation.hpp"
// #include "Timer.hpp"
#include "InitialSearch.hpp"
#include "ParetoManager.hpp"

using namespace std;

namespace optframe {

template <XESolution XMES, XEvaluation XMEv = MultiEvaluation<>,
          XSearch<XMES> XSH = Pareto<XMES>>
class InitialPareto : public InitialSearch<XMES, Pareto<XMES>> {
 public:
  using S = typename XMES::first_type;
  static_assert(is_same<S, typename XMES::first_type>::value);
  static_assert(is_same<XMEv, typename XMES::second_type>::value);

  virtual ~InitialPareto() {}

  // TODO: deprecate this in favor of 'initialSearch' (how to pass population?)
  virtual Pareto<XMES> generatePareto(unsigned populationSize,
                                      double timeLimit) = 0;

  std::pair<std::optional<Pareto<XMES>>, SearchStatus> initialSearch(
      const StopCriteria<XMEv>& stop) override {
    return make_pair(
        make_optional(generatePareto(stop.xshCount, stop.timelimit)),
        SearchStatus::NO_REPORT);
  }

  static string idComponent() {
    stringstream ss;
    ss << Component::idComponent() << ":InitialPareto";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

template <XESolution XES, XEMSolution XMES>
class BasicInitialPareto : public InitialPareto<XMES> {
 public:
  using S = typename XMES::first_type;
  using XMEv = typename XMES::second_type;

  // Constructive<S>& constructive;
  InitialSearch<XMES>& constructive;
  ParetoManager<XES, XMES> pMan;

  BasicInitialPareto(InitialSearch<XMES>& _constructive,
                     ParetoManager<XES, XMES>& _pman)
      : constructive(_constructive), pMan(_pman) {}

  virtual ~BasicInitialPareto() {}

  Pareto<XMES> generatePareto(unsigned populationSize,
                              double timelimit) override {
    Pareto<XMES> p;
    StopCriteria<XMEv> sosc(timelimit);
    for (unsigned i = 0; i < populationSize; i++)
      // pMan.addSolution(p, *constructive.generateSolution(timelimit));
      pMan.addSolution(p, constructive.initialSearch(sosc).first->first);

    return p;
  }

  static string idComponent() {
    stringstream ss;
    ss << InitialPareto<XMES>::idComponent() << ":BasicInitialPareto";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

template <XESolution XES, XEMSolution XMES>
class GRInitialPareto : public InitialPareto<XMES> {
  using S = typename XES::first_type;

 public:
  GRConstructive<S>& constructive;
  RandGen& rg;
  double maxAlpha;  // limit the solution to be not so random
  ParetoManager<XES, XMES> pMan;

  // GRInitialPareto(GRConstructive<S>& _constructive, RandGen& _rg, double
  // _maxAlpha, MultiEvaluator<S, XEv>& _mev) :
  GRInitialPareto(GRConstructive<S>& _constructive, RandGen& _rg,
                  double _maxAlpha, GeneralEvaluator<XMES>& _mev)
      : constructive(_constructive),
        rg(_rg),
        maxAlpha(_maxAlpha),
        pMan(ParetoManager<XES, XMES>(_mev)) {}

  virtual ~GRInitialPareto() {}

  Pareto<XMES>& generatePareto(unsigned populationSize,
                               double timelimit) override {
    Timer tnow;

    // NOLINTNEXTLINE
    Pareto<XMES>* p = new Pareto<XMES>;
    unsigned i = 0;
    while ((i < populationSize) && (tnow.now() < timelimit)) {
      float alpha = rg.rand01();
      while (alpha > maxAlpha) {
        alpha = rg.rand01();
      }

      if (alpha == 0) alpha = 0.00001;

      pMan.addSolution(*p, constructive.generateGRSolution(alpha));
      i++;
    }
    return *p;
  }

  static string idComponent() {
    stringstream ss;
    ss << InitialPareto<XMES>::idComponent() << ":GRInitialPareto";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_INITIALPARETO_HPP_
