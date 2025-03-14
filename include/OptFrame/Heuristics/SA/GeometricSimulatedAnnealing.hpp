// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_SA_GEOMETRICSIMULATEDANNEALING_HPP_
#define OPTFRAME_HEURISTICS_SA_GEOMETRICSIMULATEDANNEALING_HPP_

#if (__cplusplus < 202302L) || defined(NO_CXX_MODULES)

// ==== Classic Geometric Cooling Simulated Annealing ====
// This is a simplified version of BasicSimulatedAnnealing,
// without complex callbacks and avoiding generic structures:
// - replacing GeneralEvaluator with Evaluator
// - replacing InitialSearch with Constructive

// C
#include <math.h>
// C++
#include <optional>
#include <string>
#include <utility>
#include <vector>
//

#include <OptFrame/Core/Constructive.hpp>
#include <OptFrame/Core/Evaluator.hpp>
#include <OptFrame/Core/NS.hpp>
#include <OptFrame/Core/RandGen.hpp>
#include <OptFrame/Search/SingleObjSearch.hpp>

#include "./HelperSA.hpp"
#include "./SA.hpp"

#define MOD_EXPORT
#else

// CANNOT IMPORT HERE... Already part of optframe.core
/*
import std;
import optframe.component;
import optframe.concepts;
*/

// do NOT export modules on .hpp... only on .cppm

#define MOD_EXPORT export

#endif

namespace optframe {

MOD_EXPORT template <XESSolution XES>
class GeometricSimulatedAnnealing : public SingleObjSearch<XES>, public SA {
 public:
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;
  using XSH = XES;
  //
  sref<Evaluator<S, XEv, XES>> evaluator;
  sref<Constructive<S>> constructive;
  sref<NS<XES>> ns;
  double alpha;
  int SAmax;
  double Ti;
  sref<RandGen> rg;

  GeometricSimulatedAnnealing(sref<Evaluator<S, XEv, XES>> _evaluator,
                              sref<Constructive<S>> _constructive,
                              sref<NS<XES>> _ns, double _alpha, int _SAmax,
                              double _Ti, sref<RandGen> _rg = new RandGen)
      : evaluator{_evaluator},
        constructive{_constructive},
        ns{_ns},
        alpha{_alpha},
        SAmax{_SAmax},
        Ti{_Ti},
        rg{_rg} {}

  virtual ~GeometricSimulatedAnnealing() = default;

  SearchOutput<XES, XSH> searchBy(const StopCriteria<XEv>& stop,
                                  std::optional<XSH> _best) override {
    if (Component::information)
      std::cout << "GeometricSA search(" << stop.remTime() << ")" << std::endl;

    op<S> s0 = _best ? std::make_optional<S>(std::move(_best->first))
                     : constructive->generateSolution(stop.remTime());
    if (!s0) return SearchStatus::NO_SOLUTION;  // cannot continue
    XEv eStar = _best ? std::move(_best->second) : evaluator->evaluate(*s0);
    //
    XES star{std::move(*s0), std::move(eStar)};
    XES se = star;  // copy star to incument solution
    //
    double T = Ti;

    while (T >= 0.0001) {
      for (int64_t iterT = 0; iterT < SAmax; iterT++) {
        if (stop.shouldStop(star.second)) {
          // return best solution so far
          return {SearchStatus::NO_REPORT, star};
        }
        uptr<Move<XES, XSH>> move = ns->validRandomMove(se);

        if (!move) {
          if (Component::warning)
            std::cout << "SA warning: no move in iter=" << iterT << " T=" << T
                      << "!"
                      << "cannot continue..." << std::endl;
          // This is not normal... but not catastrophic stop either.
          return {SearchStatus::EARLY_STOP, star};
        }

        auto eOld = se.second;  // copy evaluation
        auto mRev = move->applyUpdate(se);
        evaluator->reevaluate(se);

        assert(!se.second.isOutdated());  // CXX CONTRACT C++26
        if (evaluator->betterStrict(se.second, eOld)) {
          // ok, 'se' is current already
          if (evaluator->betterStrict(se.second, star.second)) {
            star = se;  // copy

            if (Component::information) {
              std::cout << "Best fo: " << se.second.evaluation()
                        << " Found on Iter = " << iterT << " and T = " << T;
              std::cout << std::endl;
            }
          }
        } else {
          // 'se' didn't improve, but may accept it anyway
          double x = rg->rand01();
          double delta = ::fabs(se.second.evaluation() - eOld.evaluation());

          if (x < ::exp(-delta / T)) {
            // ok, accepted already
          } else {
            // revert!
            mRev->applyUpdate(se);
            se.second = eOld;
          }
        }
      }  // end-for: iterT
      // geometric cooling
      T = alpha * T;
    }  // end-while: T
    // return best solution so far
    return {SearchStatus::NO_REPORT, star};
  }

  // =======================================
  //               Component
  // =======================================
  // set verbose level recursive: returns 'false' if not supported.
  bool setMessageLevelR(LogLevel ll) override {
    this->setMessageLevel(ll);
    //
    evaluator->setMessageLevel(ll);
    constructive->setMessageLevel(ll);
    ns->setMessageLevel(ll);
    return true;
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (SingleObjSearch<XES>::compatible(s));
  }

  std::string id() const override { return idComponent(); }

  static std::string idComponent() {
    std::stringstream ss;
    ss << SingleObjSearch<XES>::idComponent() << ":" << SA::family()
       << "GeometricSA";
    return ss.str();
  }
};

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
MOD_EXPORT template <XESolution XES, XESolution XES2,
                     X2ESolution<XES2> X2ES = MultiESolution<XES2>>
#else
MOD_EXPORT template <typename XES, typename XES2,
                     typename X2ES = MultiESolution<XES2>>
#endif
class GeometricSimulatedAnnealingBuilder : public GlobalSearchBuilder<XES>,
                                           public SA {
  // using XM = BasicSimulatedAnnealing<S, XEv, pair<S, XEv>, Component>;
  // using XM = Component; // only general builds here
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;
  using XSH = XES;  // primary-based search type only (BestType)

 public:
  virtual ~GeometricSimulatedAnnealingBuilder() {}

  // has sptr instead of sref, is that on purpose or legacy class?
  GlobalSearch<XES>* build(Scanner& scanner, HeuristicFactory<XES>& hf,
                           std::string family = "") override {
    if (Component::debug)
      std::cout << "GeometricSA Builder Loading Parameter #0" << std::endl;
    if (!scanner.hasNext()) {
      if (Component::warning)
        std::cout << "no next1a! aborting..." << std::endl;
      return nullptr;
    }

    sptr<Evaluator<S, XEv, XES>> eval;
    std::string sid_0 = scanner.next();
    int id_0 = *scanner.nextInt();
    hf.assign(eval, id_0, sid_0);
    assert(eval);

    if (Component::debug)
      std::cout << "GeometricSA Builder Loading Parameter #2" << std::endl;
    if (!scanner.hasNext()) {
      if (Component::warning)
        std::cout << "no next1b! aborting..." << std::endl;
      return nullptr;
    }

    sptr<Constructive<S>> constructive;
    std::string sid_1 = scanner.next();
    int id_1 = *scanner.nextInt();
    hf.assign(constructive, id_1, sid_1);
    assert(constructive);

    if (Component::debug)
      std::cout << "GeometricSA Builder Loading Parameter #3" << std::endl;
    if (!scanner.hasNext()) {
      if (Component::warning)
        std::cout << "no next1c! aborting..." << std::endl;
      return nullptr;
    }

    sptr<NS<XES, XSH>> ns;
    std::string sid_2 = scanner.next();
    int id_2 = *scanner.nextInt();
    hf.assign(ns, id_2, sid_2);
    assert(ns);

    vsptr<NS<XES, XSH>> _hlist;

    if (Component::debug)
      std::cout << "GeometricSA Builder Loading Parameter #4" << std::endl;
    if (!scanner.hasNext()) {
      if (Component::debug)
        std::cout << "no next alpha! aborting..." << std::endl;
      return nullptr;
    }
    double alpha = *scanner.nextDouble();

    if (Component::debug)
      std::cout << "GeometricSA Builder Loading Parameter #5" << std::endl;
    if (!scanner.hasNext()) {
      if (Component::warning)
        std::cout << "no next SAmax! aborting..." << std::endl;
      return nullptr;
    }
    int SAmax = *scanner.nextInt();

    if (Component::debug)
      std::cout << "GeometricSA Builder Loading Parameter #6" << std::endl;
    if (!scanner.hasNext()) {
      if (Component::warning)
        std::cout << "no next Ti! aborting..." << std::endl;
      return nullptr;
    }
    double Ti = *scanner.nextDouble();

    if (Component::debug) {
      std::cout << "GeometricSA Builder: got all parameters!" << std::endl;
      std::cout << "GeometricSimulatedAnnealing with:" << std::endl;
      std::cout << "\teval=" << eval->id() << std::endl;
      std::cout << "\tconstructive=" << constructive->id() << std::endl;
      std::cout << "\tns=" << ns->id() << std::endl;
      std::cout << "\talpha=" << alpha << std::endl;
      std::cout << "\tSAmax=" << SAmax << std::endl;
      std::cout << "\tTi=" << Ti << std::endl;
    }

    // NOLINTNEXTLINE
    return new GeometricSimulatedAnnealing<XES>(eval, constructive, ns, alpha,
                                                SAmax, Ti, hf.getRandGen());
  }

  std::vector<std::pair<std::string, std::string>> parameters() override {
    std::vector<std::pair<std::string, std::string>> params;
    // params.push_back(std::make_pair(GeneralEvaluator<XES>::idComponent(),
    // "evaluation function"));
    params.push_back(
        std::make_pair(Evaluator<typename XES::first_type,
                                 typename XES::second_type, XES>::idComponent(),
                       "evaluation function"));

    params.push_back(std::make_pair(Constructive<S>::idComponent(),
                                    "constructive heuristic"));
    params.push_back(std::make_pair(NS<XES, XSH>::idComponent(), "NS"));
    params.push_back(std::make_pair("OptFrame:double", "cooling factor"));
    params.push_back(std::make_pair(
        "OptFrame:int", "number of iterations for each temperature"));
    params.push_back(std::make_pair("OptFrame:double", "initial temperature"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == GeometricSimulatedAnnealing<XES>::idComponent();
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << GlobalSearchBuilder<XES>::idComponent() << SA::family()
       << "GeometricSA";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

/*

 Procedure originally proposed by Kirkpatrick et al.

 S. Kirkpatrick, D.C. Gellat, and M.P. Vecchi. Optimization by Simulated
 Annealing. Science, 220:671–680, 1983.

 LATEX:

 */

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_SA_GEOMETRICSIMULATEDANNEALING_HPP_
