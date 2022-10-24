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

#ifndef OPTFRAME_SIMPLE_LOCAL_SEARCH_HPP_
#define OPTFRAME_SIMPLE_LOCAL_SEARCH_HPP_

#include <math.h>

#include <vector>

#include "../Constructive.hpp"
#include "../Evaluation.hpp"
#include "../Evaluator.hpp"
#include "../LocalSearch.hpp"
#include "../SingleObjSearch.hpp"

namespace optframe {

template <XESolution XES, XEvaluation XEv = Evaluation<>>
class SimpleLocalSearch : public SingleObjSearch<XES> {
 protected:
  sref<Evaluator<XES, XEv>> evaluator;
  sref<InitialSearch<XES>> constructive;
  sref<LocalSearch<XES, XEv>> localSearch;

 public:
  SimpleLocalSearch(sref<Evaluator<XES, XEv>> _evaluator, sref<InitialSearch<XES>> _constructive, sref<LocalSearch<XES, XEv>> _localSearch)
      : evaluator(_evaluator), constructive(_constructive), localSearch(_localSearch) {
  }

  virtual ~SimpleLocalSearch() {
  }

  //pair<S, Evaluation<>>* search(StopCriteria<XEv>& sosc, const S* _s = nullptr, const Evaluation<>* _e = nullptr) override
  //virtual std::optional<pair<S, XEv>> search(StopCriteria<XEv>& sosc) override
  //
  //SearchStatus search(const StopCriteria<XEv>& sosc) override
  SearchOutput<XES> search(const StopCriteria<XEv>& sosc) override {
    //op<XES>& star = this->best;
    //
    op<XES> star;  // TODO: get best from 'searchBy'
    //cout << "SimpleLocalSearch search(" << target_f << "," << timelimit << ")" << endl;

    Timer tnow;

    //std::optional<S> s = constructive.generateSolution(sosc.timelimit);
    std::optional<XES> pse = constructive->initialSearch(sosc).first;
    if (!pse)
      return SearchStatus::NO_SOLUTION;  // nothing to return
    //Evaluation<> e = evaluator.evaluate(*s);

    ////pair<S&, Evaluation<>&>& p = localSearch.search(s, e, sosc);

    //delete &s;

    //return make_optional(make_pair(*s, e));
    //star = make_optional(make_pair(*s, e));
    star = make_optional(*pse);
    //this->best = star;
    return {SearchStatus::NO_REPORT, star};
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (SingleObjSearch<XES>::compatible(s));
  }

  static string idComponent() {
    stringstream ss;
    ss << SingleObjSearch<XES>::idComponent() << "SimpleLocalSearch";
    return ss.str();
  }

  std::string id() const override {
    return idComponent();
  }

  void print() const override {
    cout << "SimpleLocalSearch with:" << endl;
    cout << "constructive: ";
    constructive->print();
    cout << "local search: ";
    localSearch->print();
  }
};

template <XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>>
class SimpleLocalSearchBuilder : public SingleObjSearchBuilder<S, XEv, XES> {
 public:
  virtual ~SimpleLocalSearchBuilder() {
  }

  SingleObjSearch<XES>* build(Scanner& scanner,
                              HeuristicFactory<S, XEv, XES, X2ES>& hf,
                              string family = "") override {
    sptr<Evaluator<XES, XEv>> eval;
    hf.assign(eval, *scanner.nextInt(), scanner.next());  // reads backwards!

    //Constructive<S>* constructive;
    sptr<InitialSearch<XES>> constructive;
    hf.assign(constructive, *scanner.nextInt(), scanner.next());  // reads backwards!

    string rest = scanner.rest();

    pair<sptr<LocalSearch<XES, XEv>>, std::string> method;
    method = hf.createLocalSearch(rest);

    sptr<LocalSearch<XES, XEv>> h = method.first;

    scanner = Scanner(method.second);

    return new SimpleLocalSearch<XES, XEv>(eval, constructive, h);
  }

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(make_pair(Evaluator<XES, XEv>::idComponent(), "evaluation function"));
    //params.push_back(make_pair(Constructive<S>::idComponent(), "constructive heuristic"));
    params.push_back(make_pair(InitialSearch<XES>::idComponent(), "constructive heuristic"));
    params.push_back(make_pair(LocalSearch<XES, XEv>::idComponent(), "local search"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == SimpleLocalSearch<XES, XEv>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << SingleObjSearchBuilder<S, XEv>::idComponent() << "SimpleLocalSearch";
    return ss.str();
  }

  std::string id() const override {
    return idComponent();
  }
};
}  // namespace optframe

#endif /*OPTFRAME_SIMPLE_LOCAL_SEARCH_HPP_*/
