#ifndef OPTFRAME_ELITISM_HPP_
#define OPTFRAME_ELITISM_HPP_

#include "../../Solution.hpp"
#include "../../Evaluation.hpp"

#include "../../Population.hpp"

template<class R, class M>
class Elitism
{
protected:

   typedef Solution<R> chromossome;
   //typedef vector<chromossome*> Population;
   typedef vector<Evaluation<M>*> FitnessValues;

public:

   virtual Population<R> & doElitism(const Population<R> &p, const FitnessValues& fv_p, unsigned popElitistSize) const = 0;

};

#endif /*OPTFRAME_ELITISM_HPP_*/
