#ifndef OPTFRAME_CROSSOVER_HPP_
#define OPTFRAME_CROSSOVER_HPP_

#include "../../Solution.hpp"
#include "../../Evaluation.hpp"

//#include "../../Population.hpp"

template<class R, class M>
class Crossover
{
protected:

   typedef Solution<R> chromossome;
   //typedef vector<chromossome*> Population;
   typedef vector<Evaluation<M>*> FitnessValues;

public:

   //virtual pair<chromossome &, chromossome *>& offspring(chromossome&, chromossome&) = 0;

   virtual pair<chromossome &, chromossome *>
   & offspring(const chromossome&, const chromossome&) const = 0;

   pair<chromossome &, chromossome *>& offspring(pair<const chromossome&, const chromossome&>& off) const
   {
      return offspring(off.first, off.second);
   }

};

#endif /*OPTFRAME_CROSSOVER_HPP_*/
