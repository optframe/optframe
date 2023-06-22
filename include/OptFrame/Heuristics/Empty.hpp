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

#ifndef OPTFRAME_EMPTY_HPP_
#define OPTFRAME_EMPTY_HPP_

#include <vector>

#include "../Evaluation.hpp"
#include "../LocalSearch.hpp"

namespace optframe {

template <XESolution XES, XEvaluation XEv = Evaluation<>>
class EmptyLocalSearch : public LocalSearch<XES> {
 public:
  EmptyLocalSearch() {}

  virtual ~EmptyLocalSearch() {}

  virtual SearchStatus searchFrom(
      XES&, const StopCriteria<XEv>& stopCriteria) override {
    // placeholder for empty local search
    return SearchStatus::NO_REPORT;
  };

  string log() const { return "Heuristic Empty: no log."; }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (LocalSearch<XES>::compatible(s));
  }

  static string idComponent() {
    stringstream ss;
    ss << LocalSearch<XES>::idComponent() << ":Empty";
    return ss.str();
  }

  virtual string id() const override { return idComponent(); }
};

template <XSolution S, XEvaluation XEv = Evaluation<>,
          XESolution XES = pair<S, XEv>,
          X2ESolution<XES> X2ES = MultiESolution<XES>>
class EmptyLocalSearchBuilder : public LocalSearchBuilder<S, XEv, XES, X2ES> {
 public:
  virtual ~EmptyLocalSearchBuilder() {}

  LocalSearch<XES>* build(Scanner& scanner,
                          HeuristicFactory<S, XEv, XES, X2ES>& hf,
                          string family = "") override {
    return new EmptyLocalSearch<XES>;
  }

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;

    return params;
  }

  bool canBuild(std::string component) override {
    return component == EmptyLocalSearch<XES>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << LocalSearchBuilder<S, XEv>::idComponent() << ":Empty";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  virtual string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif /*OPTFRAME_EMPTY_HPP_*/
