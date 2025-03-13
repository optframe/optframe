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

#ifndef OPTFRAME_NSSEQVVSWAPINTRA_HPP_
#define OPTFRAME_NSSEQVVSWAPINTRA_HPP_

// Framework includes
#include <OptFrame/Core/NSSeq.hpp>

#include "../Move.hpp"
#include "../NSIterator.hpp"
#include "Moves/MoveVVSwapIntra.hpp"
#include "NSIterators/IteratorVVSwapIntra.hpp"

using namespace std;

// Working structure: vector<vector<T> >

template <class T, class DS = OPTFRAME_DEFAULT_EMEMORY>
class NSSeqVVSwapIntra : public NSSeq<vector<vector<T>>, DS> {
  typedef vector<vector<T>> Routes;

 public:
  NSSeqVVSwapIntra() {}

  virtual ~NSSeqVVSwapIntra() {}

  Move<Routes, DS>& move(const Routes& rep) {
    int i = rand() % rep.size();

    int n = rep.at(i).size();

    int j = -1;

    if (n > 0) j = rand() % n;

    int k = j;

    if (n > 1)
      while (k == j) k = rand() % n;

    return *new MoveVVSwapIntra<T, DS>(i, j, k);
  }

  virtual NSIterator<Routes, DS>& getIterator(const Routes& r) {
    return *new NSIteratorVVSwapIntra<T, DS>(r);
  }

  virtual void print() { cout << "NSSeqVVSwapIntra" << endl; }
};

#endif /*OPTFRAME_NSSEQVVSWAPINTRA_HPP_*/
