#ifndef OPTFRAME_ITERATORNSSEQPAIRSECOND_HPP_
#define OPTFRAME_ITERATORNSSEQPAIRSECOND_HPP_

// Framework includes
#include "../../NSIterator.hpp"

#include "../Moves/MovePairSecond.hpp"

using namespace std;

class PairSecondAdapterIteratorOutOfBound
{
public:
	PairSecondAdapterIteratorOutOfBound()
	{
	}
};

template<class T1, class T2, class M = OPTFRAME_DEFAULT_MEMORY, class MOVE = MovePairSecond<T1, T2, M> >
class IteratorNSSeqPairSecond: public NSIterator<pair<T1, T2> , M>
{
private:
	NSIterator<T2, M>& iterator;

public:

	IteratorNSSeqPairSecond(NSIterator<T2, M>& it) :
		iterator(it)
	{
	}

	virtual ~IteratorNSSeqPairSecond()
	{
		delete &iterator;
	}

	void first()
	{
		iterator.first();
	}

	void next()
	{
		iterator.next();
	}

	bool isDone()
	{
		return iterator.isDone();
	}

	Move<pair<T1, T2> , M>& current()
	{

		if (!iterator.isDone())
			return *new MOVE(iterator.current());
		else
			throw PairSecondAdapterIteratorOutOfBound();
	}
};


#endif /*OPTFRAME_ITERATORNSSEQPAIRSECOND_HPP_*/
