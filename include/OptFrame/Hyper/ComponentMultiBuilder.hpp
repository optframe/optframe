// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2025 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HYPER_COMPONENTMULTIBUILDER_HPP_
#define OPTFRAME_HYPER_COMPONENTMULTIBUILDER_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

// C++
#include <string>
#include <utility>
#include <vector>
//

#include <OptFrame/Component.hpp>
// #include "Evaluation.hpp"
#include <OptFrame/Core/MultiESolution.hpp>
#include <OptFrame/Core/VEPopulation.hpp>
#include <OptFrame/Pareto/MultiEvaluation.hpp>
// #include "Solution.hpp"
// #include "Solutions/CopySolution.hpp"
#include <OptFrame/Scanner++/Scanner.hpp>

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

using scannerpp::Scanner;

namespace optframe {

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
MOD_EXPORT template <XESolution XES>
class HeuristicFactory;
#else
MOD_EXPORT template <typename XES>
class HeuristicFactory;
#endif

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
MOD_EXPORT template <XSolution S, XEvaluation XMEv = MultiEvaluation<>,
                     XESolution XMES = std::pair<S, XMEv>,
                     X2ESolution<XMES> X2MES = VEPopulation<XMES>>
#else
MOD_EXPORT template <typename S, typename XMEv = MultiEvaluation<>,
                     typename XMES = std::pair<S, XMEv>,
                     typename X2MES = VEPopulation<XMES>>
#endif
class ComponentMultiBuilder : public Component {
  // TODO: pass these as templates as well
  using XEv = typename XMEv::XEv;
  using XES = std::pair<S, XEv>;
  using X2ES = MultiESolution<XES>;  // TODO: check this!

 public:
  virtual ~ComponentMultiBuilder() {}

  virtual Component* buildMultiComponent(Scanner& scanner,
                                         HeuristicFactory<XES>& hf,
                                         std::string family = "") = 0;

  virtual std::vector<std::pair<std::string, std::string>> parameters() = 0;

  virtual bool canBuild(std::string) = 0;

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":ComponentMultiBuilder:";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HYPER_COMPONENTMULTIBUILDER_HPP_
