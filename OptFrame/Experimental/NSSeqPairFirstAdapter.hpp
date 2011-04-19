#ifndef OPTFRAME_NSSEQ_PAIRFIRST_ADAPTER_HPP_
#define OPTFRAME_NSSEQ_PAIRFIRST_ADAPTER_HPP_

// Framework includes
#include "../Move.hpp"
#include "../NSSeq.hpp"

#include "Moves/MovePairFirst.hpp"
#include "NSIterators/IteratorNSSeqPairFirst.hpp"


using namespace std;

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
