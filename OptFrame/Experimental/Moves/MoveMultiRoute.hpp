#ifndef OPTFRAME_MOVEMULTIROUTE_HPP_
#define OPTFRAME_MOVEMULTIROUTE_HPP_

// Framework includes
#include "../../Move.hpp"

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

	void print() const
	{
		cout << "MoveMultiRoute: k=" << k << "; move = ";
		m.print();
	}
};


#endif /*OPTFRAME_MOVEMULTIROUTE_HPP_*/
