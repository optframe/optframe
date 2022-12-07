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

#ifndef OPTFRAME_BI_HPP_
#define OPTFRAME_BI_HPP_

#include "../../Evaluator.hpp"
#include "../../LocalSearch.hpp"
#include "../../NSSeq.hpp"
#include "../../Timer.hpp"

namespace optframe {

template <XESolution XES, XEvaluation XEv = typename XES::second_type,
          XESolution XSH = XES>
class BestImprovement : public LocalSearch<XES, XEv> {
 private:
  sref<GeneralEvaluator<XES, XEv>> eval;
  sref<NSSeq<XES, XEv, XSH>> nsSeq;

  // logs
  double sum_time;
  int num_calls;

 public:
  BestImprovement(sref<GeneralEvaluator<XES, XEv>> _eval,
                  sref<NSSeq<XES, XEv, XSH>> _nsSeq)
      : eval(_eval), nsSeq(_nsSeq) {
    sum_time = 0.0;
    num_calls = 0;
  }

  virtual ~BestImprovement() {}

  // DEPRECATED
  // virtual void exec(S& s, const StopCriteria<XEv>& sosc) override
  //{
  //	Evaluation<> e = eval.evaluate(s);
  //	exec(s, e, sosc);
  //}

  virtual SearchStatus searchFrom(XSH& se,
                                  const StopCriteria<XEv>& sosc) override {
    // XSolution& s = se.first;
    // XEv& e = se.second;

    // double timelimit = sosc.timelimit;
    // double target_f = sosc.target_f;

    if (Component::information)
      std::cout << "BI::starts for " << nsSeq->toString() << std::endl;

    num_calls++;
    Timer t;

    // TODO: verify if it's not null
    // uptr<NSIterator<XES, XEv, XSH>> it = nsSeq.getIterator(s);
    uptr<NSIterator<XES, XEv, XSH>> it = nsSeq->getIterator(se);
    //
    if (!it) return SearchStatus::FAILED;
    //
    if (Component::information) std::cout << "BI::first()" << std::endl;
    //
    it->first();

    if (it->isDone()) {
      sum_time += t.inMilliSecs();
      return SearchStatus::NO_REPORT;
    }

    uptr<Move<XES, XEv, XSH>> bestMove = it->current();

    /*if(e.getLocalOptimumStatus(bestMove->id()))
                {
                        delete &it;
                        delete bestMove;

                        sum_time += t.inMilliSecs();
                        return;
                }*/

    // MoveCost<>* bestCost = nullptr;
    op<XEv> bestCost = nullopt;

    while (true) {
      // while (!bestMove->canBeApplied(s))
      while (!bestMove->canBeApplied(se)) {
        it->next();
        if (!it->isDone()) {
          bestMove = it->current();
        } else {
          sum_time += t.inMilliSecs();
          return SearchStatus::NO_REPORT;
        }
      }

      bestCost = eval->moveCost(*bestMove, se);
      // if (eval.isImprovement(*bestCost))
      // if (bestCost->isImprovingStrict())
      if (eval->isStrictImprovement(*bestCost)) {
        it->next();
        break;
      } else {
        it->next();

        if (!it->isDone()) {
          bestMove = it->current();
        } else {
          sum_time += t.inMilliSecs();
          return SearchStatus::NO_REPORT;
        }
      }
    }

    // it.next();
    while (!it->isDone()) {
      uptr<Move<XES, XEv>> move = it->current();
      // if (move->canBeApplied(s))
      if (move->canBeApplied(se)) {
        /// MoveCost<>* cost = eval.moveCost(*move, se);
        op<XEv> cost = eval->moveCost(*move, se);

        // if (eval.betterThan(*cost, *bestCost))
        // if (cost->betterStrict(*bestCost))
        if (eval->betterStrict(*cost, *bestCost)) {
          /////delete bestMove;
          /////delete bestCost;
          //
          // bestMove = move;
          // bestCost = cost;
          bestMove = std::move(move);
          move = nullptr;
          bestCost = cost;  // TODO: std::move?
        } else {
          // delete move;
          // delete cost;
        }
      } else {
        // delete move;
      }

      it->next();
    }

    // if (eval.isImprovement(*bestCost))
    // if (bestCost->isImprovingStrict())
    if (eval->isStrictImprovement(*bestCost)) {
      // cout << "MOVE IS IMPROVEMENT! cost=";
      // bestCost->print();

      if (bestCost->isEstimated()) {
        // TODO: have to test if bestMove is ACTUALLY an improvement move...
      }

      bestMove->applyUpdate(se);

      eval->reevaluate(se);  // updates 'e'
                             // e.setLocalOptimumStatus(bestMove->id(), false);
                             // //set NS 'id' out of Local Optimum
    } else {
      // bestMove->updateNeighStatus(s.getADS());
      // e.setLocalOptimumStatus(bestMove->id(), true); //set NS 'id' on Local
      // Optimum
    }
    // cout << "#" << num_calls << " out_bi:";
    // bestMove->print();
    // nsSeq.print();
    // e.print();

    sum_time += t.inMilliSecs();
    return SearchStatus::NO_REPORT;
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (LocalSearch<XES, XEv>::compatible(s));
  }

  static string idComponent() {
    stringstream ss;
    ss << LocalSearch<XES, XEv>::idComponent() << "BI";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  void print() const override { cout << toString() << endl; }

  virtual string toString() const override {
    stringstream ss;
    ss << "BI: " << nsSeq->toString();
    return ss.str();
  }

  virtual string log() const {
    stringstream ss;
    ss << sum_time;
    return ss.str();
  }
};

template <XSolution S, XEvaluation XEv = Evaluation<>,
          XESolution XES = pair<S, XEv>,
          X2ESolution<XES> X2ES = MultiESolution<XES>,
          XSearch<XES> XSH = std::pair<S, XEv>>
class BestImprovementBuilder : public LocalSearchBuilder<S, XEv, XES, X2ES> {
 public:
  virtual ~BestImprovementBuilder() {}

  LocalSearch<XES, XEv>* build(Scanner& scanner,
                               HeuristicFactory<S, XEv, XES, X2ES>& hf,
                               string family = "") override {
    if (!scanner.hasNext()) return nullptr;
    sptr<GeneralEvaluator<XES, XEv>> eval;
    hf.assign(eval, *scanner.nextInt(), scanner.next());  // reads backwards!

    if (!scanner.hasNext()) return nullptr;
    sptr<NSSeq<XES, XEv, XSH>> nsseq;
    hf.assign(nsseq, *scanner.nextInt(), scanner.next());  // reads backwards!

    return new BestImprovement<XES, XEv, XSH>(eval, nsseq);
  }

  virtual vector<pair<string, string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(make_pair(GeneralEvaluator<XES, XEv>::idComponent(),
                               "evaluation function"));
    params.push_back(make_pair(NSSeq<XES, XEv, XSH>::idComponent(),
                               "neighborhood structure"));

    return params;
  }

  virtual bool canBuild(string component) override {
    return component == BestImprovement<XES, XEv>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << LocalSearchBuilder<S, XEv>::idComponent() << ":BI";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif /*OPTFRAME_BI_HPP_*/
