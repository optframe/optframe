#pragma once

namespace optframe {

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES>
#else
template <typename XES>
#endif
class BestImprovementBuilder : public LocalSearchBuilder<XES> {
 public:
  virtual ~BestImprovementBuilder() {}

  LocalSearch<XES>* build(Scanner& scanner, HeuristicFactory<XES>& hf,
                          string family = "") override {
    if (this->verbose)
      std::cout << "Debug: BestImprovementBuilder::build()" << std::endl;
    if (!scanner.hasNext()) return nullptr;
    sptr<GeneralEvaluator<XES>> eval;
    std::string comp_ev_id = scanner.next();
    if (!scanner.hasNextInt()) return nullptr;
    int ev_id = *scanner.nextInt();
    hf.assign(eval, ev_id, comp_ev_id);

    if (!scanner.hasNext()) return nullptr;
    sptr<NSSeq<XES>> nsseq;
    std::string comp_nsseq_id = scanner.next();
    if (!scanner.hasNextInt()) return nullptr;
    int nsseq_id = *scanner.nextInt();
    hf.assign(nsseq, nsseq_id, comp_nsseq_id);

    return new BestImprovement<XES>(eval, nsseq);
  }

  std::vector<std::pair<std::string, std::string>> parameters() override {
    std::vector<std::pair<std::string, std::string>> params;
    params.push_back(
        make_pair(GeneralEvaluator<XES>::idComponent(), "evaluation function"));
    params.push_back(
        make_pair(NSSeq<XES>::idComponent(), "neighborhood structure"));

    return params;
  }

  bool canBuild(string component) override {
    return component == BestImprovement<XES>::idComponent();
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << LocalSearchBuilder<XES>::idComponent() << ":BI";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe