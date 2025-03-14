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

#ifndef OPTFRAME_MULTI_SOLUTION_SEARCH_HPP_
#define OPTFRAME_MULTI_SOLUTION_SEARCH_HPP_

// ===================================================
// Search that happens using MultiSolution space (2^S)
// ===================================================
// It could be MO Pareto Search, or any populational search, being it single or
// multi objective. The idea is that MS management (Population or Pareto) is
// held by X2ES class itself, thus
//   reducing the burden of Evaluation objects to keep track of it.
// This finally allows to remove ParetoManager class.
// ===================================================

// THIS CLASS COULD PERHAPS BE DEPRECATED IN FAVOR OF GlobalSearch... NOT MUCH
// USE HERE, DOES IT?

#include <cstring>
#include <iostream>
#include <utility>
#include <vector>
//
#include "Direction.hpp"
#include "Evaluation.hpp"
#include "Population.hpp"
// #include "Solution.hpp"

#include "Component.hpp"
#include "ComponentBuilder.h"
#include "MultiEvaluation.hpp"
#include "MultiEvaluator.hpp"
#include "Pareto.hpp"
#include "ParetoDominance.hpp"
#include "ParetoDominanceWeak.hpp"

using namespace std;

namespace optframe {

template <XSolution S, XEvaluation XEv, X2ESolution<S, XEv> X2ES>
class MSSearch : public Component {
 public:
  MSSearch() {}

  virtual ~MSSearch() {}

  // this should be pure virtual. useful for populational searches (single or
  // multiobj) and general multiobj searches (by pareto sets)
  virtual SearchStatus search(std::optional<X2ES>& p,
                              const StopCriteria<XEv>& stopCriteria) = 0;

  std::string log() const override { return "Empty heuristic log."; }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (Component::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << "MSSearch:";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

template <XSolution S, XEvaluation XEv, X2ESolution<S, XEv> X2ES>
class MSSearchBuilder : public ComponentBuilder<XES> {
 public:
  virtual ~MSSearchBuilder() {}

  virtual MSSearch<S, XEv, X2ES>* build(Scanner& scanner,
                                        HeuristicFactory<XES>& hf,
                                        string family = "") = 0;

  Component* buildComponent(Scanner& scanner, HeuristicFactory<XES>& hf,
                            std::string family = "") override {
    return build(scanner, hf, family);
  }

  std::vector<std::pair<std::string, std::string>> parameters() override = 0;

  bool canBuild(std::string) override = 0;

  static std::string idComponent() {
    std::stringstream ss;
    ss << ComponentBuilder<XES>::idComponent() << "MSSearch:";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif /* OPTFRAME_MULTI_SOLUTION_SEARCH_HPP_ */
