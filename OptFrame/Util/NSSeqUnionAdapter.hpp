#ifndef OPTFRAME_NSSEQUNIONADAPTER_HPP_
#define OPTFRAME_NSSEQUNIONADAPTER_HPP_

// Framework includes
#include "../Move.hpp"
#include "../NSSeq.hpp"

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

class NSSeqUnionAdapterOutOfBound
{
public:
	NSSeqUnionAdapterOutOfBound()
	{
	}
};

template<class R, class M = OPTFRAME_DEFAULT_MEMORY, class MOVE = MoveNSSeqUnion<R, M> >
class IteratorNSSeqUnion: public NSIterator<R, M>
{
private:
	NSIterator<R, M>& i1;
	NSIterator<R, M>& i2;

public:

	IteratorNSSeqUnion(NSIterator<R, M>& _i1, NSIterator<R, M>& _i2) :
		i1(_i1), i2(_i2)
	{
	}

	virtual ~IteratorNSSeqUnion()
	{
		delete &i1;
		delete &i2;
	}

	void first()
	{
		i1.first();
		i2.first();
	}

	void next()
	{
		if (!i1.isDone())
			i1.next();
		else
			i2.next();
	}

	bool isDone()
	{
		return i1.isDone() && i2.isDone();
	}

	Move<R, M>& current()
	{
		if (!i1.isDone())
			return *new MOVE(0, i1.current());
		else if (!i2.isDone())
			return *new MOVE(1, i2.current());
		else
			throw NSSeqUnionAdapterOutOfBound();
	}
};

template<class R, class M = OPTFRAME_DEFAULT_MEMORY, class MOVE = MoveNSSeqUnion<R, M> >
class NSSeqUnionAdapter: public NSSeq<R, M>
{
private:
	NSSeq<R, M>& n1;
	NSSeq<R, M>& n2;

public:

	NSSeqUnionAdapter(NSSeq<R, M>& _n1, NSSeq<R, M>& _n2) :
		n1(_n1), n2(_n2)
	{
	}

	virtual ~NSSeqUnionAdapter()
	{
	}

	Move<R, M>& move(const R& r)
	{
		int x = rand() % 2;

		if (x == 0)
			return *new MOVE(0, n1.move(r));
		else
			return *new MOVE(1, n2.move(r));
	}

	virtual NSIterator<R, M>& getIterator(const R& r)
	{
		return *new IteratorNSSeqUnion<R, M, MOVE> (n1.getIterator(r), n2.getIterator(r));
	}

	virtual void print()
	{
		cout << "NSSeqUnionAdapter {" << endl;
		n1.print();
		n2.print();
		cout << "}" << endl;
	}
};

#endif /*OPTFRAME_NSSEQUNIONADAPTER_HPP_*/
