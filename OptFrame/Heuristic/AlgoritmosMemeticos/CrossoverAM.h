#ifndef CROSSOVERAM_HPP_
#define CROSSOVERAM_HPP_

#include "../../Solution.h"

template<class R>
class CrossoverAM
{
public:
	
	virtual pair<Solution<R>*,Solution<R>*> crossover(Solution<R>* s) = 0;
	
};

#endif /*CROSSOVERAM_HPP_*/
