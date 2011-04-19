#ifndef OPTFRAME_ITERATORNSSEQPAIRFIRST_HPP_
#define OPTFRAME_ITERATORNSSEQPAIRFIRST_HPP_

// Framework includes

#include "../../NSIterator.hpp"
#include "../Moves/MovePairFirst.hpp"

using namespace std;

class PairFirstAdapterIteratorOutOfBound
{
public:
	PairFirstAdapterIteratorOutOfBound()
	{
	}
};

template<class T1, class T2, class M = OPTFRAME_DEFAULT_MEMORY, class MOVE = MovePairFirst<T1, T2, M> >
class IteratorNSSeqPairFirst: public NSIterator<pair<T1, T2> , M>
{
private:
	NSIterator<T1, M>& iterator;

public:

	IteratorNSSeqPairFirst(NSIterator<T1, M>& it) :
		iterator(it)
	{
	}

	virtual ~IteratorNSSeqPairFirst()
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
			throw PairFirstAdapterIteratorOutOfBound();
	}
};

#endif /*OPTFRAME_ITERATORNSSEQPAIRFIRST_HPP_*/
