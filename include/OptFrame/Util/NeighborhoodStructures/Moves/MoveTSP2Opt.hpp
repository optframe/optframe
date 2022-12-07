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

#ifndef OPTFRAME_MOVETSP2OPT_HPP_
#define OPTFRAME_MOVETSP2OPT_HPP_

// Framework includes
#include <OptFrame/Evaluation.hpp>
#include <OptFrame/Helper/Solutions/CopySolution.hpp>

#include "../../../Move.hpp"
#include "../BaseSolutionTSP.hpp"

// using namespace std;
// using namespace optframe; (?????????????????) Don't use namespace
// declarations in headers

// Working structure: vector<T>

namespace optframe {

// template<class T, class ADS = OPTFRAME_DEFAULT_ADS>
template <class T, class ADS = OPTFRAME_DEFAULT_ADS,
          XBaseSolution<vector<T>, ADS> S = CopySolution<vector<T>, ADS>,
          XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>>
class MoveTSP2Opt : public Move<XES, XEv> {
  typedef vector<T> Route;

 protected:
  int p1, p2;  // position 1 and position 2, respectively

  std::shared_ptr<OPTFRAME_DEFAULT_PROBLEM> problem;

 public:
  MoveTSP2Opt(int _p1, int _p2,
              std::shared_ptr<OPTFRAME_DEFAULT_PROBLEM> _problem = nullptr)
      : p1(_p1), p2(_p2), problem(_problem) {}

  virtual ~MoveTSP2Opt() {}

  int get_p1() const { return p1; }

  int get_p2() const { return p2; }

  virtual bool canBeApplied(const XES& se) override {
    const Route& rep = se.first.getR();
    bool all_positive = (p1 >= 0) && (p2 >= 0);
    bool less = (p1 < p2);
    return all_positive && (rep.size() >= 2) && less;
  }

  virtual uptr<Move<XES, XEv>> apply(XES& se) override {
    Route& rep = se.first.getR();
    reverse(rep.begin() + p1, rep.begin() + p2);

    // r1->r1, r2->r2, e1->i1, e2->i2, n1->n2, n2->n1, i1->e1, i2->e2
    return uptr<Move<XES, XEv>>(new MoveTSP2Opt(p1, p2));
  }

  bool operator==(const Move<XES, XEv>& _m) const override {
    const MoveTSP2Opt& m1 = (const MoveTSP2Opt&)_m;
    return ((m1.p1 == p1) && (m1.p2 == p2));
  }

  static string idComponent() {
    string idComp = Move<XES, XEv>::idComponent();
    idComp.append("MoveTSP2Opt");
    return idComp;
  }

  void print() const override {
    cout << "MoveTSP2Opt( ";
    cout << " edge " << p1 << " <=>  edge " << p2 << " )";
    cout << endl;
  }
};

}  // namespace optframe

#endif /*OPTFRAME_MOVETSP2OPT_HPP_*/
