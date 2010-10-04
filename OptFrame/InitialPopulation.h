#ifndef OPTFRAME_INITIALPOPULATION_H_
#define OPTFRAME_INITIALPOPULATION_H_

#include <vector>
#include "Solution.hpp"

using namespace std;

template<class R>
class InitialPopulation
{
	typedef vector<Solution<R>*> Population;

public:
	virtual ~InitialPopulation()
	{
	}

	virtual Population& generatePopulation() = 0;
};

#endif /*OPTFRAME_INITIALPOPULATION_H_*/
