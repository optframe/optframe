#ifndef OPTFRAME_NSENUM_HPP_
#define OPTFRAME_NSENUM_HPP_

#include "NSSeq.hpp"
#include "RandGen.hpp"

#include "NSEnumIterator.hpp"

using namespace std;

template<class R, class M> // M = OPTFRAME_DEFAULT_MEMORY already defined
class NSEnum: public NSSeq<R, M>
{

protected:
	RandGen& rg;

public:

	using NSSeq<R, M>::move; // prevents name hiding

	NSEnum(RandGen& _rg):rg(_rg)
	{}

	virtual ~NSEnum()
	{
	}

	virtual Move<R, M>& move(const R&)
	{
		return move( rg.rand(size()));
	}

	virtual NSIterator<R, M>& getIterator(const R&)
	{
		return *new NSEnumIterator<R, M> (*this);
	}

	virtual Move<R, M>& move(unsigned int m) = 0;

	virtual unsigned int size() = 0;

	virtual void print() = 0;
};

#endif /*OPTFRAME_NSENUM_HPP_*/
