// SPDX-License-Identifier:  MIT OR LGPL-3.0-or-later
// Copyright (C) 2007-2022 - OptFrme developers
// https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_TS_BASICTABUSEARCH_HPP_
#define OPTFRAME_HEURISTICS_TS_BASICTABUSEARCH_HPP_

// C++
#include <string>
#include <utility>
#include <vector>
//
#include "../../Evaluator.hpp"
#include "../../NSEnum.hpp"
#include "../../SingleObjSearch.hpp"
#include "TS.h"

namespace optframe {

template <XESolution XES>
class BasicTabuSearch : public SingleObjSearch<XES>, public TS {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;
  using XSH = XES;
  //
 public:
  sref<GeneralEvaluator<XES, XEv>> evaluator;
  sref<InitialSearch<XES>> constructive;
  sref<NSSeq<XES, XEv, XSH>> nsSeq;
  int tlSize;
  int tsMax;

 public:
  BasicTabuSearch(sref<GeneralEvaluator<XES, XEv>> _ev,
                  sref<InitialSearch<XES>> _constructive,
                  sref<NSSeq<XES, XEv, XSH>> _nsSeq, int _tlSize, int _tsMax)
      : evaluator(_ev),
        constructive(_constructive),
        nsSeq(_nsSeq),
        tlSize(_tlSize),
        tsMax(_tsMax) {}

  ~BasicTabuSearch() override = default;

  SearchOutput<XES, XSH> searchBy(const StopCriteria<XEv>& sosc,
                                  std::optional<XES> _best) override {
    if (Component::information) {
      std::cout << "BasicTabuSearch search(" << sosc.timelimit << ")"
                << std::endl;
    }

    std::optional<XSH> star;
    if (_best) star = std::move(_best);
    std::optional<XSH> incumbent = star;

    // Note that no comparison is necessarily made between star and incumbent,
    // as types/spaces could be different. Since we are trajectory here, spaces
    // are equal. We assume star is better than incumbent, if provided. So, if
    // star is worse than incumbent, we won't re-check that.

    // initial setup for incumbent (if has star, copy star, otherwise generate
    // one)
    if (!incumbent)
      incumbent = star ? star : constructive->initialSearch(sosc).first;

    // if no star and has incumbent, star is incumbent
    if (!star && incumbent) star = incumbent;

    // abort if no solution exists
    if (!star) return SearchStatus::NO_SOLUTION;  // no possibility to continue.

    // ===========
    // Tabu Search
    // ===========

    if (Component::information)
      std::cout << "BasicTabuSearch starts with f(s*)="
                << star->second.evaluation() << std::endl;

    XSH& se = *incumbent;

    // ---------------------
    // initialize iterations
    //
    int Iter = 0;
    int BestIter = 0;

    // initialize empty tabu list
    vector<Move<XES, XEv>*> tabuList;

    int estimative_BTmax = 0;

    // ============================================
    // initialize aspiration function A
    // idea: f(s') < A(f(s)), where A(f(s))=f(s*)
    //
    std::function<bool(const XEv&, const XEv&)> cmpA =
        [&star, this](const XEv& e_prime, const XEv&) {
          return this->evaluator->betterStrict(e_prime, star->second);
        };
    // ============================================

    if (Component::verbose) {
      std::cout << "BasicTabuSearch begins tsMax=" << tsMax << std::endl;
    }
    //
    while ((Iter - BestIter) <= tsMax && !sosc.shouldStop(star->second)) {
      //
      if (Component::verbose) {
        std::cout << "BasicTabuSearch (Iter=" << Iter
                  << " - BestIter=" << BestIter << ") <= tsMax = " << tsMax
                  << std::endl;
      }
      Iter = Iter + 1;

      if ((Iter - BestIter) > estimative_BTmax)
        estimative_BTmax = (Iter - BestIter);

      // find best non-tabu move OR
      // best move with aspiration

      uptr<Move<XES, XEv>> bestMove =
          tabuBestMoveWithAspiration(se, tabuList, cmpA);
      if (!bestMove) return SearchStatus::FAILED;

      // =====================================================
      //        's' <- 's1';
      // =====================================================
      // apply move and store new tabu move
      uptr<Move<XES, XEv>> newTabu = bestMove->applyUpdate(se);
      evaluator->reevaluate(se);

      // =======================================
      //  update tabu list: ensure |T| <= tsSize
      // =======================================
      tabuList.push_back(newTabu.release());

      if (((int)tabuList.size()) > tlSize) {
        // do not 'delete' here
        uptr<Move<XES, XEv>> firstMove{tabuList[0]};
        tabuList.erase(tabuList.begin());
        firstMove = nullptr;
      }

      // ===============================
      //       's' better than 's*'?
      // ===============================
      //
      if (evaluator->betterStrict(se.second, star->second)) {
        // Better than global
        (*star) = se;
        BestIter = Iter;
        if (Component::information) {
          std::cout << "BasicTabuSearch BestIter=" << Iter
                    << " f(s*)=" << star->second.evaluation() << std::endl;
        }
        // Can remove from tabu list
        for (unsigned int i = 0; i < tabuList.size(); i++)
          if ((*tabuList[i]) == (*bestMove)) {
            // let this move expire (no 'delete' here)
            uptr<Move<XES, XEv>> sameMove{tabuList[i]};
            tabuList.erase(tabuList.begin() + i);
            sameMove = nullptr;
            //
            break;
          }
      }
      // next iteration
    }

    // ===============
    // clear tabu list
    // ===============

    while (tabuList.size() > 0) {
      // do not 'delete' here
      uptr<Move<XES, XEv>> firstMove{tabuList[0]};
      firstMove = nullptr;
      //
      tabuList.erase(tabuList.begin());
    }

    // ===========
    // return 's*'
    // ===========
    //
    return {SearchStatus::NO_REPORT, star};
  }

  // =====================================
  // get best move (even if non-improving)
  // move should not offend tabu list OR
  // should satisfy aspiration function
  // =====================================

  uptr<Move<XES, XEv>> tabuBestMoveWithAspiration(
      XES& se, const vector<Move<XES, XEv>*>& tabuList,
      std::function<bool(const XEv&, const XEv&)> cmp) {
    // auto& s = se.first;
    //
    uptr<Move<XES, XEv>> bestMove = nullptr;
    XEv bestCost;  // TODO: initialize? how?
    //

    // initialize iterator
    uptr<NSIterator<XES, XEv>> it = nsSeq->getIterator(se);
    // find first move
    it->first();
    if (it->isDone()) {
      return bestMove;
    }
    uptr<Move<XES, XEv>> current = it->current();

    // while current move exists
    while (current) {
      // check if move is good
      if (!current->canBeApplied(se)) {
        it->next();
        if (!it->isDone())
          current = it->current();
        else
          current = nullptr;
        // try again...
        continue;
      }
      // compute move cost
      XEv currentCost = evaluator->moveCost(*current, se);
      if (Component::verbose) {
        std::cout << "currentCost1:" << currentCost.evaluation() << " move:";
        current->print();
      }
      // check if it's improving best
      bool improvingBest =
          (!bestMove) || evaluator->betterStrict(currentCost, bestCost);
      // if not improving best, try next one..
      if (!improvingBest) {
        it->next();
        if (!it->isDone())
          current = it->current();
        else
          current = nullptr;
        // try again...
        continue;
      }
      // =================================
      // okay, we have an improvement here
      // must check if it's not tabu...
      // =================================
      //
      // assuming aspiration function is cheaper!
      //

      XEv e_prime(se.second);
      currentCost.update(e_prime);  // e' := e + currentCost
      if (Component::verbose) {
        std::cout << "currentCost2:" << currentCost.evaluation() << " move:";
        current->print();
      }

      //
      // check aspiration (s', s)
      if (cmp(e_prime, se.second)) {
        // OKAY, it satisfies aspiration!
        // let's accept it directly (no need to check tabu list)
        bestMove = std::move(current);
        bestCost = std::move(currentCost);
        //
        current = nullptr;

      } else {
        //
        // finally... it's improvement, but not satisfying aspiration.
        //
        // check if it's in tabu list
        bool isTabu = inList(current, tabuList);
        if (isTabu && Component::verbose) {
          std::cout << "isTabu = true! ";
          current->print();
        }

        // if non-tabu, accept it!
        if (!isTabu) {
          bestMove = std::move(current);
          bestCost = std::move(currentCost);
          //
          current = nullptr;
        }
      }
      // get next move
      it->next();
      if (!it->isDone())
        current = it->current();
      else
        current = nullptr;
      // try again...
    }

    if (Component::verbose) {
      std::cout << "bestMove = ";
      if (bestMove)
        bestMove->print();
      else
        std::cout << "nullptr!" << std::endl;
    }

    return bestMove;
  }

  bool inList(uptr<Move<XES, XEv>>& m, const vector<Move<XES, XEv>*>& v) {
    for (unsigned int i = 0; i < v.size(); i++)
      if ((*m) == (*v[i])) return true;
    return false;
  }

  static string idComponent() {
    stringstream ss;
    ss << SingleObjSearch<XES>::idComponent() << "TS:BasicTabuSearch";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

template <XSolution S, XEvaluation XEv = Evaluation<>,
          XESolution XES = pair<S, XEv>,
          X2ESolution<XES> X2ES = MultiESolution<XES>,
          XSearch<XES> XSH = std::pair<S, XEv>>
class BasicTabuSearchBuilder : public TS,
                               public SingleObjSearchBuilder<S, XEv, XES> {
 public:
  ~BasicTabuSearchBuilder() override = default;

  // NOLINTNEXTLINE
  SingleObjSearch<XES>* build(Scanner& scanner,
                              HeuristicFactory<S, XEv, XES, X2ES>& hf,
                              string family = "") override {
    Evaluator<XES, XEv>* eval;
    hf.assign(eval, *scanner.nextInt(), scanner.next());  // reads backwards!

    Constructive<S>* constructive;
    hf.assign(constructive, *scanner.nextInt(),
              scanner.next());  // reads backwards!

    NSSeq<XES, XEv, XSH>* nsseq;
    hf.assign(nsseq, *scanner.nextInt(), scanner.next());  // reads backwards!

    if (!scanner.hasNext()) return nullptr;

    int tl = *scanner.nextInt();

    if (!scanner.hasNext()) return nullptr;

    int tsMax = *scanner.nextInt();

    // NOLINTNEXTLINE
    return new BasicTabuSearch<XES>(*eval, *constructive, *nsseq, tl, tsMax);
  }

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(
        make_pair(Evaluator<XES, XEv>::idComponent(), "evaluation function"));
    // params.push_back(make_pair(Constructive<S>::idComponent(),
    // "constructive heuristic"));
    params.push_back(
        make_pair(InitialSearch<XES>::idComponent(), "constructive heuristic"));
    params.push_back(make_pair(NSSeq<XES, XEv, XSH>::idComponent(),
                               "neighborhood structure"));
    params.push_back(make_pair("OptFrame:int", "tabu list size"));
    params.push_back(make_pair("OptFrame:int", "max number of iterations"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == BasicTabuSearchBuilder<S, XEv>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << SingleObjSearchBuilder<S, XEv>::idComponent() << ":" << TS::family()
       << ":BasicTabuSearch";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_TS_BASICTABUSEARCH_HPP_
