// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_PSO_HPP_
#define OPTFRAME_PSO_HPP_

// C++
#include <algorithm>
//
// #include "../../IPopulational.hpp"
/// #include "../../InitialPopulation.hpp"
#include <OptFrame/Core/RandGen.hpp>
#include <OptFrame/Helper/EPopulation.hpp>
#include <OptFrame/SingleObjSearch.hpp>
#include <OptFrame/printable/printable.hpp>  // TODO: remove!

/*
#include "Crossover.hpp"
#include "DecoderRandomKeys.hpp"
#include "Elitism.hpp"
#include "Mutation.hpp"
#include "Selection.hpp"
*/

// Particle Swarm Optimization - PSO

namespace optframe {

// begin PSO
// template<XEvaluation XEv = Evaluation<>, optframe::comparability KeyType =
// double>
/// template<optframe::comparability KeyType = double>
struct Bird {
  vector<double> velocity;   // assuming 'double' for now
  vector<double> position;   // assuming 'double' for now
  vector<double> localbest;  // assuming 'double' for now

  // TODO: cache localbest XEv here, and make Bird some IESolution... looks
  // promising!

  friend std::ostream& operator<<(std::ostream& os, const Bird& b) {
    os << "bird{"
       << "velocity: " << b.velocity << " position:" << b.position
       << " localbest:" << b.localbest << "}";
    return os;
  }
};

using EBird = std::pair<Bird, Evaluation<>>;

static_assert(XESolution<EBird>);
static_assert(XSearch<EBird, EBird>);

static_assert(XESolution<std::pair<std::vector<double>, Evaluation<>>>);

static_assert(X2ESolution<EPopulation<EBird>, EBird>);
static_assert(XSearch<EPopulation<EBird>, EBird>);

template <
    /// optframe::comparability KeyType = double,
    XEvaluation XEv = Evaluation<>,
    XESolution XES = std::pair<std::vector<double>, XEv>,
    XESolution XES2 = EBird, XSearch<XES2> XSH2 = EPopulation<XES2>>
class PSO : public SingleObjSearch<XES, XES2, XSH2>
// TODO: add IPopulational
//, public IPopulational<XES, XES, XES2>
{
  using S = typename XES::first_type;
  // using XEv = typename XES::second_type;
  using XSH = XES;

  // private:
  //    Bird Global;

 protected:
  Evaluator<S, XEv, XES>& evaluator;  // Check to avoid memory leaks
  // InitialPopulation<XES2>& initPop; // TODO: add if necessary
  /*
      RANDOM PSO
    * pop_size -> Número de pájaros (o partículas)
    * iter_max -> Número máximo de iteraciones de PSO
    * cI -> Vector con los topes inferiores de los parámetros
    * cS -> Vector con los topes superiores de los parámetros
    * FUNCION -> Función que pretendemos minimizar
    *
    * */
  // population size
  unsigned pop_size;
  // number of generations (stop criteria)
  int iter_max;
  // -> Vector con los topes inferiores de los parámetros
  vector<double> cI;
  // -> Vector con los topes superiores de los parámetros
  vector<double> cS;
  // random number generator
  sref<RandGen> rg;

 public:
  PSO(Evaluator<S, XEv, XES>& evaluator, unsigned pop_size, int iter_max,
      const vector<double>& cI, const vector<double>& cS,
      sref<RandGen> _rg = new RandGen)
      : evaluator(evaluator),
        pop_size(pop_size),
        iter_max(iter_max),
        cI(cI),
        cS(cS),
        rg{_rg} {
    assert(cI.size() == cS.size());
    for (unsigned i = 0; i < cI.size(); i++) assert(cI[i] < cS[i]);
  }

  virtual EPopulation<XES2> generatePopulation() {
    EPopulation<XES2> swarm;
    for (unsigned i = 0; i < this->pop_size; i++) {
      // create a new Bird
      Bird b;
      b.velocity = std::vector<double>(this->cI.size(), 0.0);
      b.position = std::vector<double>(this->cI.size(), 0.0);
      for (unsigned j = 0; j < this->cI.size(); j++) {
        b.position[j] = runif(this->cI[j], this->cS[j]);
        b.velocity[j] = 0.1 * runif(this->cI[j], this->cS[j]);
      }
      b.localbest = b.position;
      // add bird to swarm (no evaluation is given)
      swarm.push_back(b);
    }
    return swarm;
  }

  // update bird position (random mutation)
  void birdMutation(Bird& b) {
    if (rg->rand01() < 0.1 * 0.1) {
      for (unsigned j = 0; j < this->cS.size(); j++) {
        double r = rg->rand01();
        if (r < 0.5)
          b.position[j] += (cS[j] - cI[j]) * (::pow(2.0 * r, 1.0 / 21) - 1);
        else
          b.position[j] +=
              (cS[j] - cI[j]) * (::pow(2.0 * (1 - r), 1.0 / 21) + 1);
      }
    }
  }

  // checks if bird position/velocity is within valid bounds
  void boundCheck(Bird& b) {
    unsigned count = 0;
    while (count < this->cS.size()) {
      if (b.position[count] < cI[count] || b.position[count] > cS[count]) {
        for (unsigned j = 0; j < this->cS.size(); j++) {
          b.position[j] = runif(this->cI[j], this->cS[j]);
          b.velocity[j] = 0.1 * runif(this->cI[j], this->cS[j]);
        }
        break;
      }
      count++;
    }  // while
  }  // BoundCheck

  virtual ~PSO() {
    // nothing to delete... NICE!
  }

  // uniform random between interval
  double runif(double a, double b) {
    if (a == b) return a;
    return a + (b - a) * rg->rand01();
  }

  // PSO Execution Context
  struct ExecutionContext {
    // self reference can be very useful
    PSO* self;
    // TODO: add more variables with desired access on callbacks
    int count_gen;
  };

  // override callbacks (similar to "double-dispatch", but hiding with
  // inheritance)
  bool (*onBest)(ExecutionContext& ctx, const XSH& best) =
      [](ExecutionContext& ctx, const XSH& best) { return true; };
  //
  bool (*onIncumbent)(ExecutionContext& ctx, const XSH2& incumbent) =
      [](ExecutionContext& ctx, const XSH2& incumbent) { return true; };

  // SearchStatus search(const StopCriteria<XEv>& stopCriteria) override
  SearchOutput<XES> searchBy(const StopCriteria<XEv>& stopCriteria,
                             std::optional<XSH> _best) override {
    // TODO: context 'ctx'
    // ExecutionContext ctx{ .self = this };

    if (Component::debug)
      (*Component::logdata)
          << "PSO search():"
          << " pop_size=" << pop_size << " iter_max=" << iter_max << std::endl;

    // beware that 'star' may not be a Bird...
    /// op<XES>& star = this->best;

    // check if time/target conditions are met
    if (stopCriteria.shouldStop()) return SearchStatus::NO_REPORT;

    // generates initial swarm (of size 'pop_size', see parameter)
    EPopulation<XES2> swarm = generatePopulation();

    // update position of birds and check/adjust bounds
    for (unsigned i = 0; i < this->pop_size; i++) {
      Bird& b = swarm.at(i).first;
      for (unsigned j = 0; j < this->cS.size(); j++) {
        // update positions of bird b, based on its velocities
        b.position[j] += b.velocity[j];
      }
      boundCheck(b);
      XEv e = evaluator.evaluate(b.position);
      swarm.setFitness(i, e);
    }

    // first global best
    XES2 global = swarm.at(0);
    for (unsigned i = 1; i < this->pop_size; i++) {
      // compares localbest of every particle with global best
      if (evaluator.betterThan(swarm.getFitness(i), global.second)) {
        global = swarm.at(i);
        if (Component::debug)
          (*Component::logdata)
              << "PSO updates best: " << global.second.evaluation()
              << std::endl;
      }
    }

    // count generations
    int count_gen = 0;

    // main loop
    while (count_gen < iter_max) {
      // print iteration debug messages
      if (Component::debug)
        (*Component::logdata)
            << "PSO count_gen = " << count_gen << " < " << iter_max
            << " = iter_max \t global = " << global.second.evaluation()
            << std::endl;
      // Particle update
      for (unsigned i = 0; i < this->pop_size; i++) {
        Bird& b = swarm.at(i).first;

        // 0.1 chance of generating a random guy
        if (rg->rand01() < 0.1) {
          for (unsigned j = 0; j < this->cS.size(); j++) {
            b.position[j] = runif(this->cI[j], this->cS[j]);
            b.velocity[j] = 0.1 * runif(this->cI[j], this->cS[j]);
          }
        } else {
          // 0.9 chance to just update the particle
          for (unsigned j = 0; j < this->cS.size(); j++)
            b.position[j] += b.velocity[j];
        }

        // 0.01 chance to do specific mutation
        birdMutation(b);

        // After updating, check to see if there's local upgrades
        boundCheck(b);
        // execute evaluation function
        // IMPORTANT: we are keeping 'localbest' as the real value of the
        // particle

        XEv e = evaluator.evaluate(b.position);
        // swarm.setFitness(i, e);

        if (evaluator.betterThan(e, swarm.getFitness(i))) {
          // updates 'localbest'
          b.localbest = b.position;
          // updates value of localbest
          swarm.setFitness(i, e);
          // updates global best if improving (TODO: maybe it's nice here...)
          // if (evaluator.betterThan(e, global.second))
          //   global = swarm.at(i);
        }
      }

      // Speed update
      for (unsigned i = 0; i < this->pop_size; i++) {
        Bird& b = swarm.at(i).first;
        // p.v =  p.v * al( 0.25, 0.75 ) +
        //        al( 0, 1.5 ) * ( p.best - p.params ) +
        //        al( 0, 1.5 ) * ( gb[ 0 ] - p.params )
        for (unsigned j = 0; j < this->cS.size(); j++) {
          b.velocity[j] =
              (0.15 + this->rg->rand01() / 2) * b.velocity[j] +  // p1
              0.9 * this->rg->rand01() *
                  (b.localbest[j] - b.position[j]) +  // p2
              0.9 * this->rg->rand01() *
                  (global.first.localbest[j] - b.position[j]);  // p3
        }
      }

      // Global Best Maintenance
      for (unsigned i = 0; i < this->pop_size; i++) {
        // compares value with global
        if (evaluator.betterThan(swarm.getFitness(i), global.second)) {
          global = swarm.at(i);
          if (Component::debug)
            (*Component::logdata)
                << "PSO updates best: " << global.second.evaluation()
                << std::endl;
        }
      }
      // if (
      //   !star ||
      //   (evaluator.betterThan(swarm.getFitness(localBest), star->second))) {
      //    star = std::make_optional(swarm.at(localBest));
      // }

      count_gen++;
    }  // while count_gen

    // This is where magic happens...

    XSH realBest(global.first.localbest, global.second);

    // this->best = std::make_optional(realBest);
    // return SearchStatus::NO_REPORT;
    return {SearchStatus::NO_REPORT, std::make_optional(realBest)};
  }

  // reimplementing searchBy, just to make it more explicit (visible)
  // maybe add some specific logs?
  //
  /*
   virtual SearchStatus searchBy(
     std::optional<XSH>& _best,
     std::optional<XSH2>& _inc,
     const StopCriteria<XEv>& stopCriteria) //override
   {
      this->best = _best;
      this->incumbent = _inc;
      return search(stopCriteria);
   }
   */

  bool setMessageLevelR(LogLevel ll) override {
    this->setMessageLevel(ll);
    // force execution over all components
    return evaluator.setMessageLevelR(ll);
  }
};

}  // namespace optframe

#endif  // OPTFRAME_PSO_HPP_
