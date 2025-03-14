#pragma once

namespace optframe {

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES>
#else
template <typename XES>
#endif
class FirstImprovementBuilder : public LocalSearchBuilder<XES> {
 public:
  virtual ~FirstImprovementBuilder() {}

  LocalSearch<XES>* build(Scanner& scanner, HeuristicFactory<XES>& hf,
                          string family = "") override {
    if (this->verbose)
      std::cout << "Debug: FirstImprovementBuilder::build()" << std::endl;

    sptr<GeneralEvaluator<XES>> eval;
    std::string sid_0 = scanner.next();
    int id_0 = *scanner.nextInt();
    hf.assign(eval, id_0, sid_0);

    sptr<NSSeq<XES>> nsseq;
    std::string sid_1 = scanner.next();
    int id_1 = *scanner.nextInt();
    hf.assign(nsseq, id_1, sid_1);

    return new FirstImprovement<XES>(eval, nsseq);
  }

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(
        make_pair(GeneralEvaluator<XES>::idComponent(), "evaluation function"));
    params.push_back(
        make_pair(NSSeq<XES>::idComponent(), "neighborhood structure"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == FirstImprovement<XES>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << LocalSearchBuilder<XES>::idComponent() << ":FI";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  virtual string id() const override { return idComponent(); }
};

}  // namespace optframe