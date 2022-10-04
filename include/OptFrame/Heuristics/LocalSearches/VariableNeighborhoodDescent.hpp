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

#ifndef OPTFRAME_VARIABLENEIGHBORHOODDESCENT_HPP_
#define OPTFRAME_VARIABLENEIGHBORHOODDESCENT_HPP_

#include "../../Evaluator.hpp"
#include "../../LocalSearch.hpp"
#include "../../NSEnum.hpp"
#include "../../RandGen.hpp"
#include "VND.h"

namespace optframe {

//When RandGen is given as parameter it performs RVND
template <XESolution XES, XEvaluation XEv = typename XES::second_type>
class VariableNeighborhoodDescent : public LocalSearch<XES, XEv> {
 private:
  sref<GeneralEvaluator<XES, XEv>> ev;
  vsref<LocalSearch<XES, XEv>> lsList;
  sptr<RandGen> rg;

 public:
  VariableNeighborhoodDescent(sref<GeneralEvaluator<XES, XEv>> _ev, vsref<LocalSearch<XES, XEv>> _lsList, sptr<RandGen> _rg = nullptr)
      : ev(_ev), lsList(_lsList), rg(_rg) {
  }

  virtual ~VariableNeighborhoodDescent() {
  }

  // DEPRECATED
  //virtual void exec(S& s, const StopCriteria<XEv>& stopCriteria)
  //{
  //	Evaluation<> e = std::move(ev.evaluate(s));
  //	exec(s, e, stopCriteria);
  //}

  virtual SearchStatus searchFrom(XES& se, const StopCriteria<XEv>& stopCriteria) override {
    //S& s = se.first;
    //XEv& e = se.second;

    if (Component::information)
      cout << "VND::starts" << endl;

    Timer tNow;

    if (rg)
      rg->shuffle(lsList);  // shuffle elements

    int r = lsList.size();

    int k = 1;

    XES current(se);  // full backup! TODO: remove this copy
    //Evaluation<> eCurrent(e);
    //'target_f' will crash if not provided... removing
    while ((k <= r) && !stopCriteria.shouldStop())  // && (ev.betterThan(stopCriteria.target_f, e))
    {
      //eCurrent = e; // backup
      current = se;  // TODO: remove this copy

      StopCriteria<XEv> stopCriteriaNextLS = stopCriteria;
      stopCriteriaNextLS.updateTimeLimit(tNow.now());
      lsList[k - 1]->searchFrom(se, stopCriteriaNextLS);

      //if (ev.betterThan(e, eCurrent))
      //if (ev.betterThan(se, current))
      //if (se.second.betterStrict(current.second))
      if (Component::debug)
        std::cout << "VND will compare(" << se.second.isOutdated() << ";" << current.second.isOutdated() << ")" << std::endl;
      if (ev->betterStrict(se.second, current.second)) {
        // improvement
        k = 1;
        if (Component::information)
          //e.print();
          se.second.print();
      } else {
        k = k + 1;

        if (Component::information)
          cout << "VND::k=" << k << endl;
      }
    }
    return SearchStatus::NO_REPORT;
  }

  virtual bool compatible(string s) {
    return (s == idComponent()) || (LocalSearch<XES, XEv>::compatible(s));
  }

  static string idComponent() {
    stringstream ss;
    ss << LocalSearch<XES, XEv>::idComponent() << ":VND";
    return ss.str();
  }

  virtual string id() const override {
    return idComponent();
  }

  virtual string toString() const {
    stringstream ss;
    ss << "VND: [ ";
    for (unsigned i = 0; i < lsList.size(); i++) {
      auto& x = const_cast<sref<LocalSearch<XES, XEv>>&>(lsList[i]);
      //ss << lsList[i]->toString();
      ss << x->toString();
      if (i != lsList.size() - 1)
        ss << ",";
    }
    ss << "]";

    return ss.str();
  }
};

template <XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>>
class VariableNeighborhoodDescentBuilder : public LocalSearchBuilder<S, XEv, XES, X2ES> {
 public:
  virtual ~VariableNeighborhoodDescentBuilder() {
  }

  virtual LocalSearch<XES, XEv>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "") {
    sptr<GeneralEvaluator<XES, XEv>> eval;
    hf.assign(eval, *scanner.nextInt(), scanner.next());  // reads backwards!

    vsptr<LocalSearch<XES, XEv>> _hlist;
    hf.assignList(_hlist, *scanner.nextInt(), scanner.next());  // reads backwards!
    vsref<LocalSearch<XES, XEv>> hlist;
    for (auto x : _hlist)
      hlist.push_back(x);

    return new VariableNeighborhoodDescent<XES, XEv>(eval, hlist);
  }

  virtual vector<pair<string, string>> parameters() {
    vector<pair<string, string>> params;
    params.push_back(make_pair(GeneralEvaluator<XES, XEv>::idComponent(), "evaluation function"));
    stringstream ss;
    ss << LocalSearch<XES, XEv>::idComponent() << "[]";
    params.push_back(make_pair(ss.str(), "list of local searches"));

    return params;
  }

  virtual bool canBuild(string component) {
    return component == VariableNeighborhoodDescent<XES, XEv>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << LocalSearchBuilder<S, XEv>::idComponent() << ":VND";
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

#endif /*OPTFRAME_VARIABLENEIGHBORHOODDESCENT_HPP_*/
