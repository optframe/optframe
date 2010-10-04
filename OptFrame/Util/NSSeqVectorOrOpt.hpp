#ifndef OPTFRAME_NSSEQVECTOROROPT_HPP_
#define OPTFRAME_NSSEQVECTOROROPT_HPP_

// Framework includes
#include "../Move.hpp"
#include "../NSSeq.hpp"

#include "NSSeqVectorOrOptk.hpp"
#include "NSSeqUnionAdapter.hpp"

using namespace std;

// definition: OrOpt is OrOpt{1..3}

template<class T, class M = OPTFRAME_DEFAULT_MEMORY>
class NSSeqVectorOrOpt: public NSSeq<vector<T> , M>
{
	typedef vector<T> Route;
	NSSeq<Route, M>* ns;

	NSSeqVectorOrOptk<T, M>* OrOpt1;
	NSSeqVectorOrOptk<T, M>* OrOpt2;
	NSSeqVectorOrOptk<T, M>* OrOpt3;

	NSSeqUnionAdapter<vector<T>, M>* OrOpt2_3;
	NSSeqUnionAdapter<vector<T>, M>* OrOpt1_2_3;
public:

	NSSeqVectorOrOpt()
	{
		OrOpt1 = new NSSeqVectorOrOptk<T, M> (1);
		OrOpt2 = new NSSeqVectorOrOptk<T, M> (2);
		OrOpt3 = new NSSeqVectorOrOptk<T, M> (3);

		OrOpt2_3 = new NSSeqUnionAdapter<vector<T>, M> (*OrOpt2, *OrOpt3);
		OrOpt1_2_3 = new NSSeqUnionAdapter<vector<T>, M> (*OrOpt1, *OrOpt2_3);
	}

	virtual ~NSSeqVectorOrOpt()
	{
		delete OrOpt1;
		delete OrOpt2;
		delete OrOpt3;

		delete OrOpt2_3;
		delete OrOpt1_2_3;
	}

	Move<Route, M>& move(const Route& rep)
	{
		return OrOpt1_2_3->move(rep);
	}

	virtual NSIterator<Route, M>& getIterator(const Route& rep)
	{
		return OrOpt1_2_3->getIterator(rep);
	}

	virtual void print()
	{
		cout << "NSSeqVectorOrOpt{1..3}" << endl;
	}
};

#endif /*OPTFRAME_NSSEQVECTOROROPT_HPP_*/
