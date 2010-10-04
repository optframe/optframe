#ifndef OPTFRAME_NS_HPP_
#define OPTFRAME_NS_HPP_

#include "Move.hpp"
#include "Solution.hpp"

template<class R, class M = OPTFRAME_DEFAULT_MEMORY>
class NS
{
public:

	virtual ~NS()
	{
	}

    Move<R,M>& move(const Solution<R>& s)
    {
    	return move(s.getR());
    }

    virtual Move<R,M>& move(const R&) = 0;

	virtual void print() = 0;
};

#endif /*OPTFRAME_NS_HPP_*/
