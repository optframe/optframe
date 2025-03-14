// OptFrame 4.2 - Optimization Framework
// Copyright (C) 2009-2021 - MIT LICENSE
// https://github.com/optframe/optframe
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef OPTFRAME_IILSL_HPP_
#define OPTFRAME_IILSL_HPP_

#include <math.h>

#include <OptFrame/Search/LocalSearch.hpp>
#include <vector>

#include "ILSLPerturbation.hpp"
#include "Intensification.hpp"
#include "IntensifiedIteratedLocalSearch.hpp"

namespace optframe {

typedef pair<pair<int, int>, pair<int, int>> levelHistory;

template <XESolution XES, XEvaluation XEv = Evaluation<>>
class IntensifiedIteratedLocalSearchLevels
    : public IntensifiedIteratedLocalSearch<levelHistory, R, ADS, S> {
 protected:
  LocalSearch<XES>& ls;
  Intensification<S, XEv>& h2;
  ILSLPerturbation<S, XEv>& p;
  int iterMax, levelMax;

 public:
  IntensifiedIteratedLocalSearchLevels(Evaluator<XES, XEv>& e,
                                       Constructive<S>& constructive,
                                       LocalSearch<XES>& _ls,
                                       Intensification<S, XEv>& _h2,
                                       ILSLPerturbation<S, XEv>& _p,
                                       int _iterMax, int _levelMax)
      : IntensifiedIteratedLocalSearch<levelHistory, R, ADS>(e, constructive),
        ls(_ls),
        h2(_h2),
        p(_p) {
    iterMax = _iterMax;
    levelMax = _levelMax;
  }

  virtual ~IntensifiedIteratedLocalSearchLevels() {}

  virtual levelHistory& initializeHistory() {
    // cout << "initializeHistory()" << endl;
    pair<int, int> vars(0, 0);

    // IterMax e LevelMax
    pair<int, int> maxs(iterMax, levelMax);

    return *new levelHistory(vars, maxs);
  }

  virtual void intensification(S& s, Evaluation<>& e, double timelimit,
                               double target_f, levelHistory& history) {
    h2.addSolution(s);

    if (history.first.first ==
        history.second.first - 1)  // TODO intensification applied only at last
                                   // iteration of each level
    {
      S& s1 = h2.search(s);

      Evaluator<XES, XEv>& ev = this->getEvaluator();
      Evaluation<>& s1_e = ev.evaluate(s1);

      if (ev.betterThan(s1_e, e)) {
        e = s1_e;
        s = s1;
        h2.addSolution(s);
      }

      delete &s1;
    }
  }

  virtual void localSearch(S& s, Evaluation<>& e, double timelimit,
                           double target_f) {
    // cout << "localSearch(.)" << endl;
    ls.searchFrom(s, e, timelimit, target_f);
  }

  virtual void perturbation(S& s, Evaluation<>& e, double timelimit,
                            double target_f, levelHistory& history) {
    // cout << "perturbation(.)" << endl;

    int iter = history.first.first;
    int level = history.first.second;
    int iterMax = history.second.first;
    // int levelMax = history.second.second;

    // cout << "level = " << level << " e iter = " << iter << endl;

    // nivel atual: 'level'
    p.perturb(s, e, timelimit, target_f, level);

    // Incrementa a iteracao
    iter++;

    if (iter >= iterMax) {
      iter = 0;
      level++;
      cout << "level " << level << ".." << endl;
    }

    // Atualiza o historico
    history.first.first = iter;
    history.first.second = level;
  }

  virtual bool acceptanceCriterion(const Evaluation<>& e1,
                                   const Evaluation<>& e2,
                                   levelHistory& history) {
    if (IntensifiedIteratedLocalSearch<levelHistory, R, ADS>::evaluator
            .betterThan(e1, e2)) {
      // =======================
      //   Melhor solucao: 's2'
      // =======================
      cout << "Best fo: " << e1.evaluation();
      cout << " on [iter " << history.first.first << " of level "
           << history.first.second << "]" << endl;

      // =======================
      //  Atualiza o historico
      // =======================
      // iter = 0
      history.first.first = 0;
      // level = 0
      history.first.second = 0;

      // =======================
      //    Retorna s2
      // =======================

      h2.addSolution(s2);  // Deprecated TODO

      return true;
    } else
      return false;
  }

  virtual bool terminationCondition(levelHistory& history) {
    // cout << "terminationCondition(.)" << endl;
    int level = history.first.second;
    int levelMax = history.second.second;

    return (level >= levelMax);
  }

  virtual string id() const override { return idComponent(); }

  static string idComponent() {
    stringstream ss;
    ss << IntensifiedIteratedLocalSearch<levelHistory, R, ADS>::idComponent()
       << "iils";
    return ss.str();
  }
};

}  // namespace optframe

#endif /*OPTFRAME_IILSL_HPP_*/
