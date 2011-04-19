#ifndef OPTFRAME_MOVENSSEQUNION_HPP_
#define OPTFRAME_MOVENSSEQUNION_HPP_

// Framework includes
#include "../../Move.hpp"

using namespace std;

template<class R, class M = OPTFRAME_DEFAULT_MEMORY>
class MoveNSSeqUnion: public Move<R, M>
{
protected:
	int id; // NS 1 or NS 2
	Move<R, M>& m;

public:

	MoveNSSeqUnion(int _id, Move<R, M>& _m) :
		id(_id), m(_m)
	{
	}

	int get_id()
	{
		return id;
	}

	Move<R, M>& get_m()
	{
		return m;
	}

	virtual ~MoveNSSeqUnion()
	{
		delete &m;
	}

	bool canBeApplied(const R& r)
	{
		return m.canBeApplied(r);
	}

	Move<R, M>& apply(R& r)
	{
		return *new MoveNSSeqUnion<R, M> (id, m.apply(r));
	}

	Move<R, M>& apply(M& mem, R& rep)
	{
		return *new MoveNSSeqUnion<R, M> (id, m.apply(mem, rep));
	}

	virtual bool operator==(const Move<R, M>& _m) const
	{
		const MoveNSSeqUnion<R, M>& m1 = (const MoveNSSeqUnion<R, M>&) _m;
		if (id == m1.id)
			return m == m1.m;
		else
			return false;
	}

	void print() const
	{
		cout << "MoveNSSeqUnion: id=" << id << "; move = ";
		m.print();
	}
};

#endif /*OPTFRAME_MOVENSSEQUNION_HPP_*/
