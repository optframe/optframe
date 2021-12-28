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

#ifndef OPTFRAME_INITIALPOPULATION_H_
#define OPTFRAME_INITIALPOPULATION_H_

#include <chrono>
#include <random>
#include <utility>

#include "Population.hpp"

#include "Component.hpp"
#include "Constructive.hpp"
#include "Evaluation.hpp"
#include "Evaluator.hpp"
#include "Heuristics/GRASP/GRConstructive.hpp"
#include "RandGen.hpp"

#ifndef _OPTFRAME_DBG_INITIAL_POP_
#ifdef OPTFRAME_DEBUG
#define _OPTFRAME_DBG_INITIAL_POP_
#else
#define _OPTFRAME_DBG_INITIAL_POP_ while (false)
#endif /* OPTFRAME_DEBUG */
#endif /* _OPTFRAME_DBG_INITIAL_POP_ */

namespace optframe {

template<XESolution XES>
class InitialPopulation : public Component
{
   using S = typename XES::first_type;
   using XEv = typename XES::first_type;

public:
   virtual ~InitialPopulation()
   {
   }

   virtual Population<XES> generatePopulation(unsigned populationSize, double timelimit) = 0;

   static string idComponent()
   {
      stringstream ss;
      ss << Component::idComponent() << ":InitialPopulation";
      return ss.str();
   }

   std::string toString() const override
   {
      return id();
   }

   virtual string id() const
   {
      return idComponent();
   }
};

template<XESolution XES>
class BasicInitialPopulation : public InitialPopulation<XES>
{
   using S = typename XES::first_type;
   using XEv = typename XES::first_type;

public:
   Constructive<S>& constructive;

   BasicInitialPopulation(Constructive<S>& _constructive)
     : constructive(_constructive)
   {
   }

   virtual ~BasicInitialPopulation()
   {
   }

   virtual Population<XES> generatePopulation(unsigned populationSize, double timelimit)
   {
      Population<XES>* p = new Population<XES>;
      for (unsigned i = 0; i < populationSize; i++)
         p->push_back(constructive.generateSolution(timelimit));
      return *p;
   }

   static string idComponent()
   {
      stringstream ss;
      ss << Population<XES>::idComponent() << ":BasicInitialPopulation";
      return ss.str();
   }

   std::string toString() const override
   {
      return id();
   }

   virtual string id() const
   {
      return idComponent();
   }
};

template<XESolution XES>
class GRInitialPopulation : public InitialPopulation<XES>
{
   using S = typename XES::first_type;
   using XEv = typename XES::first_type;

public:
   GRConstructive<S>& constructive;
   RandGen& rg;
   double maxAlpha; // limit the solution to be not so random

   GRInitialPopulation(GRConstructive<S>& _constructive, RandGen& _rg, double _maxAlpha)
     : constructive(_constructive)
     , rg(_rg)
     , maxAlpha(_maxAlpha)
   {
   }

   virtual ~GRInitialPopulation()
   {
   }

   virtual Population<XES> generatePopulation(unsigned populationSize, double timelimit)
   {
      Population<XES> pop;
      for (unsigned i = 0; i < populationSize; i++) {
         float alpha = rg.rand01();
         while (alpha > maxAlpha) {
            alpha = rg.rand01();
         }

         if (alpha == 0)
            alpha = 0.00001;

         std::optional<S> s = constructive.generateGRSolution(alpha, timelimit);
         pop.push_back(std::move(*s)); // the end of solution
      }
      return pop;
   }

   static string idComponent()
   {
      stringstream ss;
      ss << Population<XES>::idComponent() << ":GRInitialPopulation";
      return ss.str();
   }

   std::string toString() const override
   {
      return id();
   }

   virtual string id() const
   {
      return idComponent();
   }
};

template<XESolution XES>
class SimpleInitialPopulation
{
   using S = typename XES::first_type;
   using XEv = typename XES::first_type;

protected:
   using Individual = S;
   //using Chromossome = R;
   using Fitness = XEv*; //nullptr means there's no evaluation
   using Population = vector<pair<Individual, Fitness>>;

public:
   unsigned int initialPopSize;

   SimpleInitialPopulation(unsigned int _initialPopSize)
     : initialPopSize(_initialPopSize)
   {
      assert(initialPopSize >= 2);
   };
   virtual ~SimpleInitialPopulation() = default;

   virtual Population generate() = 0;
};

/**********************/
/* INITIAL POPULATION EXAMPLES */
/**********************/

//generates random individuals based on user programmed method
template<XESolution XES>
class RandomInitialPopulation : public SimpleInitialPopulation<XES>
{
   using S = typename XES::first_type;
   using XEv = typename XES::first_type;

protected:
   using Individual = S;
   //using Chromossome = R;
   using Fitness = XEv*; //nullptr means there's no evaluation
   using Population = vector<pair<Individual, Fitness>>;

public:
   RandomInitialPopulation() = delete;
   RandomInitialPopulation(unsigned int initialPopSize)
     : SimpleInitialPopulation<XES>(initialPopSize){};
   virtual ~RandomInitialPopulation() = default;

   virtual Individual generateIndividual() = 0;

   virtual Population generate()
   {
      _OPTFRAME_DBG_INITIAL_POP_ std::cerr << "-OptDebug- Generating Initial Population with RandomInitialPopulation of size " << this->initialPopSize << std::endl;

      Population pop; //won't reserve memory, in applications with a very heavy empty constructor can be costly... I will take the cost of reallocating the pop
      for (int i = 0; i < this->initialPopSize; ++i)
         pop.emplace_back(std::make_pair<Individual, Fitness>(generateIndividual(), nullptr)); //maybe reserve memory... but i don't know how costly is the empty constructor for the solution

      _OPTFRAME_DBG_INITIAL_POP_ std::cerr << "-OptDebug- RandomInitialPopulation finished with a population of " << pop.size() << " individuals" << std::endl;
      return pop;
   };
};

}

#endif /*OPTFRAME_INITIALPOPULATION_H_*/
