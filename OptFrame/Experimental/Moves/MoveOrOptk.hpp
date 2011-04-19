#ifndef OPTFRAME_MOVEOROPTK_HPP_
#define OPTFRAME_MOVEOROPTK_HPP_

// Framework includes
#include "../../Move.hpp"

using namespace std;

// Working structure: vector<vector<T> >

template<class T, class M = OPTFRAME_DEFAULT_MEMORY>
class MoveOrOptk: public Move<vector<T> , M>
{
	typedef vector<T> Route;

private:
	int i; // origin
	int j; // destination
	int k; // number of elements

public:

	MoveOrOptk(int _i, int _j, int _k) :
		i(_i), j(_j), k(_k)
	{
	}

	virtual ~MoveOrOptk()
	{
	}

	bool canBeApplied(const Route& rep)
	{
		//return (i != j) && (i + k <= rep.size());
		return (i != j);
	}

	Move<Route, M>& apply(Route& rep)
	{
		vector<T> v_aux;
		v_aux.insert(v_aux.begin(), rep.begin() + i, rep.begin() + i + k - 1);
		rep.erase(rep.begin() + i, rep.begin() + i + k - 1);
		rep.insert(rep.begin() + j, v_aux.begin(), v_aux.end());

		return *new MoveOrOptk(j, i, k);
	}

	virtual bool operator==(const Move<Route, M>& _m) const
	{
		const MoveOrOptk& m1 = (const MoveOrOptk&) _m;
		return (m1.i == i) && (m1.j == j) && (m1.k == k);
	}

	void print() const
	{
		cout << "MoveVectorOrOpt{K=" << k << "}";
		cout << "(" << i << ";" << j << ")" << endl;
	}
};

#endif /*OPTFRAME_MOVEOROPTK_HPP_*/
