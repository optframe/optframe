#pragma once

namespace optframe {

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XRepresentation R, class ADS, XBaseSolution<R, ADS> S,
          XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>,
          X2ESolution<XES> X2ES = MultiESolution<XES>>
#else
template <typename R, class ADS, typename S, typename XEv = Evaluation<>,
          typename XES = pair<S, XEv>, typename X2ES = MultiESolution<XES>>
#endif
class VariableNeighborhoodDescentUpdateADSBuilder
    : public LocalSearchBuilder<XES> {
 public:
  virtual ~VariableNeighborhoodDescentUpdateADSBuilder() = default;

  // NOLINTNEXTLINE
  LocalSearch<XES>* build(Scanner& scanner, HeuristicFactory<XES>& hf,
                          string family = "") override {
    sptr<GeneralEvaluator<XES>> eval;
    std::string comp_id1 = scanner.next();
    int id1 = *scanner.nextInt();
    hf.assign(eval, id1, comp_id1);

    sptr<ADSManager<R, ADS, S>> adsMan;
    std::string comp_id2 = scanner.next();
    int id2 = *scanner.nextInt();
    hf.assign(adsMan, id2, comp_id2);

    vsptr<LocalSearch<XES>> _hlist;
    std::string comp_id3 = scanner.next();
    int id3 = *scanner.nextInt();
    hf.assignList(_hlist, id3, comp_id3);
    vsref<LocalSearch<XES>> hlist;
    for (auto x : _hlist) hlist.push_back(x);

    // NOLINTNEXTLINE
    return new VariableNeighborhoodDescentUpdateADS<R, ADS, S, XEv>(
        eval, adsMan, hlist);
  }

  std::vector<std::pair<std::string, std::string>> parameters() override {
    std::vector<std::pair<std::string, std::string>> params;
    params.push_back(std::make_pair(Evaluator<S, XEv, XES>::idComponent(),
                               "evaluation function"));

    params.push_back(
        make_pair(ADSManager<R, ADS, S>::idComponent(), "ADSManager function"));

    std::stringstream ss;
    ss << LocalSearch<XES>::idComponent() << "[]";
    params.push_back(std::make_pair(ss.str(), "list of local searches"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component ==
           VariableNeighborhoodDescentUpdateADS<R, ADS, S, XEv>::idComponent();
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << LocalSearchBuilder<XES>::idComponent() << ":VNDUpdateADS";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe