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

#ifndef OPTFRAME_FI_SLOW_HPP_
#define OPTFRAME_FI_SLOW_HPP_

#include <OptFrame/Core/Evaluator.hpp>
#include <OptFrame/Core/NSSeq.hpp>
#include <OptFrame/Search/LocalSearch.hpp>

namespace optframe {

template <XESolution XES, XEvaluation XEv = Evaluation<>>
class FirstImprovementSlow : public LocalSearch<XES> {
 private:
  Evaluator<S>& eval;
  NSSeq<S>& nsSeq;

 public:
  FirstImprovementSlow(Evaluator<S>& _eval, NSSeq<S>& _nsSeq)
      : eval(_eval), nsSeq(_nsSeq) {}

  virtual ~FirstImprovementSlow() {}

  virtual void exec(Solution<R, ADS>& s, double timelimit, double target_f) {
    Evaluation<>& e = eval.evaluate(s);
    exec(s, e, timelimit, target_f);
    delete &e;
  }

  virtual void exec(Solution<R, ADS>& s, Evaluation<>& e, double timelimit,
                    double target_f) {
    NSIterator<XES>& it = nsSeq.getIterator(s);
    string bestMoveId = "";
    it.first();

    if (it.isDone()) {
      delete &it;
      return;
    }

    do {
      Move<R, ADS>* move = &it.current();

      // TODO: deprecated! use LOS in NSSeq and NSSeqIterator instead
      /*
                        if(e.getLocalOptimumStatus(move->id()))
                        {
                                delete &it;
                                delete move;
                                return;
                        }
                        */

      bestMoveId = move->id();

      if (move->canBeApplied(s)) {
        MoveCost<>* eCost = &eval.moveCost(*move, se);  // estimated cost

        if (eval.isImprovement(*eCost)) {
          if (eCost->isEstimated()) {
            // TODO: find a real cost value...
          }

          if (eval.isImprovement(*eCost)) {
            delete eCost;

            Component::safe_delete(move->apply(e, s));
            delete move;

            delete &it;

            eval.evaluate(e, s);  // updates 'e'

            // TODO: deprecated! use LOS in NSSeq and NSSeqIterator instead
            // e.setLocalOptimumStatus(bestMoveId, false); //set NS 'id' out of
            // Local Optimum

            return;
          }
        }

        delete eCost;
      }

      delete move;

      it.next();
    } while (!it.isDone());

    // TODO: deprecated! use LOS in NSSeq and NSSeqIterator instead
    // if(bestMoveId != "")
    //	e.setLocalOptimumStatus(bestMoveId, true); //set NS 'id' on Local
    // Optimum

    delete &it;
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (LocalSearch<XES>::compatible(s));
  }

  static string idComponent() {
    stringstream ss;
    ss << LocalSearch<XES>::idComponent() << ":FISlow";
    return ss.str();
  }

  virtual string id() const override { return idComponent(); }

  std::string toString() const override {
    stringstream ss;
    ss << "FISlow: " << nsSeq.toString();
    return ss.str();
  }
};

}  // namespace optframe

#endif /*OPTFRAME_FI_SLOW_HPP_*/
