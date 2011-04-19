#ifndef OPTFRAME_NSSEQVVSWAPINTRA_HPP_
#define OPTFRAME_NSSEQVVSWAPINTRA_HPP_

// Framework includes
#include "../Move.hpp"
#include "../NSSeq.hpp"

#include "Moves/MoveVVSwapIntra.hpp"
#include "NSIterators/IteratorVVSwapIntra.hpp"

using namespace std;

// Working structure: vector<vector<T> >

template<class T, class M = OPTFRAME_DEFAULT_MEMORY>
class NSSeqVVSwapIntra: public NSSeq<vector<vector<T> > , M>
{
	typedef vector<vector<T> > Routes;

public:

	NSSeqVVSwapIntra()
	{
	}

	virtual ~NSSeqVVSwapIntra()
	{
	}

	Move<Routes, M>& move(const Routes& rep)
	{
		int i = rand() % rep.size();

		int n = rep.at(i).size();

		int j = -1;

		if (n > 0)
			j = rand() % n;

		int k = j;

		if (n > 1)
			while (k == j)
				k = rand() % n;

		return *new MoveVVSwapIntra<T, M> (i, j, k);
	}

	virtual NSIterator<Routes, M>& getIterator(const Routes& r)
	{
		return *new NSIteratorVVSwapIntra<T, M> (r);
	}

	virtual void print()
	{
		cout << "NSSeqVVSwapIntra" << endl;
	}

};

#endif /*OPTFRAME_NSSEQVVSWAPINTRA_HPP_*/
