#ifndef INITIALSOLUTION_H_
#define INITIALSOLUTION_H_

#include "Solution.h"

template<class R>
class InitialSolution
{	
public:
	virtual Solution<R>* generateSolution() = 0;

};

#endif /*INITIALSOLUTION_H_*/
