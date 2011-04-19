#ifndef OPTFRAME_MOVETSPSWAP_HPP_
#define OPTFRAME_MOVETSPSWAP_HPP_

// Framework includes
#include "../../Move.hpp"

using namespace std;

// Working structure: vector<T>

template<class T, class M = OPTFRAME_DEFAULT_MEMORY>
class MoveTSPSwap: public Move<vector<T> , M>
{
	typedef vector<T> Route;

protected:
	int p1, p2; // position 1 and position 2, respectively

public:

	MoveTSPSwap(int _p1, int _p2) :
		p1(_p1), p2(_p2)
	{
	}

	virtual ~MoveTSPSwap()
	{
	}

	int get_p1()
	{
		return p1;
	}

	int get_p2()
	{
		return p2;
	}

	bool canBeApplied(const Route& rep)
	{
		bool all_positive = (p1 >= 0) && (p2 >= 0);
		bool size_ok = (p1 < rep.size()) && (p2 < rep.size());
		return all_positive && size_ok && (rep.size() >= 2);
	}

	Move<Route, M>& apply(Route& rep)
	{
		T t = rep[p1];
		rep[p1] = rep[p2];
		rep[p2] = t;

		return *new MoveTSPSwap(p1, p2);
	}

	virtual bool operator==(const Move<Route, M>& _m) const
	{
		const MoveTSPSwap& m1 = (const MoveTSPSwap&) _m;
		return ((m1.p1 == p1) && (m1.p2 == p2));
	}

	void print() const
	{
		cout << "MoveTSPSwap( " << p1 << " <=> " << p2 << " )" << endl;
	}
};

#endif /*OPTFRAME_MOVETSPSWAP_HPP_*/
