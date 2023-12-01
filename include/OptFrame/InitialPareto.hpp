// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
// http://optframe.sourceforge.net/
//
// This file is part of the OptFrame optimization framework. This framework
// is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License v3 as published by the
// Free Software Foundation.

// This framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License v3 for more details.

// You should have received a copy of the GNU Lesser General Public License v3
// along with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

#ifndef OPTFRAME_INITIALPARETO_HPP_
#define OPTFRAME_INITIALPARETO_HPP_

#include "Component.hpp"
#include "Constructive.hpp"
#include "Heuristics/GRASP/GRConstructive.hpp"
#include "MultiEvaluator.hpp"
#include "Pareto.hpp"
#include "RandGen.hpp"
//#include "MultiEvaluation.hpp"
//#include "Timer.hpp"
#include "InitialSearch.hpp"
#include "ParetoManager.hpp"

using namespace std;

namespace optframe {

template <XESolution XMES, XEvaluation XMEv = MultiEvaluation<>,
          XSearch<XMES> XSH = Pareto<XMES>>
class InitialPareto
    : public InitialSearch<XMES, XMEv, Pareto<XMES>>  // public Component
{
 public:
  using S = typename XMES::first_type;
  static_assert(is_same<S, typename XMES::first_type>::value);
  static_assert(is_same<XMEv, typename XMES::second_type>::value);

  virtual ~InitialPareto() {}

  // TODO: deprecate this in favor of 'initialSearch' (how to pass population?)
  virtual Pareto<XMES> generatePareto(unsigned populationSize,
                                      double timeLimit = 10000000) = 0;

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

template <XESolution XMES, XEvaluation XMEv = MultiEvaluation<>>
class BasicInitialPareto : public InitialPareto<XMES> {
 public:
  using S = typename XMES::first_type;
  static_assert(is_same<S, typename XMES::first_type>::value);
  static_assert(is_same<XMEv, typename XMES::second_type>::value);

  // Constructive<S>& constructive;
  InitialSearch<XMES, XMEv>& constructive;
  paretoManager<S, XMEv, XMES> pMan;

  BasicInitialPareto(InitialSearch<XMES, XMEv>& _constructive,
                     paretoManager<S, XMEv, XMES>& _pman)
      : constructive(_constructive), pMan(_pman) {}

  virtual ~BasicInitialPareto() {}

  virtual Pareto<XMES> generatePareto(unsigned populationSize,
                                      double timelimit = 100000000) override {
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

template <XSolution S, XEvaluation XMEv = MultiEvaluation<>,
          XESolution XMES = pair<S, XMEv>>
class GRInitialPareto : public InitialPareto<XMES> {
 public:
  GRConstructive<S>& constructive;
  RandGen& rg;
  double maxAlpha;  // limit the solution to be not so random
  paretoManager<S, XMEv, XMES> pMan;

  // GRInitialPareto(GRConstructive<S>& _constructive, RandGen& _rg, double
  // _maxAlpha, MultiEvaluator<S, XEv>& _mev) :
  GRInitialPareto(GRConstructive<S>& _constructive, RandGen& _rg,
                  double _maxAlpha, GeneralEvaluator<XMES, XMEv>& _mev)
      : constructive(_constructive),
        rg(_rg),
        maxAlpha(_maxAlpha),
        pMan(paretoManager<S, XMEv, XMES>(_mev)) {}

  virtual ~GRInitialPareto() {}

  virtual Pareto<XMES>& generatePareto(
      unsigned populationSize, double timelimit = 100000000) override {
    Timer tnow;

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

#endif /*OPTFRAME_INITIALPARETO_H_*/
