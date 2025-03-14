// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_EA_RK_RKGA_HPP_
#define OPTFRAME_HEURISTICS_EA_RK_RKGA_HPP_

// C++
#include <algorithm>
#include <utility>
#include <vector>
//

#include <OptFrame/IPopulational.hpp>

#include "../../../InitialPopulation.hpp"
#include "../../../Search/SingleObjSearch.hpp"
#include "../Crossover.hpp"
#include "../Elitism.hpp"
#include "../Mutation.hpp"
#include "../Selection.hpp"
#include "DecoderRandomKeys.hpp"
#include "OptFrame/Concepts/MyConcepts.hpp"
#include "OptFrame/Search/SearchStatus.hpp"

// RKGA - Random Key Genetic Algorithms
// Bean, J.C.: Genetic algorithms and random keys for
// sequencing and optimization. ORSA J. Comput. 6, 154-160 (1994)
// http://pubsonline.informs.org/doi/abs/10.1287/ijoc.6.2.154
namespace optframe {

// generates a random population of 'random_keys' (with size 'key_size')
// template<XRSolution<random_keys> RSK = RSolution<random_keys>, XEvaluation
// XEv = Evaluation<>>
//
template <XEvaluation XEv = Evaluation<>,
          ConceptsComparability KeyType = double>
class RandomKeysInitEPop
    : public InitialEPopulation<std::pair<std::vector<KeyType>, XEv>> {
  using RSK = std::vector<KeyType>;
  using XES = std::pair<std::vector<KeyType>, XEv>;

 private:
  int rksz;
  sref<RandGen> rg;

 public:
  explicit RandomKeysInitEPop(int size, sref<RandGen> _rg = new RandGen)
      : rksz(size), rg{_rg} {}

  // this generator cannot evaluate solutions
  bool canEvaluate() const override { return false; }

  VEPopulation<XES> generateEPopulation(unsigned populationSize,
                                        double timelimit) override {
    VEPopulation<XES> pop;

    for (unsigned i = 0; i < populationSize; i++) {
      //
      // random_keys* d = new random_keys(rksz);
      random_keys vd(rksz);
      //
      for (int j = 0; j < rksz; j++)
        vd.at(j) =
            (rg->rand() % 100000) /
            100000.0;  // TODO: take precision from Template or from good RNG
      // RSK* sol = new RSK(d);
      // pop.push_back(sol);
      //
      // pop.push_back(std::move(vd)); // TODO: pass by std::move() or
      // unique_ptr
      assert(!this->canEvaluate());
      pop.push_back(XES{std::move(vd),
                        XEv{}});  // TODO: pass by std::move() or unique_ptr

      if (Component::debug) {
        // (*Component::logdata) << "generatePopulation new: " << *d <<
        // std::endl;
        // cannot use 'printable' here
      }
    }

    return pop;
  }

  bool setMessageLevelR(LogLevel ll) override {
    this->setMessageLevel(ll);
    return InitialEPopulation<XES>::setMessageLevelR(ll);
  }
};

// RKGA searches on XRS solution space, by means of a decoder (R ->
// random_keys). TODO: this may be XRS perhaps XRS is not good to be default, as
// it must come from outside, and be compatible
//
// template<XRepresentation R, XRSolution<R> XRS, XEvaluation XEv, XESolution
// XES = pair<XRS, XEv>, XRepresentation RKeys = optframe::random_keys> // one
// should pass a compatible one, regarding R
//
// template<XSolution S, XEvaluation XEv, optframe::comparability KeyType =
// double, XESolution XES = pair<S, XEv>>
//
template <XESolution XES, ConceptsComparability KeyType = double,
          XESolution XES2 =
              std::pair<std::vector<KeyType>, typename XES::second_type>>
class RKGA : public Populational<XES, XES, XES2> {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;
  using XSH = XES;
  // using RSK = std::vector<KeyType>;
  using RSK = typename XES2::first_type;

 protected:
  // decoder function
  sref<DecoderRandomKeys<XES, KeyType>> decoder;
  // population generator
  sref<InitialEPopulation<XES2>> initPop;  // implicit key_size
  // population size
  unsigned popSize;
  // number of elite individuals
  unsigned eliteSize;
  // number of mutants
  unsigned randomSize;
  // number of generations (stop criteria)
  unsigned numGenerations;
  // random number generator
  sref<RandGen> rg;

 public:
  // unified constructors (receive 'initPop' object)
  // to pass 'key_size' just use parameter "_initPop =
  // RandomKeysInitPop(key_size)"
  RKGA(sref<DecoderRandomKeys<XES, KeyType>> _decoder,
       sref<InitialEPopulation<XES2>> _initPop, unsigned _popSize,
       unsigned numGenerations, double fracTOP, double fracBOT,
       sref<RandGen> _rg = new RandGen)
      : decoder(_decoder),
        initPop(_initPop),
        popSize(_popSize),
        eliteSize(fracTOP * _popSize),
        randomSize(fracBOT * _popSize),
        numGenerations(numGenerations),
        rg{_rg} {
    assert(eliteSize < popSize);
    assert(randomSize + eliteSize < popSize);
    // must not evaluate solutions at first...
    // TODO: could we allow this at the expense of user efficiency? or leave
    // like this?
    assert(!_initPop->canEvaluate());
  }

  virtual ~RKGA() = default;

  void decodePopulation(VEPopulation<XES2>& p) {
    for (unsigned i = 0; i < p.size(); ++i) {
      // random_keys& rk = p.at(i).getR();
      random_keys& rk = p.at(i).first;
      pair<XEv, op<S>> pe = decoder->decode(rk, false);
      //
      // p.setFitness(i, pe.first.evaluation());
      p.at(i).second = pe.first.evaluation();

      if (Component::debug) {
        (*Component::logdata) << "RKGA: decodePopulation p.setFitness(" << i
                              << ") = " << pe.first.evaluation() << std::endl;
        if (pe.second)
          (*Component::logdata) << "RKGA: decodePopulation has solution (" << i
                                << ") = " << *pe.second << std::endl;
      }
    }
  }

  // returns an owning reference (take good care of it, and delete!)
  virtual RSK cross(const VEPopulation<XES2>& pop) {
    // assert(key_size > 0); // In case of using InitPop, maybe must receive a
    // Selection or Crossover object...

    const RSK& p1 = pop.at(rg->rand() % pop.size()).first;
    const RSK& p2 = pop.at(rg->rand() % pop.size()).first;

    // random_keys* v = new random_keys(key_size, 0.0);
    VEPopulation<XES2> p_single =
        initPop->generateEPopulation(1, 0.0);  // implicit 'key_size'
    // TODO: should cache 'key_size' to prevent unused rands on generation
    //
    // random_keys* v = new random_keys(p_single.at(0)); // copy or 'move' ?
    random_keys v(p_single.at(0).first);
    //
    std::fill(v.begin(), v.end(), 0);
    for (unsigned i = 0; i < v.size(); i++)
      if (rg->rand() % 2 == 0)
        // v->at(i) = p1.getR()[i];
        v.at(i) = p1[i];
      else
        // v->at(i) = p2.getR()[i];
        v.at(i) = p2[i];
    // return *new RSK(v);
    return v;  // TODO: return by std::move() or unique_ptr
  }

  // PSO Execution Context
  struct ExecutionContext {
    // self reference can be very useful
    RKGA* self;
    // TODO: add more variables with desired access on callbacks
    int count_gen;
  };

  // override callbacks (similar to "double-dispatch", but hiding with
  // inheritance)
  bool (*onBestCtx)(ExecutionContext& ctx, const XSH& best) =
      [](ExecutionContext& ctx, const XSH& best) {
        return ctx.self->onBest(*ctx.self, best);
      };
  using IncType = typename Populational<XES, XES, XES2>::IncumbentType;
  //
  bool (*onIncumbentCtx)(ExecutionContext& ctx, const IncType& incumbent) =
      [](ExecutionContext& ctx, const IncType& incumbent) {
        return ctx.self->onIncumbent(*ctx.self, incumbent);
      };

  SearchOutput<XES> searchPopulational(
      std::optional<XES>& _best, IncType& _inc,
      const StopCriteria<XEv>& stopCriteria) override {
    // TODO: IMPLEMENT!!
    assert(false);
    return SearchOutput<XES>{SearchStatus::FAILED};
  }

  // pair<CopySolution<random_keys>&, Evaluation<>&>* search(double timelimit =
  // 100000000, double target_f = 0, const CopySolution<random_keys>* _s =
  // nullptr, const Evaluation<>* _e = nullptr)
  /// virtual pair<CopySolution<random_keys>, XEv>* search(StopCriteria<XEv>&
  /// stopCriteria, const CopySolution<random_keys>* _s = nullptr, const XEv* _e
  /// = nullptr) override
  // virtual pair<XRS, XEv>* search(StopCriteria<XEv>& stopCriteria, const XRS*
  // _s = nullptr, const XEv* _e = nullptr) override
  //
  // virtual std::optional<pair<XRS, XEv>> search(StopCriteria<XEv>&
  // stopCriteria) override
  //
  // SearchStatus search(const StopCriteria<XEv>& stopCriteria) override
  //
  SearchOutput<XES> searchBy(const StopCriteria<XEv>& stopCriteria,
                             std::optional<XSH> _best) override {
    ExecutionContext ctx{this};

    if (Component::debug)
      (*Component::logdata)
          << "RKGA search():"
          << " popSize=" << popSize << " eliteSize=" << eliteSize
          << " randomSize=" << randomSize
          << " numGenerations=" << numGenerations << std::endl;

    // op<XES>& star = this->best;
    op<XES> star;
    // check if time/target conditions are met
    if (stopCriteria.shouldStop()) return SearchStatus::NO_REPORT;

    // count generations
    ctx.count_gen = 0;

    if (Component::debug)
      (*Component::logdata)
          << "RKGA: will initPop.generatePopulation(popSize=" << popSize << ")"
          << std::endl;

    // initialize population (of random_keys)
    VEPopulation<XES2> p =
        initPop->generateEPopulation(popSize, stopCriteria.timelimit);

    if (Component::debug)
      (*Component::logdata) << "RKGA: p.size() = " << p.size() << std::endl;

    if (Component::debug)
      (*Component::logdata) << "RKGA: will decodePopulation(p)" << std::endl;

    // decode population
    decodePopulation(p);

    if (Component::debug)
      (*Component::logdata)
          << "RKGA: will p.sort(isMin=" << decoder->isMinimization() << ")"
          << std::endl;

    // sort population
    // p.sort(decoder->isMinimization());
    decoder->vsort(p);

    if (Component::debug)
      (*Component::logdata) << "RKGA: will trigger onIncumbent(p)" << std::endl;

    // trigger onIncumbent
    if (!this->onIncumbentCtx(ctx, p)) return SearchStatus::NO_REPORT;

    if (Component::debug)
      (*Component::logdata) << "RKGA: will p.getSingleFitness(0)" << std::endl;

    // evtype best_f = p.getSingleFitness(0);
    evtype best_f = p.at(0).second.evaluation();

    if (Component::information)
      (*Component::logdata)
          << "RKGA: best fitness at initial population: " << best_f << endl;

    // stop by number of generations.
    // other stopping criteria? TIME, GAP, ...
    while (ctx.count_gen < int(numGenerations)) {
      // check if time/target conditions are met
      if (stopCriteria.shouldStop()) return SearchStatus::NO_REPORT;

      if (Component::debug)
        (*Component::logdata)
            << "RKGA: count_gen=" << ctx.count_gen << " < " << numGenerations
            << "=numGenerations" << std::endl;

      if (Component::debug)
        (*Component::logdata)
            << "RKGA: will initPop.generatePopulation(randomSize=" << randomSize
            << ")" << std::endl;

      // create mutants in new population
      VEPopulation<XES2> nextPop =
          initPop->generateEPopulation(randomSize, stopCriteria.timelimit);

      if (Component::debug)
        (*Component::logdata)
            << "RKGA: nextPop.size() = " << nextPop.size() << std::endl;

      if (Component::debug)
        (*Component::logdata) << "RKGA: will push eliteSize=" << eliteSize
                              << " top elements" << std::endl;

      // move 'eliteSize' elements to new population
      for (unsigned i = 0; i < eliteSize; i++) nextPop.push_back(p.at(i));
      // TODO: we could get the current evaluations and avoid new
      // decodifications

      if (Component::debug)
        (*Component::logdata)
            << "RKGA: nextPop.size() = " << nextPop.size() << std::endl;

      // populate the rest
      while (nextPop.size() < popSize) {
        if (Component::debug)
          (*Component::logdata) << "RKGA: nextPop.size()=" << nextPop.size()
                                << " < " << popSize << "=popSize" << std::endl;

        if (Component::debug)
          (*Component::logdata) << "RKGA: will cross(p)" << std::endl;

        RSK vkeys = cross(p);
        // nextPop.push_back(se);
        nextPop.push_back(XES2{vkeys, XEv{}});
      }

      if (Component::debug)
        (*Component::logdata)
            << "RKGA: will p.clear() and p = nextPop " << std::endl;

      // delete p; // KILL ALL INDIVIDUALS
      p.clear();  // KILL ALL INDIVIDUALS
      p = std::move(nextPop);
      nextPop.clear();  // KILL (what?)

      if (Component::debug)
        (*Component::logdata) << "RKGA: p.size() = " << p.size() << std::endl;

      if (Component::debug)
        (*Component::logdata) << "RKGA: will decodePopulation(p)" << std::endl;

      // decode solutions
      decodePopulation(p);

      if (Component::debug)
        (*Component::logdata)
            << "RKGA: will p.sort(isMin=" << decoder->isMinimization() << ")"
            << std::endl;

      // sort population
      // p.sort(decoder->isMinimization());
      decoder->vsort(p);

      if (Component::debug)
        (*Component::logdata)
            << "RKGA: will trigger onIncumbent(p)" << std::endl;

      // trigger onIncumbent
      if (!this->onIncumbentCtx(ctx, p)) return SearchStatus::NO_REPORT;

      // evtype pop_best = p.getSingleFitness(0);
      evtype pop_best = p.at(0).second.evaluation();

      if ((decoder->isMinimization() && pop_best < best_f) ||
          (!decoder->isMinimization() && pop_best > best_f)) {
        best_f = pop_best;
        if (Component::debug)
          (*Component::logdata) << "RKGA: best fitness " << best_f
                                << " at generation " << ctx.count_gen << endl;

        // send machine logs (TODO: check which format... txt, json... suppose
        // 'txt')
        if (Component::mlog)
          (*Component::mlog) << "RKGA_best"
                             << "\t" << best_f << "\t"
                             << "gen"
                             << "\t" << ctx.count_gen << std::endl;

        // TODO: do we need to decode this all the time, or only when exiting?
        RSK best_rkeys = p.at(0).first;
        pair<XEv, op<S>> pe = decoder->decode(best_rkeys, true);
        if (pe.second) {
          star = op<XES>(XES{*pe.second, pe.first});
          // check update callback
          if (!this->onBestCtx(ctx, *star)) return SearchStatus::NO_REPORT;
        }
      }  // end if

      ctx.count_gen++;
    }  // end while

    if (Component::debug)
      (*Component::logdata)
          << "RKGA: will p.sort(isMin=" << decoder->isMinimization() << ")"
          << std::endl;

    // sort to get best (not necessary)
    // p.sort(decoder->isMinimization());
    decoder->vsort(p);

    RSK best = p.at(0).first;
    // TODO: avoid legacy method 'remove(0)'
    // replace into 'pop_back' and use reversed ordered population (to be
    // compatible to std::vector)
    // RSK& best = p.remove(0);
    p.erase(p.begin());

    // TODO: we should enfoce a boolean here (NEEDS SOLUTION = TRUE!!)
    // pair<XEv, op<S>> pe = decoder.decode(best.getR(), true);
    pair<XEv, op<S>> pe = decoder->decode(best, true);
    XEv& e = pe.first;

    // WARNING: something VERY strange here... why returning random_keys and not
    // elements?
    // TODO: I may be wrong, but next part seems to be very wrong... how does
    // 'override' allowed this in the past??

    /*
    // ignoring second value
    if (pe.second)
       delete pe.second;
    //delete p;
    p.clear();
    return new pair<RSK, XEv>(best, e);
    */
    if (Component::debug) {
      cout << "RKGA print sol: optional(" << ((bool)pe.second) << ") -> ";
      if (!pe.second) {
        cout << "RKGA ERROR!! DOESNT HAVE A SOLUTION!! VALUE IS: "
             << pe.first.evaluation() << endl;
        cout << "SHOULD WE RETURN EMPTY PAIR<S, XEv> OR FORCE SOME DECODER TO "
                "AT "
                "LEAST PROVIDE A SOLUTION?"
             << endl;
        assert(false);
      }
    }

    S finalSol(*pe.second);  // TODO: avoid loss

    // return std::optional<pair<XRS, XEv>>(make_pair(finalSol, e));
    star = std::make_optional(make_pair(finalSol, e));
    // this->best = star;
    return {SearchStatus::NO_REPORT, *star};
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
    bool b1 = decoder->setMessageLevelR(ll);
    bool b2 = initPop->setMessageLevelR(ll);
    return b1 && b2;
  }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_EA_RK_RKGA_HPP_
