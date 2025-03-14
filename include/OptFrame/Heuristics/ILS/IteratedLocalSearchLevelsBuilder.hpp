
#pragma once

namespace optframe {

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES>
#else
template <typename XES>
#endif
class IteratedLocalSearchLevelsBuilder : public ILS,
                                         public SingleObjSearchBuilder<XES> {
 public:
  ~IteratedLocalSearchLevelsBuilder() override = default;

  SingleObjSearch<XES>* build(Scanner& scanner, HeuristicFactory<XES>& hf,
                              std::string family = "") override {
    sptr<GeneralEvaluator<XES>> eval = nullptr;
    std::string sid_0 = scanner.next();
    int id_0 = *scanner.nextInt();
    hf.assign(eval, id_0, sid_0);
    if (!eval) return nullptr;

    // Constructive<S>* constructive = nullptr;
    sptr<InitialSearch<XES>> constructive = nullptr;
    std::string sid_1 = scanner.next();
    int id_1 = *scanner.nextInt();
    hf.assign(constructive, id_1, sid_1);
    if (!constructive) return nullptr;

    std::string rest = scanner.rest();

    pair<sptr<LocalSearch<XES>>, std::string> method;
    method = hf.createLocalSearch(rest);

    sptr<LocalSearch<XES>> h = method.first;

    scanner = Scanner(method.second);
    if (!h) return nullptr;

    sptr<ILSLPerturbation<XES>> pert;
    std::string sid_3 = scanner.next();
    int id_3 = *scanner.nextInt();
    hf.assign(pert, id_3, sid_3);
    if (!pert) return nullptr;

    int iterMax = -1;

    auto oiterMax = scanner.nextInt();

    if (!oiterMax) {
      return nullptr;
    }

    iterMax = *oiterMax;

    int levelMax = -1;

    auto olevelMax = scanner.nextInt();

    if (!olevelMax) {
      return nullptr;
    }

    levelMax = *olevelMax;

    return new IteratedLocalSearchLevels<XES>(eval, constructive, h, pert,
                                              iterMax, levelMax);
  }

  std::vector<std::pair<std::string, std::string>> parameters() override {
    std::vector<std::pair<std::string, std::string>> params;
    params.push_back(
        make_pair(GeneralEvaluator<XES>::idComponent(), "evaluation function"));
    // params.push_back(std::make_pair(Constructive<S>::idComponent(),
    // "constructive heuristic"));
    params.push_back(
        make_pair(InitialSearch<XES>::idComponent(), "constructive heuristic"));
    params.push_back(
        make_pair(LocalSearch<XES>::idComponent(), "local search"));
    params.push_back(
        make_pair(ILSLPerturbation<XES>::idComponent(), "ilsL perturbation"));
    params.push_back(
        make_pair("int", "max number of iterations without improvement"));
    params.push_back(std::make_pair("int", "levelMax of perturbation"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == IteratedLocalSearchLevels<XES>::idComponent();
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << SingleObjSearchBuilder<XES>::idComponent() << ":" << ILS::family()
       << "ILSLevels";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe