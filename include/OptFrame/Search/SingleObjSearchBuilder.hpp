// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2025 - OptFrame - https://github.com/optframe/optframe

#pragma once

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

#define MOD_EXPORT
#else

// CANNOT IMPORT HERE... Already part of optframe.core
/*
import std;
import optframe.component;
import optframe.concepts;
*/

// do NOT import/export modules on .hpp... only on .cppm

#define MOD_EXPORT export

#endif

namespace optframe {

// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
#ifdef OPTFRAME_USE_STD_CONCEPTS
MOD_EXPORT template <XESSolution XESS>  // single objective type XESSOlution
#else
MOD_EXPORT template <typename XESS>
#endif
class SingleObjSearchBuilder : public ComponentBuilder<XESS> {
  using _XES = XESS;  // single objective type XESSOlution

 public:
  virtual ~SingleObjSearchBuilder() = default;

  // NOLINTNEXTLINE
  virtual SingleObjSearch<_XES>* build(Scanner& scanner,
                                       HeuristicFactory<_XES>& hf,
                                       std::string family = "") = 0;

  // NOLINTNEXTLINE
  Component* buildComponent(Scanner& scanner, HeuristicFactory<_XES>& hf,
                            std::string family = "") override {
    return build(scanner, hf, family);
  }

  std::vector<std::pair<std::string, std::string>> parameters() override = 0;

  bool canBuild(std::string) override = 0;

  static std::string idComponent() {
    std::stringstream ss;
    ss << ComponentBuilder<_XES>::idComponent();
    ss << "SingleObjSearch";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  std::string toString() const override { return id(); }
};

// template<class R, class ADS = OPTFRAME_DEFAULT_ADS, XBaseSolution<R,ADS> S =
// CopySolution<R,ADS>, XEvaluation XEv = Evaluation<>> template<XESolution XES,
// XEvaluation XEv = Evaluation<>>
// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
#ifdef OPTFRAME_USE_STD_CONCEPTS
template <XESolution XES>
#else
template <typename XES>
#endif
class SingleObjSearchAction : public Action<XES> {
  using _S = typename XES::first_type;
  using _XEv = typename XES::second_type;

 public:
  virtual ~SingleObjSearchAction() = default;

  virtual std::string usage() {
    return "OptFrame:SingleObjSearch idx   search    timelimit  target_f  "
           "OptFrame:Solution idx|-1   OptFrame:Evaluation idx|-1   "
           "[output_variable] => OptFrame:Solution|nullptr";
  }

  virtual bool handleComponent(std::string type) {
    return ComponentHelper::compareBase(SingleObjSearch<XES>::idComponent(),
                                        type);
  }

  virtual bool handleComponent(Component& component) {
    return component.compatible(SingleObjSearch<XES>::idComponent());
  }

  virtual bool handleAction(std::string action) { return (action == "search"); }

  virtual bool doCast(std::string component, int id, std::string type,
                      std::string variable, HeuristicFactory<XES>& hf,
                      std::map<std::string, std::string>& d) {
    if (!handleComponent(type)) {
      std::cout
          << "SingleObjSearchAction::doCast error: can't handle component "
             "type '"
          << type << " " << id << "'" << std::endl;
      return false;
    }

    Component* comp = hf.components[component].at(id);

    if (!comp) {
      std::cout << "SingleObjSearchAction::doCast error: nullptr component '"
                << component << " " << id << "'" << std::endl;
      return false;
    }

    if (!ComponentHelper::compareBase(comp->id(), type)) {
      std::cout << "SingleObjSearchAction::doCast error: component '"
                << comp->id() << " is not base of " << type << "'" << std::endl;
      return false;
    }

    // remove old component from factory
    hf.components[component].at(id) = nullptr;

    // cast object to lower type
    Component* final = nullptr;

    if (type == SingleObjSearch<XES>::idComponent()) {
      final = (SingleObjSearch<XES>*)comp;
    } else {
      std::cout << "SingleObjSearchAction::doCast error: no cast for type '"
                << type << "'" << std::endl;
      return false;
    }

    // add new component
    Scanner scanner(variable);
    return ComponentAction<XES>::addAndRegister(scanner, *final, hf, d);
  }

  virtual bool doAction(
      std::string content, HeuristicFactory<XES>& hf,
      std::map<std::string, std::string>& dictionary,
      std::map<std::string, std::vector<std::string>>& ldictionary) {
    // std::cout << "LocalSearch::doAction '" << content << "'" << std::endl;

    Scanner scanner(content);

    if (!scanner.hasNext()) return false;

    SingleObjSearch<XES>* sios;
    hf.assign(sios, *scanner.nextInt(), scanner.next());

    if (!sios) return false;

    if (!scanner.hasNext()) return false;

    std::string action = scanner.next();

    if (!handleAction(action)) return false;

    if (action == "search") {
      if (!scanner.hasNext()) return false;

      double timelimit = *scanner.nextDouble();

      if (!scanner.hasNext()) return false;

      double target_f = *scanner.nextDouble();

      if (!scanner.hasNext()) return false;

      _S* s;
      hf.assign(s, *scanner.nextInt(), scanner.next());

      if (!scanner.hasNext()) return false;

      _XEv* e;
      hf.assign(e, *scanner.nextInt(), scanner.next());

      std::pair<_S, _XEv>* p =
          sios->search(StopCriteria<_XEv>(timelimit, target_f), s, e);

      if (!p) return true;

      // TODO: use Move Semantics
      _S* s2 = new _S(p->first);

      delete p;

      return Action<XES>::addAndRegister(scanner, *s2, hf, dictionary);
    }

    // no action found!
    return false;
  }
};

}  // namespace optframe
