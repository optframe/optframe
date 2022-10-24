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

#ifndef OPTFRAME_BI_LOS_HPP_
#define OPTFRAME_BI_LOS_HPP_

#include "../../Evaluator.hpp"
#include "../../LocalSearch.hpp"
#include "../../NSSeq.hpp"
#include "../../Timer.hpp"

namespace optframe {

template <XESolution XES, XEvaluation XEv = Evaluation<>, XESolution XSH = XES>
class BestImprovementLOS : public LocalSearch<S, XEv, XSH> {
 private:
  Evaluator<XES, XEv>& eval;
  NSSeq<XES, XEv, XSH>& nsSeq;

 public:
  BestImprovementLOS(Evaluator<XES, XEv>& _eval, NSSeq<XES, XEv, XSH>& _nsSeq)
      : eval(_eval), nsSeq(_nsSeq) {
  }

  virtual ~BestImprovementLOS() {
  }

  virtual void exec(S& s, const StopCriteria<XEv>& sosc) override {
    Evaluation<> e = eval.evaluate(s);

    exec(s, e, sosc);
  }

  virtual void exec(S& s, Evaluation<>& e, const StopCriteria<XEv>& sosc) override {
    Timer t;

    //TODO: use block iterator and manage each partial local optima discovered

    //NSBlockIterator<S, XEv>& itb = *nsSeq.getBlockIterator(s);
    NSBlockIterator<S, XEv>& itb = *nsSeq.getBlockIterator(se);

    cout << "TODO: BestImprovementLOS UNIMPLEMENTED!" << endl;

    return;

    //NSIterator<S, XEv>& it = *nsSeq.getIterator(s);
    NSIterator<S, XEv>& it = *nsSeq.getIterator(se);

    it.first();

    if (it.isDone()) {
      delete &it;
      return;
    }

    Move<S, XEv>* bestMove = it.current();

    /*if(e.getLocalOptimumStatus(bestMove->id()))
		{
			delete &it;
			delete bestMove;

			sum_time += t.inMilliSecs();
			return;
		}*/

    MoveCost<>* bestCost = nullptr;

    while (true) {
      //while (!bestMove->canBeApplied(s))
      while (!bestMove->canBeApplied(se)) {
        delete bestMove;
        it.next();
        if (!it.isDone()) {
          bestMove = it.current();
        } else {
          delete &it;
          return;
        }
      }

      bestCost = eval.moveCost(*bestMove, se);
      if (eval.isImprovement(*bestCost)) {
        it.next();
        break;
      } else {
        delete bestCost;
        delete bestMove;
        it.next();

        if (!it.isDone()) {
          bestMove = it.current();
        } else {
          delete &it;
          return;
        }
      }
    }

    //it.next();
    while (!it.isDone()) {
      Move<S, XEv>* move = it.current();
      if (move->canBeApplied(s)) {
        bool mayEstimate = false;
        MoveCost<>* cost = eval.moveCost(*move, se, mayEstimate);

        if (eval.betterThan(*cost, *bestCost)) {
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

    if (eval.isImprovement(*bestCost)) {
      //cout << "MOVE IS IMPROVEMENT! cost=";
      //bestCost->print();

      if (bestCost->isEstimated()) {
        // TODO: have to test if bestMove is ACTUALLY an improvement move...
      }

      Component::safe_delete(bestMove->applyUpdate(e, s));

      eval.reevaluate(e, s);  // updates 'e'
                              //e.setLocalOptimumStatus(bestMove->id(), false); //set NS 'id' out of Local Optimum
    } else {
      //bestMove->updateNeighStatus(s.getADS());
      //e.setLocalOptimumStatus(bestMove->id(), true); //set NS 'id' on Local Optimum
    }
    //cout << "#" << num_calls << " out_bi:";
    //bestMove->print();
    //nsSeq.print();
    //e.print();

    delete bestCost;
    delete bestMove;
    delete &it;
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (LocalSearch<XES, XEv>::compatible(s));
  }

  static string idComponent() {
    stringstream ss;
    ss << LocalSearch<XES, XEv>::idComponent() << "BI_LOS";
    return ss.str();
  }

  virtual string id() const override {
    return idComponent();
  }

  void print() const override {
    cout << toString() << endl;
  }

  std::string toString() const override {
    stringstream ss;
    ss << "BI_LOS: " << nsSeq.toString();
    return ss.str();
  }
};

template <XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>>
class BestImprovementBuilder : public LocalSearchBuilder<S, XEv, XES, X2ES> {
 public:
  virtual ~BestImprovementBuilder() {
  }

  LocalSearch<XES, XEv>* build(Scanner& scanner,
                               HeuristicFactory<S, XEv, XES, X2ES>& hf,
                               string family = "") override {
    if (!scanner.hasNext())
      return nullptr;
    Evaluator<XES, XEv>* eval;
    hf.assign(eval, *scanner.nextInt(), scanner.next());  // reads backwards!

    if (!scanner.hasNext())
      return nullptr;
    NSSeq<XES, XEv, XSH>* nsseq;
    hf.assign(nsseq, *scanner.nextInt(), scanner.next());  // reads backwards!

    return new BestImprovementLOS<S, XEv>(*eval, *nsseq);
  }

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(make_pair(Evaluator<XES, XEv>::idComponent(), "evaluation function"));
    params.push_back(make_pair(NSSeq<XES, XEv, XSH>::idComponent(), "neighborhood structure"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == BestImprovementLOS<S, XEv>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << LocalSearchBuilder<S, XEv>::idComponent() << ":BI_LOS";
    return ss.str();
  }

  virtual string id() const override {
    return idComponent();
  }
};

}  // namespace optframe

#endif /*OPTFRAME_BI_LOS_HPP_*/
