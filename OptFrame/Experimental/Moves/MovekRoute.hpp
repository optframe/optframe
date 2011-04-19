#ifndef OPTFRAME_MOVEKROUTE_HPP_
#define OPTFRAME_MOVEKROUTE_HPP_

// Framework includes
#include "../../Move.hpp"

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

	void print() const
	{
		cout << "MovekRoute: k=" << k << "; move = ";
		m.print();
	}
};


#endif /*OPTFRAME_MOVEKROUTE_HPP_*/
