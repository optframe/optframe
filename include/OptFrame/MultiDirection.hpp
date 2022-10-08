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

#ifndef OPTFRAME_MULTI_DIRECTION_HPP_
#define OPTFRAME_MULTI_DIRECTION_HPP_

// C
#include <float.h>
// C++
#include <iostream>
#include <limits>
//
#include <OptFrame/Evaluation.hpp>
#include <OptFrame/Move.hpp>
#include <OptFrame/MoveCost.hpp>
// #include "Solution.hpp"

#include <OptFrame/Component.hpp>
#include <OptFrame/Hyper/Action.hpp>

// using namespace std;
// using namespace scannerpp;

namespace optframe {

// ===============================================
// this is homogeneous multi-obj with obj type XEv
// ===============================================

template <XEvaluation XEv = Evaluation<>>
class MultiDirection : public Component {
  // using XEv = Evaluation<>;

 protected:
  vsref<Direction<XEv>> vDir;

 public:
  unsigned nObjectives;

  explicit MultiDirection(vsref<Direction<XEv>>& _vDir) : vDir{_vDir} {
    // for (unsigned i = 0; i < _vDir.size(); i++)
    //  if (_vDir[i])
    //    vDir.push_back(_vDir[i]);
    nObjectives = vDir.size();
  }

  MultiDirection(MultiDirection& mDir)
      : vDir(mDir.vDir), nObjectives(mDir.nObjectives) {
  }

  MultiDirection() {
    nObjectives = 0;
  }

  virtual ~MultiDirection() {
  }

  virtual MultiDirection& addObjective(Direction<XEv>* ds) {
    if (ds)
      vDir.push_back(ds);
    nObjectives = vDir.size();

    return *this;
  }

  vsref<Direction<XEv>> getDirections() {
    return vDir;
  }

  // ============ betterThan ===========

  template <class T>
  inline bool betterThanAt(unsigned obj, const T& a, const T& b) {
    return vDir[obj]->betterThan(a, b);
  }

  // ============ betterOrEquals ===========

  template <class T>
  inline bool betterOrEqualsAt(unsigned obj, const T& a, const T& b) {
    return vDir[obj]->betterOrEquals(a, b);
  }

  // ============ equals ============

  template <class T>
  inline bool equalsAt(unsigned obj, const T& a, const T& b) {
    return vDir[obj]->equals(a, b);
  }

  // ============= improvement =============

  ///inline bool isImprovement(unsigned obj, const MoveCost<>& mc, const Evaluation<>& e1, const Evaluation<>& e2)
  inline bool isImprovementAt(unsigned obj, const Evaluation<>& mc, const Evaluation<>& e1, const Evaluation<>& e2) {
    return vDir[obj]->isImprovement(mc, e1, e2);
  }

  ///inline bool isImprovement(unsigned obj, const MoveCost<>& mc)
  inline bool isImprovementAt(unsigned obj, const Evaluation<>& mc) {
    return vDir[obj]->isImprovement(mc);
  }

  // ============= direction ==============

  inline bool isMinimizationAt(unsigned obj) {
    return vDir[obj]->isMinimization();
  }

  inline bool isMaximizationAt(unsigned obj) {
    return vDir[obj]->isMaximization();
  }

  // ============ estimation =============

  inline double ideal(unsigned obj) {
    return vDir[obj]->ideal();
  }

  // inline double worst(unsigned obj)
  inline double nadir(unsigned obj) {
    return vDir[obj]->nadir();
  }

  inline double min(unsigned obj) {
    return vDir[obj]->min();
  }

  inline double max(unsigned obj) {
    return vDir[obj]->max();
  }

  // ============= Component ===============

  virtual bool compatible(string s) override {
    return (s == idComponent()) || (Component::compatible(s));
  }

  virtual std::string toString() const override {
    return id();
  }

  static string idComponent() {
    stringstream ss;
    ss << Component::idComponent() << ":MultiDirection";
    return ss.str();
  }

  virtual string id() const override {
    return idComponent();
  }
};

}  // namespace optframe

#endif /*OPTFRAME_MULTI_DIRECTION_HPP_*/
