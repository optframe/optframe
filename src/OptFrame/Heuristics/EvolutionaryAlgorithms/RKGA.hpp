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

// generates a random population of 'random_keys' (with size 'sz')
template<XRSolution<random_keys> RSK = RSolution<random_keys>, XEvaluation XEv = Evaluation<>>
class RandomKeysInitPop : public InitialPopulation<RSK, XEv>
{
private:
   int sz;

public:
   RandomKeysInitPop(int size)
     : sz(size)
   {
   }

   virtual Population<RSK, XEv> generatePopulation(unsigned populationSize, double timelimit) override
   {
      Population<RSK, XEv> pop;

      for (unsigned i = 0; i < populationSize; i++) {
         random_keys* d = new random_keys(sz);
         for (int j = 0; j < sz; j++)
            d->at(j) = (rand() % 100000) / 100000.0; // TODO: take precision from Template or from good RNG
         RSK* sol = new RSK(d);
         pop.push_back(sol);
      }

      return pop;
   }
};

// RKGA searches on XRS solution space, by means of a decoder (R -> random_keys). TODO: this may be XRS perhaps
// XRS is not good to be default, as it must come from outside, and be compatible
template<Representation R, XRSolution<R> XRS, XEvaluation XEv = Evaluation<>> // one should pass a compatible one, regarding R
class RKGA : public SingleObjSearch<XRS, XEv>
{
   using RSK = RSolution<random_keys>;
protected:
   DecoderRandomKeys<R, XRS, XEv>& decoder;
   Evaluator<XRS, XEv>* evaluator; // Check to avoid memory leaks

   InitialPopulation<RSK, XEv>& initPop;
   int sz; // Check to avoid memory leaks

   unsigned popSize, eliteSize, randomSize;
   unsigned numGenerations;

public:
   RKGA(DecoderRandomKeys<R, XRS, XEv>& _decoder, InitialPopulation<RSK, XEv>& _initPop, unsigned numGenerations, unsigned _popSize, double fracTOP, double fracBOT)
     : decoder(_decoder)
     , evaluator(nullptr)
     , initPop(_initPop)
     , sz(-1)
     , popSize(_popSize)
     , eliteSize(fracTOP * _popSize)
     , randomSize(fracBOT * _popSize)
   {
      this->numGenerations = numGenerations;
      assert(eliteSize < popSize);
      assert(randomSize + eliteSize < popSize);
   }

   RKGA(DecoderRandomKeys<R, XRS, XEv>& _decoder, int key_size, unsigned numGenerations, unsigned _popSize, double fracTOP, double fracBOT)
     : decoder(_decoder)
     , evaluator(nullptr)
     , initPop(*new RandomKeysInitPop(key_size))
     , sz(key_size)
     , popSize(_popSize)
     , eliteSize(fracTOP * _popSize)
     , randomSize(fracBOT * _popSize)
   {
      this->numGenerations = numGenerations;
      assert(eliteSize < popSize);
      assert(randomSize + eliteSize < popSize);
   }

   RKGA(Evaluator<XRS, XEv>& _evaluator, int key_size, unsigned numGenerations, unsigned _popSize, double fracTOP, double fracBOT)
     : decoder(*new DecoderRandomKeysEvaluator<random_keys, XRS>(_evaluator))
     , evaluator(&_evaluator)
     , initPop(*new RandomKeysInitPop(key_size))
     , sz(key_size)
     , popSize(_popSize)
     , eliteSize(fracTOP * _popSize)
     , randomSize(fracBOT * _popSize)
   {
      this->numGenerations = numGenerations;
      assert(eliteSize < popSize);
      assert(randomSize + eliteSize < popSize);
   }

   virtual ~RKGA()
   {
      if (evaluator)
         delete &decoder;
      if (sz != -1)
         delete &initPop;
   }

   void decodePopulation(Population<RSK, XEv>& p)
   {
      for (unsigned i = 0; i < p.size(); ++i) {
         //p.at(i).print();
         random_keys& rk = p.at(i).getR();
         pair<XEv, XRS*> pe = decoder.decode(rk);
         p.setFitness(i, pe.first.evaluation());
         //delete &pe.first;
         if (pe.second)
            delete pe.second;
      }
   }

   virtual RSK& cross(const Population<RSK, XEv>& pop) const
   {
      assert(sz > 0); // In case of using InitPop, maybe must receive a Selection or Crossover object...

      const RSK& p1 = pop.at(rand() % pop.size());
      const RSK& p2 = pop.at(rand() % pop.size());

      random_keys* v = new random_keys(sz, 0.0);
      for (int i = 0; i < sz; i++)
         if (rand() % 2 == 0)
            v->at(i) = p1.getR()[i];
         else
            v->at(i) = p2.getR()[i];
      return *new RSK(v);
   }

   //pair<CopySolution<random_keys>&, Evaluation<>&>* search(double timelimit = 100000000, double target_f = 0, const CopySolution<random_keys>* _s = nullptr, const Evaluation<>* _e = nullptr)
   ///virtual pair<CopySolution<random_keys>, XEv>* search(SOSC& stopCriteria, const CopySolution<random_keys>* _s = nullptr, const XEv* _e = nullptr) override
   //virtual pair<XRS, XEv>* search(SOSC& stopCriteria, const XRS* _s = nullptr, const XEv* _e = nullptr) override
   virtual std::optional<pair<XRS, XEv>> search(SOSC& stopCriteria) override
   {
      // count generations
      int count_gen = 0;

      // initialize population (of random_keys)
      Population<RSK, XEv> p = initPop.generatePopulation(popSize, stopCriteria.timelimit);
      // decode population
      decodePopulation(p);

      // sort population
      p.sort(decoder.isMinimization());

      evtype best_f = p.getSingleFitness(0);
      if (Component::information)
         cout << "RKGA: best fitness at initial population: " << best_f << endl;

      // stop by number of generations.
      // other stopping criteria? TIME, GAP, ...
      while (count_gen < int(numGenerations)) {
         // create mutants in new population
         Population<RSK, XEv> nextPop = initPop.generatePopulation(randomSize, stopCriteria.timelimit);

         // move 'eliteSize' elements to new population
         for (unsigned i = 0; i < eliteSize; i++)
            nextPop.push_back(p.at(i));
         // TODO: we could get the current evaluations and avoid new decodifications

         // populate the rest
         while (nextPop.size() < popSize) {
            RSK* s = &cross(p);
            nextPop.push_back(s);
         }

         //delete p; // KILL ALL INDIVIDUALS
         p.clear();// KILL ALL INDIVIDUALS
         p = nextPop;

         // decode solutions
         decodePopulation(p);

         // sort population
         p.sort(decoder.isMinimization());

         evtype pop_best = p.getSingleFitness(0);

         if (decoder.isMinimization() && pop_best < best_f) {
            best_f = pop_best;
            cout << "RKGA: best fitness " << best_f << " at generation " << count_gen << endl;
         }

         if (!decoder.isMinimization() && pop_best > best_f) {
            best_f = pop_best;
            if (Component::information)
               cout << "RKGA: best fitness " << best_f << " at generation " << count_gen << endl;
         }

         count_gen++;
      }

      // sort to get best (not necessary)
      p.sort(decoder.isMinimization());

      RSK& best = p.remove(0);
      pair<XEv, XRS*> pe = decoder.decode(best.getR());
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
     XRS finalSol(*pe.second); // TODO: avoid loss
     return std::optional<pair<XRS, XEv>>(make_pair(finalSol, e)); 
   }
};
}

#endif /*OPTFRAME_RKGA_HPP_*/
