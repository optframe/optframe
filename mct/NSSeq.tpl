#ifndef $project_NSSEQ$neighborhood_H_
#define $project_NSSEQ$neighborhood_H_

// Framework includes
#include "../../OptFrame/NSSeq.hpp"

// Own includes
#include "ProblemInstance.h"
#include "DeltaStructure.h"
#include "Solution.h"

using namespace std;

namespace $project
{

class Move$neighborhood: public Move< Rep$project , MY_ADS $commadproject >
{
public:

	using Move< Rep$project , MY_ADS $commadproject >::apply; // prevents name hiding
	using Move< Rep$project , MY_ADS $commadproject >::canBeApplied; // prevents name hiding

	Move$neighborhood();

	virtual ~Move$neighborhood();

	bool canBeApplied(const Rep$project& rep, const MY_ADS&);

	Move< Rep$project , MY_ADS $commadproject >& apply(Rep$project& rep, MY_ADS&);

	virtual bool operator==(const Move< Rep$project , MY_ADS $commadproject >& _m) const;
	
	void print() const;

	string id() const;
};

class NSIterator$neighborhood: public NSIterator< Rep$project , MY_ADS $commadproject >
{
public:
	NSIterator$neighborhood();

	virtual ~NSIterator$neighborhood();

	virtual void first();
	virtual void next();
	virtual bool isDone();
	virtual Move< Rep$project , MY_ADS $commadproject >& current();
};

class NSSeq$neighborhood: public NSSeq< Rep$project , MY_ADS $commadproject >
{
public:

	using NSSeq< Rep$project , MY_ADS $commadproject >::move; // prevents name hiding

	NSSeq$neighborhood();

	virtual ~NSSeq$neighborhood();

	virtual Move<Rep$project , MY_ADS $commadproject>& move(const Rep$project& rep, const MY_ADS&);
	
	virtual NSIterator<Rep$project , MY_ADS $commadproject>& getIterator(const Rep$project& rep, const MY_ADS&);
	
	virtual void print() const;
};

}

#endif /*$project_NSSEQ$neighborhood_H_*/
