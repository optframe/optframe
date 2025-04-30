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

#ifndef OPTFRAME_ITERATORNSSEQUNION_HPP_
#define OPTFRAME_ITERATORNSSEQUNION_HPP_

// Framework includes

#include <OptFrame/Core/Move.hpp>
#include <OptFrame/Core/NSIterator.hpp>

#include "../Moves/MoveNSSeqUnion.hpp"

using namespace std;

class NSSeqUnionAdapterOutOfBound {
 public:
  NSSeqUnionAdapterOutOfBound() {}
};

// template<class T, class ADS = OPTFRAME_DEFAULT_ADS,
// XBaseSolution<vector<T>,ADS> S = CopySolution<vector<T>,ADS>, class MOVE =
// MoveTSPSwap<T, ADS>, class P = OPTFRAME_DEFAULT_PROBLEM, XEvaluation XEv =
// Evaluation<>> template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class MOVE
// = MoveNSSeqUnion<R, ADS>,  >
template <class R, class ADS = OPTFRAME_DEFAULT_ADS,
          XBaseSolution<R, ADS> S = CopySolution<R, ADS>,
          XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>,
          class MOVE = MoveNSSeqUnion<S, XEv, XES>>
class IteratorNSSeqUnion : public NSIterator<XES> {
 private:
  vector<uptr<NSIterator<XES>>> it;
  int k;

 public:
  /*
  // ERROR! CANNOT PASS uptr CORRECTLY!
       IteratorNSSeqUnion(std::vector<uptr<NSIterator<XES>>>&& _it) :
                       it(_it)
       {
               k = 0;
       }
*/
  // these pointers are passed together with ownership!! never delete them!
  IteratorNSSeqUnion(std::vector<NSIterator<XES>*> _it) {
    k = 0;
    for (unsigned i = 0; i < _it.size(); i++)
      it.push_back(uptr<NSIterator<XES>>(_it[i]));
  }

  virtual ~IteratorNSSeqUnion() {
    // for (unsigned int i = 0; i < it.size(); i++)
    //	delete it[i];
  }

  void first() override {
    for (unsigned int i = 0; i < it.size(); i++) it[i]->first();
    k = 0;
  }

  void next() override {
    if (!it[k]->isDone())
      it[k]->next();
    else {
      k++;
      it[k]->next();
    }
  }

  bool isDone() override {
    for (unsigned int i = 0; i < it.size(); i++)
      if (!it[i]->isDone()) return false;

    return true;
  }

  // Move<R, ADS>& current() override
  uptr<Move<XES>> current() override {
    if (!it[k]->isDone())
      return uptr<Move<XES>>(new MOVE(k, it[k]->current().release()));
    else
      // throw NSSeqUnionAdapterOutOfBound();
      return nullptr;
  }
};

#endif /*OPTFRAME_ITERATORNSSEQUNION_HPP_*/
