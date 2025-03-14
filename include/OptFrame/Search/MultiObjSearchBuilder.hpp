#pragma once

namespace optframe {

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES>
#else
template <typename XES>
#endif
class MultiObjSearchBuilder : public ComponentBuilder<XES> {
 public:
  virtual ~MultiObjSearchBuilder() = default;

  // NOLINTNEXTLINE
  virtual MultiObjSearch<XES>* build(Scanner& scanner,
                                     HeuristicFactory<XES>& hf,
                                     string family = "") = 0;
  // NOLINTNEXTLINE
  Component* buildComponent(Scanner& scanner, HeuristicFactory<XES>& hf,
                            std::string family = "") override {
    return build(scanner, hf, family);
  }

  std::vector<std::pair<std::string, std::string>> parameters() override = 0;

  bool canBuild(std::string) override = 0;

  static std::string idComponent() {
    std::stringstream ss;
    ss << ComponentBuilder<XES>::idComponent() << "MultiObjSearch:";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe