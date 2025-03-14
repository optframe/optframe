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

#ifndef OPTFRAME_EXTENDED_MULTI_OBJ_SEARCH_HPP_
#define OPTFRAME_EXTENDED_MULTI_OBJ_SEARCH_HPP_

#include <iostream>
#include <string>
#include <utility>
#include <vector>
//
#include "Component.hpp"
#include "ComponentBuilder.hpp"
#include "Evaluation.hpp"
#include "MultiEvaluator.hpp"
#include "MultiObjSearch.hpp"
#include "Population.hpp"
#include "Solution.hpp"

namespace optframe {

template <class R, class X, class ADS = OPTFRAME_DEFAULT_ADS>
class ExtendedPareto {
 private:
  vector<Solution<R>*> paretoSet;
  vector<vector<MultiEvaluation<>*>> paretoFront;
  vector<Population<X, ADS>*> decodedSolutions;

 public:
  ExtendedPareto() {}

  virtual ~ExtendedPareto() {
    for (unsigned i = 0; i < paretoSet.size(); i++) delete paretoSet[i];
    paretoSet.clear();

    for (unsigned i = 0; i < paretoFront.size(); i++) {
      for (unsigned j = 0; j < paretoFront[i].size(); j++)
        delete paretoFront[i][j];
      paretoFront[i].clear();
    }
    paretoFront.clear();

    for (unsigned i = 0; i < decodedSolutions.size(); i++)
      delete decodedSolutions[i];
    decodedSolutions.clear();
  }

  Pareto<X>* getPareto() { return nullptr; }

  void push_back(Solution<R>* s, vector<MultiEvaluation<>*>& v_e,
                 Population<X, ADS>* v_x) {
    paretoSet.push_back(s);
    paretoFront.push_back(v_e);
    decodedSolutions.push_back(v_x);
  }

  void push_back(const Solution<R, ADS>& s,
                 const vector<MultiEvaluation<>*>& v_e,
                 const Population<X, ADS>& v_x) {
    paretoSet.push_back(&s->clone());
    vector<MultiEvaluation<>*> ve;
    for (unsigned mev = 0; mev < v_e.size(); mev++)
      ve.push_back(&v_e[mev]->clone());
    paretoFront.push_back(ve);
    decodedSolutions.push_back(&v_x.clone());
  }

  unsigned size() { return paretoSet.size(); }

  pair<Solution<R>*,
       pair<vector<MultiEvaluation<>*>, vector<Population<X, ADS>*>>>
  erase(unsigned index) {
    vector<MultiEvaluation<>*> vme = paretoFront.at(index);
    Population<X, ADS>* pop = decodedSolutions.at(index);
    pair<vector<MultiEvaluation<>*>, Population<X, ADS>*> p1 =
        make_pair(vme, pop);
    pair<Solution<R>*, pair<vector<MultiEvaluation<>*>, Population<X, ADS>*>> p;
    p = make_pair(paretoSet.at(index), p1);
    paretoSet.erase(paretoSet.begin() + index);
    paretoSet.erase(paretoFront.begin() + index);
    decodedSolutions.erase(decodedSolutions.begin() + index);

    return p;
  }

  pair<Solution<R>*, pair<vector<MultiEvaluation<>*>, Population<X, ADS>*>> at(
      unsigned index) {
    vector<MultiEvaluation<>*> vme = paretoFront.at(index);
    Population<X, ADS>* pop = decodedSolutions.at(index);
    pair<vector<MultiEvaluation<>*>, Population<X, ADS>*> p1 =
        make_pair(vme, pop);
    return make_pair(paretoSet.at(index), p1);
  }

  vector<Solution<R, ADS>*> getParetoSet() { return paretoSet; }

  vector<vector<Evaluation<>*>> getParetoFront() { return paretoFront; }

  void print() const override {
    std::cout << "ExtendedPareto size=" << paretoFront.size();
    std::cout << std::endl;
  }
};

template <class R, class X, class ADS = OPTFRAME_DEFAULT_ADS>
class ExtendedMultiObjSearch : public Component {
 public:
  ExtendedMultiObjSearch() {}

  virtual ~ExtendedMultiObjSearch() {}

  virtual ExtendedPareto<R, X, ADS>* search(
      double timelimit = 100000000, double target_f = 0,
      ExtendedPareto<R, X, ADS>* _pf = nullptr) = 0;

  virtual string log() { return "Empty heuristic log."; }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (Component::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << "ExtendedMultiObjSearch:";
    return ss.str();
  }

  virtual std::string id() const override { return idComponent(); }
};

template <class R, class X, class ADS = OPTFRAME_DEFAULT_ADS>
class ExtendedMultiObjSearchBuilder : public ComponentBuilder<R, ADS> {
 public:
  virtual ~ExtendedMultiObjSearchBuilder() {}

  virtual ExtendedMultiObjSearch<R, X, ADS>* build(Scanner& scanner,
                                                   HeuristicFactory<R, ADS>& hf,
                                                   string family = "") = 0;

  Component* buildComponent(Scanner& scanner, HeuristicFactory<R, ADS>& hf,
                            string family = "") override {
    return build(scanner, hf, family);
  }

  std::vector<std::pair<std::string, std::string>> parameters() override = 0;

  virtual bool canBuild(string) = 0;

  static std::string idComponent() {
    std::stringstream ss;
    ss << ComponentBuilder<R, ADS>::idComponent() << "ExtendedMultiObjSearch:";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif /* OPTFRAME_EXTENDED_MULTI_OBJ_SEARCH_HPP_ */
