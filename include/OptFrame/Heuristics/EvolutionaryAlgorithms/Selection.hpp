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

#ifndef SELECTION_HPP_
#define SELECTION_HPP_

#include <utility>
#include <vector>

#include "../../Evaluator.hpp"
#include "../../MultiEvaluation.hpp"
#include "../../MultiSolution.hpp"
#include "EA.h"

//#include "../../printable/printable.h" // this should not be required here

#ifndef _OPTFRAME_DBG_SELECTION_
#ifdef OPTFRAME_DEBUG
#define _OPTFRAME_DBG_SELECTION_
#else
#define _OPTFRAME_DBG_SELECTION_ while (false)
#endif /* OPTFRAME_DEBUG */
#endif /* _OPTFRAME_DBG_SELECTION_ */

namespace optframe {

template<XSolution S>
class Selection : public Component
  , public EA
{
public:
   virtual ~Selection()
   {
   }

   virtual pair<unsigned, unsigned> select(const MultiSolution<S>& population, const MultiEvaluation<>& mev, const std::vector<double>& fv) = 0;

   static double getMax(const std::vector<double>& fv)
   {
      double lmax = -10000000;
      for (int i = 0; i < fv.size(); i++)
         if (fv[i] > lmax)
            lmax = fv[i];
      return lmax;
   }

   static double getSum(const std::vector<double>& fv)
   {
      double s = 0;
      for (int i = 0; i < fv.size(); i++)
         s += fv[i];
      return s;
   }

   static void normalize(std::vector<double>& fv)
   {
      double sum = getSum(fv);
      if (sum == 0)
         sum = 1;
      for (int i = 0; i < fv.size(); i++) {
         fv[i] = fv[i] / sum;
         if (fv[i] != fv[i]) // is nan
         {
            std::cout << "Selection::normalize()::NAN VALUE!" << std::endl;
            //std::cout << fv << std::endl; // cannot depend on 'printable' here
            exit(1);
         }
      }
   }

   virtual bool compatible(string s)
   {
      return (s == idComponent()) || (Component::compatible(s));
   }

   static string idComponent()
   {
      stringstream ss;
      ss << Component::idComponent() << ":" << EA::family() << ":Selection";
      return ss.str();
   }

   virtual string id() const override
   {
      return idComponent();
   }
};

//temporary fix for the true basic genetic algorithm! I will revisit this in the future to perform a proper naming convention
template<XSolution S, XEvaluation XEv, XESolution XES = pair<S, XEv>>
class SimpleSelection
{
protected:
   using Individual = S;
   //using Chromossome = R;
   using Fitness = XEv*; //nullptr means there's no evaluation
   using VPopulation = vector<pair<Individual, Fitness>>;

   Evaluator<S, XEv, XES>& evaluator;

public:
   SimpleSelection(Evaluator<S, XEv, XES>& _evaluator)
     : evaluator(_evaluator){};
   virtual ~SimpleSelection() = default;

   virtual void select(VPopulation& population) = 0;

   //this is a support function to be used by programmers who need to rank the population before selection. See GA manual to check if the population is ranked when select is called.
   virtual void sortPopulation(VPopulation& population)
   {
      _OPTFRAME_DBG_SELECTION_ std::cerr << "-OptDebug- Selection operator is sorting the population" << std::endl;
      auto compare = [&](const pair<Individual, Fitness>& a, const pair<Individual, Fitness>& b) -> bool {
         if (a.second && b.second)
            return evaluator.betterThan(*a.second, *b.second);
         else
            return a.second != nullptr;
      };

      std::sort(population.begin(), population.end(), compare);
      _OPTFRAME_DBG_SELECTION_ std::cerr << "-OptDebug- Population ranked with selection operator" << std::endl;
   }
};

/**********************/
/* SELECTION EXAMPLES */
/**********************/

//Selects the 100alpha% most fit individuals
template<XSolution S, XEvaluation XEv, XESolution XES = pair<S, XEv>>
class ElitismSelection final : public SimpleSelection<S, XEv, XES>
{
protected:
   using Individual = S;
   //using Chromossome = R;
   using Fitness = XEv*; //nullptr means there's no evaluation
   using VPopulation = vector<pair<Individual, Fitness>>;

private:
   double alpha; //selectionRate

public:
   //optional parameter
   bool sortPopulationBeforeSelect = false; //this selection need to operate over a ranked population. If the GA used doesn't rank them, then you should flip this to true

   ElitismSelection(Evaluator<S, XEv, XES>& _evaluator, double selectionRate)
     : SimpleSelection<S, XEv, XES>(_evaluator)
     , alpha(selectionRate)
   {
      assert(selectionRate >= 0.0 && selectionRate <= 1.0);
   };
   ~ElitismSelection() = default;

   void select(VPopulation& population) override
   {
      _OPTFRAME_DBG_SELECTION_ std::cerr << "-OptDebug- ElitismSelection is selecting the " << population.size() * alpha << " most fit individuals and killing the rest" << std::endl;
      if (this->sortPopulationBeforeSelect)
         this->sortPopulation(population);
      int oldSize = population.size();
      int newSize = oldSize * alpha;
      for (int i = oldSize - 1; i >= newSize; --i)
         if (population[i].second)
            delete population[i].second;
      population.erase(population.begin() + newSize, population.end());
      _OPTFRAME_DBG_SELECTION_ std::cerr << "-OptDebug- ElitismSelection old size: " << oldSize << " new size: " << population.size() << std::endl;
   }
};

}

#endif /* SELECTION_HPP_ */
