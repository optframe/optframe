#ifndef OPTFRAME_NSSEQTSPSWAP_HPP_
#define OPTFRAME_NSSEQTSPSWAP_HPP_

/*
 Classical Problem: Traveling Salesman Problem

 The Classic Swap Neighborhood Structure.

 Article:

 Swap is applied for any problem that representation is like a vector<T>, where T is the type of the vector.

 e.g: vector<T> where type of T is int
 s means Solution

 initial s: 1 2 3 4 5 6 7 8

 s' after apply MoveTSPSwap(2,5) in s

 s': 1 2 6 4 5 3 7 8

 */

// Framework includes
#include "../Move.hpp"
#include "../NSSeq.hpp"

#include "Moves/MoveTSPSwap.hpp"
#include "NSIterators/IteratorTSPSwap.hpp"

using namespace std;

// Working structure: vector<T>

template<class T, class M = OPTFRAME_DEFAULT_MEMORY, class MOVE = MoveTSPSwap<T, M> >
class NSSeqTSPSwap: public NSSeq<vector<T> , M>
{
	typedef vector<T> Route;

private:

public:

	NSSeqTSPSwap()
	{
	}

	virtual ~NSSeqTSPSwap()
	{
	}

	Move<Route, M>& move(const Route& rep)
	{
		if (rep.size() < 2)
			return *new MOVE(-1, -1);

		int p1 = rand() % rep.size();

		int p2 = p1;

		while (p2 == p1)
			p2 = rand() % rep.size();

		return *new MOVE(p1, p2);
	}

	virtual NSIterator<Route, M>& getIterator(const Route& r)
	{
		return *new NSIteratorTSPSwap<T, M, MOVE> (r.size());
	}

	virtual void print()
	{
		cout << "NSSeqTSPSwap" << endl;
	}
};

#endif /*OPTFRAME_NSSEQTSPSWAP_HPP_*/
