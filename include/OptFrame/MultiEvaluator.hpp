// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_MULTIEVALUATOR_HPP_
#define OPTFRAME_MULTIEVALUATOR_HPP_

// C++
#include <iostream>
#include <string>
#include <utility>
#include <vector>
//
#include <OptFrame/Concepts/MyConcepts.hpp>
#include <OptFrame/Evaluator.hpp>
#include <OptFrame/Helper/MultiEvaluation.hpp>
#include <OptFrame/Hyper/Action.hpp>
#include <OptFrame/Hyper/ComponentBuilder.hpp>
#include <OptFrame/Hyper/ComponentMultiBuilder.hpp>
#include <OptFrame/IEvaluator.hpp>
// #include <OptFrame/MultiDirection.hpp>
// #include <OptFrame/MultiEvaluator.hpp>

namespace optframe {

// MultiEvaluator implements IEvaluator and GeneralEvaluator
// just a bunch/pack of evaluators...

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES, XEMSolution XMES, XSearch<XMES> XSH = XMES>
#else
template <typename XES, typename XMES, typename XSH = XMES>
#endif
class MultiEvaluator : public GeneralEvaluator<XMES, XSH>,
                       public IEvaluator<XMES> {
  using S = typename XES::first_type;
  using XMEv = typename XMES::second_type;
  using XEv = typename XMEv::XEv;

  using myObjType = typename XMEv::XEv::objType;

 public:
  // TODO(igormcoelho): why do we need this?
  //  -> Can't we just extract each Direction for each Evaluator?
  //
  // sref<MultiDirection<XEv>> mdirection;
  //
  vsref<Direction<XEv>> vDir;
  unsigned nObjectives;
  // ========== end old MultiDirection part ==========

 protected:
  // vector<Evaluator<S, XEv, XES>*> sngEvaluators;  // single evaluators
  //
  // single evaluators
  vsref<Evaluator<S, XEv, XES>> sngEvaluators;
  // move.cost() is enabled or disabled for this Evaluator
  bool allowCosts;

 public:
  // MultiEvaluator(vector<Evaluator<S, XEv, XES>*> _veval)
  explicit MultiEvaluator(const vsref<Evaluator<S, XEv, XES>>& _veval)
      : sngEvaluators{_veval}, allowCosts{false} {
    for (unsigned i = 0; i < _veval.size(); i++)
      vDir.push_back(_veval[i]->direction);
    nObjectives = vDir.size();
  }

  explicit MultiEvaluator(bool _allowCosts = false) : allowCosts(_allowCosts) {
    nObjectives = 0;
  }

  virtual void addEvaluator(sref<Evaluator<S, XEv, XES>> ev) {
    sngEvaluators.push_back(ev);
  }

  virtual ~MultiEvaluator() {}

  unsigned size() const { return sngEvaluators.size(); }

  virtual bool betterThanAt(const XEv& ev1, const XEv& ev2, int index) {
    return sngEvaluators[index]->betterThan(ev1, ev2);
  }

  virtual bool equalsAt(const XEv& ev1, const XEv& ev2, int index) {
    return sngEvaluators[index]->equals(ev1, ev2);
  }

  // changed to Meval without point TODO
  XMEv evaluate(const S& s) override {
    XMEv nev;
    for (unsigned i = 0; i < sngEvaluators.size(); i++) {
      XEv ev{sngEvaluators[i]->evaluate(s)};
      nev.addEvaluation(ev);
    }

    return nev;
  }

  void clear() { sngEvaluators.clear(); }

  // virtual void reevaluateMEV(MultiXEv& mev, const XES& se)
  //
  // virtual void reevaluate(pair<S, MultiXEv>& se) override
  // virtual void reevaluate(pair<S, XMEv>& se) override
  void reevaluate(XMES& se) override {
    XMEv& mev = se.second;
    //
    for (unsigned i = 0; i < sngEvaluators.size(); i++) {
      // XEv e { std::move(mev[i]) }; // TODO (IGOR): why move????
      // sngEvaluators[i]->reevaluate(e, s);
      // mev[i] = std::move(e);
      //
      XEv& e = mev[i];  // TODO: embed MEV in 'se'

      pair<decltype(se.first), XEv> pse(
          se.first, e);  // TODO: we should AVOID this 's' and 'e' copy... by
                         // keeping s,e together.
      sngEvaluators[i]->reevaluate(pse);
      e = std::move(pse.second);  // TODO: verify if this works

      // mev[i] = std::move(e);
    }
  }

  // =======================

  // this strictly better than parameter 'e' (for mini, 'this' < 'e')
  bool betterStrict(const XMEv& e1, const XMEv& e2) override {
    assert(false);
    return false;
  }

  // returns 'true' if this 'cost' (represented by this Evaluation) is
  // improvement
  bool isStrictImprovement(const XMEv& e) override {
    assert(false);
    return false;
  }

  // returns 'true' if this 'cost' (represented by this Evaluation) is
  // improvement
  bool isNonStrictImprovement(const XMEv& e) override {
    assert(false);
    return false;
  }

  bool equals(const XMEv& e1, const XMEv& e2) override {
    assert(false);
    return false;
  }

  // ================================================

 public:
  // ============= Component ===============
  bool compatible(std::string s) override {
    return (s == idComponent()) || (GeneralEvaluator<XMES, XSH>::compatible(s));
  }

  static string idComponent() {
    stringstream ss;
    ss << GeneralEvaluator<XMES, XSH>::idComponent() << ":MultiEvaluator"
       << Domain::getAlternativeDomain<XMES>("<XMESf64>");
    return ss.str();
  }

  string id() const override { return idComponent(); }
};

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES>
#else
template <typename XES>
#endif
class MultiEvaluatorBuilder : public ComponentBuilder<XES> {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;
  using XMEv = MultiEvaluation<typename XEv::objType>;
  using XMES = std::pair<S, XMEv>;

 public:
  virtual ~MultiEvaluatorBuilder() {}

  Component* buildComponent(Scanner& scanner, HeuristicFactory<XES>& hf,
                            string family = "") override {
    //
    vsptr<Evaluator<S, XEv, XES>> _evlist;
    std::string sid_0 = scanner.next();
    int id_0 = *scanner.nextInt();
    hf.assignList(_evlist, id_0, sid_0);
    vsref<Evaluator<S, XEv, XES>> evlist;
    for (sptr<Evaluator<S, XEv, XES>> x : _evlist) {
      assert(x);
      evlist.push_back(x);
    }

    // NOLINTNEXTLINE
    return new MultiEvaluator<XES, XMES>(evlist);
  }

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;
    std::stringstream ss;
    ss << Evaluator<S, XEv, XES>::idComponent() << "[]";
    params.push_back(make_pair(ss.str(), "list of evaluator"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == MultiEvaluator<XES, XMES>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << ComponentBuilder<XES>::idComponent();
    ss << "MultiEvaluator";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  string id() const override { return idComponent(); }
};

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XSolution S, XEvaluation XMEv = MultiEvaluation<>,
          XESolution XMES = pair<S, XMEv>,
          X2ESolution<XMES> X2MES = VEPopulation<XMES>>
#else
template <typename S, typename XMEv = MultiEvaluation<>,
          typename XMES = pair<S, XMEv>, typename X2MES = VEPopulation<XMES>>
#endif
class MultiEvaluatorMultiBuilder
    : public ComponentMultiBuilder<S, XMEv, XMES, X2MES> {
  // using XMEv = MultiEvaluation<typename XEv::objType>;
  // using XMES = std::pair<S, XMEv>;
  using XEv = typename XMEv::XEv;
  using XES = std::pair<S, XEv>;
  using X2ES = MultiESolution<XES>;  // TODO: is this right??

 public:
  virtual ~MultiEvaluatorMultiBuilder() {}

  Component* buildMultiComponent(Scanner& scanner, HeuristicFactory<XES>& hf,
                                 string family = "") override {
    vsptr<Evaluator<S, XEv, XES>> _evlist;
    std::string sid_0 = scanner.next();
    int id_0 = *scanner.nextInt();
    hf.assignList(_evlist, id_0, sid_0);
    vsref<Evaluator<S, XEv, XES>> evlist;
    for (sptr<Evaluator<S, XEv, XES>> x : _evlist) {
      assert(x);
      evlist.push_back(x);
    }

    // NOLINTNEXTLINE
    return new MultiEvaluator<XES, XMES>(evlist);
  }

  vector<pair<string, string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(
        make_pair(Evaluator<S, XEv, XES>::idComponent(), "list of evaluators"));

    return params;
  }

  bool canBuild(string component) override {
    return component == MultiEvaluator<XES, XMES>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << ComponentMultiBuilder<S, XMEv, XMES, X2MES>::idComponent();
    ss << "MultiEvaluatorMultiBuilder";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  string id() const override { return idComponent(); }
};

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES, XEMSolution XMES>
#else
template <typename XES, typename XMES>
#endif
class MultiEvaluatorAction : public Action<XES> {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;

 public:
  virtual ~MultiEvaluatorAction() {}

  virtual string usage() {
    return ":MultiEvaluator idx  evaluate   :Solution idx  [output_variable] "
           "=> OptFrame:Evaluation";
  }

  virtual bool handleComponent(string type) {
    return ComponentHelper::compareBase(
        MultiEvaluator<XES, XMES>::idComponent(), type);
  }

  virtual bool handleComponent(Component& component) {
    return component.compatible(MultiEvaluator<XES, XMES>::idComponent());
  }

  virtual bool handleAction(string action) { return (action == "evaluate"); }

  virtual bool doCast(string component, int id, string type, string variable,
                      HeuristicFactory<XES>& hf, map<string, string>& d) {
    cout << "MultiEvaluator::doCast: NOT IMPLEMENTED!" << endl;
    return false;

    if (!handleComponent(type)) {
      cout << "EvaluatorAction::doCast error: can't handle component type '"
           << type << " " << id << "'" << endl;
      return false;
    }

    Component* comp = hf.components[component].at(id);

    if (!comp) {
      cout << "EvaluatorAction::doCast error: nullptr component '" << component
           << " " << id << "'" << endl;
      return false;
    }

    if (!ComponentHelper::compareBase(comp->id(), type)) {
      cout << "EvaluatorAction::doCast error: component '" << comp->id()
           << " is not base of " << type << "'" << endl;
      return false;
    }

    // remove old component from factory
    hf.components[component].at(id) = nullptr;

    // cast object to lower type
    Component* final = nullptr;

    if (type == Evaluator<S, XEv, XES>::idComponent()) {
      final = (Evaluator<XES, XEv>*)comp;
    } else {
      cout << "EvaluatorAction::doCast error: no cast for type '" << type << "'"
           << endl;
      return false;
    }

    // add new component
    Scanner scanner(variable);
    return ComponentAction<XES>::addAndRegister(scanner, *final, hf, d);
  }

  virtual bool doAction(string content, HeuristicFactory<XES>& hf,
                        map<string, string>& dictionary,
                        map<string, vector<string>>& ldictionary) {
    cout << "MultiEvaluator::doAction: NOT IMPLEMENTED!" << endl;
    return false;

    // cout << "Evaluator::doAction '" << content << "'" << endl;

    Scanner scanner(content);

    if (!scanner.hasNext()) return false;

    Evaluator<XES, XEv>* ev;
    hf.assign(ev, *scanner.nextInt(), scanner.next());

    if (!ev) return false;

    if (!scanner.hasNext()) return false;

    string action = scanner.next();

    if (!handleAction(action)) return false;

    if (action == "evaluate") {
      if (!scanner.hasNext()) return false;

      S* s;
      hf.assign(s, *scanner.nextInt(), scanner.next());

      if (!s) return false;

      XEv& e = ev->evaluate(*s);

      return Action<XES>::addAndRegister(scanner, e, hf, dictionary);
    }

    // no action found!
    return false;
  }
};

}  // namespace optframe

#endif  // OPTFRAME_MULTIEVALUATOR_HPP_
