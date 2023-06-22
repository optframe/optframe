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

#ifndef OPTFRAME_NSSEQUNIONADAPTER_HPP_
#define OPTFRAME_NSSEQUNIONADAPTER_HPP_

// C++
#include <string>
// Framework includes
#include "../../Move.hpp"
#include "../../NSIterator.hpp"
#include "../../NSSeq.hpp"
#include "./Moves/MoveNSSeqUnion.hpp"
#include "./NSIterators/IteratorNSSeqUnion.hpp"

using namespace std;  // NOLINT

// template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class MOVE =
// MoveNSSeqUnion<R, ADS> >
template <class R, class ADS = OPTFRAME_DEFAULT_ADS,
          XBaseSolution<R, ADS> S = CopySolution<R, ADS>,
          XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>,
          class MOVE = MoveNSSeqUnion<S, XEv, XES>>
class NSSeqUnionAdapter : public NSSeq<XES> {
 private:
  vector<NSSeq<XES>*> ns;

 public:
  NSSeqUnionAdapter(NSSeq<XES>& _n1, NSSeq<XES>& _n2) {
    ns.push_back(&_n1);
    ns.push_back(&_n2);
  }

  void add_ns(NSSeq<XES>& _ns) { ns.push_back(&_ns); }

  virtual ~NSSeqUnionAdapter() {}

  // Move<R, ADS>& move(const R& r, const ADS& ads)
  uptr<Move<XES>> randomMove(const XES& se) override {
    // const R& r = s.first.getR();
    int x = rand() % ns.size();

    uptr<Move<XES>> mvv = ns[x]->randomMove(se);
    MoveNSSeqUnion<S, XEv, XES>* pm =
        new MoveNSSeqUnion<S, XEv, XES>(x, mvv.release());

    // uptr<Move<XES>> mvv2 = std::move(mvv);
    // MoveNSSeqUnion<S, XEv, XES> *pm = new MoveNSSeqUnion<S, XEv, XES>(x,
    // std::move(mvv2));
    Move<XES>* pm2 = pm;
    uptr<Move<XES>> mv2(pm2);
    return mv2;
  }

  /*
       //Move<R, ADS>* validMove(const R& r, const ADS& ads) override
  uptr<Move<XES>> validRandomMove(const R& r, const ADS& ads) override
       {
               Move<R, ADS>* m = &move(r, ads);
               if(m->canBeApplied(r, ads))
                       return m;
               else
               {
                       delete m;
                       return nullptr;
               }
       }
  */

  virtual uptr<NSIterator<XES>> getIterator(const XES& se) override {
    // vector<uptr<NSIterator<XES>>> it;
    vector<NSIterator<XES>*> it;
    for (unsigned int i = 0; i < ns.size(); i++)
      it.push_back(ns[i]->getIterator(se).release());

    return uptr<NSIterator<XES>>(
        new IteratorNSSeqUnion<R, ADS, S, XEv, XES, MOVE>(it));
  }

  string toString() const override {
    stringstream ss;
    ss << "NSSeqUnionAdapter: [";
    for (unsigned int i = 0; i < ns.size(); i++) {
      ss << ns[i]->toString();

      if (i != ns.size() - 1) ss << ",";
    }
    ss << "] ";
    return ss.str();
  }
};

#endif /*OPTFRAME_NSSEQUNIONADAPTER_HPP_*/
