#ifndef OPTFRAME_MOVEPAIRSECOND_ADAPTER_HPP_
#define OPTFRAME_MOVEPAIRSECOND_ADAPTER_HPP_

// Framework includes
#include "../../Move.hpp"

using namespace std;

template<class T1, class T2, class M = OPTFRAME_DEFAULT_MEMORY>
class MovePairSecond: public Move<pair<T1, T2> , M>
{
protected:
	Move<T2, M>& m;

public:

	MovePairSecond(Move<T2, M>& _m) :
		m(_m)
	{
	}

	Move<T2, M>& get_m()
	{
		return m;
	}

	virtual ~MovePairSecond()
	{
		delete &m;
	}

	bool canBeApplied(const pair<T1, T2>& rep)
	{
		return m.canBeApplied(rep.second);
	}

	Move<pair<T1, T2> , M>& apply(pair<T1, T2>& rep)
	{
		return *new MovePairSecond<T1, T2, M> (m.apply(rep.second));
	}

	virtual bool operator==(const Move<pair<T1, T2> , M>& _m) const
	{
		const MovePairSecond<T1, T2, M>& m1 = (const MovePairSecond<T1, T2, M>&) _m;
		return m == m1.m;
	}

	void print() const
	{
		cout << "MovePairSecond: move = ";
		m.print();
	}
};

#endif /*OPTFRAME_MOVEPAIRSECOND_ADAPTER_HPP_*/
