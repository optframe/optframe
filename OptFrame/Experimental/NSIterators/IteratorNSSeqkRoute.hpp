#ifndef OPTFRAME_ITERATORNSSEQKROUTE_HPP_
#define OPTFRAME_ITERATORNSSEQKROUTE_HPP_

// Framework includes
#include "../../NSIterator.hpp"

#include "../Moves/MovekRoute.hpp"


using namespace std;

template<class T, class M = OPTFRAME_DEFAULT_MEMORY, class MOVE = MovekRoute<T, M> >
class IteratorNSSeqkRoute: public NSIterator<vector<vector<T> > , M>
{
	typedef vector<T> Route;
	typedef vector<vector<T> > MultiRoute;

private:
	int k;
	NSIterator<Route, M>& iterator;

public:

	IteratorNSSeqkRoute(int _k, NSIterator<Route, M>& it) :
		k(_k), iterator(it)
	{
	}

	virtual ~IteratorNSSeqkRoute()
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

	Move<MultiRoute, M>& current()
	{
		return *new MOVE(k, iterator.current());
	}
};

#endif /*OPTFRAME_ITERATORNSSEQKROUTE_HPP_*/
