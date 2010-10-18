#ifndef CROSSOVERAG_HPP_
#define CROSSOVERAG_HPP_

#include "../../Solution.h"

template<class R>
class CrossoverAG
{
public:
	
	virtual pair<Solution<R>*,Solution<R>*> crossover(Solution<R>*, Solution<R>*) = 0;
	
};

#endif /*CROSSOVERAG_HPP_*/
