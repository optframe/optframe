#pragma once

namespace optframe {

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES>
#else
template <typename XES>
#endif
class HillClimbingBuilder : public LocalSearchBuilder<XES> {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;

 public:
  virtual ~HillClimbingBuilder() = default;

  // NOLINTNEXTLINE
  LocalSearch<XES>* build(Scanner& scanner, HeuristicFactory<XES>& hf,
                          string family = "") override {
    sptr<GeneralEvaluator<XES>> eval;
    std::string comp_id1 = scanner.next();
    int id1 = *scanner.nextInt();
    hf.assign(eval, id1, comp_id1);

    string rest = scanner.rest();

    pair<sptr<LocalSearch<XES>>, std::string> method;
    method = hf.createLocalSearch(rest);

    sptr<LocalSearch<XES>> h = method.first;

    scanner = Scanner(method.second);

    // NOLINTNEXTLINE
    return new HillClimbing<XES>(eval, h);
  }

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(make_pair(Evaluator<S, XEv, XES>::idComponent(),
                               "evaluation function"));
    params.push_back(
        make_pair(LocalSearch<XES>::idComponent(), "local search"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == HillClimbing<XES, XEv>::idComponent();
  }

  static std::string idComponent() {
    stringstream ss;
    ss << LocalSearchBuilder<XES>::idComponent() << ":HC";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  string id() const override { return idComponent(); }
};

}  // namespace optframe