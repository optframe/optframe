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

#ifndef OPTFRAME_NSENUMITERATOR_HPP_
#define OPTFRAME_NSENUMITERATOR_HPP_

#include <OptFrame/Move.hpp>
#include <OptFrame/NSIterator.hpp>

// using namespace std;

namespace optframe {

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES, XSearch<XES> XSH>
#else
template <typename XES, typename XSH>
#endif
class NSEnum;

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES, XSearch<XES> XSH = XES>
#else
template <typename XES, typename XSH = XES>
#endif
class NSEnumIterator : public NSIterator<XES, XSH> {
 private:
  NSEnum<XES, XSH>& ns;
  unsigned int move;
  unsigned int nsSize;

 public:
  NSEnumIterator(NSEnum<XES, XSH>& _ns) : ns(_ns) {
    move = 0;
    nsSize = _ns.size();
  }

  virtual ~NSEnumIterator() {}

  void first() override { move = 0; }

  void next() override { move++; }

  bool isDone() override { return move >= nsSize; }

  uptr<Move<XES, XSH>> current() override {
    if (isDone())
      // throw IteratorOutOfBound(move);
      return nullptr;
    return ns.indexMove(move);
  }

  uptr<Move<XES, XSH>> at(unsigned int m) {
    // TODO: throw exception if m >= size
    return ns.move(m);
  }

  unsigned int size() { return nsSize; }
};

}  // namespace optframe

#endif  // OPTFRAME_NSENUMITERATOR_HPP_
