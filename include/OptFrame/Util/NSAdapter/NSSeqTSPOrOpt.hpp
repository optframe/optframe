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

#ifndef OPTFRAME_NSSEQVECTOROROPT_HPP_
#define OPTFRAME_NSSEQVECTOROROPT_HPP_

// Framework includes
#include "../../Move.hpp"
#include "../../NSSeq.hpp"
#include "BaseSolutionTSP.hpp"
#include "NSSeqTSPOrOptk.hpp"
#include "NSSeqUnionAdapter.hpp"

using namespace std;

// definition: OrOpt is OrOpt{1..3}

namespace optframe {

// template<class T, class ADS = OPTFRAME_DEFAULT_ADS>
// template<class T, class ADS = OPTFRAME_DEFAULT_ADS,
// XBaseSolution<vector<T>,ADS> S = CopySolution<vector<T>,ADS>, class MOVE =
// MoveTSPSwap<T, ADS, S>, class P = OPTFRAME_DEFAULT_PROBLEM, class NSITERATOR
// = NSIteratorTSPSwap<T, ADS, S, MOVE, P>, XEvaluation XEv = Evaluation<>>
template <class T, class ADS, XBaseSolution<vector<T>, ADS> S,
          class MOVE = MoveTSPSwap<T, ADS, S>,
          class P = OPTFRAME_DEFAULT_PROBLEM,
          class NSITERATOR = NSIteratorTSPSwap<T, ADS, S, MOVE, P>,
          XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>,
          XSearch<XES> XSH = std::pair<S, XEv>>
class NSSeqTSPOrOpt : public NSSeq<XES, XSH> {
  typedef vector<T> Route;

  NSSeqTSPOrOptk<T, ADS, S>* OrOpt1;
  NSSeqTSPOrOptk<T, ADS, S>* OrOpt2;
  NSSeqTSPOrOptk<T, ADS, S>* OrOpt3;

  NSSeqUnionAdapter<vector<T>, ADS, S, XEv, XES>* OrOpt1_2_3;

 public:
  NSSeqTSPOrOpt() {
    OrOpt1 = new NSSeqTSPOrOptk<T, ADS, S>(1);
    OrOpt2 = new NSSeqTSPOrOptk<T, ADS, S>(2);
    OrOpt3 = new NSSeqTSPOrOptk<T, ADS, S>(3);

    OrOpt1_2_3 = new NSSeqUnionAdapter<vector<T>, ADS>(*OrOpt1, *OrOpt2);
    OrOpt1_2_3->add_ns(*OrOpt3);
  }

  virtual ~NSSeqTSPOrOpt() {
    delete OrOpt1;
    delete OrOpt2;
    delete OrOpt3;

    delete OrOpt1_2_3;
  }

  // Maybe S& should be the Representation itself.... no getR() here.
  // It makes more sense to pass RepTSP + ESolutionTSP... than SolutionTSP +
  // ESolutionTSP
  uptr<Move<XES, XSH>> randomMove(const XES& se) override {
    // const Route& rep = se.first.getR();
    return OrOpt1_2_3->randomMove(se);
  }

  /*
        virtual uptr<Move<XES, XSH>> validRandomMove(const XES& se) override
        {
      const Route& r = se.first.getR();
                uptr<Move<XES, XSH>> m = move(se);
                if (m->canBeApplied(se))
                        return m;
                else
                {
                        //delete m;
                        return nullptr;
                }
        }
*/

  uptr<NSIterator<XES>> getIterator(const XES& se) override {
    return OrOpt1_2_3->getIterator(se);
  }

  static string idComponent() {
    stringstream ss;
    ss << NSSeq<XES>::idComponent() << ":NSSeqTSPOrOpt";
    return ss.str();
  }

  string id() const override { return idComponent(); }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (NSSeq<XES>::compatible(s));
  }

  std::string toString() const override {
    stringstream ss;
    ss << "NSSeqVectorOrOpt{1..3}";
    return ss.str();
  }
};

// compile tests
// using mynsseq_nsseq_tsp_2opt_test = NSSeqTSP2Opt<int, short,
// IsSolution<vector<int>, short>, IsEvaluation<double>,
// pair<IsSolution<vector<int>, short>, IsEvaluation<double>> >;
using mynsseq_nsseq_tsp_oropt_test =
    NSSeqTSPOrOpt<int, short, IsSolution<vector<int>, short>>;
//
static_assert(
    std::is_base_of<nsseq_test_base, mynsseq_nsseq_tsp_oropt_test>::value,
    "not inherited from NSSeq");

}  // namespace optframe

#endif /*OPTFRAME_NSSEQVECTOROROPT_HPP_*/
