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

#ifndef OPTFRAME_MORI_HPP_
#define OPTFRAME_MORI_HPP_

#include "../../Evaluator.hpp"
#include "../../MOLocalSearch.hpp"
#include "../../MultiObjSearch.hpp"
#include "../../NSSeq.hpp"
#include "../../Timer.hpp"

namespace optframe {

// Basic MORI does not considering valid move, parameter iterMax only.
template <XESolution XES, XEMSolution XMES>
class MORandomImprovement : public MOLocalSearch<XES, XMES> {
  using S = typename XMES::first_type;
  using XMEv = typename XMES::second_type;

 private:
  // MultiEvaluator<S, XEv>& mev;
  sref<GeneralEvaluator<XMES>> mev;
  sref<NS<XMES>> ns;

  // logs
  double sum_time;
  int num_calls;
  int iterMax;

 public:
  // MORandomImprovement(MultiEvaluator<S, XEv>& _mev, NS<XES, XSH>& _ns,
  // unsigned int _iterMax) :
  MORandomImprovement(sref<GeneralEvaluator<XMES>> _mev, sref<NS<XMES>> _ns,
                      unsigned int _iterMax)
      : mev(_mev), ns(_ns), iterMax(_iterMax) {
    sum_time = 0.0;
    num_calls = 0;
  }

  virtual ~MORandomImprovement() {}

  /*
        virtual void moSearchFrom(Pareto<XMES>& p, S& s, paretoManager<S,
     XMEv, XMES>& pManager, const StopCriteria<XMEv>& stopCriteria) override
        {
                MultiEvaluation<> sMev(std::move(mev.evaluate(s)));

                moSearchFrom(p, s, sMev, pManager, stopCriteria);
        }
*/

  virtual void moSearchFrom(Pareto<XMES>& p, XMES& se,
                            ParetoManager<XES, XMES>& pManager,
                            const StopCriteria<XMEv>& stopCriteria) override {
    num_calls++;
    Timer t;

    // XES se = make_pair(s, Evaluation<>());

    int iter = 0;

    while ((iter < iterMax) && ((t.now() - stopCriteria.timelimit) < 0)) {
      uptr<Move<XMES>> move = ns->randomMove(se);
      if (move->canBeApplied(se)) {
        // Move and mark sMev as outdated
        uptr<Move<XMES>> mov_rev = move->apply(se);

        // Call method to reevaluate sMev and try to include TODO
        //				pManager->addSolutionWithMEVReevaluation(p,
        //*s,*sMev);

        pManager.addSolution(p, se.first);
        // delete mov_rev->apply(s);
        mov_rev->apply(se);
        // delete mov_rev;

        //			vector<MoveCost<>*> vMoveCost;
        //			for (int ev = 0; ev < v_e.size(); ev++)
        //			{
        //				vMoveCost.push_back(&v_e[ev].moveCost(sMev[ev],
        // move, s));
        //			}
        //			bool itsWorthAdding =
        // pManager.checkDominance(pManager.getParetoInsideManager(), &sMev);
        // if (itsWorthAdding)
      }
      // delete move;

      iter++;
    }

    sum_time += t.inMilliSecs();
  }
  bool compatible(std::string s) override {
    return (s == idComponent()) || (MOLocalSearch<XES, XMES>::compatible(s));
  }

  static string idComponent() {
    stringstream ss;
    ss << MOLocalSearch<XES, XMES>::idComponent() << "MO-RI";
    return ss.str();
  }

  virtual string id() const override { return idComponent(); }

  void print() const override { cout << toString() << endl; }

  std::string toString() const override {
    stringstream ss;
    ss << "MORI: " << ns->toString();
    return ss.str();
  }

  virtual string log() const {
    stringstream ss;
    ss << sum_time;
    return ss.str();
  }
};

}  // namespace optframe

#endif /*OPTFRAME_MORI_HPP_*/
