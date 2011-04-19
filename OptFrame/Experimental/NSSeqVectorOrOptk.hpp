#ifndef OPTFRAME_NSSEQVECTOROROPTK_HPP_
#define OPTFRAME_NSSEQVECTOROROPTK_HPP_

// Framework includes
#include "../NSSeq.hpp"

#include "Moves/MoveOrOptk.hpp"
#include "NSIterators/IteratorOrOptk.hpp"

using namespace std;

// Working structure: vector<vector<T> >

template<class T, class M = OPTFRAME_DEFAULT_MEMORY>
class NSSeqVectorOrOptk: public NSSeq<vector<T> , M>
{
	typedef vector<T> Route;
	int k;

public:

	NSSeqVectorOrOptk(int _k) :
		k(_k)
	{
	}

	virtual ~NSSeqVectorOrOptk()
	{
	}

	Move<Route, M>& move(const Route& rep)
	{
		int n = rep.size();

		if (n - k <= 0)
			return *new MoveOrOptk<T, M> (0, 0, k);

		int i = rand() % (n - k + 1);

		int j = i;
		while (i == j)
			j = rand() % (n - k + 1);

		return *new MoveOrOptk<T, M> (i, j, k);
	}

	virtual NSIterator<Route, M>& getIterator(const Route& r)
	{
		return *new NSIteratorOrOptk<T, M> (r.size(), k);
	}

	virtual void print()
	{
		cout << "NSSeqVectorOrOpt{K=" << k << "}" << endl;
	}
};

#endif /*OPTFRAME_NSSEQVECTOROROPTK_HPP_*/
