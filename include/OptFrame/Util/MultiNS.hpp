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

#ifndef OPTFRAME_MULTI_NS_HPP_
#define OPTFRAME_MULTI_NS_HPP_

#include <vector>

#include "../Move.hpp"
#include "../NS.hpp"
#include "../RandGen.hpp"
#include "../Solution.hpp"

namespace optframe {

template <class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class MultiMove : public Move<R, ADS, DS> {
 public:
  vector<Move<R, ADS, DS>*> vm;

  MultiMove(vector<Move<R, ADS, DS>*>& _vm)
      : vm(_vm) {
  }

  virtual ~MultiMove() {
    for (unsigned i = 0; i < vm.size(); i++)
      delete vm[i];
    vm.clear();
  }

  virtual bool canBeApplied(const R& r, const ADS& ads) {
    for (unsigned i = 0; i < vm.size(); i++)
      if (!vm[i]->canBeApplied(r, ads))
        return false;
    // MAY STILL BE UNAPPLIABLE!! SOLVE THIS WITH RETURN nullptr IN APPLY?? TODO
    // OR... return an UNKNOWN state here (may be pointer, but better a tri-state element).
    return true;
  }

  virtual Move<R, ADS, DS>& apply(R& r, ADS& ads) {
    vector<Move<R, ADS, DS>*> vrev;
    for (unsigned i = 0; i < vm.size(); i++)
      vrev.push_back(&vm[i]->apply(r, ads));
    return *new MultiMove<R, ADS, DS>(vrev);
  }

  virtual bool operator==(const Move<R, ADS, DS>& m) const {
    cout << "STUB IN MultiMove: TODO IMPLEMENT operator==" << endl;
    return true;  // STUB
  }

  static string idComponent() {
    return "OptFrame:Move:MultiMove";
  }

  virtual string id() const override {
    return idComponent();
  }

  void print() const override {
    cout << "MultiMove(" << vm.size() << "):";
    for (unsigned i = 0; i < vm.size(); i++)
      vm[i]->print();
  }
};

template <class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class MultiNS : public NS<R, ADS, DS> {
 public:
  RandGen& rg;
  NS<R, ADS, DS>& ns;
  unsigned kMin, kMax;

  MultiNS(RandGen& _rg, NS<R, ADS, DS>& _ns, unsigned _kMin, unsigned _kMax)
      : rg(_rg), ns(_ns), kMin(_kMin), kMax(_kMax) {
  }

  virtual ~MultiNS() {
  }

 protected:
  virtual Move<R, ADS, DS>& move(const R& r, const ADS& ads) {
    vector<Move<R, ADS, DS>*> vm;
    unsigned k = rg.rand(kMin, kMax);
    //cout << "K=" << k << endl;
    for (unsigned i = 0; i < k; i++)
      vm.push_back(&ns.move(r, ads));
    //cout << "created move!" << endl;
    return *new MultiMove<R, ADS, DS>(vm);
  }

 public:
  static string idComponent() {
    return "OptFrame:NS:MultiNS";
  }

  virtual string id() const override {
    return idComponent();
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (NS<R, ADS, DS>::compatible(s));
  }
};

}  // namespace optframe

#endif /*OPTFRAME_MULTI_NS_HPP_*/
