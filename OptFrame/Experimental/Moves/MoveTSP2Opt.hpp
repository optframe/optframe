#ifndef OPTFRAME_MOVETSP2OPT_HPP_
#define OPTFRAME_MOVETSP2OPT_HPP_

// Framework includes
#include "../../Move.hpp"

using namespace std;

// Working structure: vector<T>

template<class T, class M = OPTFRAME_DEFAULT_MEMORY>
class MoveTSP2Opt: public Move<vector<T> , M>
{
	typedef vector<T> Route;

protected:
	int p1, p2; // position 1 and position 2, respectively

public:

	MoveTSP2Opt(int _p1, int _p2) :
		p1(_p1), p2(_p2)
	{
	}

	virtual ~MoveTSP2Opt()
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
		return all_positive && (rep.size() >= 2);
	}

	Move<Route, M>& apply(Route& rep)
	{

		reverse(rep.begin() + p1, rep.begin() + p2);

		// r1->r1, r2->r2, e1->i1, e2->i2, n1->n2, n2->n1, i1->e1, i2->e2
		return *new MoveTSP2Opt(p1, p2);
	}

	virtual bool operator==(const Move<Route, M>& _m) const
	{
		const MoveTSP2Opt& m1 = (const MoveTSP2Opt&) _m;
		return ((m1.p1 == p1) && (m1.p2 == p2));
	}

	void print() const
	{
		cout << "MoveTSP2Opt( ";
		cout << " edge " << p1 << " <=>  edge " << p2 << " )";
		cout << endl;
	}
};

#endif /*OPTFRAME_MOVETSP2OPT_HPP_*/
