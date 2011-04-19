#ifndef OPTFRAME_ITERATORNSSEQMULTIROUTE_HPP_
#define OPTFRAME_ITERATORNSSEQMULTIROUTE_HPP_

// Framework includes
#include "../../Move.hpp"
#include "../../NSIterator.hpp"

using namespace std;

class NSSeqMultiRouteIteratorOutOfBound
{
public:
	NSSeqMultiRouteIteratorOutOfBound()
	{
	}
};

template<class T, class M = OPTFRAME_DEFAULT_MEMORY, class MOVE = MoveMultiRoute<T, M> >
class IteratorNSSeqMultiRoute: public NSIterator<vector<vector<T> > , M>
{
	typedef vector<T> Route;
	typedef vector<vector<T> > MultiRoute;

private:
	vector<NSIterator<Route, M>*>& iterators;
	int i;

public:

	IteratorNSSeqMultiRoute(vector<NSIterator<Route, M>*>& it) :
		iterators(it)
	{
		i = 0;
	}

	virtual ~IteratorNSSeqMultiRoute()
	{
		for (int j = 0; j < iterators.size(); j++)
			delete iterators[j];
		delete &iterators;
	}

	void first()
	{
		for (int j = 0; j < iterators.size(); j++)
			iterators[j]->first();

		i = 0;
		while (i < iterators.size())
			if (!iterators[i]->isDone())
				break;
			else
				i++;
	}

	void next()
	{
		iterators[i]->next();
		while (i < iterators.size())
			if (!iterators[i]->isDone())
				break;
			else
				i++;
	}

	bool isDone()
	{
		for (int j = i; j < iterators.size(); j++)
			if (!iterators[j]->isDone())
				return false;
		return true;
	}

	Move<MultiRoute, M>& current()
	{
		if ((i < iterators.size()) && (!iterators[i]->isDone()))
			return *new MOVE(i, iterators[i]->current());
		else
			throw NSSeqMultiRouteIteratorOutOfBound();
	}
};


#endif /*OPTFRAME_ITERATORNSSEQMULTIROUTE_HPP_*/
