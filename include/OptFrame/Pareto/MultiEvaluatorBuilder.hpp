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
MOD_EXPORT template <XESolution XES>
#else
MOD_EXPORT template <typename XES>
#endif
class MultiEvaluatorBuilder : public ComponentBuilder<XES> {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;
  using XMEv = MultiEvaluation<typename XEv::objType>;
  using XMES = std::pair<S, XMEv>;

 public:
  virtual ~MultiEvaluatorBuilder() {}

  Component* buildComponent(Scanner& scanner, HeuristicFactory<XES>& hf,
                            std::string family = "") override {
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

  std::vector<std::pair<std::string, std::string>> parameters() override {
    std::vector<std::pair<std::string, std::string>> params;
    std::stringstream ss;
    ss << Evaluator<S, XEv, XES>::idComponent() << "[]";
    params.push_back(std::make_pair(ss.str(), "list of evaluator"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == MultiEvaluator<XES, XMES>::idComponent();
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << ComponentBuilder<XES>::idComponent();
    ss << "MultiEvaluator";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
#ifdef OPTFRAME_USE_STD_CONCEPTS
template <XSolution S, XEvaluation XMEv = MultiEvaluation<>,
          XESolution XMES = std::pair<S, XMEv>,
          X2ESolution<XMES> X2MES = VEPopulation<XMES>>
#else
template <typename S, typename XMEv = MultiEvaluation<>,
          typename XMES = std::pair<S, XMEv>,
          typename X2MES = VEPopulation<XMES>>
#endif
class MultiEvaluatorMultiBuilder
    : public ComponentMultiBuilder<S, XMEv, XMES, X2MES> {
  using XEv = typename XMEv::XEv;
  using XES = std::pair<S, XEv>;
  using X2ES = MultiESolution<XES>;

 public:
  virtual ~MultiEvaluatorMultiBuilder() {}

  Component* buildMultiComponent(Scanner& scanner, HeuristicFactory<XES>& hf,
                                 std::string family = "") override {
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

  std::vector<std::pair<std::string, std::string>> parameters() override {
    std::vector<std::pair<std::string, std::string>> params;
    params.push_back(
        make_pair(Evaluator<S, XEv, XES>::idComponent(), "list of evaluators"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == MultiEvaluator<XES, XMES>::idComponent();
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << ComponentMultiBuilder<S, XMEv, XMES, X2MES>::idComponent();
    ss << "MultiEvaluatorMultiBuilder";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
#ifdef OPTFRAME_USE_STD_CONCEPTS
template <XESolution XES, XEMSolution XMES>
#else
template <typename XES, typename XMES>
#endif
class MultiEvaluatorAction : public Action<XES> {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;

 public:
  virtual ~MultiEvaluatorAction() {}

  virtual std::string usage() {
    return ":MultiEvaluator idx  evaluate   :Solution idx  [output_variable] "
           "=> OptFrame:Evaluation";
  }

  virtual bool handleComponent(std::string type) {
    return ComponentHelper::compareBase(
        MultiEvaluator<XES, XMES>::idComponent(), type);
  }

  virtual bool handleComponent(Component& component) {
    return component.compatible(MultiEvaluator<XES, XMES>::idComponent());
  }

  virtual bool handleAction(std::string action) {
    return (action == "evaluate");
  }

  virtual bool doCast(std::string component, int id, std::string type,
                      std::string variable, HeuristicFactory<XES>& hf,
                      std::map<std::string, std::string>& d) {
    std::cout << "MultiEvaluator::doCast: NOT IMPLEMENTED!" << std::endl;
    return false;

    if (!handleComponent(type)) {
      std::cout
          << "EvaluatorAction::doCast error: can't handle component type '"
          << type << " " << id << "'" << std::endl;
      return false;
    }

    Component* comp = hf.components[component].at(id);

    if (!comp) {
      std::cout << "EvaluatorAction::doCast error: nullptr component '"
                << component << " " << id << "'" << std::endl;
      return false;
    }

    if (!ComponentHelper::compareBase(comp->id(), type)) {
      std::cout << "EvaluatorAction::doCast error: component '" << comp->id()
                << " is not base of " << type << "'" << std::endl;
      return false;
    }

    // remove old component from factory
    hf.components[component].at(id) = nullptr;

    // cast object to lower type
    Component* final = nullptr;

    if (type == Evaluator<S, XEv, XES>::idComponent()) {
      final = (Evaluator<XES, XEv>*)comp;
    } else {
      std::cout << "EvaluatorAction::doCast error: no cast for type '" << type
                << "'" << std::endl;
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
    std::cout << "MultiEvaluator::doAction: NOT IMPLEMENTED!" << std::endl;
    return false;

    // std::cout << "Evaluator::doAction '" << content << "'" << std::endl;

    Scanner scanner(content);

    if (!scanner.hasNext()) return false;

    Evaluator<XES, XEv>* ev;
    hf.assign(ev, *scanner.nextInt(), scanner.next());

    if (!ev) return false;

    if (!scanner.hasNext()) return false;

    std::string action = scanner.next();

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