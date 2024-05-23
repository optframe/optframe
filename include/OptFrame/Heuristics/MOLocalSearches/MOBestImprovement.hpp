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

#ifndef OPTFRAME_MOBI_HPP_
#define OPTFRAME_MOBI_HPP_

#include "../../Evaluator.hpp"
#include "../../MOLocalSearch.hpp"
#include "../../MultiObjSearch.hpp"
#include "../../NSSeq.hpp"
#include "../../Timer.hpp"

namespace optframe {

template <XESolution XES, XESolution XMES>
class MOBestImprovement : public MOLocalSearch<XES, XMES> {
 private:
  using S = typename XES::first_type;
  using XMEv = typename XMES::second_type;
  // MultiEvaluator<S, XEv>& v_e;
  GeneralEvaluator<XMES>& v_e;
  NSSeq<S>& nsSeq;

  // logs
  double sum_time;
  int num_calls;

 public:
  // MOBestImprovement(MultiEvaluator<S, XEv>& _v_e, NSSeq<S>& _nsSeq) :
  MOBestImprovement(GeneralEvaluator<XMES>& _v_e, NSSeq<S>& _nsSeq)
      : v_e(_v_e), nsSeq(_nsSeq) {
    sum_time = 0.0;
    num_calls = 0;
  }

  virtual ~MOBestImprovement() {}

  virtual void exec(IESolution<S, XMEv>& s, ParetoManager<XES, XMES>& pManager,
                    double timelimit, double target_f) {
    MultiEvaluation<>& sMev = v_e.evaluate(s);

    exec(s, sMev, pManager, timelimit, target_f);

    sMev.clear();
    delete &sMev;
  }

  virtual void exec(IESolution<S, XMEv>& s, XMEv& sMev,
                    ParetoManager<XES, XMES>& pManager, double timelimit,
                    double target_f) {
    num_calls++;
    Timer t;

    NSIterator<XMES, XMEv>& it = nsSeq.getIterator(s);

    it.first();

    if (it.isDone()) {
      delete &it;
      sum_time += t.inMilliSecs();
      return;
    }

    while (!it.isDone()) {
      Move<XMES>* move = &it.current();
      if (move->canBeApplied(s)) {
        //				cout << "before anything" << endl;
        //				sMev.print();
        Move<XMES>* mov_rev = move->apply(sMev, s);
        v_e.evaluate(sMev, s);

        //				cout << "after move" << endl;
        //				sMev.print();

        pManager.addSolution(&s, &sMev);

        delete mov_rev->apply(s);
        delete mov_rev;
        delete move;

        //				v_e.evaluate(sMev, s);

        //				cout << "reverse move" << endl;
        //				sMev.print();
        //				getchar();
        //				for (int eI = 0; eI < sMev.size(); eI++)
        //					delete &sMev[eI];

      } else
        delete move;

      it.next();
    }

    delete &it;
    sum_time += t.inMilliSecs();
  }
  bool compatible(std::string s) override {
    return (s == idComponent()) || (MOLocalSearch<S, XMEv>::compatible(s));
  }

  static string idComponent() {
    stringstream ss;
    ss << MOLocalSearch<XES, XMES>::idComponent() << ":MO-BI";
    return ss.str();
  }

  virtual string id() const override { return idComponent(); }

  void print() const override { cout << toString() << endl; }

  std::string toString() const override {
    stringstream ss;
    ss << "MOBI: " << nsSeq.toString();
    return ss.str();
  }

  virtual string log() const {
    stringstream ss;
    ss << sum_time;
    return ss.str();
  }
};

}  // namespace optframe

#endif /*OPTFRAME_MOBI_HPP_*/
