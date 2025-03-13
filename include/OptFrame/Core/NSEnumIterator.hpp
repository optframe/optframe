// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2025 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_NSENUMITERATOR_HPP_
#define OPTFRAME_NSENUMITERATOR_HPP_

#if (__cplusplus < 202302L) || defined(NO_CXX_MODULES)

#include <OptFrame/Core/Move.hpp>
#include <OptFrame/Core/NSIterator.hpp>
#define MOD_EXPORT
#else

// CANNOT IMPORT HERE... Already part of optframe.core
/*
import std;
import optframe.component;
import optframe.concepts;
*/

// do NOT export modules on .hpp... only on .cppm

#define MOD_EXPORT export

#endif

namespace optframe {

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
MOD_EXPORT template <XESolution XES, XSearch<XES> XSH>
#else
MOD_EXPORT template <typename XES, typename XSH>
#endif
class NSEnum;

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
MOD_EXPORT template <XESolution XES, XSearch<XES> XSH = XES>
#else
MOD_EXPORT template <typename XES, typename XSH = XES>
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
