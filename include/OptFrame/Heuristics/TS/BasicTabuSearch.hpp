// SPDX-License-Identifier:  MIT OR LGPL-3.0-or-later
// Copyright (C) 2007-2022 - OptFrme developers
// https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_TS_BASICTABUSEARCH_HPP_
#define OPTFRAME_HEURISTICS_TS_BASICTABUSEARCH_HPP_

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
 private:
  // Evaluator<XES, XEv>& evaluator;
  // InitialSearch<XES>& constructive;
  // NSSeq<XES, XEv, XSH>& nsSeq;
  sref<GeneralEvaluator<XES, XEv>> evaluator;
  sref<InitialSearch<XES>> constructive;
  vsref<NSSeq<XES, XEv, XSH>> nsSeq;
  int tlSize;
  int tsMax;

 public:
  // BasicTabuSearch(Evaluator<XES, XEv>& _ev, InitialSearch<XES>&
  // _constructive, NSSeq<XES, XEv, XSH>& _nsSeq, int _tlSize, int _tsMax)
  BasicTabuSearch(sref<GeneralEvaluator<XES, XEv>> _ev,
                  sref<InitialSearch<XES>> _constructive,
                  sref<NSSeq<XES, XEv, XSH>> _nsSeq, int _tlSize, int _tsMax)
      : evaluator(_ev),
        constructive(_constructive),
        nsSeq(_nsSeq),
        tlSize(_tlSize),
        tsMax(_tsMax) {}

  ~BasicTabuSearch() override = default;

  // pair<S&, Evaluation<>&>* search(double timelimit = 100000000, double
  // target_f = 0, const S* _s = nullptr, const Evaluation<>* _e = nullptr)
  // override
  SearchOutput<XES, XSH> search(const StopCriteria<XEv>& sosc) override {
    // cout << "BasicTabuSearch exec(" << target_f << "," << timelimit << ")" <<
    // endl;

    // long tini = time(nullptr);
    // long timelimit = stop.timelimit;

    // XSolution AUTO_CONCEPTS& s = constructive.generateSolution();
    // XSolution AUTO_CONCEPTS& s = constructive.generateSolution();
    // Evaluation<>& e = evaluator.evaluate(s);

    // XSolution AUTO_CONCEPTS* sStar = &s.clone();
    // Evaluation<>* evalSStar = &evaluator.evaluate(*sStar);

    // =========================

    std::cout << "TS search(" << sosc.timelimit << ")" << std::endl;

    // TODO: receive on 'searchBy'
    std::optional<XSH> star;
    std::optional<XSH> incumbent;

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

    XSH& se = *incumbent;

    // evalSStar->print();

    int Iter = 0;

    int BestIter = 0;

    const vector<Move<XES, XEv>*> emptyList;
    vector<Move<XES, XEv>*> tabuList;

    // long tnow = time(nullptr);

    int estimative_BTmax = 0;

    while (Iter - BestIter <= tsMax &&
           sosc.shouldStop(star->second)) {  //((tnow - tini) < timelimit)) {
      Iter = Iter + 1;

      if ((Iter - BestIter) > estimative_BTmax)
        estimative_BTmax = (Iter - BestIter);

      // cout << "Iter " << Iter << " (" << (Iter - BestIter - 1) << " without
      // improvement)" << endl;

      // ==================
      // First: aspiration
      // ==================

      Move<XES, XEv>* bestMove = tabuBestMove(
          se.first, se.second, emptyList);  // tabuBestMove(s, e, emptyList);

      // XSolution* s1 = &s.clone();
      XES se1 = se;

      // Move<XES, XEv>* newTabu = &bestMove->apply(se1.first);//(*s1);
      // XEv* evalS1 = &evaluator.evaluate(se1.first);//(*s1);

      uptr<Move<XES, XEv>> newTabu = bestMove->applyUpdate(se1);  //(*s1);
      evaluator.reevaluate(se1);
      //
      // XEv* evalS1 = &evaluator.evaluate(se1.first);//(*s1);

      // if (evaluator.betterThan(*evalS1, *evalSStar)) {
      if (evaluator->betterStrict(se1.second, star->second)) {
        // Better than global!

        for (unsigned int i = 0; i < tabuList.size(); i++)
          if ((*tabuList[i]) == (*bestMove)) {
            delete tabuList[i];
            tabuList.erase(tabuList.begin() + i);
            break;
          }
      } else {
        // delete s1;
        // delete evalS1;
        // delete newTabu;
        delete bestMove;

        // bestMove = tabuBestMove(s, e, tabuList);
        bestMove = tabuBestMove(se.first, se.second, tabuList);

        // s1 = &s.clone();
        // XES se1 = se;
        se1 = se;

        // newTabu = &bestMove->apply(*s1);
        // evalS1 = &evaluator.evaluate(*s1);
        newTabu = bestMove->applyUpdate(se1);
        evaluator->reevaluate(se1);

        delete bestMove;
      }

      // =====================================================
      //    |T| <= tsSize
      // =====================================================

      tabuList.push_back(newTabu);

      if (((int)tabuList.size()) > tlSize) {
        delete tabuList[0];
        tabuList.erase(tabuList.begin());
      }

      //-----------------------------------------------------------
      if (((int)tabuList.size()) > tlSize)
        throw string("ERROR on Tabu Search! more elements than expected...");
      //-----------------------------------------------------------

      // =====================================================
      //        's' <- 's1';
      // =====================================================
      se = std::move(se1);
      // s = *s1;
      // e = *evalS1;
      // delete s1;
      // delete evalS1;

      // =====================================================
      //        's' better than 's*' (?)
      // =====================================================

      // if (evaluator.betterThan(e, *evalSStar)) {
      if (evaluator->betterStrict(se.second, star->second)) {
        // delete sStar;
        // delete evalSStar;

        // sStar = &s.clone();
        // evalSStar = &evaluator.evaluate(*sStar);
        star = se;

        BestIter = Iter;

        // cout << "Improvement on " << BestIter << ": fo=" <<
        // evalSStar->evaluation() << endl;
      }

      // tnow = time(nullptr);
    }

    while (tabuList.size() > 0) {
      delete tabuList[0];
      tabuList.erase(tabuList.begin());
    }

    // ===========
    // 's' <- 's*'
    // ===========

    // s = *sStar;
    // e = *evalSStar;
    // se = star;

    // delete sStar;
    // delete evalSStar;

    FILE* ftabu = fopen("tabu.txt", "a");
    if (!ftabu) {
      cout << "Error creating file 'tabu.txt'" << endl;
    } else {
      fprintf(ftabu, "%d\n", estimative_BTmax);
      fclose(ftabu);
    }

    // return new pair<S&, Evaluation<>&>(s, e);
    // return STATUS
    return {SearchStatus::NO_REPORT, star};
  }

  Move<XES, XEv>* tabuBestMove(XES& se,
                               const vector<Move<XES, XEv>*>& tabuList) {
    XSolution AUTO_CONCEPTS& s = se.first;
    // Evaluation<>& e = se.second;

    NSIterator<XES, XEv>& it = nsSeq.getIterator(s.getR(), s.getADS());

    it.first();

    if (it.isDone()) {
      delete &it;
      return nullptr;
    }

    Move<XES, XEv>* bestMove = &it.current();

    while (!bestMove->canBeApplied(s) || inList(bestMove, tabuList)) {
      delete bestMove;
      it.next();

      if (!it.isDone()) {
        bestMove = &it.current();
      } else {
        delete &it;
        return nullptr;
      }
    }

    MoveCost<XEv>* bestCost = &evaluator.moveCost(*bestMove, se);
    it.next();
    while (!it.isDone()) {
      Move<XES, XEv>* move = &it.current();
      if (move->canBeApplied(s) && !inList(bestMove, tabuList)) {
        MoveCost<XEv>* cost = &evaluator.moveCost(*move, se);

        if (evaluator.betterThan(*cost, *bestCost)) {
          delete bestMove;
          delete bestCost;
          bestMove = move;
          bestCost = cost;
        } else {
          delete move;
          delete cost;
        }
      } else
        delete move;
      it.next();
    }

    delete &it;

    delete bestCost;

    return bestMove;
  }

  bool inList(Move<XES, XEv>* m, const vector<Move<XES, XEv>*>& v) {
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
  virtual ~BasicTabuSearchBuilder() {}

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

    return new BasicTabuSearch<XES>(*eval, *constructive, *nsseq, tl, tsMax);
  }

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(
        make_pair(Evaluator<XES, XEv>::idComponent(), "evaluation function"));
    // params.push_back(make_pair(Constructive<S>::idComponent(), "constructive
    // heuristic"));
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
