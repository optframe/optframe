#pragma once

#include <OptFrame/Hyper/ComponentBuilder.hpp>
#include <OptFrame/Search/LocalSearch.hpp>

namespace optframe {

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES>
#else
template <typename XES>
#endif
class LocalSearchBuilder : public ComponentBuilder<XES> {
  using XSH = XES;

 public:
  virtual ~LocalSearchBuilder() = default;

  // NOLINTNEXTLINE
  virtual LocalSearch<XES, XSH>* build(Scanner& scanner,
                                       HeuristicFactory<XES>& hf,
                                       string family = "") = 0;

  // NOLINTNEXTLINE
  Component* buildComponent(Scanner& scanner, HeuristicFactory<XES>& hf,
                            string family = "") override {
    return build(scanner, hf, family);
  }

  std::vector<std::pair<std::string, std::string>> parameters() override = 0;

  bool canBuild(string) override = 0;

  static string idComponent() {
    stringstream ss;
    ss << ComponentBuilder<XES>::idComponent() << "LocalSearch";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe