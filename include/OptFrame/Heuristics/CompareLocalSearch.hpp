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

#ifndef OPTFRAME_COMPARE_LOCAL_SEARCH_HPP_
#define OPTFRAME_COMPARE_LOCAL_SEARCH_HPP_

#include "../Evaluation.hpp"
#include "../Evaluator.hpp"
#include "../LocalSearch.hpp"
#include "../NSSeq.hpp"

namespace optframe {

template <XESolution XES, XEvaluation XEv = Evaluation<>>
class CompareLocalSearch : public LocalSearch<XES, XEv> {
 private:
  sref<Evaluator<XES, XEv>> eval;
  sref<LocalSearch<XES, XEv>> ls1;
  sref<LocalSearch<XES, XEv>> ls2;

 public:
  CompareLocalSearch(sref<Evaluator<XES, XEv>> _eval,
                     sref<LocalSearch<XES, XEv>> _ls1,
                     sref<LocalSearch<XES, XEv>> _ls2)
      : eval(_eval), ls1(_ls1), ls2(_ls2) {}

  virtual ~CompareLocalSearch() {}

  // DEPRECATED
  // virtual void exec(S& s, const StopCriteria<XEv>& stopCriteria)
  //{
  //	Evaluation<> e = std::move(ev.evaluate(s));
  //	exec(s, e, stopCriteria);
  //}

  virtual SearchStatus searchFrom(XES& se,
                                  const StopCriteria<XEv>& sosc) override {
    // S& s = se.first;
    XEv& e = se.second;

    // S& s2 = s.clone();
    // Evaluation<>& e2   = e.clone();
    XES p2 = se;  // clone!
    XEv& e2 = p2.second;

    ls1->searchFrom(se, sosc);
    ls2->searchFrom(p2, sosc);

    if (!eval->equals(e, e2)) {
      cout << "CompareLocalSearch error: difference between " << e.evaluation()
           << " and " << e2.evaluation() << endl;
      cout << "LocalSearch 1: ";
      ls1->print();
      cout << "LocalSearch 2: ";
      ls2->print();
      exit(1);
    }

    // delete &s2;
    // delete &e2;
    return SearchStatus::NO_REPORT;
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (LocalSearch<XES, XEv>::compatible(s));
  }

  static string idComponent() {
    stringstream ss;
    ss << LocalSearch<XES, XEv>::idComponent() << "CompareLocalSearch";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  virtual string toString() const override {
    stringstream ss;
    ss << "CLS: (" << ls1->toString() << "," << ls2->toString() << ")";
    return ss.str();
  }
};

template <XSolution S, XEvaluation XEv = Evaluation<>,
          XESolution XES = pair<S, XEv>,
          X2ESolution<XES> X2ES = MultiESolution<XES>>
class CompareLocalSearchBuilder : public LocalSearchBuilder<S, XEv, XES, X2ES> {
 public:
  virtual ~CompareLocalSearchBuilder() {}

  LocalSearch<XES, XEv>* build(Scanner& scanner,
                               HeuristicFactory<S, XEv, XES, X2ES>& hf,
                               string family = "") override {
    std::shared_ptr<Evaluator<XES, XEv>> eval;
    hf.assign(eval, *scanner.nextInt(), scanner.next());  // reads backwards!

    string rest = scanner.rest();

    pair<sptr<LocalSearch<XES, XEv>>, std::string> method;
    method = hf.createLocalSearch(rest);

    sptr<LocalSearch<XES, XEv>> h = method.first;

    scanner = Scanner(method.second);

    string rest2 = scanner.rest();

    pair<sptr<LocalSearch<XES, XEv>>, std::string> method2;
    method2 = hf.createLocalSearch(rest2);

    sptr<LocalSearch<XES, XEv>> h2 = method2.first;

    scanner = Scanner(method2.second);

    return new CompareLocalSearch<XES, XEv>(eval, h, h2);
  }

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(
        make_pair(Evaluator<XES, XEv>::idComponent(), "evaluation function"));
    params.push_back(
        make_pair(LocalSearch<XES, XEv>::idComponent(), "local search 1"));
    params.push_back(
        make_pair(LocalSearch<XES, XEv>::idComponent(), "local search 2"));

    return params;
  }

  virtual bool canBuild(string component) override {
    return component == FirstImprovement<XES, XEv>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << LocalSearchBuilder<S, XEv>::idComponent() << "CompareLocalSearch";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif /*OPTFRAME_COMPARE_LOCAL_SEARCH_HPP_*/
