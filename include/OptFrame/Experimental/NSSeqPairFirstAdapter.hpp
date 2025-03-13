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

#ifndef OPTFRAME_NSSEQ_PAIRFIRST_ADAPTER_HPP_
#define OPTFRAME_NSSEQ_PAIRFIRST_ADAPTER_HPP_

// Framework includes
#include <OptFrame/Core/NSSeq.hpp>

#include "../Move.hpp"
#include "../NSIterator.hpp"
#include "Moves/MovePairFirst.hpp"
#include "NSIterators/IteratorNSSeqPairFirst.hpp"

using namespace std;

template <class T1, class T2, class DS = OPTFRAME_DEFAULT_EMEMORY,
          class MOVE = MovePairFirst<T1, T2, DS>>
class NSSeqPairFirstAdapter : public NSSeq<pair<T1, T2>, DS> {
 protected:
  NSSeq<T1, DS>& ns;

 public:
  NSSeqPairFirstAdapter(NSSeq<T1, DS>& _ns) : ns(_ns) {}

  virtual ~NSSeqPairFirstAdapter() {}

  Move<pair<T1, T2>, DS>& move(const pair<T1, T2>& r) {
    return *new MOVE(ns.move(r.first));
  }

  virtual NSIterator<pair<T1, T2>, DS>& getIterator(const pair<T1, T2>& r) {
    return *new IteratorNSSeqPairFirst<T1, T2, DS, MOVE>(
        ns.getIterator(r.first));
  }

  virtual void print() {
    cout << "NSSeqPairFirstAdapter {" << endl;
    ns.print();
    cout << "}" << endl;
  }
};

#endif /*OPTFRAME_NSSEQ_PAIRFIRST_ADAPTER_HPP_*/
