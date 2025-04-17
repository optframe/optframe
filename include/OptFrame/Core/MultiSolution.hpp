// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2025 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HELPER_MULTISOLUTION_HPP_
#define OPTFRAME_HELPER_MULTISOLUTION_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

// C++
#include <string>
#include <utility>
#include <vector>
//
#include <OptFrame/Component.hpp>
#include <OptFrame/Helper/Solution.hpp>

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

MOD_EXPORT template <XSolution S>
class MultiSolution : public Component {
 protected:
  std::vector<S> p;

 public:
  MultiSolution() {}

  MultiSolution(const MultiSolution& pop) {
    for (unsigned i = 0; i < pop.size(); i++) p.push_back(pop.at(i));
  }

  virtual ~MultiSolution() { clear(); }

  unsigned size() const { return p.size(); }

  S& at(unsigned c) { return (p.at(c)); }

  const S& at(unsigned c) const { return (p.at(c)); }

  void insert(unsigned pos, S& c) { p.insert(p.begin() + pos, S{c}); }

  /*
  void push_back(S* c) {
    if (c)  // not null
      p.push_back(c);
  }
  */

  void push_back(const S& c) { p.push_back(c); }

  S remove(unsigned pos) {
    // steal
    S c = std::move(p.at(pos));
    p.erase(p.begin() + pos);
    return c;
  }

  void add(const MultiSolution<S>& pop) {
    for (unsigned i = 0; i < pop.size(); i++) {
      const S& s = pop.at(i);
      push_back(s);
    }
  }

  // clear
  void clear() { p.clear(); }

  bool empty() { return p.empty(); }

  virtual MultiSolution<S>& operator=(const MultiSolution<S>& p) {
    if (&p == this)  // auto ref check
      return *this;
    // simple
    this->p = p.p;
    return (*this);
  }

  /*
  virtual MultiSolution<S>& clone() const {
    return *new MultiSolution<S>(*this);
  }
*/

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":MultiSolution";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
  std::string toString() const override {
    std::stringstream ss;
    ss << "MultiSolution(" << p.size() << ")";
    ss << std::endl;
    for (unsigned i = 0; i < p.size(); i++) {
      ss << p.at(i);
    }
    return ss.str();
  }

  void print() const override { std::cout << toString() << std::endl; }
};

}  // namespace optframe

#endif  // OPTFRAME_HELPER_MULTISOLUTION_HPP_
