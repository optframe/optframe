// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2025 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_RANDGEN_BUILDER_HPP_
#define OPTFRAME_RANDGEN_BUILDER_HPP_

namespace optframe {

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES>
#else
template <typename XES>
#endif
class RandGenBuilder : public ComponentBuilder<XES> {
 public:
  virtual ~RandGenBuilder() {}

  Component* buildComponent(Scanner& scanner, HeuristicFactory<XES>& hf,
                            string family = "") override {
    if (!scanner.hasNext()) return nullptr;

    long seed = *scanner.nextLong();

    return new RandGen(seed);
  }

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(make_pair("long", "seed"));
    return params;
  }

  bool canBuild(std::string component) override {
    return component == RandGen::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << ComponentBuilder<XES>::idComponent() << "RandGen";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  virtual std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_RANDGEN_BUILDER_HPP_
