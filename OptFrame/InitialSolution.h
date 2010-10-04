#ifndef OPTFRAME_INITIALSOLUTION_H_
#define OPTFRAME_INITIALSOLUTION_H_

#include "Solution.hpp"

template<class R>
class InitialSolution
{
public:
	virtual ~InitialSolution()
	{
	}

	virtual Solution<R>& generateSolution() = 0;
};

#endif /*OPTFRAME_INITIALSOLUTION_H_*/
