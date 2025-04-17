// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2025 - OptFrame - https://github.com/optframe/optframe

#pragma once

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

#include <OptFrame/Hyper/ComponentBuilder.hpp>

#define MOD_EXPORT
#else

// CANNOT IMPORT HERE... Already part of optframe.core
/*
import std;
import optframe.component;
import optframe.concepts;
*/

// do NOT import/export modules on .hpp... only on .cppm

#define MOD_EXPORT export

#endif

namespace optframe {

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
MOD_EXPORT template <XESolution XES, XSearch<XES> XSH = XES>
#else
MOD_EXPORT template <typename XES, typename XSH = XES>
#endif
class GlobalSearchBuilder : public ComponentBuilder<XES> {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;

 public:
  virtual ~GlobalSearchBuilder() = default;

  // NOLINTNEXTLINE
  virtual GlobalSearch<XES, XSH>* build(Scanner& scanner,
                                        HeuristicFactory<XES>& hf,
                                        std::string family = "") = 0;
  // NOLINTNEXTLINE
  Component* buildComponent(Scanner& scanner, HeuristicFactory<XES>& hf,
                            std::string family = "") override {
    return build(scanner, hf, family);
  }

  std::vector<std::pair<std::string, std::string>> parameters() override = 0;

  bool canBuild(std::string) override = 0;

  std::string toString() const override { return id(); }

  static std::string idComponent() {
    std::stringstream ss;
    ss << ComponentBuilder<XES>::idComponent() << "GlobalSearch:";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe
