#ifndef OPTFRAME_NSSEQMULTIROUTEADAPTER_HPP_
#define OPTFRAME_NSSEQMULTIROUTEADAPTER_HPP_

// Framework includes
#include "../Move.hpp"
#include "../NSSeq.hpp"

using namespace std;

template<class T, class M = OPTFRAME_DEFAULT_MEMORY>
class MoveMultiRoute: public Move<vector<vector<T> > , M>
{
	typedef vector<T> Route;
	typedef vector<vector<T> > MultiRoute;

protected:
	int k; // route number
	Move<Route, M>& m;

public:

	MoveMultiRoute(int _k, Move<Route, M>& _m) :
		k(_k), m(_m)
	{
	}

	int get_k()
	{
		return k;
	}

	Move<Route, M>& get_m()
	{
		return m;
	}

	virtual ~MoveMultiRoute()
	{
		delete &m;
	}

	bool canBeApplied(const MultiRoute& rep)
	{
		return m.canBeApplied(rep[k]);
	}

	Move<MultiRoute, M>& apply(MultiRoute& rep)
	{
		return *new MoveMultiRoute<T, M> (k, m.apply(rep[k]));
	}

	Move<MultiRoute, M>& apply(M& mem, MultiRoute& rep)
	{
		return *new MoveMultiRoute<T, M> (k, m.apply(mem, rep[k]));
	}

	virtual bool operator==(const Move<MultiRoute, M>& _m) const
	{
		const MoveMultiRoute<T, M>& m1 = (const MoveMultiRoute<T, M>&) _m;
		if (k == m1.k)
			return m == m1.m;
		else
			return false;
	}

	void print()
	{
		cout << "MoveMultiRoute: k=" << k << "; move = ";
		m.print();
	}
};

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
