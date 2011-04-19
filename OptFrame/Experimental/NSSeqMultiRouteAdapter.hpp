#ifndef OPTFRAME_NSSEQMULTIROUTEADAPTER_HPP_
#define OPTFRAME_NSSEQMULTIROUTEADAPTER_HPP_

// Framework includes

#include "../Move.hpp"
#include "../NSSeq.hpp"

#include "Moves/MoveMultiRoute.hpp"
#include "NSIterators/IteratorNSSeqMultiRoute.hpp"

using namespace std;

template<class T, class M = OPTFRAME_DEFAULT_MEMORY, class MOVE = MoveMultiRoute<T, M> >
class NSSeqMultiRouteAdapter: public NSSeq<vector<vector<T> > , M>
{
	typedef vector<T> Route;
	typedef vector<vector<T> > MultiRoute;

private:
	NSSeq<Route, M>& ns;

public:

	NSSeqMultiRouteAdapter(NSSeq<Route, M>& _ns) :
		ns(_ns)
	{
	}

	virtual ~NSSeqMultiRouteAdapter()
	{
	}

	Move<MultiRoute, M>& move(const MultiRoute& r)
	{
		int x = rand() % r.size();
		return *new MOVE(x, ns.move(r[x]));
	}

	virtual NSIterator<MultiRoute, M>& getIterator(const MultiRoute& r)
	{
		vector<NSIterator<Route, M>*>& iterators = *new vector<NSIterator<Route, M>*> ;
		for (int i = 0; i < r.size(); i++)
			iterators.push_back(&ns.getIterator(r[i]));

		return *new IteratorNSSeqMultiRoute<T, M, MOVE> (iterators);
	}

	virtual void print()
	{
		cout << "NSSeqMultiRouteAdapter {" << endl;
		ns.print();
		cout << "}" << endl;
	}
};

#endif /*OPTFRAME_NSSEQMULTIROUTEADAPTER_HPP_*/
