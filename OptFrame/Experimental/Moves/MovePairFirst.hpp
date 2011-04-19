#ifndef OPTFRAME_MOVEPAIRFIRST_ADAPTER_HPP_
#define OPTFRAME_MOVEPAIRFIRST_ADAPTER_HPP_

// Framework includes
#include "../../Move.hpp"

using namespace std;

template<class T1, class T2, class M = OPTFRAME_DEFAULT_MEMORY>
class MovePairFirst: public Move<pair<T1, T2> , M>
{
protected:
	Move<T1, M>& m;

public:

	MovePairFirst(Move<T1, M>& _m) :
		m(_m)
	{
	}

	Move<T1, M>& get_m()
	{
		return m;
	}

	virtual ~MovePairFirst()
	{
		delete &m;
	}

	bool canBeApplied(const pair<T1, T2>& rep)
	{
		return m.canBeApplied(rep.first);
	}

	Move<pair<T1, T2> , M>& apply(pair<T1, T2>& rep)
	{
		return *new MovePairFirst<T1, T2, M> (m.apply(rep.first));
	}

	virtual bool operator==(const Move<pair<T1, T2> , M>& _m) const
	{
		const MovePairFirst<T1, T2, M>& m1 = (const MovePairFirst<T1, T2, M>&) _m;
		return m == m1.m;
	}

	void print() const
	{
		cout << "MovePairFirst: move = ";
		m.print();
	}
};

#endif /*OPTFRAME_MOVEPAIRFIRST_ADAPTER_HPP_*/
