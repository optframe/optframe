#ifndef OPTFRAME_NSSEQ_PAIRSECOND_ADAPTER_HPP_
#define OPTFRAME_NSSEQ_PAIRSECOND_ADAPTER_HPP_

// Framework includes
#include "../Move.hpp"
#include "../NSSeq.hpp"

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

class PairSecondAdapterIteratorOutOfBound
{
public:
	PairSecondAdapterIteratorOutOfBound()
	{
	}
};

template<class T1, class T2, class M = OPTFRAME_DEFAULT_MEMORY, class MOVE = MovePairSecond<T1, T2, M> >
class IteratorNSSeqPairSecond: public NSIterator<pair<T1, T2> , M>
{
private:
	NSIterator<T2, M>& iterator;

public:

	IteratorNSSeqPairSecond(NSIterator<T2, M>& it) :
		iterator(it)
	{
	}

	virtual ~IteratorNSSeqPairSecond()
	{
		delete &iterator;
	}

	void first()
	{
		iterator.first();
	}

	void next()
	{
		iterator.next();
	}

	bool isDone()
	{
		return iterator.isDone();
	}

	Move<pair<T1, T2> , M>& current()
	{

		if (!iterator.isDone())
			return *new MOVE(iterator.current());
		else
			throw PairSecondAdapterIteratorOutOfBound();
	}
};

template<class T1, class T2, class M = OPTFRAME_DEFAULT_MEMORY, class MOVE = MovePairSecond<T1, T2, M> >
class NSSeqPairSecondAdapter: public NSSeq<pair<T1, T2> , M>
{
protected:
	NSSeq<T2, M>& ns;

public:

	NSSeqPairSecondAdapter(NSSeq<T2, M>& _ns) :
		ns(_ns)
	{
	}

	virtual ~NSSeqPairSecondAdapter()
	{
	}

	Move<pair<T1, T2> , M>& move(const pair<T1, T2>& r)
	{
		return *new MOVE(ns.move(r.second));
	}

	virtual NSIterator<pair<T1, T2> , M>& getIterator(const pair<T1, T2>& r)
	{
		return *new IteratorNSSeqPairSecond<T1, T2, M, MOVE> (ns.getIterator(r.second));
	}

	virtual void print()
	{
		cout << "NSSeqPairSecondAdapter {" << endl;
		ns.print();
		cout << "}" << endl;
	}
};

#endif /*OPTFRAME_NSSEQ_PAIRSECOND_ADAPTER_HPP_*/
