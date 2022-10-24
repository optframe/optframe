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

#ifndef OPTFRAME_SINGLE_OBJ_SEARCH_TO_LOCAL_SEARCH_HPP_
#define OPTFRAME_SINGLE_OBJ_SEARCH_TO_LOCAL_SEARCH_HPP_

#include <math.h>

#include <vector>

#include "../Constructive.hpp"
#include "../Evaluation.hpp"
#include "../Evaluator.hpp"
#include "../LocalSearch.hpp"
#include "../SingleObjSearch.hpp"

namespace optframe {

template <XESolution XES, XEvaluation XEv = Evaluation<>>
class SingleObjSearchToLocalSearch : public LocalSearch<XES, XEv> {
 protected:
  sref<Evaluator<XES, XEv>> evaluator;
  sref<SingleObjSearch<XES>> sios;

 public:
  SingleObjSearchToLocalSearch(sref<Evaluator<XES, XEv>> _evaluator, sref<SingleObjSearch<XES>> _sios)
      : evaluator(_evaluator), sios(_sios) {
  }

  virtual ~SingleObjSearchToLocalSearch() {
  }

  // DEPRECATED
  //virtual void exec(S& s, const StopCriteria<XEv>& stopCriteria)
  //{
  //	Evaluation<> e = std::move(ev.evaluate(s));
  //	exec(s, e, stopCriteria);
  //}

  virtual SearchStatus searchFrom(XES& se, const StopCriteria<XEv>& sosc) override {
    XSolution AUTO_CONCEPTS& s = se.first;
    XEv& e = se.second;

    // will ignore 'se'
    // maybe ALL SingleObjSearch should inherit from LocalSearch!
    // maybe 'LocalSearch' should become 'Search', and 'SingleObjSearch' -> 'GlobalSearch'... must think!

    std::optional<XES> r = make_optional(se);

    assert(false);
    // TODO: must use 'searchBy'
    //
    //sios.best = r;
    //sios.search(sosc);
    //r = sios.best;

    if (r) {
      s = r->first;
      e = r->second;
      //delete r;  // no need
    }
    return SearchStatus::NO_REPORT;
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (LocalSearch<XES, XEv>::compatible(s));
  }

  static string idComponent() {
    stringstream ss;
    ss << SingleObjSearch<XES>::idComponent() << "SingleObjSearchToLocalSearch";
    return ss.str();
  }

  std::string toString() const override {
    return id();
  }

  virtual string id() const override {
    return idComponent();
  }

  void print() const override {
    cout << "SingleObjSearchToLocalSearch with sios: ";
    sios->print();
  }
};

template <XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>>
class SingleObjSearchToLocalSearchBuilder : public LocalSearchBuilder<S, XEv, XES, X2ES> {
 public:
  virtual ~SingleObjSearchToLocalSearchBuilder() {
  }

  LocalSearch<XES, XEv>* build(Scanner& scanner,
                               HeuristicFactory<S, XEv, XES, X2ES>& hf,
                               string family = "") override {
    sptr<Evaluator<XES, XEv>> eval;
    hf.assign(eval, *scanner.nextInt(), scanner.next());  // reads backwards!

    string rest = scanner.rest();

    pair<sptr<SingleObjSearch<XES>>, std::string> method;
    method = hf.createSingleObjSearch(rest);

    sptr<SingleObjSearch<XES>> h = method.first;

    scanner = Scanner(method.second);

    return new SingleObjSearchToLocalSearch<XES, XEv>(eval, h);
  }

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(make_pair(Evaluator<XES, XEv>::idComponent(), "evaluation function"));
    params.push_back(make_pair(SingleObjSearch<XES>::idComponent(), "single obj search"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == SingleObjSearchToLocalSearch<XES, XEv>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << LocalSearchBuilder<S, XEv>::idComponent() << "SingleObjSearchToLocalSearch";
    return ss.str();
  }

  std::string toString() const override {
    return id();
  }

  virtual string id() const override {
    return idComponent();
  }
};
}  // namespace optframe

#endif
