#ifndef OPTFRAME_NSSEQ_PAIRSECOND_ADAPTER_HPP_
#define OPTFRAME_NSSEQ_PAIRSECOND_ADAPTER_HPP_

// Framework includes
#include "../Move.hpp"
#include "../NSSeq.hpp"

#include "Moves/MovePairSecond.hpp"
#include "NSIterators/IteratorNSSeqPairSecond.hpp"

using namespace std;

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
