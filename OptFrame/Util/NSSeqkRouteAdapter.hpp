#ifndef OPTFRAME_NSSEQkROUTEADAPTER_HPP_
#define OPTFRAME_NSSEQkROUTEADAPTER_HPP_

// Framework includes
#include "../Move.hpp"
#include "../NSSeq.hpp"

using namespace std;

template<class T, class M>
class MovekRoute: public Move<vector<vector<T> > , M>
{
	typedef vector<T> Route;
	typedef vector<vector<T> > MultiRoute;

protected:
	int k; // route number
	Move<Route, M>& m;

public:

	MovekRoute(int _k, Move<Route, M>& _m) :
		k(_k), m(_m)
	{
	}

	virtual ~MovekRoute()
	{
		delete &m;
	}

	bool canBeApplied(const MultiRoute& rep)
	{
		return m.canBeApplied(rep[k]);
	}

	Move<MultiRoute, M>& apply(MultiRoute& rep)
	{
		return *new MovekRoute<T, M> (k, m.apply(rep[k]));
	}

	Move<MultiRoute, M>& apply(M& mem, MultiRoute& rep)
	{
		return *new MovekRoute<T, M> (k, m.apply(mem, rep[k]));
	}

	virtual bool operator==(const Move<MultiRoute, M>& _m) const
	{
		const MovekRoute<T, M>& m1 = (const MovekRoute<T, M>&) _m;
		if (k == m1.k)
			return m == m1.m;
		else
			return false;
	}

	void print()
	{
		cout << "MovekRoute: k=" << k << "; move = ";
		m.print();
	}
};

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
