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

#ifndef OPTFRAME_PSO_HPP_
#define OPTFRAME_PSO_HPP_

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

// Particle Swarm Optimization - PSO

namespace optframe {

template<XEvaluation XEv = Evaluation<>, optframe::comparability KeyType = double>

// begin PSO

struct Bird
{
   vector<double> velocity;
   vector<double> position;
};

template<
  optframe::comparability KeyType = double,
  XEvaluation XEv = Evaluation<>,
  XESolution XES = std::pair<std::vector<KeyType>, XEv>,
  XESolution XES2 = std::pair<Bird, Evaluation<>>,
  XSearch<XES2> XSH2 = Population<XES2>>
class PSO : public SingleObjSearch<XES, XES2, XSH2>
  , public IPopulational<XES, XES, XES2>
{
   using S = typename XES::first_type;
   //using XEv = typename XES::second_type;
   using XSH = XES;

   //private:
   //   Bird Global;

protected:
   Evaluator<S, XEv, XES>& evaluator; // Check to avoid memory leaks
   //InitialPopulation<XES2>& initPop; // TODO: add if necessary
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
   unsigned iter_max;
   // -> Vector con los topes inferiores de los parámetros
   vector<double> cI;
   // -> Vector con los topes superiores de los parámetros
   vector<double> cS;
   // random number generator
   RandGen& rg;

public:
   PSO(Evaluator<S, XEv, XES>& evaluator, unsigned pop_size, unsigned iter_max, const vector<double>& cI, const vector<double>& cS, RandGen& _rg)
     : evaluator(evaluator)
     , pop_size(pop_size)
     , iter_max(iter_max)
     , cI(cI)
     , cS(cS)
     , rg{ _rg }
   {
   }

   virtual Population<XES2> generatePopulation()
   {
      Population<XES2> swarm;
      for (unsigned i = 0; i < this->pop_size; i++) {
         // create a new Bird
         Bird b;
         for (unsigned j = 0; j < this->cI.size(); j++) {
            b.position[j] = this->cI[j] + (this->cS[i] - this->cI[i]) * rg.rand01();
            b.velocity[j] = 0.1 * (this->cI[j] + (this->cS[i] - this->cI[i]) * rg.rand01());
         }
         // add bird to swarm (no evaluation is given)
         swarm.push_back(b);
      }
      return swarm;
   }

   // update bird position (random mutation)
   void birdMutation(Bird& b)
   {
      if (rg.rand01() < 0.1 * 0.1) {
         for (unsigned j = 0; j < this->cS.size(); j++) {
            double r = rg.rand01();
            if (r < 0.5)
               b.position[j] += (cS[j] - cI[j]) * (::pow(2.0 * r, 1.0 / 21) - 1);
            else
               b.position[j] += (cS[j] - cI[j]) * (::pow(2.0 * (1 - r), 1.0 / 21) + 1);
         }
      }
   }

   // checks if bird position/velocity is within valid bounds
   void boundCheck(Bird& b)
   {
      unsigned count = 0;
      while (counter < this->cS.size()) {
         if (b.position[count] < cI || b.position[count] > cS) {
            for (unsigned j = 0; j < this->cS.size(); j++) {
               b.position[j] = this->cI[j] + (this->cS[i] - this->cI[i]) * rg.rand01();
               b.velocity[j] = 0.1 * (this->cI[j] + (this->cS[i] - this->cI[i]) * rg.rand01());
            }
            break;
         }
         count++;
      } // while
   }    // BoundCheck

   virtual ~PSO()
   {
      // nothing to delete... NICE!
   }

   SearchStatus search(const StopCriteria<XEv>& stopCriteria) override
   {
      if (Component::debug)
         (*Component::logdata) << "PSO search():"
                               << " pop_size=" << pop_size << " iter_max=" << iter_max
                               << std::endl;

      op<XES>& star = this->best;
      // check if time/target conditions are met
      if (stopCriteria.shouldStop(std::nullopt))
         return SearchStatus::NO_REPORT;

      Population<XES2> swarm = generatePopulation();

      for (unsigned i = 0; i < this->pop_size; i++) {
         Bird& b = swarm[i];
         for (unsigned j = 0; j < this->cS.size(); j++) {
            // update positions of bird b, based on its velocities
            b.position[j] += b.velocity[j];
         }
         boundCheck();
      }

      //first global best
      for (unsigned i = 0; i < this->pop_size; i++) {
         if (evaluator(swarm[i].localbest) < evaluator(Global)))
            Global = swarm[i];
      }

      // count generations
      int count_gen = 0;

      while (count_gen < iter_max) {

         // Particle update
         for (unsigned i = 0; i < this->pop_size; i++) {
            Bird& b = swarm[i];

            // 0.1 chance of generating a random guy
            if (rand01() < 0.1) {
               for (unsigned j = 0; j < this->cS.size(); j++) {
                  b.position[j] = this->cI[j] + (this->cS[i] - this->cI[i]) * rg.rand01();
                  b.velocity[j] = 0.1 * (this->cI[j] + (this->cS[i] - this->cI[i]) * rg.rand01());
               }
            }

            // 0.9 chance to just update the particle
            else {
               for (unsigned j = 0; j < this->cS.size(); j++)
                  b.position[j] += b.velocity[j];
            }

            BirdMutation(b) // 0.01 chance to do specific mutation

              // After updating, check to see if there's local upgrades
              BoundCheck(b);
            if (evaluator(b.position) < evaluator(b.localbest)))
            b.localbest = b.position;
         }

         // Speed update
         for (unsigned i = 0; i < this->pop_size; i++) {
            Bird& b = swarm[i];
            for (unsigned j = 0; j < this->cS.size(); j++) {
               b.velocity = (0.25 + this->rg.rand01() / 2) * b.velocity + 1.5 * this->rg.rand01() * (b.localbest - b.position) + 1.5 * this->rg.rand01() * (Global.localbest - b.position)
            }
         }

         // Global Best Maintenance
         for (unsigned i = 0; i < this->pop_size; i++) {
            if (evaluator(swarm[i].localbest) < evaluator(Global)))
            Global = swarm[i];
         }

         count_gen++;
      } //while count_gen

      // This is where magic happens...

      this->best = star;
      return SearchStatus::NO_REPORT;
   }

   // reimplementing searchBy, just to make it more explicit (visible)
   // maybe add some specific logs?
   virtual SearchStatus searchBy(
     std::optional<XSH>& _best,
     std::optional<XSH2>& _inc,
     const StopCriteria<XEv>& stopCriteria) override
   {
      this->best = _best;
      this->incumbent = _inc;
      return search(stopCriteria);
   }

   virtual bool
   setSilentR() override
   {
      this->setSilent();
      // force execution over all components
      return evaluator.setSilentR();
   }

   virtual bool
   setVerboseR() override
   {
      this->setVerbose();
      // force execution over all components
      return evaluator.setVerboseR();
   }
};

} // namespace optframe

#endif /*OPTFRAME_PSO_HPP_*/
