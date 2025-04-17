// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2025 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_NSITERATOR_HPP_
#define OPTFRAME_NSITERATOR_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

#include <OptFrame/Component.hpp>
#include <OptFrame/Concepts/BaseConcepts.hpp>
#include <OptFrame/Core/Move.hpp>
#define MOD_EXPORT
#else

// CANNOT IMPORT HERE... Already part of optframe.core
/*
import std;
import optframe.component;
import optframe.concepts;
*/

// do NOT import/export modules on .hpp... only on .cppm

#define MOD_EXPORT export

#endif

namespace optframe {

// Local Optimum Status
MOD_EXPORT enum LOS { los_yes, los_no, los_unknown, los_partial };

// exemplo: mochila NSSeq de incrementos (+1).
// Comeca 'unknown'.
// Se nao pode mais incrementar nada vira 'yes'.
// Se perturba um pouquinho vira 'no' (pq sabe como voltar).

// 'partial' indicates that the solution is partially in a local optimum (maybe
// the word semi-local optimum is better...) it may happen for example in a VRP,
// where a route is in LO while others are not.

// TODO: unused?
class IteratorOutOfBound {
 private:
  int id;

 public:
  IteratorOutOfBound(int _id) : id(_id) {}

  int getId() { return id; }
};

MOD_EXPORT template <XESolution XES, XESolution XSH = XES>
class NSIterator : public Component {
 public:
  ~NSIterator() override = default;

  virtual void first() = 0;

  virtual void firstValid(const XES& se) {
    first();

    while (!isDone()) {
      uptr<Move<XES, XSH>> m = current();

      if (m && m->canBeApplied(se)) break;

      next();
    }
  }

  virtual void next() = 0;

  virtual void nextValid(const XES& se) {
    next();

    while (!isDone()) {
      uptr<Move<XES, XSH>> m = current();

      if (m && m->canBeApplied(se)) break;

      next();
    }
  }

  virtual bool isDone() = 0;
  virtual uptr<Move<XES, XSH>> current() = 0;

  // INSERT LOCAL OPTIMUM INFORMATION IN SOLUTION (IN ADS? USER DECIDES.)
  virtual void setLOS(LOS status, XES& s) {}

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":NSIterator";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  virtual std::string toString() const override { return id(); }
};

}  // namespace optframe

#endif  // OPTFRAME_NSITERATOR_HPP_
