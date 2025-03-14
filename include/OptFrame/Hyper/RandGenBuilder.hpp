// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2025 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_RANDGEN_BUILDER_HPP_
#define OPTFRAME_RANDGEN_BUILDER_HPP_

#if (__cplusplus < 202302L) || defined(NO_CXX_MODULES)

#define MOD_EXPORT
#else

// CANNOT IMPORT HERE... Already part of optframe.core
/*
import std;
import optframe.component;
import optframe.concepts;
*/

// do NOT export modules on .hpp... only on .cppm

#define MOD_EXPORT export

#endif

namespace optframe {

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
MOD_EXPORT template <XESolution XES>
#else
MOD_EXPORT template <typename XES>
#endif
class RandGenBuilder : public ComponentBuilder<XES> {
 public:
  virtual ~RandGenBuilder() {}

  Component* buildComponent(Scanner& scanner, HeuristicFactory<XES>& hf,
                            std::string family = "") override {
    if (!scanner.hasNext()) return nullptr;

    long seed = *scanner.nextLong();

    return new RandGen(seed);
  }

  std::vector<std::pair<std::string, std::string>> parameters() override {
    std::vector<std::pair<std::string, std::string>> params;
    params.push_back(std::make_pair("long", "seed"));
    return params;
  }

  bool canBuild(std::string component) override {
    return component == RandGen::idComponent();
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << ComponentBuilder<XES>::idComponent() << "RandGen";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  virtual std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_RANDGEN_BUILDER_HPP_
