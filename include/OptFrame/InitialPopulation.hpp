// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_INITIALPOPULATION_HPP_
#define OPTFRAME_INITIALPOPULATION_HPP_

// #include <chrono>
#include <random>
#include <string>
#include <utility>
#include <vector>
//
#include <OptFrame/Component.hpp>
#include <OptFrame/Constructive.hpp>
#include <OptFrame/Helper/Population.hpp>
#include <OptFrame/Helper/VEPopulation.hpp>
#include <OptFrame/Heuristics/GRASP/GRConstructive.hpp>
#include <OptFrame/RandGen.hpp>

namespace optframe {

// IMPORTANT:
// A difference from InitialPopulation and InitialEPopulation is that
// initpop does not care about evaluations, but initepop guarantees it.
//
// However, it's bad to have distinct Population types, separating solutions
// from evaluations, when we need both united... so a proposed solution here is
// to allow InitialEPopulation to NOT evaluate solutions, just keep them
// outdated, if needed.
//

// XES is the BASE TYPE
// X2ES is the REAL TYPE
// By default, VEPopulation is std::vector<XES>
//
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES, X2ESolution<XES> X2ES = VEPopulation<XES>>
#else
template <typename XES, typename X2ES = VEPopulation<XES>>
#endif
class InitialEPopulation : public Component {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;

 public:
  // NOLINTNEXTLINE
  virtual ~InitialEPopulation() override = default;

  // By default, this generator will evaluate ESolution pairs here,
  // but user can opt-out of this, leaving evaluations for the future
  // See BRKGA on a implementation over this concept.
  virtual bool canEvaluate() const { return true; }

  virtual X2ES generateEPopulation(unsigned populationSize,
                                   double timelimit) = 0;

  static string idComponent() {
    stringstream ss;
    ss << Component::idComponent() << ":InitialEPopulation"
       << Domain::getAlternativeDomain<X2ES>("<X2ESf64>");
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

// =================================================================
// THIS IS WHERE THE CONFUSION BEGINS... BECAUSE OF LEGACY STUFF!
// Should InitialPopulation guarantee that evaluations are correct?
// I don't think so...
// otherwise, BasicInitialPopulation would not work, as it doesnt require an
// Evaluator
// =================================================================

// S is the BASE TYPE
// X2S is the REAL TYPE
// By default, VPopulation is std::vector<XS>
//
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XSolution S, X2Solution<S> X2S = VPopulation<S>>
#else
template <typename S, typename X2S = VPopulation<S>>
#endif
class InitialPopulation : public Component {
 public:
  ~InitialPopulation() override = default;

  virtual X2S generatePopulation(unsigned populationSize, double timelimit) = 0;

  static string idComponent() {
    stringstream ss;
    ss << Component::idComponent() << ":InitialPopulation"
       << Domain::getAlternativeDomain<X2S>("<X2S>");
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XSolution S, X2Solution<S> X2S = VPopulation<S>>
#else
template <typename S, typename X2S = VPopulation<S>>
#endif
class BasicInitialPopulation : public InitialPopulation<S, X2S> {
 public:
  Constructive<S>& constructive;

  explicit BasicInitialPopulation(Constructive<S>& _constructive)
      : constructive(_constructive) {}

  virtual ~BasicInitialPopulation() = default;

  // Population<XES>
  virtual X2S generatePopulation(unsigned populationSize, double timelimit) {
    // Population<XES>* p = new Population<XES>;
    X2S p;
    for (unsigned i = 0; i < populationSize; i++)
      p.push_back(constructive.generateSolution(timelimit));
    return p;
    // return *p;
  }

  static string idComponent() {
    stringstream ss;
    ss << InitialPopulation<S, X2S>::idComponent() << ":BasicInitialPopulation";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XSolution S, X2Solution<S> X2S = VPopulation<S>>
#else
template <typename S, typename X2S = VPopulation<S>>
#endif
class GRInitialPopulation : public InitialPopulation<S, X2S> {
 public:
  GRConstructive<S>& constructive;
  RandGen& rg;
  double maxAlpha;  // limit the solution to be not so random

  GRInitialPopulation(GRConstructive<S>& _constructive, RandGen& _rg,
                      double _maxAlpha)
      : constructive(_constructive), rg(_rg), maxAlpha(_maxAlpha) {}

  virtual ~GRInitialPopulation() = default;

  // Population<XES>
  X2S generatePopulation(unsigned populationSize, double timelimit) override {
    // Population<XES> pop;
    X2S pop;
    for (unsigned i = 0; i < populationSize; i++) {
      float alpha = rg.rand01();
      while (alpha > maxAlpha) {
        alpha = rg.rand01();
      }

      if (alpha == 0) alpha = 0.00001;

      std::optional<S> s = constructive.generateGRSolution(alpha, timelimit);
      // XES se = { *s, Evaluation<>{} };
      S sol = (*s);
      pop.push_back(sol);  // the end of solution
    }
    return pop;
  }

  static string idComponent() {
    stringstream ss;
    ss << InitialPopulation<S, X2S>::idComponent() << ":GRInitialPopulation";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  std::string id() const override { return idComponent(); }
};

template <XESolution XES>
class SimpleInitialPopulation {
  using S = typename XES::first_type;
  using XEv = typename XES::first_type;

 protected:
  using Individual = S;
  // using Chromossome = R;
  using Fitness = XEv*;  // nullptr means there's no evaluation
  using Population = vector<pair<Individual, Fitness>>;

 public:
  unsigned int initialPopSize;

  explicit SimpleInitialPopulation(unsigned int _initialPopSize)
      : initialPopSize(_initialPopSize) {
    assert(initialPopSize >= 2);
  }

  virtual ~SimpleInitialPopulation() = default;

  virtual Population generate() = 0;
};

/**********************/
/* INITIAL POPULATION EXAMPLES */
/**********************/

// generates random individuals based on user programmed method
template <XESolution XES>
class RandomInitialPopulation : public SimpleInitialPopulation<XES> {
  using S = typename XES::first_type;
  using XEv = typename XES::first_type;

 protected:
  using Individual = S;
  // using Chromossome = R;
  using Fitness = XEv*;  // nullptr means there's no evaluation
  using Population = vector<pair<Individual, Fitness>>;

 public:
  RandomInitialPopulation() = delete;
  explicit RandomInitialPopulation(unsigned int initialPopSize)
      : SimpleInitialPopulation<XES>(initialPopSize) {}
  virtual ~RandomInitialPopulation() = default;

  virtual Individual generateIndividual() = 0;

  virtual Population generate() {
    if (Component::verbose) {
      std::cerr << "-OptDebug- Generating Initial Population with "
                   "RandomInitialPopulation of size "
                << this->initialPopSize << std::endl;
    }

    Population pop;  // won't reserve memory, in applications with a very heavy
                     // empty constructor can be costly... I will take the cost
                     // of reallocating the pop
    for (int i = 0; i < this->initialPopSize; ++i)
      pop.emplace_back(std::pair<Individual, Fitness>(
          generateIndividual(),
          nullptr));  // maybe reserve memory... but i don't know how costly is
                      // the empty constructor for the solution

    if (Component::verbose) {
      std::cerr
          << "-OptDebug- RandomInitialPopulation finished with a population of "
          << pop.size() << " individuals" << std::endl;
    }
    return pop;
  }
};

}  // namespace optframe

#endif  // OPTFRAME_INITIALPOPULATION_HPP_
