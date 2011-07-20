#ifndef $project_NSSEQ$neighborhood_HPP_
#define $project_NSSEQ$neighborhood_HPP_

// Framework includes
#include "../../OptFrame/NSSeq.hpp"

// Own includes
#include "ProblemInstance.hpp"
#include "Memory.h"
#include "Solution.h"

using namespace std;

class Move$neighborhood: public Move< Rep$project $commamproject >
{
public:

	using Move< Rep$project $commamproject >::apply; // prevents name hiding
	using Move< Rep$project $commamproject >::canBeApplied; // prevents name hiding

	Move$neighborhood()
	{
	}

	virtual ~Move$neighborhood()
	{
	}

	bool canBeApplied(const Rep$project& rep)
	{
		return true;
	}

	Move< Rep$project $commamproject >& apply(Rep$project& rep)
	{
		// apply this move to 'rep'
		// rep. (...) = (...)
		// return reverse move
		return * new Move$neighborhood; 
	}

	virtual bool operator==(const Move< Rep$project $commamproject >& _m) const
	{
		const Move$neighborhood& m = (const Move$neighborhood&) _m;
		return false;
	}

	void print() const
	{
	}
};

class NSIterator$neighborhood: public NSIterator< Rep$project $commamproject >
{
public:
	NSIterator$neighborhood()
	{
	}

	virtual ~NSIterator$neighborhood()
	{
	}

	virtual void first(){};
	virtual void next(){};
	virtual bool isDone(){};
	virtual Move< Rep$project $commamproject >& current(){};
};

class NSSeq$neighborhood: public NSSeq< Rep$project $commamproject >
{
public:

	using NSSeq< Rep$project $commamproject >::move; // prevents name hiding

	NSSeq$neighborhood()
	{
	}

	virtual ~NSSeq$neighborhood()
	{
	}

	virtual Move<Rep$project $commamproject>& move(const Rep$project& rep)
	{
		return * new Move$neighborhood; // return a random move
	}

	virtual NSIterator<Rep$project $commamproject>& getIterator(const Rep$project& rep)
	{
		return * new NSIterator$neighborhood; // return an iterator to the neighbors of 'rep' 
	}

	virtual void print()
	{
	}
};

#endif /*$project_NSSEQ$neighborhood_HPP_*/

