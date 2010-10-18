#ifndef OPTFRAME_INITIALPOPULATION_H_
#define OPTFRAME_INITIALPOPULATION_H_

#include "Population.hpp"

using namespace std;

template<class R>
class InitialPopulation
{
public:
   virtual Population<R>& generatePopulation(unsigned populationSize) = 0;
};

#endif /*OPTFRAME_INITIALPOPULATION_H_*/
