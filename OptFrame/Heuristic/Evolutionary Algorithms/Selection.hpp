#ifndef SELECTION_HPP_
#define SELECTION_HPP_

#include "../../Population.hpp"

template<class R, class M>
class Selection
{
   //private:
protected:

   typedef Solution<R> chromossome;
   //typedef vector<chromossome*> Population;
   typedef vector<Evaluation<M>*> FitnessValues;

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
    virtual Population<R>&
    select(unsigned populationSize, Population<R>& currentPopulation, FitnessValues& fv_crrtPop,
    Population<R>& intermediatePopulation, FitnessValues& fv_intPop) = 0;
    */

   /*
    virtual Population<R>&
    select(const Population<R>& currentPopulation, const FitnessValues& fv_crrtPop,
    const Population<R>& intermediatePopulation, const FitnessValues& fv_intPop) const = 0;
    */

   virtual Population<R>&
   select(const Population<R>& currentPopulation, const FitnessValues& fv_crrtPop,
         const Population<R>& intermediatePopulation, const FitnessValues& fv_intPop, const unsigned elitist_chrm_nb) const = 0;

};

#endif /* SELECTION_HPP_ */
