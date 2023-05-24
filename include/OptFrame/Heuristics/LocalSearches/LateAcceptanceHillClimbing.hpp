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

#ifndef LATE_ACCEPTANCE_HILL_CLIMBING_HPP_
#define LATE_ACCEPTANCE_HILL_CLIMBING_HPP_

#include "../../Evaluator.hpp"
#include "../../LocalSearch.hpp"
#include "../../NS.hpp"

#define BRAND_NEW

namespace optframe {

template <XESolution XES, XEvaluation XEv = Evaluation<>>
class LateAcceptanceHillClimbing : public LocalSearch<XES, XEv> {
 private:
  sref<GeneralEvaluator<XES, XEv>> ev;
  vsref<NS<XES, XEv>> lns;
  int L;        // size of list
  int iterMax;  // max iterations without improvement

 public:
  LateAcceptanceHillClimbing(sref<GeneralEvaluator<XES, XEv>> _ev, sref<NS<XES, XEv>> _ns, int _L, int _iterMax)
      : ev(_ev), L(_L), iterMax(_iterMax) {
    lns.push_back(&_ns);
  }

  LateAcceptanceHillClimbing(sref<GeneralEvaluator<XES, XEv>> _ev, vsref<NS<XES, XEv>> _lns, int _L, int _iterMax)
      : ev(_ev), lns(_lns), L(_L), iterMax(_iterMax) {
  }

  virtual ~LateAcceptanceHillClimbing() {
  }

  // DEPRECATED
  //virtual void exec(S& s, const StopCriteria<XEv>& stopCriteria)
  //{
  //	Evaluation<> e = std::move(ev->evaluate(s));
  //	exec(s, e, stopCriteria);
  //}

  SearchStatus searchFrom(XES& seBest, const StopCriteria<XEv>& sosc) override {
    using S = typename XES::first_type;
    // convenient local references on 'seBest'
    S&   sStar = seBest.first;
    XEv& eStar = seBest.second;

    double timelimit = sosc.timelimit;

    //XEv target_f(sosc.target_f); // 'target_f' will break... removing

    long tini = time(nullptr);

#ifdef BRAND_NEW
    vector<Evaluation<>*> eList;
    for (int i = 0; i < L; i++)
      eList.push_back(&eStar.clone());
#else
    vector<double> eList(L, eStar.evaluation());
#endif

    int iter = 1;
    unsigned index = 0;

    S   s = sStar; // copy
    XEv e = eStar; // copy

    long tnow = time(nullptr);

    while ((iter <= iterMax) && ((tnow - tini) < timelimit))  //&& ev->betterThan(target_f, eStar))
    {
      // choose random neighborhood
      int ns_k = rand() % lns.size();

      uptr<Move<XES, XEv>> move = lns[ns_k]->validRandomMove(seBest);

      if (!move) {
        cout << "Warning in LAHC: cannot find an appliable move for neighborhood ";
        lns[ns_k]->print();
        // TODO: return FAIL here
      }

      if (move && move->canBeApplied(seBest)) {
        bool mayEstimate = false;
        ///MoveCost<>& cost = *ev->moveCost(*move, se, mayEstimate);
        op<XEv> cost = ev->moveCost(*move, seBest, mayEstimate);

        // test for current index
#ifdef BRAND_NEW
        //if (ev->isImprovement(*cost, e, *eList[index]))
        //if (cost->isImprovingStrict( e, *eList[index]))
        if (ev->isImprovingStrictly(*cost, e, *eList[index]))
#else
        if (ev->betterThan(cost.cost() + e.evaluation(), eList[index]))
#endif
        {
          move->applyUpdate(seBest);
          ev->reevaluate(seBest);

#ifdef BRAND_NEW
          delete eList[index];
          eList[index] = &e.clone();
#else
          eList[index] = e.evaluation();
#endif
          if (ev->betterStrict(e, eStar)) {
            sStar = s; // copy
            eStar = e; // copy

            cout << "LAHC: best solution in iter=" << iter << " => ";
            e.print();

            iter = 0;
          }
        }
      }

      iter++;

      index++;
      if (index == eList.size())
        index = 0;

      tnow = time(nullptr);
    }

    // delete &e;
    // delete &s;

    // free list

#ifdef BRAND_NEW
    for (unsigned i = 0; i < eList.size(); i++)
      delete eList[i];
#endif

    eList.clear();
    return SearchStatus::NO_REPORT;
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (LocalSearch<XES, XEv>::compatible(s));
  }

  static string idComponent() {
    stringstream ss;
    ss << LocalSearch<XES, XEv>::idComponent() << ":LAHC";
    return ss.str();
  }

  virtual string id() const override {
    return idComponent();
  }

  std::string toString() const override {
    stringstream ss;
    ss << "LAHC: [ ";
    for (unsigned i = 0; i < lns.size(); i++) {
      ss << lns[i]->toString();
      if (i != lns.size() - 1)
        ss << ",";
    }
    ss << "]";

    return ss.str();
  }
};

template <XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>>
class LateAcceptanceHillClimbingBuilder : public LocalSearchBuilder<S, XEv, XES, X2ES> {
 public:
  virtual ~LateAcceptanceHillClimbingBuilder() {
  }

  LocalSearch<XES, XEv>* build(Scanner& scanner,
                               HeuristicFactory<S, XEv, XES, X2ES>& hf,
                               string family = "") override {
    sptr<GeneralEvaluator<XES, XEv>> eval;
    hf.assign(eval, *scanner.nextInt(), scanner.next());  // reads backwards!

    vsptr<NS<XES, XEv>> _nslist;
    hf.assignList(_nslist, *scanner.nextInt(), scanner.next());  // reads backwards!
    vsref<NS<XES, XEv>> nslist;
    for (auto x : _nslist)
      nslist.push_back(x);

    int L = *scanner.nextInt();

    int iterMax = *scanner.nextInt();

    return new LateAcceptanceHillClimbing<XES, XEv>(eval, nslist, L, iterMax);
  }

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(make_pair(Evaluator<XES, XEv>::idComponent(), "evaluation function"));
    stringstream ss;
    ss << NS<XES, XEv>::idComponent() << "[]";
    params.push_back(make_pair(ss.str(), "list of NS"));
    params.push_back(make_pair("OptFrame:int", "list size L"));
    params.push_back(make_pair("OptFrame:int", "iterMax iterations without improvement"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == LateAcceptanceHillClimbing<XES, XEv>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << LocalSearchBuilder<S, XEv>::idComponent() << ":LAHC";
    return ss.str();
  }

  std::string toString() const override {
    return id();
  }

  virtual string id() const override {
    return idComponent();
  }
};

}  // namespace optframe

#endif /* LATE_ACCEPTANCE_HILL_CLIMBING_HPP_ */
