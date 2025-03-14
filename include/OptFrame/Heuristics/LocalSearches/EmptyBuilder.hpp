
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

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;

    return params;
  }

  bool canBuild(std::string component) override {
    return component == EmptyLocalSearch<XES>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << LocalSearchBuilder<XES>::idComponent() << ":Empty";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  virtual string id() const override { return idComponent(); }
};

}  // namespace optframe