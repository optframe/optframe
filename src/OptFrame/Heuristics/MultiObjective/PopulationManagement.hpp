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

#ifndef OPTFRAME_POPULATION_MANAGEMENT_HPP_
#define OPTFRAME_POPULATION_MANAGEMENT_HPP_

#include "../../Component.hpp"
#include "../EvolutionaryAlgorithms/GeneralCrossover.hpp"
#include "MOSIndividual.hpp"

namespace optframe {

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class PopulationManagement : public Component
{
public:
   PopulationManagement()
   {
   }

   virtual ~PopulationManagement()
   {
   }

   // create a new population
   virtual vector<MOSIndividual<R, ADS, DS>*>& initialize(unsigned pSize) = 0;

   // create next population
   virtual vector<MOSIndividual<R, ADS, DS>*>& createNext(unsigned target_size, const vector<const MOSIndividual<R, ADS, DS>*>& P) = 0;

   virtual void print() const
   {
      cout << "PopulationManagement" << endl;
   }
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class BasicPopulationManagement : public PopulationManagement<R, ADS, DS>
{
public:
   InitialMultiSolution<S>& initPop;
   vector<NS<RepCARP>*> mutations;
   double mutationRate; // probability of applying a mutation
   vector<GeneralCrossover<R, ADS>*> crossovers;
   double renewRate; // percentage of children population to be entirely reconstructed
   RandGen& rg;

   BasicPopulationManagement(InitialMultiSolution<S>& _initPop, vector<NS<RepCARP>*> _mutations, double _mutationRate, vector<GeneralCrossover<RepCARP>*> _crossovers, double _renewRate, RandGen& _rg)
     : initPop(_initPop)
     , mutations(_mutations)
     , mutationRate(_mutationRate)
     , crossovers(_crossovers)
     , renewRate(_renewRate)
     , rg(_rg)
   {
      if (renewRate > 1)
         renewRate = 1;
      if (renewRate < 0)
         renewRate = 0;
   }

   virtual ~BasicPopulationManagement()
   {
   }

   virtual MOSIndividual<R, ADS, DS>* createIndividual(Solution<R, ADS>* s)
   {
      MultiEvaluation<DS>* mev = nullptr;

      // no evaluation here!

      return new MOSIndividual<R, ADS, DS>(s, mev);
   }

   virtual vector<MOSIndividual<R, ADS, DS>*>& initialize(unsigned pSize)
   {
      Population<XES>* p = &initPop.generatePopulation(pSize);

      vector<MOSIndividual<R, ADS, DS>*> r;
      for (unsigned i = 0; i < p->size(); i++)
         r.push_back(createIndividual(&p->at(i)));

      p->clearNoKill();
      delete p;

      return *new vector<MOSIndividual<R, ADS, DS>*>(r);
   }

   // multiple crossovers in a binary tournament and apply many mutations
   virtual vector<MOSIndividual<R, ADS, DS>*>& createNext(unsigned target_size, const vector<const MOSIndividual<R, ADS, DS>*>& P)
   {
      vector<MOSIndividual<R, ADS, DS>*>* children = nullptr;

      unsigned size_renew = renewRate * target_size;

      if (size_renew > 0)
         children = &initialize(size_renew);
      else
         children = new vector<MOSIndividual<R, ADS, DS>*>();

      unsigned rest = target_size - size_renew;

      vector<const MOSIndividual<R, ADS, DS>*> Pconst(P.begin(), P.end());
      vector<const MOSIndividual<R, ADS, DS>*>& pool = binaryTournment(rest, Pconst);

      vector<MOSIndividual<R, ADS, DS>*>& crossMut = basicCrossoverMutation(pool);

      children->insert(children->end(), crossMut.begin(), crossMut.end());

      delete &crossMut;

      if (children->size() != target_size)
         if (Component::warning) {
            cout << "WARNING: BasicPopulationManagement::createNext() tried to create population of " << target_size << " but got " << children->size();
            cout << endl;
         }

      return *children;
   }

   vector<const MOSIndividual<R, ADS, DS>*>& binaryTournment(unsigned poolSize, const vector<const MOSIndividual<R, ADS, DS>*>& P)
   {
      vector<const MOSIndividual<R, ADS, DS>*>& pool = *new vector<const MOSIndividual<R, ADS, DS>*>;

      for (unsigned t = 1; t <= poolSize; t++) {
         int i = rg.rand(P.size());
         int j = i;
         while (i == j)
            j = rg.rand(P.size());

         if (P[i]->betterThan(*P[j]))
            pool.push_back(P[i]);
         else
            pool.push_back(P[j]);
      }

      return pool;
   }

   vector<MOSIndividual<R, ADS, DS>*>& basicCrossoverMutation(const vector<const MOSIndividual<R, ADS, DS>*>& pool)
   {
      vector<MOSIndividual<R, ADS, DS>*>* children = new vector<MOSIndividual<R, ADS, DS>*>();

      if (pool.size() == 0)
         return *children;

      for (unsigned i = 0; i < pool.size(); i++) {
         unsigned j = i + 1;
         if (j == pool.size())
            j = 0;

         pair<Solution<R, ADS>*, Solution<R, ADS>*> crossChildren;

         if (crossovers.size() > 0) {
            int crossId = rg.rand(crossovers.size());
            crossChildren = crossovers[crossId]->cross(*pool[i]->s, *pool[j]->s);
         } else
            crossChildren = make_pair(&pool[i]->s->clone(), &pool[j]->s->clone());

         vector<Solution<R, ADS>*> vChildren(2, nullptr);
         vChildren[0] = crossChildren.first;
         vChildren[1] = crossChildren.second;

         for (unsigned c = 0; c < 2; c++) {
            if ((mutations.size() > 0) && rg.randP(mutationRate)) // mutation rate
            {
               int neigh = rg.rand(mutations.size());
               Move<R, ADS, DS>* move = mutations[neigh]->validMove(*vChildren[c]);

               if (move) {
                  delete &move->apply(*vChildren[c]);
                  delete move;
                  move = nullptr;
               }
            }

            children->push_back(new MOSIndividual<R, ADS, DS>(vChildren[c], nullptr));
         }
      }

      return *children;
   }
};

}

#endif /*OPTFRAME_POPULATION_MANAGEMENT_HPP_*/
