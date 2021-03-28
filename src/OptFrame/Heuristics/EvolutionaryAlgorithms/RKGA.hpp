// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
// http://optframe.sourceforge.net/
//
// This file is part of the OptFrame optimization framework. This framework
// is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License v3 as published by the
// Free Software Foundation.

// This framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License v3 for more details.

// You should have received a copy of the GNU Lesser General Public License v3
// along with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

#ifndef OPTFRAME_RKGA_HPP_
#define OPTFRAME_RKGA_HPP_

#include <algorithm>

#include "../../IPopulational.hpp"
#include "../../InitialPopulation.h"
#include "../../Population.hpp"
#include "../../SingleObjSearch.hpp"

#include "Crossover.hpp"
#include "DecoderRandomKeys.hpp"
#include "Elitism.hpp"
#include "Mutation.hpp"
#include "Selection.hpp"

// RKGA - Random Key Genetic Algorithms
// Bean, J.C.: Genetic algorithms and random keys for
// sequencing and optimization. ORSA J. Comput. 6, 154-160 (1994)
// http://pubsonline.informs.org/doi/abs/10.1287/ijoc.6.2.154
namespace optframe {

// generates a random population of 'random_keys' (with size 'key_size')
//template<XRSolution<random_keys> RSK = RSolution<random_keys>, XEvaluation XEv = Evaluation<>>
//
template<XEvaluation XEv = Evaluation<>, optframe::comparability KeyType = double>
class RandomKeysInitPop : public InitialPopulation<std::pair<std::vector<KeyType>, XEv>>
{
   using RSK = std::vector<KeyType>;
   using XES = std::pair<std::vector<KeyType>, XEv>;

private:
   int rksz;
   RandGen& rg;

public:
   RandomKeysInitPop(int size, RandGen& _rg)
     : rksz(size)
     , rg{ _rg }
   {
   }

   virtual Population<XES> generatePopulation(unsigned populationSize, double timelimit) override
   {
      Population<XES> pop;

      for (unsigned i = 0; i < populationSize; i++) {
         random_keys* d = new random_keys(rksz);
         for (int j = 0; j < rksz; j++)
            d->at(j) = (rg.rand() % 100000) / 100000.0; // TODO: take precision from Template or from good RNG
         //RSK* sol = new RSK(d);
         //pop.push_back(sol);
         pop.push_back(d); // TODO: pass by std::move() or unique_ptr

         if (Component::debug)
            (*Component::logdata) << "generatePopulation new: " << *d << std::endl;
      }

      return pop;
   }

   virtual bool setVerboseR() override
   {
      this->setVerbose();
      return InitialPopulation<XES>::setVerboseR();
   }
};

// RKGA searches on XRS solution space, by means of a decoder (R -> random_keys). TODO: this may be XRS perhaps
// XRS is not good to be default, as it must come from outside, and be compatible
//
//template<XRepresentation R, XRSolution<R> XRS, XEvaluation XEv, XESolution XES = pair<XRS, XEv>, XRepresentation RKeys = optframe::random_keys> // one should pass a compatible one, regarding R
//
//template<XSolution S, XEvaluation XEv, optframe::comparability KeyType = double, XESolution XES = pair<S, XEv>>
//
template<
  XESolution XES,
  optframe::comparability KeyType = double,
  XESolution XES2 = std::pair<std::vector<KeyType>, typename XES::second_type>,
  XSearch<XES2> XSH2 = Population<XES2>>
class RKGA : public SingleObjSearch<XES, XES2, XSH2>
  //  TODO: cannot make this IPopulational, unless using EPopulation instead of "legacy" Population
  //, public IPopulational<XES, XES, XES2>
{
   using S = typename XES::first_type;
   using XEv = typename XES::second_type;
   using XSH = XES;
   //using RSK = std::vector<KeyType>;
   using RSK = typename XES2::first_type;

protected:
   // decoder function
   sref<DecoderRandomKeys<S, XEv, KeyType>> decoder;
   // population generator
   sref<InitialPopulation<XES2>> initPop; // implicit key_size
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

   // unified constructors (receive 'key_size' value)
   RKGA(sref<DecoderRandomKeys<S, XEv, KeyType>> _decoder, int key_size, unsigned numGenerations, unsigned _popSize, double fracTOP, double fracBOT, sref<RandGen> _rg)
     : decoder(_decoder)
     , initPop(new RandomKeysInitPop(key_size, _rg))
     , popSize(_popSize)
     , eliteSize(fracTOP * _popSize)
     , randomSize(fracBOT * _popSize)
     , numGenerations(numGenerations)
     , rg{ _rg }
   {
      assert(eliteSize < popSize);
      assert(randomSize + eliteSize < popSize);
   }

   // unified constructors (receive 'initPop' object)
   RKGA(sref<DecoderRandomKeys<S, XEv, KeyType>> _decoder, sref<InitialPopulation<XES2>> _initPop, unsigned numGenerations, unsigned _popSize, double fracTOP, double fracBOT, sref<RandGen> _rg)
     : decoder(_decoder)
     , initPop(_initPop)
     , popSize(_popSize)
     , eliteSize(fracTOP * _popSize)
     , randomSize(fracBOT * _popSize)
     , numGenerations(numGenerations)
     , rg{ _rg }
   {
      assert(eliteSize < popSize);
      assert(randomSize + eliteSize < popSize);
   }



/*
   RKGA(DecoderRandomKeys<S, XEv, KeyType>& _decoder, InitialPopulation<XES2>& _initPop, int key_size, unsigned numGenerations, unsigned _popSize, double fracTOP, double fracBOT, RandGen& _rg)
     : decoder(_decoder)
     , evaluator(nullptr)
     , initPop(_initPop)
     , key_size(key_size)
     , popSize(_popSize)
     , eliteSize(fracTOP * _popSize)
     , randomSize(fracBOT * _popSize)
     , rg{ _rg }
   {
      this->numGenerations = numGenerations;
      assert(eliteSize < popSize);
      assert(randomSize + eliteSize < popSize);
   }

   RKGA(DecoderRandomKeys<S, XEv, KeyType>& _decoder, int key_size, unsigned numGenerations, unsigned _popSize, double fracTOP, double fracBOT, RandGen& _rg)
     : decoder(_decoder)
     , evaluator(nullptr)
     , initPop(*new RandomKeysInitPop(key_size, _rg))
     , del_initPop(true)
     , key_size(key_size)
     , popSize(_popSize)
     , eliteSize(fracTOP * _popSize)
     , randomSize(fracBOT * _popSize)
     , rg{ _rg }
   {
      this->numGenerations = numGenerations;
      assert(eliteSize < popSize);
      assert(randomSize + eliteSize < popSize);
   }

   RKGA(Evaluator<S, XEv>& _evaluator, InitialPopulation<XES2>& _initPop, int key_size, unsigned numGenerations, unsigned _popSize, double fracTOP, double fracBOT, RandGen& _rg)
     : decoder(*new DecoderRandomKeysEvaluator<S, XEv, KeyType, XES>(_evaluator))
     , evaluator(&_evaluator)
     , initPop(_initPop)
     , key_size(key_size)
     , popSize(_popSize)
     , eliteSize(fracTOP * _popSize)
     , randomSize(fracBOT * _popSize)
     , rg{ _rg }
   {
      this->numGenerations = numGenerations;
      assert(eliteSize < popSize);
      assert(randomSize + eliteSize < popSize);
   }

   RKGA(Evaluator<S, XEv>& _evaluator, int key_size, unsigned numGenerations, unsigned _popSize, double fracTOP, double fracBOT, RandGen& _rg)
     : decoder(*new DecoderRandomKeysEvaluator<S, XEv, KeyType, XES>(_evaluator))
     , evaluator(&_evaluator)
     , initPop(*new RandomKeysInitPop(key_size, _rg))
     , del_initPop(true)
     , key_size(key_size)
     , popSize(_popSize)
     , eliteSize(fracTOP * _popSize)
     , randomSize(fracBOT * _popSize)
     , rg{ _rg }
   {
      this->numGenerations = numGenerations;
      assert(eliteSize < popSize);
      assert(randomSize + eliteSize < popSize);
   }

   virtual ~RKGA()
   {
      if (evaluator)
         delete &decoder;
      if (del_initPop)
         delete &initPop;
   }
*/

   virtual ~RKGA()
   {
   }

   void decodePopulation(Population<XES2>& p)
   {
      for (unsigned i = 0; i < p.size(); ++i) {
         //random_keys& rk = p.at(i).getR();
         random_keys& rk = p.at(i);
         pair<XEv, op<S>> pe = decoder->decode(rk, false);
         p.setFitness(i, pe.first.evaluation());

         if (Component::debug) {
            (*Component::logdata) << "RKGA: decodePopulation p.setFitness(" << i << ") = " << pe.first.evaluation() << std::endl;
            if (pe.second)
               (*Component::logdata) << "RKGA: decodePopulation has solution (" << i << ") = " << *pe.second << std::endl;
         }
      }
   }

   virtual RSK& cross(const Population<XES2>& pop)
   {
      //assert(key_size > 0); // In case of using InitPop, maybe must receive a Selection or Crossover object...

      const RSK& p1 = pop.at(rand() % pop.size());
      const RSK& p2 = pop.at(rand() % pop.size());

      //random_keys* v = new random_keys(key_size, 0.0);
      Population<XES2> p_single = initPop->generatePopulation(1, 0.0); // implicit 'key_size'
      // TODO: should cache 'key_size' to prevent unused rands on generation
      random_keys* v = new random_keys(p_single.at(0)); // copy or 'move' ?
      std::fill(v->begin(), v->end(), 0);
      for (unsigned i = 0; i < v->size(); i++)
         if (rand() % 2 == 0)
            //v->at(i) = p1.getR()[i];
            v->at(i) = p1[i];
         else
            //v->at(i) = p2.getR()[i];
            v->at(i) = p2[i];
      //return *new RSK(v);
      return *v; // TODO: return by std::move() or unique_ptr
   }

   // PSO Execution Context
   struct ExecutionContext
   {
      // self reference can be very useful
      RKGA* self;
      // TODO: add more variables with desired access on callbacks
      int count_gen;
   };

   // override callbacks (similar to "double-dispatch", but hiding with inheritance)
   bool (*onBest)(ExecutionContext& ctx, const XSH& best) =
     [](ExecutionContext& ctx, const XSH& best) { return true; };
   //
   bool (*onIncumbent)(ExecutionContext& ctx, const XSH2& incumbent) =
     [](ExecutionContext& ctx, const XSH2& incumbent) { return true; };

   //pair<CopySolution<random_keys>&, Evaluation<>&>* search(double timelimit = 100000000, double target_f = 0, const CopySolution<random_keys>* _s = nullptr, const Evaluation<>* _e = nullptr)
   ///virtual pair<CopySolution<random_keys>, XEv>* search(StopCriteria<XEv>& stopCriteria, const CopySolution<random_keys>* _s = nullptr, const XEv* _e = nullptr) override
   //virtual pair<XRS, XEv>* search(StopCriteria<XEv>& stopCriteria, const XRS* _s = nullptr, const XEv* _e = nullptr) override
   //
   //virtual std::optional<pair<XRS, XEv>> search(StopCriteria<XEv>& stopCriteria) override
   //
   //SearchStatus search(const StopCriteria<XEv>& stopCriteria) override
   //
   SearchOutput<XES> search(const StopCriteria<XEv>& stopCriteria) override
   {
      ExecutionContext ctx {.self = this};

      if (Component::debug)
         (*Component::logdata) << "RKGA search():"
                               << " popSize=" << popSize << " eliteSize=" << eliteSize << " randomSize=" << randomSize << " numGenerations=" << numGenerations
                               << std::endl;

      //op<XES>& star = this->best;
      op<XES> star;
      // check if time/target conditions are met
      if (stopCriteria.shouldStop(std::nullopt))
         return SearchStatus::NO_REPORT;

      // count generations
      ctx.count_gen = 0;

      if (Component::debug)
         (*Component::logdata) << "RKGA: will initPop.generatePopulation(popSize=" << popSize << ")" << std::endl;

      // initialize population (of random_keys)
      Population<XES2> p = initPop->generatePopulation(popSize, stopCriteria.timelimit);

      if (Component::debug)
         (*Component::logdata) << "RKGA: p.size() = " << p.size() << std::endl;

      if (Component::debug)
         (*Component::logdata) << "RKGA: will decodePopulation(p)" << std::endl;

      // decode population
      decodePopulation(p);

      if (Component::debug)
         (*Component::logdata) << "RKGA: will p.sort(isMin=" << decoder->isMinimization() << ")" << std::endl;

      // sort population
      p.sort(decoder->isMinimization());

      if (Component::debug)
         (*Component::logdata) << "RKGA: will trigger onIncumbent(p)" << std::endl;

      // trigger onIncumbent
      if (!this->onIncumbent(ctx, p))
         return SearchStatus::NO_REPORT;

      if (Component::debug)
         (*Component::logdata) << "RKGA: will p.getSingleFitness(0)" << std::endl;

      evtype best_f = p.getSingleFitness(0);
      if (Component::information)
         (*Component::logdata) << "RKGA: best fitness at initial population: " << best_f << endl;

      // stop by number of generations.
      // other stopping criteria? TIME, GAP, ...
      while (ctx.count_gen < int(numGenerations)) {
         // check if time/target conditions are met
         if (stopCriteria.shouldStop(std::nullopt))
            return SearchStatus::NO_REPORT;

         if (Component::debug)
            (*Component::logdata) << "RKGA: count_gen=" << ctx.count_gen << " < " << numGenerations << "=numGenerations" << std::endl;

         if (Component::debug)
            (*Component::logdata) << "RKGA: will initPop.generatePopulation(randomSize=" << randomSize << ")" << std::endl;

         // create mutants in new population
         Population<XES2> nextPop = initPop->generatePopulation(randomSize, stopCriteria.timelimit);

         if (Component::debug)
            (*Component::logdata) << "RKGA: nextPop.size() = " << nextPop.size() << std::endl;

         if (Component::debug)
            (*Component::logdata) << "RKGA: will push eliteSize=" << eliteSize << " top elements" << std::endl;

         // move 'eliteSize' elements to new population
         for (unsigned i = 0; i < eliteSize; i++)
            nextPop.push_back(p.at(i));
         // TODO: we could get the current evaluations and avoid new decodifications

         if (Component::debug)
            (*Component::logdata) << "RKGA: nextPop.size() = " << nextPop.size() << std::endl;

         // populate the rest
         while (nextPop.size() < popSize) {
            if (Component::debug)
               (*Component::logdata) << "RKGA: nextPop.size()=" << nextPop.size() << " < " << popSize << "=popSize" << std::endl;

            if (Component::debug)
               (*Component::logdata) << "RKGA: will cross(p)" << std::endl;

            RSK* s = &cross(p);
            nextPop.push_back(s);
         }

         if (Component::debug)
            (*Component::logdata) << "RKGA: will p.clear() and p = nextPop " << std::endl;

         //delete p; // KILL ALL INDIVIDUALS
         p.clear(); // KILL ALL INDIVIDUALS
         p = nextPop;

         if (Component::debug)
            (*Component::logdata) << "RKGA: p.size() = " << p.size() << std::endl;

         if (Component::debug)
            (*Component::logdata) << "RKGA: will decodePopulation(p)" << std::endl;

         // decode solutions
         decodePopulation(p);

         if (Component::debug)
            (*Component::logdata) << "RKGA: will p.sort(isMin=" << decoder->isMinimization() << ")" << std::endl;

         // sort population
         p.sort(decoder->isMinimization());

         if (Component::debug)
            (*Component::logdata) << "RKGA: will trigger onIncumbent(p)" << std::endl;

         // trigger onIncumbent
         if (!this->onIncumbent(ctx, p))
            return SearchStatus::NO_REPORT;

         evtype pop_best = p.getSingleFitness(0);

         if ((decoder->isMinimization() && pop_best < best_f) || (!decoder->isMinimization() && pop_best > best_f)) {
            best_f = pop_best;
            if (Component::debug)
               (*Component::logdata) << "RKGA: best fitness " << best_f << " at generation " << ctx.count_gen << endl;

            // send machine logs (TODO: check which format... txt, json... suppose 'txt')
            if (Component::mlog)
               (*Component::mlog) << "RKGA_best"
                                  << "\t" << best_f << "\t"
                                  << "gen"
                                  << "\t" << ctx.count_gen << std::endl;

            // TODO: do we need to decode this all the time, or only when exiting?
            RSK& best_rkeys = p.at(0);
            pair<XEv, op<S>> pe = decoder->decode(best_rkeys, true);
            if (pe.second) {
               star = op<XES>(XES{ *pe.second, pe.first });
               // check update callback
               if (!this->onBest(ctx, *star))
                  return SearchStatus::NO_REPORT;
            }
         } // end if

         ctx.count_gen++;
      } // end while

      if (Component::debug)
         (*Component::logdata) << "RKGA: will p.sort(isMin=" << decoder->isMinimization() << ")" << std::endl;

      // sort to get best (not necessary)
      p.sort(decoder->isMinimization());

      RSK& best = p.remove(0);

      // TODO: we should enfoce a boolean here (NEEDS SOLUTION = TRUE!!)
      //pair<XEv, op<S>> pe = decoder.decode(best.getR(), true);
      pair<XEv, op<S>> pe = decoder->decode(best, true);
      Evaluation<>& e = pe.first;

      // WARNING: something VERY strange here... why returning random_keys and not elements?
      // TODO: I may be wrong, but next part seems to be very wrong... how does 'override' allowed this in the past??

      /*
      // ignoring second value
      if (pe.second)
         delete pe.second;
      //delete p;
      p.clear();
      return new pair<RSK, XEv>(best, e);
      */
      cout << "RKGA print sol: optional(" << ((bool)pe.second) << ") -> ";
      if (!pe.second) {
         cout << "RKGA ERROR!! DOESNT HAVE A SOLUTION!! VALUE IS: " << pe.first.evaluation() << endl;
         cout << "SHOULD WE RETURN EMPTY PAIR<S, XEv> OR FORCE SOME DECODER TO AT LEAST PROVIDE A SOLUTION?" << endl;
         assert(false);
      }

      S finalSol(*pe.second); // TODO: avoid loss

      //return std::optional<pair<XRS, XEv>>(make_pair(finalSol, e));
      star = make_optional(make_pair(finalSol, e));
      //this->best = star;
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

   virtual bool
   setSilentR() override
   {
      this->setSilent();
      // force execution over all components
      return decoder->setSilentR() &
             //(evaluator ? evaluator->setSilentR() : true) &
             initPop->setSilentR();
   }

   virtual bool
   setVerboseR() override
   {
      this->setVerbose();
      // force execution over all components
      return decoder->setVerboseR() &
             //(evaluator ? evaluator->setVerboseR() : true) &
             initPop->setVerboseR();
   }
};

} // namespace optframe

#endif /*OPTFRAME_RKGA_HPP_*/
