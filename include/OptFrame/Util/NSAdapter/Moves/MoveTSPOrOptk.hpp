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

#ifndef OPTFRAME_MOVE_TSP_OROPTK_HPP_
#define OPTFRAME_MOVE_TSP_OROPTK_HPP_

// Framework includes
#include <OptFrame/Core/Evaluation.hpp>
#include <OptFrame/Core/Move.hpp>
#include <OptFrame/Helper/Solutions/CopySolution.hpp>

// using namespace std;
// using namespace optframe; (?????????????????) Don't use namespace
// declarations in headers

// Working structure: vector<vector<T> >

namespace optframe {

// template<class T, class ADS = OPTFRAME_DEFAULT_ADS>
template <class T, class ADS = OPTFRAME_DEFAULT_ADS,
          XBaseSolution<vector<T>, ADS> S = CopySolution<vector<T>, ADS>,
          XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>>
class MoveTSPOrOptk : public Move<XES> {
  typedef vector<T> Route;

 protected:
  int i;  // origin
  int j;  // destination
  int k;  // number of elements

  std::shared_ptr<OPTFRAME_DEFAULT_PROBLEM> problem;

 public:
  using Move<XES>::apply;
  using Move<XES>::canBeApplied;

  MoveTSPOrOptk(int _i, int _j, int _k,
                std::shared_ptr<OPTFRAME_DEFAULT_PROBLEM> _problem = nullptr)
      : i(_i), j(_j), k(_k), problem(_problem) {}

  virtual ~MoveTSPOrOptk() {}

  int get_i() const { return i; }

  int get_j() const { return j; }

  int get_k() const { return k; }

  virtual bool canBeApplied(const XES& se) override {
    // const Route& rep = s.getR();
    // return (i != j) && (i + k <= rep.size());
    return abs(i - j) >= k;
  }

  virtual uptr<Move<XES>> apply(XES& se) override {
    Route& rep = se.first.getR();
    vector<T> v_aux;
    v_aux.insert(v_aux.begin(), rep.begin() + i, rep.begin() + i + k);
    rep.erase(rep.begin() + i, rep.begin() + i + k);
    rep.insert(rep.begin() + j, v_aux.begin(), v_aux.end());

    return uptr<Move<XES>>(new MoveTSPOrOptk(j, i, k));
  }

  bool operator==(const Move<XES>& _m) const override {
    const MoveTSPOrOptk& m1 = (const MoveTSPOrOptk&)_m;
    return (m1.i == i) && (m1.j == j) && (m1.k == k);
  }

  static std::string idComponent() {
    string idComp = Move<XES>::idMoveComponent();
    idComp.append("MoveTSPOrOptk");
    return idComp;
  }

  void print() const override {
    std::cout << "MoveTSPOrOpt{K=" << k << "}";
    std::cout << "(" << i << ";" << j << ")" << std::endl;
  }
};

}  // namespace optframe

#endif /*OPTFRAME_MOVE_TSP_OROPTK_HPP_*/
