// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_EA_MUTATION_HPP_
#define OPTFRAME_HEURISTICS_EA_MUTATION_HPP_

// C++
#include <chrono>
#include <random>
#include <string>
#include <utility>
#include <vector>
//
#include <OptFrame/Component.hpp>
#include <OptFrame/Evaluation.hpp>
#include <OptFrame/Helper/Solution.hpp>
#include <OptFrame/NS.hpp>
#include <OptFrame/RandGen.hpp>

#include "EA.hpp"

#ifndef _OPTFRAME_DBG_MUTATION_
#ifdef OPTFRAME_DEBUG
#define _OPTFRAME_DBG_MUTATION_
#else
#define _OPTFRAME_DBG_MUTATION_ while (false)
#endif /* OPTFRAME_DEBUG */
#endif /* _OPTFRAME_DBG_MUTATION_ */

namespace optframe {

template <XSolution S, XEvaluation XEv = Evaluation<>,
          XESolution XES = pair<S, XEv>>
class Mutation : public Component, public EA {
 public:
  virtual ~Mutation() {}

  virtual void mutate(S& individual, XEv& e) = 0;

  static string idComponent() {
    stringstream ss;
    ss << Component::idComponent() << ":" << EA::family() << ":Mutation";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

template <XSolution S, XEvaluation XEv = Evaluation<>,
          XESolution XES = pair<S, XEv>>
class BasicMutation : public Mutation<S, XEv> {
  using XSH = XES;  // primary-based search type only (BestType)

 protected:
  unsigned n;
  vector<NS<XES, XSH>*> vNS;
  RandGen& rg;

 public:
  BasicMutation(unsigned _n, vector<NS<XES, XSH>*> _vNS, RandGen& _rg)
      : n(_n), vNS(_vNS), rg(_rg) {}

  virtual ~BasicMutation() {}

  virtual void mutate(S& s, XEv& e) {
    for (unsigned i = 0; i < n; i++) {
      int x = rg.rand(vNS.size());
      Move<S, XEv>* mp = vNS[x]->validMove(s);
      if (!mp) {
        cout << "Warning: no move in BasicMutation!" << endl;
      } else {
        delete &mp->apply(e, s);
        delete mp;
      }
    }
  }

  static string idComponent() {
    stringstream ss;
    ss << Component::idComponent() << ":" << EA::family() << ":Mutation";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES>
#else
template <typename XES>
#endif
class BasicMutationBuilder : public ComponentBuilder<XES> {
  using XSH = XES;  // primary-based search type only (BestType)

 public:
  virtual ~BasicMutationBuilder() {}

  Component* buildComponent(Scanner& scanner, HeuristicFactory<XES>& hf,
                            string family = "") override {
    int n = *scanner.nextInt();

    vector<NS<XES, XSH>*> ns_list;
    std::string comp_id1 = scanner.next();
    int id1 = *scanner.nextInt();
    hf.assignList(ns_list, id1, comp_id1);

    // NOLINTNEXTLINE
    return new BasicMutation<XES>(n, ns_list, hf.getRandGen());
  }

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(make_pair("OptFrame:int", "number of moves"));
    stringstream ss;
    ss << NS<XES, XSH>::idComponent() << "[]";
    params.push_back(make_pair(ss.str(), "list of neighborhood structures"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == BasicMutation<XES>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << ComponentBuilder<XES>::idComponent() << "" << EA::family()
       << ":BasicMutation";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

// temporary fix for the true basic genetic algorithm! I will revisit this in
// the future to perform a proper naming convention
template <XSolution S, XEvaluation XEv = Evaluation<>,
          XESolution XES = pair<S, XEv>>
class SimpleMutation {
 protected:
  using Individual = S;
  // using Chromossome = R;
  using Fitness = XEv*;  // nullptr means there's no evaluation
  using VPopulation = std::vector<std::pair<Individual, Fitness>>;

 public:
  SimpleMutation() = default;
  virtual ~SimpleMutation() = default;

  virtual void mutate(VPopulation& population) = 0;
};

/**********************/
/* MUTATION EXAMPLES */
/**********************/

// changes 100beta% individuals chosen randomly -- may choose the same
// individual more than once user should program the function that changes the
// individual
template <XSolution S, XEvaluation XEv = Evaluation<>,
          XESolution XES = pair<S, XEv>>
class DefaultMutation : public SimpleMutation<S, XEv> {
 protected:
  using Individual = S;
  // using Chromossome = R;
  using Fitness = XEv*;  // nullptr means there's no evaluation
  using VPopulation = std::vector<std::pair<Individual, Fitness>>;
  double omega;

 public:
  DefaultMutation() = delete;
  explicit DefaultMutation(double mutationRate) : omega(mutationRate) {
    assert(mutationRate >= 0.0 && mutationRate <= 1.0);
  }
  virtual ~DefaultMutation() = default;

  virtual void mutate(
      Individual&) = 0;  // Should change the solution unpredicatelly
                         // Individual is passed so the user may change ADS if
                         // he wants to

  void mutate(VPopulation& population) override {
    _OPTFRAME_DBG_MUTATION_ std::cerr
        << "-OptDebug- Starting mutation operator. Will insert "
        << static_cast<int>(population.size() * omega)
        << " mutants into the population." << std::endl;

    // todo: use randgen
    std::mt19937 mersenne_twister(
        std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<unsigned int> dist(0u, population.size() - 1);
    int iteration_count = population.size() * omega;
    for (int i = 0; i < iteration_count; ++i) {
      int mut_pos = dist(mersenne_twister);
      _OPTFRAME_DBG_MUTATION_ std::cerr << "-OptDebug- Mutating individual "
                                        << mut_pos << std::endl;
      mutate(population[mut_pos].first.getR());
      if (population[mut_pos].second) delete population[mut_pos].second;
      population[mut_pos].second = nullptr;
    }

    _OPTFRAME_DBG_MUTATION_ std::cerr << "-OptDebug- Mutation operator ended"
                                      << std::endl;
  }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_EA_MUTATION_HPP_
