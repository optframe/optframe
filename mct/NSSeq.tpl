#ifndef $project_NSSEQ$neighborhood_HPP_
#define $project_NSSEQ$neighborhood_HPP_

// Framework includes
#include "../../OptFrame/NSSeq.hpp"

// Own includes
#include "ProblemInstance.hpp"
#include "DeltaStructure.h"
#include "Solution.h"

using namespace std;

class Move$neighborhood: public Move< Rep$project , OPTFRAME_DEFAULT_ADS $commadproject >
{
public:

	using Move< Rep$project , OPTFRAME_DEFAULT_ADS $commadproject >::apply; // prevents name hiding
	using Move< Rep$project , OPTFRAME_DEFAULT_ADS $commadproject >::canBeApplied; // prevents name hiding

	Move$neighborhood()
	{
	}

	virtual ~Move$neighborhood()
	{
	}

	bool canBeApplied(const Rep$project& rep, const OPTFRAME_DEFAULT_ADS&)
	{
		return true;
	}

	Move< Rep$project , OPTFRAME_DEFAULT_ADS $commadproject >& apply(Rep$project& rep, OPTFRAME_DEFAULT_ADS&)
	{
		// apply this move to 'rep'
		// rep. (...) = (...)
		// return reverse move
		return * new Move$neighborhood; 
	}

	virtual bool operator==(const Move< Rep$project , OPTFRAME_DEFAULT_ADS $commadproject >& _m) const
	{
		const Move$neighborhood& m = (const Move$neighborhood&) _m;
		return false;
	}

	void print() const
	{
		cout << id() << "with params: '" << "" << "'" << endl;
	}

	string id() const
	{
		return Move<Rep$project , OPTFRAME_DEFAULT_ADS $commadproject>::idComponent().append(":Move$neighborhood");
	}
};

class NSIterator$neighborhood: public NSIterator< Rep$project , OPTFRAME_DEFAULT_ADS $commadproject >
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
	virtual Move< Rep$project , OPTFRAME_DEFAULT_ADS $commadproject >& current(){};
};

class NSSeq$neighborhood: public NSSeq< Rep$project , OPTFRAME_DEFAULT_ADS $commadproject >
{
public:

	using NSSeq< Rep$project , OPTFRAME_DEFAULT_ADS $commadproject >::move; // prevents name hiding

	NSSeq$neighborhood()
	{
	}

	virtual ~NSSeq$neighborhood()
	{
	}

	virtual Move<Rep$project , OPTFRAME_DEFAULT_ADS $commadproject>& move(const Rep$project& rep, const OPTFRAME_DEFAULT_ADS&)
	{
		return * new Move$neighborhood; // return a random move
	}

	virtual NSIterator<Rep$project , OPTFRAME_DEFAULT_ADS $commadproject>& getIterator(const Rep$project& rep, const OPTFRAME_DEFAULT_ADS&)
	{
		return * new NSIterator$neighborhood; // return an iterator to the neighbors of 'rep' 
	}

	virtual void print() const
	{
	}
};

#endif /*$project_NSSEQ$neighborhood_HPP_*/

