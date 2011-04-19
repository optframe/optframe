#ifndef OPTFRAME_NSSEQUNIONADAPTER_HPP_
#define OPTFRAME_NSSEQUNIONADAPTER_HPP_

// Framework includes
//#include "../../../Move.hpp"
#include "../NSSeq.hpp"

#include "./Moves/MoveNSSeqUnion.hpp"
#include "./NSIterators/IteratorNSSeqUnion.hpp"

using namespace std;

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
