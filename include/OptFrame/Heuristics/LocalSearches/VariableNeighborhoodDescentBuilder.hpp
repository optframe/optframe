#pragma once

namespace optframe {

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES>
#else
template <typename XES>
#endif
class VariableNeighborhoodDescentBuilder : public LocalSearchBuilder<XES> {
 public:
  virtual ~VariableNeighborhoodDescentBuilder() {}

  LocalSearch<XES>* build(Scanner& scanner, HeuristicFactory<XES>& hf,
                          string family = "") override {
    if (this->verbose)
      std::cout << "Debug: VariableNeighborhoodDescentBuilder::build()"
                << std::endl;

    sptr<GeneralEvaluator<XES>> eval;
    std::string comp_ev_id = scanner.next();
    int ev_id = *scanner.nextInt();
    hf.assign(eval, ev_id, comp_ev_id);

    vsptr<LocalSearch<XES>> _hlist;
    std::string comp_list_id = scanner.next();
    int list_id = *scanner.nextInt();
    hf.assignList(_hlist, list_id, comp_list_id);
    vsref<LocalSearch<XES>> hlist;
    for (auto x : _hlist) hlist.push_back(x);

    return new VariableNeighborhoodDescent<XES>(eval, hlist);
  }

  std::vector<std::pair<std::string, std::string>> parameters() override {
    std::vector<std::pair<std::string, std::string>> params;
    params.push_back(
        make_pair(GeneralEvaluator<XES>::idComponent(), "evaluation function"));
    std::stringstream ss;
    ss << LocalSearch<XES>::idComponent() << "[]";
    params.push_back(std::make_pair(ss.str(), "list of local searches"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == VariableNeighborhoodDescent<XES>::idComponent();
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << LocalSearchBuilder<XES>::idComponent() << ":VND";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe