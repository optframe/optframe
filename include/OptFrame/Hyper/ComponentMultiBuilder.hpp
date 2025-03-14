// OptFrame 4.2 - Optimization Framework
// Copyright (C) 2009-2021 - MIT LICENSE
// https://github.com/optframe/optframe
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef OPTFRAME_HYPER_COMPONENTMULTIBUILDER_HPP_
#define OPTFRAME_HYPER_COMPONENTMULTIBUILDER_HPP_

#if (__cplusplus < 202302L) || defined(NO_CXX_MODULES)

// C++
#include <string>
#include <utility>
#include <vector>
//

#include <OptFrame/Component.hpp>
// #include "Evaluation.hpp"
#include <OptFrame/Core/MultiESolution.hpp>
#include <OptFrame/Core/VEPopulation.hpp>
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

// do NOT export modules on .hpp... only on .cppm

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
