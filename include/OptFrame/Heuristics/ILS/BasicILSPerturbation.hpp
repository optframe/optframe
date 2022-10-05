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

#ifndef OPTFRAME_BASICILSPerturbation_HPP_
#define OPTFRAME_BASICILSPerturbation_HPP_

#include <math.h>

#include <vector>
//
#include <OptFrame/Hyper/ComponentBuilder.hpp>
#include <OptFrame/NS.hpp>
#include <OptFrame/RandGen.hpp>

#include "ILS.h"

namespace optframe {

template <XESolution XES, XEvaluation XEv = Evaluation<>>
class BasicILSPerturbation : public ILS, public Component {
 private:
  sref<GeneralEvaluator<XES, XEv>> evaluator;
  int pMin;
  int pMax;
  vsref<NS<XES, XEv>> ns;
  sref<RandGen> rg;

 public:
  BasicILSPerturbation(sref<GeneralEvaluator<XES, XEv>> e, int _pMin, int _pMax, vsref<NS<XES, XEv>>& _ns, sref<RandGen> _rg)
      : evaluator(e), pMin(_pMin), pMax(_pMax), ns(_ns), rg(_rg) {
    if (pMax < pMin) {
      cout << "BasicILSPerturbation warning: pMax > pMin! Swapping both." << endl;
      int aux = pMax;
      pMax = pMin;
      pMin = aux;
    }

    if (ns.size() == 0)
      cout << "BasicILSPerturbation warning: empty neighborhood list." << endl;
  }

  BasicILSPerturbation(sref<GeneralEvaluator<XES, XEv>> e, int _pMin, int _pMax, sref<NS<XES, XEv>> _ns, sref<RandGen> _rg)
      : evaluator(e), pMin(_pMin), pMax(_pMax), rg(_rg) {
    ns.push_back(&_ns);
    if (pMax < pMin) {
      cout << "BasicILSPerturbation warning: pMax > pMin! Swapping both." << endl;
      int aux = pMax;
      pMax = pMin;
      pMin = aux;
    }

    if (ns.size() == 0)
      cout << "BasicILSPerturbation warning: empty neighborhood list." << endl;
  }

  virtual ~BasicILSPerturbation() {
  }

  void add_ns(NS<XES, XEv>& _ns) {
    ns.push_back(&_ns);
  }

  void perturb(XES& se, const StopCriteria<XEv>& stopCriteria)  // TODO: override?? what?
  {
    //XSolution& s = se.first;
    //XEv& e = se.second;
    //
    for (int i = pMin; i < pMax; i++) {
      int nk = rand() % ns.size();

      uptr<Move<XES, XEv>> mp = ns[nk]->validRandomMove(se);

      if (!mp) {
        cout << "BasicILSPerturbation warning: perturbation found no valid move for neighborhood: ";
        ns[nk]->print();
      } else {
        mp->applyUpdate(se);
      }
    }

    evaluator->reevaluate(se);  // updates 'e'
  }

  std::string toString() const override {
    return id();
  }

  virtual string id() const override {
    return idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << Component::idComponent() << ":" << ILS::family() << "basic_pert";
    return ss.str();
  }
};

template <XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>>
class BasicILSPerturbationBuilder : public ComponentBuilder<S, XEv, XES, X2ES> {
 public:
  virtual ~BasicILSPerturbationBuilder() {
  }

  virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "") {
    sptr<GeneralEvaluator<XES, XEv>> eval;
    hf.assign(eval, *scanner.nextInt(), scanner.next());  // reads backwards!

    int pMin = *scanner.nextInt();
    int pMax = *scanner.nextInt();

    vsptr<NS<XES, XEv>> _ns_list;
    hf.assignList(_ns_list, *scanner.nextInt(), scanner.next());  // reads backwards!
    vsref<NS<XES, XEv>> ns_list;
    for (auto x : _ns_list)
      ns_list.push_back(x);

    return new BasicILSPerturbation<XES, XEv>(eval, pMin, pMax, ns_list, hf.getRandGen());
  }

  virtual vector<pair<string, string>> parameters() {
    vector<pair<string, string>> params;
    params.push_back(make_pair(GeneralEvaluator<XES, XEv>::idComponent(), "evaluation function"));
    params.push_back(make_pair("OptFrame:int", "pMin: min number of moves"));
    params.push_back(make_pair("OptFrame:int", "pMax: max number of moves"));
    stringstream ss;
    ss << NS<XES, XEv>::idComponent() << "[]";
    params.push_back(make_pair(ss.str(), "list of neighborhood structures"));

    return params;
  }

  virtual bool canBuild(string component) {
    return component == BasicILSPerturbation<XES, XEv>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << ComponentBuilder<S, XEv, XES, X2ES>::idComponent() << ILS::family() << "basic_pert";
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

#endif /*OPTFRAME_BASICILSPerturbation_HPP_*/
