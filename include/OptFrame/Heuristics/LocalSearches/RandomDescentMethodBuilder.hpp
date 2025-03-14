#pragma once

namespace optframe {

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES>
#else
template <typename XES>
#endif
class RandomDescentMethodBuilder : public LocalSearchBuilder<XES> {
  using XSH = XES;  // primary-based search type only (BestType)

 public:
  virtual ~RandomDescentMethodBuilder() = default;

  // NOLINTNEXTLINE
  LocalSearch<XES>* build(Scanner& scanner, HeuristicFactory<XES>& hf,
                          string family = "") override {
    sptr<GeneralEvaluator<XES>> eval;
    std::string comp_id1 = scanner.next();
    int id1 = *scanner.nextInt();
    hf.assign(eval, id1, comp_id1);

    sptr<NS<XES, XSH>> ns;
    std::string comp_id2 = scanner.next();
    int id2 = *scanner.nextInt();
    hf.assign(ns, id2, comp_id2);

    int iterMax = *scanner.nextInt();

    // NOLINTNEXTLINE
    return new RandomDescentMethod<XES>(eval, ns, iterMax);
  }

  std::vector<std::pair<std::string, std::string>> parameters() override {
    std::vector<std::pair<std::string, std::string>> params;
    params.push_back(
        make_pair(GeneralEvaluator<XES>::idComponent(), "evaluation function"));
    params.push_back(
        make_pair(NS<XES, XSH>::idComponent(), "neighborhood structure"));
    params.push_back(std::make_pair("OptFrame:int",
                               "max number of iterations without improvement"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == RandomDescentMethod<XES>::idComponent();
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << LocalSearchBuilder<XES>::idComponent() << ":RDM";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe