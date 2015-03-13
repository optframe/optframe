#ifndef SCP_NSSEQNSTest_H_
#define SCP_NSSEQNSTest_H_

// Framework includes
#include "../../Examples/SCP/DeltaStructure.h"
#include "../../Examples/SCP/ProblemInstance.h"
#include "../../Examples/SCP/Solution.h"
#include "../../OptFrame/NSSeq.hpp"
#include "../../OptFrame/RandGen.hpp"

#include "../../OptFrame/SetInspired/Adapter.hpp"

// Own includes

using namespace std;

// TEST FOR SUBSET NEIGHBORHOODS

namespace SCP
{

class MoveNSTest: public Move<RepSCP, MY_ADS>
{
private:
	SubsetAdapter<RepSCP>& adapter;  // USE THE ADAPTER TO UPDATE THE CORRECT PART OF RepSCP!

public:
	using Move<RepSCP, MY_ADS>::apply; // prevents name hiding
	using Move<RepSCP, MY_ADS>::canBeApplied; // prevents name hiding

	MoveNSTest(SubsetAdapter<RepSCP>& _adapter) :
			adapter(_adapter)
	{
	}

	virtual ~MoveNSTest()
	{
	}

	void print() const
	{
		cout << id() << " with params: '" << "ADD MY PARAMETERS" << "'" << endl;
	}

	string id() const
	{
		return Move<RepSCP, MY_ADS>::idComponent().append(":MoveNSTest");
	}

	bool operator==(const Move<RepSCP, MY_ADS>& _m) const
	{
		const MoveNSTest& m = (const MoveNSTest&) _m;
		// COMPARE PARAMETERS AND RETURN TRUE IF EQUALS
		return false;
	}

	// Implement these methods in the .cpp file

	bool canBeApplied(const RepSCP& rep, const MY_ADS&);

	Move<RepSCP, MY_ADS>& apply(RepSCP& rep, MY_ADS&);

	MoveCost* cost(const Evaluation<>&, const RepSCP& rep, const MY_ADS& ads);
};

class NSIteratorNSTest: public NSIterator<RepSCP, MY_ADS>
{
private:
	// ITERATOR PARAMETERS

public:
	NSIteratorNSTest() // ADD ITERATOR PARAMETERS
	{
	}

	virtual ~NSIteratorNSTest()
	{
	}

	// Implement these methods in the .cpp file

	void first();
	void next();
	bool isDone();
	Move<RepSCP, MY_ADS>& current();
};

// TEST FOR SUBSET NEIGHBORHOODS

template<class MOVE = MoveNSTest<RepSCP, MY_ADS> >
class NSSeqNSTest: public NSSeq<RepSCP, MY_ADS, MOVE>
{
private:
	ProblemInstance& pSCP; // problem instance data
	RandGen& rg;                // random number generator
	SubsetAdapter<RepSCP>& adapter;

public:

	using NSSeq<RepSCP, MY_ADS>::move; // prevents name hiding

	NSSeqNSTest(ProblemInstance& _pSCP, RandGen& _rg, SubsetAdapter<RepSCP>& _adapter) :
			pSCP(_pSCP), rg(_rg), adapter(_adapter)
	{
	}

	virtual ~NSSeqNSTest()
	{
	}

	void print() const
	{
		cout << "NSSeqNSTest" << endl;
	}

	string id() const
	{
		return NSSeq<RepSCP, MY_ADS>::idComponent().append(":NSSeqNSTest");
	}

	NSIterator<RepSCP, MY_ADS>& getIterator(const RepSCP& rep, const MY_ADS&)
	{
		// return an iterator to the neighbors of 'rep'
		return *new NSIteratorNSTest;  // ADD POSSIBLE ITERATOR PARAMETERS
	}

	// Implement this method in the .cpp file

	Move<RepSCP, MY_ADS>& move(const RepSCP& rep, const MY_ADS& ads)
	{
		return moveSubset(adapter.adapt(rep), ads);
	}

	Move<RepSCP, MY_ADS>& moveSubset(const Subset& sub, const MY_ADS& ads)
	{
		return *new MOVE(adapter);
	}
};

}

#endif /*SCP_NSSEQNSTest_H_*/
