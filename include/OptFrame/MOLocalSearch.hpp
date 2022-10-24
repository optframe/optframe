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

#ifndef OPTFRAME_MO_LOCAL_SEARCH_HPP_
#define OPTFRAME_MO_LOCAL_SEARCH_HPP_

#include <iostream>
#include <vector>

using namespace std;

#include "Component.hpp"
#include "ComponentBuilder.h"
#include "Evaluation.hpp"
#include "MultiObjSearch.hpp"
#include "ParetoManager.hpp"
#include "Solution.hpp"

namespace optframe {

template <XESolution XMES, XEvaluation XMEv = MultiEvaluation<>>
class MOLocalSearch : public Component {
  using S = typename XMES::first_type;
  static_assert(is_same<S, typename XMES::first_type>::value);
  static_assert(is_same<XMEv, typename XMES::second_type>::value);

 public:
  MOLocalSearch() {
  }

  virtual ~MOLocalSearch() {
  }

  // core methods

  // 1
  //virtual void moSearchFrom(Pareto<XMES>& p, S& s, paretoManager<S, XMEv, XMES>& pManager, const StopCriteria<XMEv>& stopCriteria) = 0;
  // removed this method.. now only pair XMES!

  // 2
  virtual void moSearchFrom(Pareto<XMES>& p, XMES& smev, paretoManager<S, XMEv, XMES>& pManager, const StopCriteria<XMEv>& stopCriteria) = 0;

  bool compatible(std::string s) override {
    return (s == idComponent()) || (Component::compatible(s));
  }

  virtual std::string toString() const override {
    return id();
  }

  static string idComponent() {
    stringstream ss;
    ss << Component::idComponent() << ":MOLocalSearch";
    return ss.str();
  }

  virtual string id() const override {
    return idComponent();
  }
};

}  // namespace optframe

#endif /* OPTFRAME_MO_LOCAL_SEARCH_HPP_ */
