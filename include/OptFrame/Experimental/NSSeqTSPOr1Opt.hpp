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

#ifndef OPTFRAME_NSSEQTSP0R1OPT_HPP_
#define OPTFRAME_NSSEQTSP0R1OPT_HPP_

#include "../Move.hpp"
#include "../NSIterator.hpp"
#include "./Moves/MoveTSPOr1Opt.hpp"
#include "./NSIterators/IteratorNSSeqTSPOr1Opt.hpp"

// Framework includes
#include <OptFrame/Core/NSSeq.hpp>

using namespace std;

/*
 Classical Problem: Traveling Salesman Problem

 The Neighborhood Structure Or-Opt has proposed by ??

 Or-Opt is aplied for any problem that representation is like a vector<T>, where
 T is the type of the vector.

 e.g: vector<T> where type of T is int
 s means Solution

 initial s: 1 2 3 4 5 6 7 8

 s' after apply MoveTSPOr1Opt(2,5) in s

 s': 1 2 4 5 3 6 7 8

 s" after apply MoveTSP2Opt(3,8) in s'

 s": 1 2 4 3 6 7 5 8

 */

template <class T, class DS = OPTFRAME_DEFAULT_EMEMORY>
class NSSeqTSPOr1Opt : public NSSeq<vector<T>, DS> {
 private:
  typedef vector<T> Route;

 public:
  NSSeqTSPOr1Opt() {}

  virtual ~NSSeqTSPOr1Opt() {}

  using NSSeq<Route, DS>::move;
  using NSSeq<Route, DS>::getIterator;

  Move<Route, DS>& move(const Route& rep) {
    if (rep.size() < 2) return *new MoveTSPOr1Opt<T, DS>(-1, -1);

    int c = rand() % rep.size();

    int pos = rand() % (rep.size() + 1);

    return *new MoveTSPOr1Opt<T, DS>(c, pos);
  }

  virtual NSIterator<Route, DS>& getIterator(const Route& r) {
    return *new IteratorNSSeqTSPOr1Opt<T, DS>(r);
  }

  virtual void print() { std::cout << "NSSeqTSPOr1Opt" << std::endl; }
};

#endif /*OPTFRAME_NSSEQTSP0R1OPT_HPP_*/
