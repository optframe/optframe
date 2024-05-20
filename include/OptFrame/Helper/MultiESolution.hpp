// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HELPER_MULTIESOLUTION_HPP_
#define OPTFRAME_HELPER_MULTIESOLUTION_HPP_

#include <vector>
//
#include <OptFrame/Component.hpp>
#include <OptFrame/Evaluation.hpp>
#include <OptFrame/Helper/MultiSolution.hpp>
#include <OptFrame/Helper/Solution.hpp>

namespace optframe {

// Pointer-based "EPopulation" structure (DEPRECATED!)
template <XESolution XES>
class MultiESolution : public Component {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;

 protected:
  std::vector<S*> p;
  std::vector<XEv*> pev;

 public:
  using value_type = XES;

  MultiESolution() {}

  MultiESolution(const MultiESolution& pop) {
    for (unsigned i = 0; i < pop.size(); i++) {
      // NOLINTNEXTLINE
      S* s = new S{pop.at(i).first};
      p.push_back(s);
    }
  }

  ~MultiESolution() override { clear(); }

  unsigned size() const { return p.size(); }

  // todo: implement
  XES& getP(size_t i) {
    // return make_pair(uptr<S>(),uptr<XEv>());
    XES* p;
    return *p;
  }

  // CANNOT IMPLEMENT THIS!
  XES& at(size_t i) {
    XES* p = nullptr;
    assert(false);
    return *p;
  }

  // CANNOT IMPLEMENT THIS!
  const XES& at(size_t i) const {
    XES* p = nullptr;
    assert(false);
    return *p;
  }

  void insert(unsigned pos, S& c) { p.insert(p.begin() + pos, new S(c)); }

  void push_back(S* c) {
    if (c)  // not null
      p.push_back(c);
  }

  void push_back(const S& c) {
    p.push_back(new S(c));  // copy constructor
  }

  S& remove(unsigned pos) {
    S& c = *p.at(pos);
    p.erase(p.begin() + pos);
    return c;
  }

  void add(const MultiSolution<S>& pop) {
    for (unsigned i = 0; i < pop.size(); i++) {
      const S& s = pop.at(i);
      push_back(s);
    }
  }

  // clear and kill
  void clear() {
    for (unsigned i = 0; i < p.size(); i++) delete p.at(i);

    p.clear();
  }

  void clearNoKill() { p.clear(); }

  bool empty() { return p.empty(); }

  MultiESolution<XES>& operator=(const MultiESolution<XES>& p) {
    if (&p == this)  // auto ref check
      return *this;

    unsigned sizePop = this->p.size();

    for (unsigned i = 0; i < sizePop; i++) {
      if (this->p.at(i)) {
        // If no nullptr pointing.
        delete this->p.at(i);
      }
    }

    this->p.clear();

    sizePop = p.size();

    for (unsigned i = 0; i < sizePop; i++) {
      if (&p.at(i))  // If no nullptr pointing.
      {
        this->p.push_back(new S(p.at(i)));
      } else {
        this->p.push_back(nullptr);
      }
    }

    return (*this);
  }

  virtual MultiESolution<XES>& clone() const {
    return *new MultiESolution<XES>(*this);
  }

  static string idComponent() {
    stringstream ss;
    ss << Component::idComponent() << ":MultiESolution";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  virtual std::string toString() const override { return id(); }

  void print() const override {
    cout << "MultiESolution(" << p.size() << ")";
    cout << endl;

    for (unsigned i = 0; i < p.size(); i++) {
      cout << *p.at(i) << endl;
    }
  }
};

// basic compilation test (TODO: improve)
// static_assert(X2ESolution<MultiESolution<IsESolution<double>>,
// IsSolution<double>, IsEvaluation<int>, IsESolution<double>>);
// static_assert(X2ESolution<MultiESolution<IsSolution<double>,
// IsEvaluation<int>, IsESolution<double>>, IsESolution<double>>);

}  // namespace optframe

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
// TODO: only in unit tests (or somehow prevent #include "printable.h")
#include "MultiESolution.ctest.hpp"

#endif  // cpp_concepts

#endif  // OPTFRAME_HELPER_MULTIESOLUTION_HPP_
