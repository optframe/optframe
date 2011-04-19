#ifndef OPTFRAME_NSSEQkROUTEADAPTER_HPP_
#define OPTFRAME_NSSEQkROUTEADAPTER_HPP_

// Framework includes
#include "../NSSeq.hpp"

#include "./Moves/MovekRoute.hpp"
#include "./NSIterators/IteratorNSSeqkRoute.hpp"

using namespace std;

template<class T, class M = OPTFRAME_DEFAULT_MEMORY, class MOVE = MovekRoute<T, M> >
class NSSeqkRouteAdapter: public NSSeq<vector<vector<T> > , M>
{
	typedef vector<T> Route;
	typedef vector<vector<T> > MultiRoute;

private:
	int k;
	NSSeq<Route, M>& ns;

public:

	NSSeqkRouteAdapter(int _k, NSSeq<Route, M>& _ns) :
		k(_k), ns(_ns)
	{
	}

	virtual ~NSSeqkRouteAdapter()
	{
	}

	Move<MultiRoute, M>& move(const MultiRoute& r)
	{
		return *new MOVE(k, ns.move(r[k]));
	}

	virtual NSIterator<MultiRoute, M>& getIterator(const MultiRoute& r)
	{
		NSIterator<Route, M>& iterator = ns.getIterator(r[k]);
		return *new IteratorNSSeqkRoute<T, M, MOVE> (k, iterator);
	}

	virtual void print()
	{
		cout << "NSSeqkRouteAdapter {" << endl;
		ns.print();
		cout << "}" << endl;
	}
};

#endif /*OPTFRAME_NSSEQkROUTEADAPTER_HPP_*/
