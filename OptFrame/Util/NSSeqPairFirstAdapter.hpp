#ifndef OPTFRAME_NSSEQ_PAIRFIRST_ADAPTER_HPP_
#define OPTFRAME_NSSEQ_PAIRFIRST_ADAPTER_HPP_

// Framework includes
#include "../Move.hpp"
#include "../NSSeq.hpp"

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

	void print()
	{
		cout << "MovePairFirst: move = ";
		m.print();
	}
};

class PairFirstAdapterIteratorOutOfBound
{
public:
	PairFirstAdapterIteratorOutOfBound()
	{
	}
};

template<class T1, class T2, class M = OPTFRAME_DEFAULT_MEMORY, class MOVE = MovePairFirst<T1, T2, M> >
class IteratorNSSeqPairFirst: public NSIterator<pair<T1, T2> , M>
{
private:
	NSIterator<T1, M>& iterator;

public:

	IteratorNSSeqPairFirst(NSIterator<T1, M>& it) :
		iterator(it)
	{
	}

	virtual ~IteratorNSSeqPairFirst()
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
			throw PairFirstAdapterIteratorOutOfBound();
	}
};

template<class T1, class T2, class M = OPTFRAME_DEFAULT_MEMORY, class MOVE = MovePairFirst<T1, T2, M> >
class NSSeqPairFirstAdapter: public NSSeq<pair<T1, T2> , M>
{
protected:
	NSSeq<T1, M>& ns;

public:

	NSSeqPairFirstAdapter(NSSeq<T1, M>& _ns) :
		ns(_ns)
	{
	}

	virtual ~NSSeqPairFirstAdapter()
	{
	}

	Move<pair<T1, T2> , M>& move(const pair<T1, T2>& r)
	{
		return *new MOVE(ns.move(r.first));
	}

	virtual NSIterator<pair<T1, T2> , M>& getIterator(const pair<T1, T2>& r)
	{
		return *new IteratorNSSeqPairFirst<T1, T2, M, MOVE> (ns.getIterator(r.first));
	}

	virtual void print()
	{
		cout << "NSSeqPairFirstAdapter {" << endl;
		ns.print();
		cout << "}" << endl;
	}
};

#endif /*OPTFRAME_NSSEQ_PAIRFIRST_ADAPTER_HPP_*/
