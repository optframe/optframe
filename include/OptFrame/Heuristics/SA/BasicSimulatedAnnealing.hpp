// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_SA_BASICSIMULATEDANNEALING_HPP_  // NOLINT
#define OPTFRAME_HEURISTICS_SA_BASICSIMULATEDANNEALING_HPP_  // NOLINT

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

// C
#include <math.h>
// C++
#include <string>
#include <utility>
#include <vector>
//

#include <OptFrame/Core/NS.hpp>
#include <OptFrame/Core/RandGen.hpp>
#include <OptFrame/Search/ILoop.hpp>
#include <OptFrame/Search/ITrajectory.hpp>
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

// do NOT import/export modules on .hpp... only on .cppm

#define MOD_EXPORT export

#endif

namespace optframe {

// forward declaration
MOD_EXPORT template <XESSolution XES>
class BasicSimulatedAnnealing;

MOD_EXPORT template <XESSolution XES>
struct SearchContextSA {
  BasicSimulatedAnnealing<XES>& self;
  std::optional<XES>& best;
  std::optional<XES>& incumbent;
  //
  double T;
  int iterT;
};

MOD_EXPORT template <XESSolution XES>
class BasicSimulatedAnnealing : public SingleObjSearch<XES>,
                                public SA,
                                public ILoop<SearchContextSA<XES>, XES>,
                                public ITrajectory<XES> {
 public:
  using XEv = typename XES::second_type;
  using XSH = XES;  // XSearch
  //
  sref<GeneralEvaluator<XES>> evaluator;
  sref<InitialSearch<XES>> constructive;
  vsref<NS<XES, XSH>> neighbors;
  sref<RandGen> rg;
  double alpha;
  int SAmax;
  double Ti;

  // single neighborhood
  // reordered the different term to the front because of peeve, heheheh
  // standart RandGen in declaration could be new standart
  BasicSimulatedAnnealing(sref<GeneralEvaluator<XES>> _evaluator,
                          sref<InitialSearch<XES>> _constructive,
                          sref<NS<XES, XSH>> _neighbors, double _alpha,
                          int _SAmax, double _Ti,
                          sref<RandGen> _rg = new RandGen)
      : evaluator(_evaluator), constructive(_constructive), rg(_rg) {
    neighbors.push_back(_neighbors);
    alpha = (_alpha);
    SAmax = (_SAmax);
    Ti = (_Ti);
  }

  // vector of neighborhoods
  // reordered the different term to the front because of peeve, heheheh
  // standart RandGen in declaration could be new standart
  BasicSimulatedAnnealing(sref<GeneralEvaluator<XES>> _evaluator,
                          sref<InitialSearch<XES>> _constructive,
                          vsref<NS<XES, XSH>> _neighbors, double _alpha,
                          int _SAmax, double _Ti,
                          sref<RandGen> _rg = new RandGen)
      : evaluator(_evaluator),
        constructive(_constructive),
        neighbors(_neighbors),
        rg(_rg) {
    alpha = (_alpha);
    SAmax = (_SAmax);
    Ti = (_Ti);
  }

  virtual ~BasicSimulatedAnnealing() = default;

  // callback to handle main loop and stop criteria
  bool (*onLoopCtx)(const SearchContextSA<XES>& ctx,
                    const StopCriteria<XEv>& sosc) = [](auto& ctx, auto& sosc) {
    return (ctx.T >= 0.000001) && !sosc.shouldStop(ctx.best->second);
  };

  // callback to update local variables
  void (*onBeforeLoopCtx)(SearchContextSA<XES>& ctx) = [](auto& ctx) {
    if (ctx.iterT < ctx.self.SAmax) {
      ctx.iterT++;
    } else {
      ctx.iterT = 0;
      ctx.T = ctx.self.alpha * ctx.T;  // geometric cooling
    }
  };

  // GlobalSearch
  bool (*onBestCtx)(SearchContextSA<XSH>& ctx, const XSH& best) =
      [](SearchContextSA<XSH>& ctx, const XSH& best) {
        return ctx.self.onBest(ctx.self, best);
      };

  // ITrajectory
  bool (*onIncumbentCtx)(SearchContextSA<XSH>& ctx, const XSH& inc) =
      [](SearchContextSA<XSH>& ctx, const XSH& inc) {
        return ctx.self.onIncumbent(ctx.self, inc);
      };

  // ILoop
  bool onLoop(const SearchContextSA<XES>& ctx,
              const StopCriteria<XEv>& sosc) override {
    return onLoopCtx(ctx, sosc);
  }

  // ILoop
  void onBeforeLoop(SearchContextSA<XES>& ctx) override {
    onBeforeLoopCtx(ctx);
  }

  // ITrajectory
  SearchOutput<XES, XSH> searchBy(const StopCriteria<XEv>& sosc,
                                  std::optional<XSH> _best) override {
    if (Component::information)
      std::cout << "SA search(" << sosc.timelimit << ")" << std::endl;

    std::optional<XSH> star = _best;

    // -------------------------------------------------------------------
    // assuming trajectory implementation (XES = XSH = XBest = XIncumbent)
    // -------------------------------------------------------------------
    if (Component::verbose)
      std::cout << "SA: will build initialSearch" << std::endl;

    // disable 'constructive' if star is given
    std::optional<XSH> incumbent =
        star ? star : constructive->initialSearch(sosc).first;

    if (Component::verbose)
      std::cout << "SA: post build initialSearch" << std::endl;

    if (Component::verbose) {
      std::cout << "SA begin: incumbent? " << (bool)incumbent << std::endl;
      std::cout << "SA begin: star? " << (bool)star << std::endl;
    }

    // if no star and has incumbent, star is incumbent
    if (!star && incumbent) star = incumbent;

    if (Component::verbose) {
      std::cout << "SA begin: incumbent? " << (bool)incumbent << std::endl;
      std::cout << "SA begin: star? " << (bool)star << std::endl;
    }

    // abort if no star exists
    if (!star) return SearchStatus::NO_SOLUTION;  // cannot continue!

    if (Component::verbose) {
      std::cout << "star value: " << star->second.evaluation() << std::endl;
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
      if constexpr (XOStreamable<XES>)
        std::cout << "star: " << star->first << std::endl;
#endif
    }

    // initialize search context for Simulated Annealing
    SearchContextSA<XES> ctx{*this, star, incumbent};

    if (Component::verbose) std::cout << "SA: begin SearchContext" << std::endl;

    // ===================
    // Simulated Annealing
    // ===================

    if (Component::verbose)
      std::cout << "incumbent? " << (bool)incumbent << std::endl;

    XSH& se = *incumbent;
    this->onIncumbentCtx(ctx, *incumbent);
    this->onBestCtx(ctx, *star);

    ctx.T = Ti;
    ctx.iterT = 0;

    if (Component::verbose)
      std::cout << "SA(verbose): BEGIN se: " << se << std::endl;

    assert(!se.second.isOutdated());  // CXX CONTRACT C++26

    while (onLoop(ctx, sosc)) {
      if (Component::verbose)
        std::cout << "SA(verbose): after onLoop" << std::endl;
      int n = rg->rand(neighbors.size());

      uptr<Move<XES, XSH>> move = neighbors[n]->validRandomMove(se);

      if (!move) {
        if (Component::warning)
          std::cout << "SA warning: no move in iter=" << ctx.iterT
                    << " T=" << ctx.T << "! cannot continue..." << std::endl;
        // This is not normal... but not catastrophic stop either.
        return {SearchStatus::EARLY_STOP, star};
      }

      if (Component::verbose)
        std::cout << "SA(verbose): will copy to current=se: " << se
                  << std::endl;
      // copy solution
      XES current(se);

      if (Component::verbose)
        std::cout << "SA(verbose): current: " << current << std::endl;

      if (Component::verbose)
        std::cout << "SA(verbose): will apply move. current=" << current
                  << std::endl;

      move->applyUpdate(current);

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
      if constexpr (XOStreamable<XES>)
        if (Component::verbose)
          std::cout << "SA(verbose): will reevaluate. current.first="
                    << current.first << std::endl;
#endif

      evaluator->reevaluate(current);

      assert(!current.second.isOutdated());  // CXX CONTRACT C++26

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
      if constexpr (XOStreamable<XES>)
        if (Component::verbose)
          std::cout << "SA(verbose): after reevaluate. current=" << current
                    << std::endl;
#endif

      if (Component::verbose)
        std::cout << "SA(verbose): will compare betterStrict" << std::endl;

      // if (evaluator.betterThan(eCurrent, e)) // TODO: replace by 'se' here,
      // and use 'se.second' to compare directly if(eCurrent.betterStrict(e))
      if (evaluator->betterStrict(current.second, se.second)) {
        se = current;
        this->onIncumbentCtx(ctx, se);

        if (evaluator->betterStrict(se.second, star->second)) {
          star = make_optional(se);

          if (Component::information) {
            std::cout << "Best fo: " << se.second.evaluation()
                      << " Found on Iter = " << ctx.iterT
                      << " and T = " << ctx.T;
            std::cout << std::endl;
            this->onBestCtx(ctx, *star);
          }
        }
      } else {
        // 'current' didn't improve, but may accept it anyway
        double x = rg->rand01();
        double delta =
            std::fabs(current.second.evaluation() - se.second.evaluation());

        if (x < std::exp(-delta / ctx.T)) {
          se = current;
          this->onIncumbentCtx(ctx, se);
        }
      }

      // onBeforeLoop: update iterT and temperature in next method
      onBeforeLoop(ctx);
    }
    if (Component::debug) {
      std::cout << "T=" << ctx.T << std::endl;
    }

    // return best solution
    return {SearchStatus::NO_REPORT, star};
  }

  // =======================================
  //               Component
  // =======================================
  // set verbose level recursive: returns 'false' if not supported.
  bool setMessageLevelR(modlog::LogLevel ll) override {
    this->setMessageLevel(ll);
    //
    // evaluator ?? how to do this on GeneralEvaluator...
    constructive->setMessageLevel(ll);
    for (unsigned i = 0; i < neighbors.size(); i++)
      neighbors[i]->setMessageLevel(ll);
    return true;
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (SingleObjSearch<XES>::compatible(s));
  }

  std::string id() const override { return idComponent(); }

  static std::string idComponent() {
    std::stringstream ss;
    ss << SingleObjSearch<XES>::idComponent() << ":" << SA::family()
       << "BasicSA";
    return ss.str();
  }
};

/*

 Procedure originally proposed by Kirkpatrick et al.

 S. Kirkpatrick, D.C. Gellat, and M.P. Vecchi. Optimization by Simulated
 Annealing. Science, 220:671–680, 1983.

 LATEX:

 */

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_SA_BASICSIMULATEDANNEALING_HPP_  // NOLINT
