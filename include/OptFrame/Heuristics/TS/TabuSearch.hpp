// SPDX-License-Identifier:  MIT OR LGPL-3.0-or-later
// Copyright (C) 2007-2022 - OptFrame developers
// https://github.com/optframe/optframe

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

#ifndef OPTFRAME_HEURISTICS_TS_TABUSEARCH_HPP_  // NOLINT
#define OPTFRAME_HEURISTICS_TS_TABUSEARCH_HPP_  // NOLINT

#include <OptFrame/Core/Evaluator.hpp>
#include <OptFrame/Search/SingleObjSearch.hpp>

#include "../../NSEnum.hpp"

namespace optframe {

template <XESolution XES, XEvaluation XEv = Evaluation<>>
class TabuSearch : public SingleObjSearch<XES> {
 private:
  Evaluator<XES, XEv>& evaluator;
  InitialSearch<XES>& constructive;
  NSSeq<XES, XSH>& nsSeq;
  int tlSize;
  int tsMax;

 public:
  TabuSearch(Evaluator<XES, XEv>& _ev, InitialSearch<XES>& _constructive,
             NSSeq<XES, XSH>& _nsSeq, int _tlSize, int _tsMax)
      : evaluator(_ev),
        constructive(_constructive),
        nsSeq(_nsSeq),
        tlSize(_tlSize),
        tsMax(_tsMax) {}

  virtual ~TabuSearch() {}

  pair<S&, Evaluation<DS>&>* search(double timelimit = 100000000,
                                    double target_f = 0, const S* _s = nullptr,
                                    const Evaluation<DS>* _e = nullptr) {
    // cout << "TabuSearch exec(" << target_f << "," << timelimit << ")" <<
    // endl;

    long tini = time(nullptr);

    S& s = constructive.generateSolution();
    Evaluation<DS>& e = evaluator.evaluate(s);

    S* sStar = &s.clone();
    Evaluation<DS>* evalSStar = &evaluator.evaluate(*sStar);

    // evalSStar->print();

    int Iter = 0;

    int BestIter = 0;

    const vector<Move<S, XEv>*> emptyList;
    vector<Move<S, XEv>*> tabuList;

    long tnow = time(nullptr);

    int estimative_BTmax = 0;

    while (Iter - BestIter <= tsMax && ((tnow - tini) < timelimit)) {
      Iter = Iter + 1;

      if ((Iter - BestIter) > estimative_BTmax)
        estimative_BTmax = (Iter - BestIter);

      // cout << "Iter " << Iter << " (" << (Iter - BestIter - 1) << " without
      // improvement)" << endl;

      // ==================
      // First: aspiration
      // ==================

      Move<S, XEv>* bestMove = tabuBestMove(s, e, emptyList);

      S* s1 = &s.clone();

      Move<S, XEv>* newTabu = &bestMove->apply(*s1);
      Evaluation<DS>* evalS1 = &evaluator.evaluate(*s1);

      if (evaluator.betterThan(*evalS1, *evalSStar)) {
        // Better than global!

        for (unsigned int i = 0; i < tabuList.size(); i++)
          if ((*tabuList[i]) == (*bestMove)) {
            delete tabuList[i];
            tabuList.erase(tabuList.begin() + i);
            break;
          }
      } else {
        delete s1;
        delete evalS1;
        delete newTabu;
        delete bestMove;

        bestMove = tabuBestMove(s, e, tabuList);
        s1 = &s.clone();
        newTabu = &bestMove->apply(*s1);
        evalS1 = &evaluator.evaluate(*s1);

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
      s = *s1;
      e = *evalS1;

      delete s1;
      delete evalS1;

      // =====================================================
      //        's' better than 's*' (?)
      // =====================================================

      if (evaluator.betterThan(e, *evalSStar)) {
        delete sStar;
        delete evalSStar;

        sStar = &s.clone();
        evalSStar = &evaluator.evaluate(*sStar);

        BestIter = Iter;

        // cout << "Improvement on " << BestIter << ": fo=" <<
        // evalSStar->evaluation() << endl;
      }

      tnow = time(nullptr);
    }

    while (tabuList.size() > 0) {
      delete tabuList[0];
      tabuList.erase(tabuList.begin());
    }

    // ===========
    // 's' <- 's*'
    // ===========

    s = *sStar;
    e = *evalSStar;

    delete sStar;
    delete evalSStar;

    FILE* ftabu = fopen("tabu.txt", "a");
    if (!ftabu) {
      cout << "Error creating file 'tabu.txt'" << endl;
    } else {
      fprintf(ftabu, "%d\n", estimative_BTmax);
      fclose(ftabu);
    }

    return new pair<S&, Evaluation<DS>&>(s, e);
  }

  Move<S, XEv>* tabuBestMove(S& s, Evaluation<DS>& e,
                             const vector<Move<S, XEv>*>& tabuList) {
    NSIterator<XES>& it = nsSeq.getIterator(e.getDS(), s.getR(), s.getADS());

    it.first();

    if (it.isDone()) {
      delete &it;
      return nullptr;
    }

    Move<S, XEv>* bestMove = &it.current();

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

    MoveCost<>* bestCost = &evaluator.moveCost(*bestMove, se);
    it.next();
    while (!it.isDone()) {
      Move<S, XEv>* move = &it.current();
      if (move->canBeApplied(s) && !inList(bestMove, tabuList)) {
        MoveCost<>* cost = &evaluator.moveCost(*move, se);

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

  bool inList(Move<S, XEv>* m, const vector<Move<S, XEv>*>& v) {
    for (unsigned int i = 0; i < v.size(); i++)
      if ((*m) == (*v[i])) return true;
    return false;
  }

  static string idComponent() {
    stringstream ss;
    ss << SingleObjSearch<XES>::idComponent() << "TS:basic_ts";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  virtual vector<pair<string, string>> parameters() const {
    vector<pair<string, string>> p;
    p.push_back(make_pair("OptFrame:ev", "evaluator"));
    p.push_back(make_pair("OptFrame:nsseq", "neighborhood_structure"));
    p.push_back(make_pair("int", "tabu_list_size"));
    p.push_back(make_pair("int", "ts_max"));

    return p;
  }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_TS_TABUSEARCH_HPP_ // NOLINT
