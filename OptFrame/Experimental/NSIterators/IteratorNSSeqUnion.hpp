#ifndef OPTFRAME_ITERATORNSSEQUNION_HPP_
#define OPTFRAME_ITERATORNSSEQUNION_HPP_

// Framework includes

#include "../../NSIterator.hpp"
#include "../Moves/MoveNSSeqUnion.hpp"

using namespace std;

class NSSeqUnionAdapterOutOfBound
{
public:
	NSSeqUnionAdapterOutOfBound()
	{
	}
};

template<class R, class M = OPTFRAME_DEFAULT_MEMORY, class MOVE = MoveNSSeqUnion<R, M> >
class IteratorNSSeqUnion: public NSIterator<R, M>
{
private:
	NSIterator<R, M>& i1;
	NSIterator<R, M>& i2;

public:

	IteratorNSSeqUnion(NSIterator<R, M>& _i1, NSIterator<R, M>& _i2) :
		i1(_i1), i2(_i2)
	{
	}

	virtual ~IteratorNSSeqUnion()
	{
		delete &i1;
		delete &i2;
	}

	void first()
	{
		i1.first();
		i2.first();
	}

	void next()
	{
		if (!i1.isDone())
			i1.next();
		else
			i2.next();
	}

	bool isDone()
	{
		return i1.isDone() && i2.isDone();
	}

	Move<R, M>& current()
	{
		if (!i1.isDone())
			return *new MOVE(0, i1.current());
		else if (!i2.isDone())
			return *new MOVE(1, i2.current());
		else
			throw NSSeqUnionAdapterOutOfBound();
	}
};



#endif /*OPTFRAME_ITERATORNSSEQUNION_HPP_*/
