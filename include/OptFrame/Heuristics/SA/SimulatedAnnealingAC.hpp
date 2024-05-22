// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_SA_SIMULATEDANNEALINGAC_HPP_  // NOLINT
#define OPTFRAME_HEURISTICS_SA_SIMULATEDANNEALINGAC_HPP_  // NOLINT

// C
#include <math.h>
// C++
#include <string>
#include <utility>
#include <vector>
//

#include "../../ILoop.hpp"
#include "../../ITrajectory.hpp"
#include "../../NS.hpp"
#include "../../RandGen.hpp"
#include "../../SingleObjSearch.hpp"
#include "./HelperSA.hpp"
#include "./SA.hpp"

namespace optframe {

// forward declaration
template <XESolution XES>
class SimulatedAnnealingAC;

template <XESolution XES>
struct SearchContextSA_AC {
  SimulatedAnnealingAC<XES>& self;
  std::optional<XES>& best;
  std::optional<XES>& incumbent;
  //
  double T;
  int iterT;
};

template <XESolution XES>
class SimulatedAnnealingAC : public SingleObjSearch<XES>,
                             public SA,
                             public ILoop<SearchContextSA_AC<XES>, XES>,
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

  SimulatedAnnealingAC(sref<GeneralEvaluator<XES>> _evaluator,
                       sref<InitialSearch<XES>> _constructive,
                       vsref<NS<XES, XSH>> _neighbors, double _alpha,
                       int _SAmax, double _Ti, sref<RandGen> _rg = new RandGen)
      : evaluator(_evaluator),
        constructive(_constructive),
        neighbors(_neighbors),
        rg(_rg) {
    alpha = (_alpha);
    SAmax = (_SAmax);
    Ti = (_Ti);
  }

  virtual ~SimulatedAnnealingAC() = default;

  // callback to handle main loop and stop criteria
  bool (*onLoopCtx)(const SearchContextSA_AC<XES>& ctx,
                    const StopCriteria<XEv>& sosc) = [](auto& ctx, auto& sosc) {
    return (ctx.T >= 0.000001) && !sosc.shouldStop(ctx.best->second);
  };

  // callback to update local variables
  void (*onBeforeLoopCtx)(SearchContextSA_AC<XES>& ctx) = [](auto& ctx) {
    if (ctx.iterT < ctx.self.SAmax) {
      ctx.iterT++;
    } else {
      ctx.iterT = 0;
      ctx.T = ctx.self.alpha * ctx.T;  // geometric cooling
    }
  };

  // ILoop
  bool onLoop(const SearchContextSA_AC<XES>& ctx,
              const StopCriteria<XEv>& sosc) override {
    return onLoopCtx(ctx, sosc);
  }

  // ILoop
  void onBeforeLoop(SearchContextSA_AC<XES>& ctx) override {
    onBeforeLoopCtx(ctx);
  }

  // ITrajectory
  SearchOutput<XES, XSH> searchBy(const StopCriteria<XEv>& sosc,
                                  std::optional<XSH> _best) override {
    if (Component::information)
      std::cout << "SA search(" << sosc.timelimit << ")" << std::endl;

    std::optional<XSH> star = _best;
    // assuming trajectory implementation (XES = XSH = XBest = XIncumbent)
    std::optional<XSH> incumbent = star;

    if (Component::verbose)
      std::cout << "SA: will build initialSearch" << std::endl;

    if (!incumbent)
      incumbent = star ? star : constructive->initialSearch(sosc).first;

    if (Component::verbose)
      std::cout << "SA: post build initialSearch" << std::endl;

#ifdef OPTFRAME_AC
    // requires first part of solution to be a component
    static_assert(
        XRSolution<typename XSH::first_type, typename XSH::first_type::typeR>);
    {
      std::stringstream ss;
      ss << incumbent->second.evaluation();
      incumbent->first.listAC.push_back(
          ContextAC{.id{"SA_INIT"}, .message{ss.str()}, .payload {
                      incumbent->first.sharedClone()
                    }});
    }
#endif

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

#ifdef OPTFRAME_AC
    // requires first part of solution to be a component
    static_assert(
        XRSolution<typename XSH::first_type, typename XSH::first_type::typeR>);
    {
      std::stringstream ss;
      ss << star->second.evaluation();
      star->first.listAC.push_back(
          ContextAC{.id{"SA_NEW_STAR"}, .message{ss.str()}, .payload {
                      star->first.sharedClone()
                    }});
    }
    //
    // star->first.printListAC();
#endif

    // abort if no solution exists
    if (!star) return SearchStatus::NO_SOLUTION;  // no possibility to continue.

    if (Component::verbose) {
      std::cout << "star value: " << star->second.evaluation() << std::endl;
      std::cout << "star: " << star->first << std::endl;
    }

    // initialize search context for Simulated Annealing
    SearchContextSA_AC<XES> ctx{*this, star, incumbent};

    if (Component::verbose) std::cout << "SA: begin SearchContext" << std::endl;

    // ===================
    // Simulated Annealing
    // ===================

    if (Component::verbose)
      std::cout << "incumbent? " << (bool)incumbent << std::endl;

    XSH& se = *incumbent;

    ctx.T = Ti;
    ctx.iterT = 0;

    if (Component::verbose)
      std::cout << "SA(verbose): BEGIN se: " << se << std::endl;
    // SANITY CHECK
    assert(!se.second.isOutdated());

    while (onLoop(ctx, sosc)) {
      if (Component::verbose)
        std::cout << "SA(verbose): after onLoop" << std::endl;
      int n = rg->rand(neighbors.size());

      uptr<Move<XES, XSH>> move = neighbors[n]->validRandomMove(
          se);  // TODO: pass 'se.first' here (even 'se' should also work...)

      if (!move) {
        if (Component::warning)
          cout << "SA warning: no move in iter=" << ctx.iterT << " T=" << ctx.T
               << "! continue..." << endl;
        // TODO: return FAIL?
        // continue;
        return {SearchStatus::NO_REPORT, star};
      }

      if (Component::verbose)
        std::cout << "SA(verbose): will copy to current=se: " << se
                  << std::endl;
      XES current(se);  // implicit clone??

      if (Component::verbose)
        std::cout << "SA(verbose): current: " << current << std::endl;

      /*
#ifdef OPTFRAME_AC
         std::cout << "begin current: " << std::endl;
         current.first.printListAC();
         assert(current.first.hasInListAC("SA_INIT"));
         std::cout << "begin se: " << std::endl;
         se.first.printListAC();
         assert(se.first.hasInListAC("SA_INIT"));
         std::cout << std::endl;
#endif
*/
      // S* sCurrent = &s.clone();
      // Evaluation<>* eCurrent = &e.clone();
      // S& sCurrent = current.first;
      // XEv& eCurrent = current.second;

      if (Component::verbose)
        std::cout << "SA(verbose): will apply move. current=" << current
                  << std::endl;

      move->applyUpdate(current);

      if (Component::verbose)
        std::cout << "SA(verbose): will reevaluate. current.first="
                  << current.first << std::endl;

      evaluator->reevaluate(current);

      // SANITY CHECK
      assert(!current.second.isOutdated());

      if (Component::verbose)
        std::cout << "SA(verbose): after reevaluate. current=" << current
                  << std::endl;

      /*
#ifdef OPTFRAME_AC
         // requires first part of solution to be a component
         static_assert(XRSolution<typename XSH::first_type, typename
XSH::first_type::typeR>);
         {
            std::stringstream ss;
            ss << move->toString();
            current.first.listAC.push_back(ContextAC{ .id{ "SA_MOVE_APPLY" },
.message{ ss.str() }, .payload{ move->sharedClone() } });
         }
         //
         //star->first.printListAC();
#endif
*/

      if (Component::verbose)
        std::cout << "SA(verbose): will compare betterStrict" << std::endl;

      // if (evaluator.betterThan(eCurrent, e)) // TODO: replace by 'se' here,
      // and use 'se.second' to compare directly if(eCurrent.betterStrict(e))
      if (evaluator->betterStrict(current.second, se.second)) {
        se = current;

// CONSIDER THAT MOVE IS PERFORMED HERE!!
#ifdef OPTFRAME_AC
        // requires first part of solution to be a component
        static_assert(XRSolution<typename XSH::first_type,
                                 typename XSH::first_type::typeR>);
        {
          std::stringstream ss;
          ss << move->toString();
          se.first.listAC.push_back(ContextAC{
              .id{"SA_MOVE_APPLY_GOOD"}, .message{ss.str()}, .payload {
                move->sharedClone()
              }});
        }
        //
        // star->first.printListAC();
#endif

        /*
#ifdef OPTFRAME_AC
            std::cout << "current: " << std::endl;
            current.first.printListAC();
            assert(current.first.hasInListAC("SA_MOVE_APPLY"));
            std::cout << "se: " << std::endl;
            se.first.printListAC();
            assert(se.first.hasInListAC("SA_MOVE_APPLY"));
            std::cout << std::endl;
#endif
*/
        // if improved, accept it
        // e = *eCurrent;
        // s = *sCurrent;
        // delete sCurrent;
        // delete eCurrent;

        // if (evaluator.betterThan(e, eStar))
        // if(e.betterStrict(eStar))
        if (evaluator->betterStrict(se.second, star->second)) {
          // delete sStar;
          // sStar = &s.clone();
          // delete eStar;
          // eStar = &e.clone();
          star = make_optional(se);
          /*
#ifdef OPTFRAME_AC
               std::cout << "se: " << std::endl;
               se.first.printListAC();
               assert(se.first.hasInListAC("SA_MOVE_APPLY"));
               std::cout << "star: " << std::endl;
               star->first.printListAC();
               //assert(star->first.hasInListAC("SA_MOVE_APPLY"));
               assert(star->first.listAC.size() == se.first.listAC.size());
#endif
   */
          if (Component::information) {
            std::cout << "Best fo: " << se.second.evaluation()
                      << " Found on Iter = " << ctx.iterT
                      << " and T = " << ctx.T;
            std::cout << endl;
          }

#ifdef OPTFRAME_AC
          // requires first part of solution to be a component
          static_assert(XRSolution<typename XSH::first_type,
                                   typename XSH::first_type::typeR>);
          {
            std::stringstream ss;
            ss << star->second.evaluation();
            star->first.listAC.push_back(
                ContextAC{.id{"SA_NEW_STAR"}, .message{ss.str()}, .payload {
                            star->first.sharedClone()
                          }});
          }
          //
          // star->first.printListAC();
#endif

          /*
#ifdef OPTFRAME_AC
               assert(se.first.hasInListAC("SA_MOVE_APPLY"));
               assert(star->first.hasInListAC("SA_MOVE_APPLY"));
#endif
*/
        }
      } else {
        // 'current' didn't improve, but may accept it anyway
        double x = rg->rand01();
        double delta =
            ::fabs(current.second.evaluation() - se.second.evaluation());

        if (x < ::exp(-delta / ctx.T)) {
          se = current;
          // s = *sCurrent;
          // e = *eCurrent;
          // delete sCurrent;
          // delete eCurrent;
          //  } else {
          // delete sCurrent;
          // delete eCurrent;

// CONSIDER THAT MOVE IS PERFORMED HERE!!
#ifdef OPTFRAME_AC
          // requires first part of solution to be a component
          static_assert(XRSolution<typename XSH::first_type,
                                   typename XSH::first_type::typeR>);
          {
            std::stringstream ss;
            ss << move->toString();
            se.first.listAC.push_back(ContextAC{
                .id{"SA_MOVE_APPLY_BAD"}, .message{ss.str()}, .payload {
                  move->sharedClone()
                }});
          }
          //
          // star->first.printListAC();
#endif
        }
      }

      // onBeforeLoop: update iterT and temperature in next method
      onBeforeLoop(ctx);
    }
    if (Component::debug) {
      std::cout << "T=" << ctx.T << std::endl;
    }

#ifdef OPTFRAME_AC
    // requires first part of solution to be a component
    static_assert(
        XRSolution<typename XSH::first_type, typename XSH::first_type::typeR>);
    {
      std::stringstream ss;
      ss << star->second.evaluation();
      star->first.listAC.push_back(
          ContextAC{.id{"SA_FINAL"}, .message{ss.str()}, .payload {
                      star->first.sharedClone()
                    }});
    }
    //
    // std::cout << "SA: final star" << std::endl;
    // star->first.printListAC();
#endif

    return {SearchStatus::NO_REPORT, star};
  }

  // =======================================
  //               Component
  // =======================================
  // set verbose level recursive: returns 'false' if not supported.
  bool setMessageLevelR(LogLevel ll) override {
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

  static string idComponent() {
    stringstream ss;
    ss << SingleObjSearch<XES>::idComponent() << ":" << SA::family()
       << "SimulatedAnnealingAC";
    return ss.str();
  }
};

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES, XESolution XES2,
          X2ESolution<XES2> X2ES = MultiESolution<XES2>>
#else
template <typename XES, XESolution XES2, typename X2ES = MultiESolution<XES2>>
#endif
class SimulatedAnnealingACBuilder : public SA, public GlobalSearchBuilder<XES> {
  // using XM = BasicSimulatedAnnealing<S, XEv, pair<S, XEv>, Component>;
  // using XM = Component; // only general builds here
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;
  using XSH = XES;  // primary-based search type only (BestType)

 public:
  virtual ~SimulatedAnnealingACBuilder() = default;

  // has sptr instead of sref, is that on purpose or legacy class?
  GlobalSearch<XES>* build(Scanner& scanner, HeuristicFactory<XES>& hf,
                           string family = "") override {
    if (Component::debug)
      std::cout << "BasicSA Builder Loading Parameter #0" << std::endl;
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
      std::cout << "BasicSA Builder Loading Parameter #2" << std::endl;
    if (!scanner.hasNext()) {
      if (Component::warning)
        std::cout << "no next1b! aborting..." << std::endl;
      return nullptr;
    }
    // Constructive<S>* constructive;
    sptr<InitialSearch<XES>> constructive;
    std::string sid_1 = scanner.next();
    int id_1 = *scanner.nextInt();
    hf.assign(constructive, id_1, sid_1);
    assert(constructive);

    // return nullptr;

    if (Component::debug)
      std::cout << "BasicSA Builder Loading Parameter #3" << std::endl;
    if (!scanner.hasNext()) {
      if (Component::warning)
        std::cout << "no next1c! aborting..." << std::endl;
      return nullptr;
    }
    vsptr<NS<XES, XSH>> _hlist;

    std::string sid_2 = scanner.next();
    int id_2 = *scanner.nextInt();
    hf.assignList(_hlist, id_2, sid_2);
    vsref<NS<XES, XSH>> hlist;
    for (sptr<NS<XES, XSH>> x : _hlist) {
      assert(x);
      hlist.push_back(x);
    }

    if (Component::debug) std::cout << "list ok!" << hlist.size() << std::endl;

    if (Component::debug)
      std::cout << "BasicSA Builder Loading Parameter #4" << std::endl;
    if (!scanner.hasNext()) {
      if (Component::debug)
        std::cout << "no next alpha! aborting..." << std::endl;
      return nullptr;
    }
    double alpha = *scanner.nextDouble();

    if (Component::debug)
      std::cout << "BasicSA Builder Loading Parameter #5" << std::endl;
    if (!scanner.hasNext()) {
      if (Component::warning)
        std::cout << "no next SAmax! aborting..." << std::endl;
      return nullptr;
    }
    int SAmax = *scanner.nextInt();

    if (Component::debug)
      std::cout << "BasicSA Builder Loading Parameter #6" << std::endl;
    if (!scanner.hasNext()) {
      if (Component::warning)
        std::cout << "no next Ti! aborting..." << std::endl;
      return nullptr;
    }
    double Ti = *scanner.nextDouble();

    if (Component::debug) {
      std::cout << "BasicSA Builder: got all parameters!" << std::endl;
      std::cout << "BasicSimulatedAnnealing with:" << std::endl;
      std::cout << "\teval=" << ge->id() << std::endl;
      std::cout << "\tconstructive=" << constructive->id() << std::endl;
      std::cout << "\t|hlist|=" << hlist.size() << std::endl;
      std::cout << "\thlist[0]=" << hlist[0]->id() << std::endl;
      std::cout << "\talpha=" << alpha << std::endl;
      std::cout << "\tSAmax=" << SAmax << std::endl;
      std::cout << "\tTi=" << Ti << std::endl;
    }

    return new SimulatedAnnealingAC<XES>(ge, constructive, hlist, alpha, SAmax,
                                         Ti, hf.getRandGen());
  }

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;
    // params.push_back(make_pair(GeneralEvaluator<XES>::idComponent(),
    // "evaluation function"));
    params.push_back(
        make_pair(Evaluator<typename XES::first_type, typename XES::second_type,
                            XES>::idComponent(),
                  "evaluation function"));
    //
    // params.push_back(make_pair(Constructive<S>::idComponent(), "constructive
    // heuristic"));
    params.push_back(
        make_pair(InitialSearch<XES>::idComponent(), "constructive heuristic"));
    stringstream ss;
    ss << NS<XES, XSH>::idComponent() << "[]";
    params.push_back(make_pair(ss.str(), "list of NS"));
    params.push_back(make_pair("OptFrame:double", "cooling factor"));
    params.push_back(
        make_pair("OptFrame:int", "number of iterations for each temperature"));
    params.push_back(make_pair("OptFrame:double", "initial temperature"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == SimulatedAnnealingAC<XES>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << GlobalSearchBuilder<XES>::idComponent() << SA::family()
       << "SimulatedAnnealingAC";
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

#endif  // OPTFRAME_HEURISTICS_SA_SIMULATEDANNEALINGAC_HPP_  // NOLINT
