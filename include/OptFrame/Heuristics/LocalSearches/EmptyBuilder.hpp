
namespace optframe {

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES>
#else
template <typename XES>
#endif
class EmptyLocalSearchBuilder : public LocalSearchBuilder<XES> {
 public:
  virtual ~EmptyLocalSearchBuilder() {}

  LocalSearch<XES>* build(Scanner& scanner, HeuristicFactory<XES>& hf,
                          string family = "") override {
    return new EmptyLocalSearch<XES>;
  }

  std::vector<std::pair<std::string, std::string>> parameters() override {
    std::vector<std::pair<std::string, std::string>> params;

    return params;
  }

  bool canBuild(std::string component) override {
    return component == EmptyLocalSearch<XES>::idComponent();
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << LocalSearchBuilder<XES>::idComponent() << ":Empty";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  virtual std::string id() const override { return idComponent(); }
};

}  // namespace optframe