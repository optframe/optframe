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

#ifndef OPTFRAME_NSSEQ_TSP_OROPTK_HPP_
#define OPTFRAME_NSSEQ_TSP_OROPTK_HPP_

// Framework includes
#include <OptFrame/Core/Move.hpp>
#include <OptFrame/Core/NSSeq.hpp>

#include "BaseSolutionTSP.hpp"
#include "Moves/MoveTSPOrOptk.hpp"
#include "NSIterators/IteratorTSPOrOptk.hpp"

using namespace std;

// Working structure: vector<vector<T> >

using namespace optframe;

// template<class T, class ADS, XBaseSolution<vector<T>,ADS> S, XEvaluation XEv
// = Evaluation<>, XESolution XES = pair<S, XEv>, class MOVE = MoveTSPOrOptk<T,
// ADS, S, XEv, XES>, class P = OPTFRAME_DEFAULT_PROBLEM, class NSITERATOR =
// NSIteratorTSPOrOptk<T, ADS, S, XEv, XES, MOVE, P>, XSearch<XES> XSH =
// std::pair<S, XEv>>
template <class T, class ADS, XBaseSolution<vector<T>, ADS> S,
          class MOVE = MoveTSPOrOptk<T, ADS, S>,
          class P = OPTFRAME_DEFAULT_PROBLEM,
          class NSITERATOR = NSIteratorTSPOrOptk<T, ADS, S, MOVE, P>,
          XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>>
class NSSeqTSPOrOptk : public NSSeq<XES> {
  typedef vector<T> Route;

 private:
  int k;
  std::shared_ptr<P> p;  // has to be the last

 public:
  NSSeqTSPOrOptk(int _k, std::shared_ptr<P> _p = nullptr) : k(_k), p(_p) {
    static_assert(XSolution<S>);
    static_assert(XESolution<XES>);
    static_assert(XEvaluation<XEv>);
  }

  virtual ~NSSeqTSPOrOptk() {}

  uptr<Move<XES>> randomMove(const XES& se) override {
    const Route& rep = se.first.getR();
    int n = rep.size();

    if (n - k <= 0) {
      // THROW EXCEPTION!
      cerr << "CANNOT GENERATE MOVE OPTK FOR SOLUTION " << rep << std::endl;
      exit(1);
      // return *new MOVE(0, 0, k, p);
    }

    int i = rand() % (n - k + 1);

    int j = i;
    while (abs(i - j) < k) j = rand() % (n - k + 1);

    uptr<Move<XES>> m(new MOVE(i, j, k, p));
    // S sol(rep); // TODO: think
    // if (!m->canBeApplied(sol)) {
    if (!m->canBeApplied(se)) {
      std::cout << "ERROR IN GENERATION!" << std::endl;
      m->print();
      exit(1);
    }
    return m;
  }

  uptr<Move<XES>> validRandomMove(const XES& se) override {
    // const Route& r = s.getR();
    uptr<Move<XES>> m = randomMove(se);
    if (m->canBeApplied(se))
      return m;
    else {
      /// delete m;
      return nullptr;
    }
  }

  uptr<NSIterator<XES>> getIterator(const XES& se) override {
    const Route& r = se.first.getR();
    return uptr<NSIterator<XES>>(new NSITERATOR(r.size(), k, p));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << NSSeq<XES>::idComponent() << ":NSSeqTSPOrOptk";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (NSSeq<XES>::compatible(s));
  }

  std::string toString() const override {
    std::stringstream ss;
    ss << "NSSeqTSPOrOpt{K=" << k << "} with move: " << MOVE::idMoveComponent();
    return ss.str();
  }
};

// compile tests
#if 0
// using mynsseq_nsseq_tsp_oroptk_test = NSSeqTSPOrOptk<int, short,
// IsSolution<vector<int>, short>, IsEvaluation<double>,
// pair<IsSolution<vector<int>, short>, IsEvaluation<double>> >;
using mynsseq_nsseq_tsp_oroptk_test =
    NSSeqTSPOrOptk<int, short, IsSolution<vector<int>, short>>;
//
static_assert(
    std::is_base_of<nsseq_test_base, mynsseq_nsseq_tsp_oroptk_test>::value,
    "not inherited from NSSeq");
static_assert(!std::is_abstract<mynsseq_nsseq_tsp_oroptk_test>::value,
              "abstract nsseq");
#endif

#endif /*OPTFRAME_NSSEQ_TSP_OROPTK_HPP_*/
