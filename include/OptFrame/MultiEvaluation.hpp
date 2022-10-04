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

#ifndef OPTFRAME_MULTIEVALUATION_HPP_
#define OPTFRAME_MULTIEVALUATION_HPP_

#include <string>
#include <utility>
#include <vector>
//

#include <OptFrame/Evaluation.hpp>

// using namespace std;

namespace optframe {

// evtype is default (double?)

template <optframe::objval ObjType = evtype>
class MultiEvaluation : public Component {
 public:
  // internal Evaluation type
  using XEv = Evaluation<ObjType>;
  // this is not 'double' but a 'vector of double'
  using objType = std::vector<XEv>;

 protected:
  // internal structure is a pack of XEv
  std::vector<XEv> vev;

 public:
  // this 'outdated' should be true, when any
  // bool outdated{true};

 public:
  MultiEvaluation() {
    static_assert(XEvaluation<MultiEvaluation<ObjType>>);
  }

  explicit MultiEvaluation(Evaluation<ObjType> ev) {
    vev.push_back(ev);
    static_assert(XEvaluation<MultiEvaluation<ObjType>>);
  }

  explicit MultiEvaluation(const vector<ObjType>& vd) {
    for (unsigned i = 0; i < vd.size(); i++)
      vev.push_back(XEv{vd[i]});
  }

  MultiEvaluation(const MultiEvaluation<ObjType>& mev) {
    for (unsigned i = 0; i < mev.vev.size(); i++)
      vev.push_back(mev.vev[i]);
  }

  MultiEvaluation(MultiEvaluation<ObjType>&& mev)
      : vev(std::move(mev.vev)) {
  }

  virtual ~MultiEvaluation() {
    this->clear();
  }

  // ===============
  // getters/setters
  // ===============

  // if any objective is outdated, this is also outdated
  bool isOutdated() {
    for (unsigned i = 0; i < vev.size(); i++)
      if (vev[i].isOutdated())
        return true;
    return false;
  }

  // makes all evaluations 'outdated'
  void invalidate() {
    for (unsigned i = 0; i < vev.size(); i++)
      vev[i].invalidate();
  }

  // if any objective is estimated, this is also estimated
  bool isEstimated() {
    for (unsigned i = 0; i < vev.size(); i++)
      if (vev[i].isEstimated())
        return true;
    return false;
  }

  std::vector<XEv>& evaluation() {
    return vev;
  }

  void print() {
    for (unsigned i = 0; i < vev.size(); i++)
      vev[i].print();
  }

  void addEvaluation(Evaluation<ObjType>& ev) {
    vev.push_back(ev);
  }

  void addEvaluation(Evaluation<ObjType>&& ev) {
    vev.push_back(std::move(ev));
  }

  unsigned size() const {
    return vev.size();
  }

  void erase(unsigned index) {
    vev.erase(vev.begin() + index);
  }

  XEv& at(unsigned index) {
    return vev[index];
  }

  const XEv& at(unsigned index) const {
    return vev[index];
  }

  evtype atObjVal(unsigned index) {
    return vev[index].evaluation();
  }

  MultiEvaluation<ObjType> diff(const MultiEvaluation<ObjType>& other) const {
    MultiEvaluation<ObjType> r = *this;
    for (unsigned i = 0; i < r.size(); i++)
      r.at(i) = r.at(i).diff(other.at(i));
    return r;
  }

  XEv& operator[](unsigned index) {
    return vev[index];
  }

  const XEv& operator[](unsigned index) const {
    return vev[index];
  }

  void setOutdated(unsigned index, bool status) {
    vev[index].outdated = status;
  }

  virtual MultiEvaluation<ObjType>& operator=(const MultiEvaluation<ObjType>& mev) {
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

  void clear() {
    this->vev.clear();
  }

  // update Evaluation with costs
  virtual void update(MultiEvaluation<ObjType>& mevTarget) const {
    // this task was performed before by MoveCost... now unifying in Evaluation
    // how to do this?
    assert(false);
  }

  void print() const override {
    cout << toString() << endl;
  }

  std::string toString() const override {
    std::stringstream ss;
    ss << "MultiXEv (" << vev.size() << "):";
    for (unsigned i = 0; i < vev.size(); i++)
      ss << vev[i].toString() << endl;
    return ss.str();
  }

  friend std::ostream& operator<<(std::ostream& os, const MultiEvaluation& me) {
    os << me.toString();
    return os;
  }
};

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

// Compilation test for concepts
// MultiEvaluation is considered an XEvaluation type (compatible with past implementations)
// TODO: remove this
static_assert(XEvaluation<MultiEvaluation<double>>);

//#ifndef NDEBUG
struct optframe_debug_test_multievaluation {
  MultiEvaluation<double> testEvaluation;
};
//#endif

}  // namespace optframe

#endif  // OPTFRAME_MULTIEVALUATION_HPP_
