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

#ifndef SELECTION_HPP_
#define SELECTION_HPP_

#include "../../Population.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class Selection
{
   //private:
protected:

   typedef Solution<R, ADS> chromossome;
   //typedef vector<chromossome*> Population;
   typedef vector<Evaluation<DS>*> FitnessValues;

   //Population * population;
   //FitnessValues * fitnessValues;

public:

   //virtual pair<chromossome&, chromossome&>& select(Population& p, FitnessValues& fv) = 0;

   //virtual pair<chromossome&, chromossome&> select(Population& p, FitnessValues& fv) = 0;

   /*
    virtual Population&
    select(Population& currentPopulation, FitnessValues& fv_crrtPop,
    Population& intermediatePopulation, FitnessValues& fv_intPop) = 0;
    */

   /*
    virtual Population&
    select(unsigned populationSize, Population& currentPopulation, FitnessValues& fv_crrtPop,
    Population& intermediatePopulation, FitnessValues& fv_intPop) = 0;
    */

   /*
    virtual Population<R, ADS>&
    select(unsigned populationSize, Population<R, ADS>& currentPopulation, FitnessValues& fv_crrtPop,
    Population<R, ADS>& intermediatePopulation, FitnessValues& fv_intPop) = 0;
    */

   /*
    virtual Population<R, ADS>&
    select(const Population<R, ADS>& currentPopulation, const FitnessValues& fv_crrtPop,
    const Population<R, ADS>& intermediatePopulation, const FitnessValues& fv_intPop) const = 0;
    */

   virtual Population<R, ADS>&
   select(const Population<R, ADS>& currentPopulation, const FitnessValues& fv_crrtPop,
         const Population<R, ADS>& intermediatePopulation, const FitnessValues& fv_intPop, const unsigned elitist_chrm_nb) const = 0;

};

}

#endif /* SELECTION_HPP_ */
