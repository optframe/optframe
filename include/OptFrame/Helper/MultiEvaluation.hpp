// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HELPER_MULTIEVALUATION_HPP_
#define OPTFRAME_HELPER_MULTIEVALUATION_HPP_

#include <string>
#include <utility>
#include <valarray>
#include <vector>
//
#include <OptFrame/Concepts/BaseConcepts.hpp>
#include <OptFrame/Evaluation.hpp>

namespace optframe {

// evtype is default (double?)

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <optframe::objval ObjType = evtype>
#else
template <typename ObjType = evtype>
#endif
class MultiEvaluation : public Component {
 public:
  // internal Evaluation type
  using XEv = Evaluation<ObjType>;
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
  static_assert(basic_arithmetics<XEv>);
  static_assert(objval<typename XEv::objType>);
#endif
  // this is not 'double' but a 'vector of double'
  using objType = std::valarray<XEv>;

 protected:
  // internal structure is a pack of XEv
  std::vector<XEv> vev;

 public:
  // this 'outdated' should be true, when any
  // bool outdated{true};

 public:
  MultiEvaluation() {
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
    static_assert(XEvaluation<MultiEvaluation<ObjType>>);
#endif
  }

  explicit MultiEvaluation(Evaluation<ObjType> ev) {
    vev.push_back(ev);
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
    static_assert(XEvaluation<MultiEvaluation<ObjType>>);
#endif
  }

  explicit MultiEvaluation(const vector<ObjType>& vd) {
    for (unsigned i = 0; i < vd.size(); i++) vev.push_back(XEv{vd[i]});
  }

  MultiEvaluation(const MultiEvaluation<ObjType>& mev) {
    for (unsigned i = 0; i < mev.vev.size(); i++) vev.push_back(mev.vev[i]);
  }

  MultiEvaluation(MultiEvaluation<ObjType>&& mev) noexcept
      : vev(std::move(mev.vev)) {}

  ~MultiEvaluation() override { this->clear(); }

  // ===============
  // getters/setters
  // ===============

  // if any objective is outdated, this is also outdated
  bool isOutdated() const {
    for (unsigned i = 0; i < vev.size(); i++)
      if (vev[i].isOutdated()) return true;
    return false;
  }

  // makes all evaluations 'outdated'
  void invalidate() {
    for (unsigned i = 0; i < vev.size(); i++) vev[i].invalidate();
  }

  // if any objective is estimated, this is also estimated
  bool isEstimated() {
    for (unsigned i = 0; i < vev.size(); i++)
      if (vev[i].isEstimated()) return true;
    return false;
  }

  // this is 'valarray', not vector! needs to support operators +/-
  std::valarray<XEv>& evaluation() { return vev; }

  void addEvaluation(Evaluation<ObjType>& ev) { vev.push_back(ev); }

  void addEvaluation(Evaluation<ObjType>&& ev) { vev.push_back(std::move(ev)); }

  unsigned size() const { return vev.size(); }

  void erase(unsigned index) { vev.erase(vev.begin() + index); }

  XEv& at(unsigned index) { return vev[index]; }

  const XEv& at(unsigned index) const { return vev[index]; }

  evtype atObjVal(unsigned index) { return vev[index].evaluation(); }

  MultiEvaluation<ObjType> diff(const MultiEvaluation<ObjType>& other) const {
    MultiEvaluation<ObjType> r = *this;
    for (unsigned i = 0; i < r.size(); i++) r.at(i) = r.at(i).diff(other.at(i));
    return r;
  }

  /*
  MultiEvaluation operator-(const MultiEvaluation& other) const {
    return this->diff(other);
  }
  */

  MultiEvaluation& operator-=(const MultiEvaluation& e) {
    vev -= e.vev;
    return *this;
  }

  MultiEvaluation& operator+=(const MultiEvaluation& e) {
    vev += e.vev;
    return *this;
  }

  friend MultiEvaluation operator-(MultiEvaluation lhs,
                                   const MultiEvaluation& rhs) {
    lhs -= rhs;
    return lhs;
  }

  friend MultiEvaluation operator+(MultiEvaluation lhs,
                                   const MultiEvaluation& rhs) {
    lhs += rhs;
    return lhs;
  }

  // ============

  XEv& operator[](unsigned index) { return vev[index]; }

  const XEv& operator[](unsigned index) const { return vev[index]; }

  void setOutdated(unsigned index, bool status) {
    vev[index].outdated = status;
  }

  MultiEvaluation<ObjType>& operator=(const MultiEvaluation<ObjType>& mev) {
    if (&mev == this)  // auto ref check
      return *this;

    this->vev.clear();
    for (unsigned i = 0; i < mev.vev.size(); i++)
      this->vev.push_back(mev.vev[i]);

    return *this;
  }

  virtual MultiEvaluation<ObjType>& clone() const {
    return *new MultiEvaluation(*this);
  }

  void clear() { this->vev.clear(); }

  // update Evaluation with costs
  virtual void update(MultiEvaluation<ObjType>& mevTarget) const {
    // this task was performed before by MoveCost... now unifying in Evaluation
    // how to do this?
    assert(false);
  }

  void print() const override { cout << toString() << endl; }

  std::string toString() const override {
    std::stringstream ss;
    ss << "MultiEvaluation (" << vev.size() << "):";
    if (isOutdated()) {
      ss << "OUTDATED";
    } else if (vev.size() == 0) {
      ss << "EMPTY";
    } else {
      ss << " [";  // << std::endl;
      for (unsigned i = 0; i < vev.size(); i++)
        ss << vev[i].evaluation() << "; ";  // <<  std::endl;
      ss << "]";
    }
    return ss.str();
  }

  friend std::ostream& operator<<(std::ostream& os, const MultiEvaluation& me) {
    os << me.toString();
    return os;
  }
};

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
/*
optframe::evgoal<Self>&&
   HasClone<Self>&&
         HasToString<Self>&&
            HasGetObj<Self>&&
               optframe::ostreamable<Self>&& requires(Self e) {
                  typename Self::objType;
               } &&
*/
static_assert(optframe::evgoal<MultiEvaluation<double>>);

// ========= Compilation test for concepts =========
// Evaluation needs to be `basic_arithmetics`, to be part of MultiEvaluation
static_assert(optframe::basic_arithmetics<Evaluation<double>>);

// MultiEvaluation is considered an XEvaluation type and also XMEvaluation
static_assert(
    optframe::basic_arithmetics<typename MultiEvaluation<double>::objType>);
static_assert(XEvaluation<MultiEvaluation<double>>);
static_assert(XMEvaluation<MultiEvaluation<double>>);
#endif

// #ifndef NDEBUG
struct optframe_debug_test_multievaluation {
  MultiEvaluation<double> testEvaluation;
};
// #endif

}  // namespace optframe

#endif  // OPTFRAME_HELPER_MULTIEVALUATION_HPP_
