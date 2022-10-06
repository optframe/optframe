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

#ifndef OPTFRAME_HELPER_INITIALMULTIESOLUTION_HPP_
#define OPTFRAME_HELPER_INITIALMULTIESOLUTION_HPP_

// C++
#include <string>
//
#include <OptFrame/Component.hpp>
#include <OptFrame/Constructive.hpp>
#include <OptFrame/Evaluation.hpp>
#include <OptFrame/Helper/MultiESolution.hpp>
#include <OptFrame/Helper/VEPopulation.hpp>

// #include "EA.h"

// using namespace std;

namespace optframe {

// IMPORTANT: by default, MultiESolution is generating VEPopulation types
// Because MultiESolution class is currently broken... Too bad!

// template <XESolution XES, X2ESolution<XES> X2ES  = MultiESolution<XES>>
template <XESolution XES, X2ESolution<XES> X2ES = VEPopulation<XES>>
class InitialMultiESolution : public Component {
 public:
  virtual ~InitialMultiESolution() {
  }

  virtual X2ES generateEPopulation(unsigned populationSize) = 0;

  bool compatible(std::string s) override {
    return (s == idComponent()) || (Component::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":InitialMultiESolution";
    return ss.str();
  }

  std::string id() const override {
    return idComponent();
  }
};

// ================================================================
// BasicInitialMultiESolution<XES> -> generates MultiESolution<XES>
// ================================================================
// IMPORTANT: by default, MultiESolution is generating VEPopulation types
// Because MultiESolution class is currently broken... Too bad!
// ================================================================

// TODO(igormcoelho): should we allow this to initialize multiple
//  "MultiESolution"-like types?
// I mean, this could initialize VEPopulation, EPopulation, etc...
// Maybe this is useful! This can be done!
//  But this overcomplicates things with builders...
//  so, we need some specific type anyway.
//
template <XESolution XES>
class BasicInitialMultiESolution : public InitialMultiESolution<
                                       XES, VEPopulation<XES>> {
  using XEv = typename XES::second_type;

 public:
  using S = typename XES::first_type;

  sref<Constructive<S>> constructive;
  sref<IEvaluator<XES>> evaluator;

  explicit BasicInitialMultiESolution(
      sref<Constructive<S>> _constructive,
      sref<IEvaluator<XES>> _evaluator)
      : constructive{_constructive},
        evaluator{_evaluator} {
  }

  virtual ~BasicInitialMultiESolution() {
  }

  VEPopulation<XES> generateEPopulation(unsigned populationSize) override {
    VEPopulation<XES> p;
    // TODO(igormcoelho): generatePopulation must have timelimit!
    double timelimit = 0.0;
    for (unsigned i = 0; i < populationSize; i++) {
      S s = *constructive->generateSolution(timelimit);
      XEv e = evaluator->evaluate(s);
      p.push_back({s, e});
    }
    return p;
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) ||
           (InitialMultiESolution<XES>::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << InitialMultiESolution<XES>::idComponent();
    ss << ":BasicInitialPopulation";
    return ss.str();
  }

  std::string toString() const override {
    return id();
  }

  std::string id() const override {
    return idComponent();
  }
};

// TODO(igormcoelho): fix this builder later!!!

/*

template <XSolution S,
          XEvaluation XEv = Evaluation<>,
          XESolution XES = std::pair<S, XEv>>
class BasicInitialPopulationBuilder : public ComponentBuilder<S, XEv, XES> {
 public:
  virtual ~BasicInitialPopulationBuilder() {
  }

  virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<R, ADS>& hf, string family = "") {
    Constructive<S>* c;
    hf.assign(c, *scanner.nextInt(), scanner.next());  // reads backwards!

    return new BasicInitialMultiSolution<S>(*c);
  }

  virtual vector<pair<string, string>> parameters() {
    vector<pair<string, string>> params;
    //params.push_back(make_pair(Constructive<S>::idComponent(), "constructive heuristic"));
    params.push_back(make_pair(InitialSearch<XES>::idComponent(), "constructive heuristic"));
    return params;
  }

  virtual bool canBuild(string component) {
    return component == BasicInitialMultiSolution<S>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << ComponentBuilder<R, ADS>::idComponent() << "" << EA::family() << ":BasicInitialPopulation";
    return ss.str();
  }

  virtual string id() const override {
    return idComponent();
  }
};
*/

}  // namespace optframe

#endif  // OPTFRAME_HELPER_INITIALMULTIESOLUTION_HPP_
