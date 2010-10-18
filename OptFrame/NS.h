#ifndef NS_H_
#define NS_H_

#include "Move.hpp"
#include "Solution.h"

// ======================
// Neighborhood Structure
// Abstract Class
// ======================

template<class R, class M>
class NS
{
public:
    virtual Move<R,M>* move(Solution<R>* s)
    {
    	return move(s->getRepresentation());
    }

    virtual Move<R,M>* move(R*) = 0;

	virtual void print() = 0;
};


#endif /*NS_H_*/
