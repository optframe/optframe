#ifndef PN_NSSEQSwap_H_
#define PN_NSSEQSwap_H_

// Framework includes
#include "../../OptFrame/NSSeq.hpp"
#include "../../OptFrame/RandGen.hpp"

// Own includes
#include "ProblemInstance.h"
#include "DeltaStructure.h"
#include "Solution.h"

using namespace std;

namespace PN
{

class MoveSwap: public Move< RepPN , MY_ADS  >
{
private:
    // MOVE PARAMETERS
    int i, j;
    ProblemInstance& pPN;

public:
    using Move< RepPN , MY_ADS  >::apply; // prevents name hiding
    using Move< RepPN , MY_ADS  >::canBeApplied; // prevents name hiding

    MoveSwap(int _i, int _j, ProblemInstance& _pPN): i(_i), j(_j), pPN(_pPN)
    {
    }

    virtual ~MoveSwap()
    {
    }
    
    void print() const
    {
        cout << id() << " with params: '" << i << "," << j << endl;
    }
    
    string id() const
    {
        return Move<RepPN , MY_ADS >::idComponent().append(":MoveSwap");
    }
    
    bool operator==(const Move< RepPN , MY_ADS  >& _m) const
    {
        const MoveSwap& m = (const MoveSwap&) _m;
        return (i == m.i) && (j == m.j);
    }
    
    // Implement these methods in the .cpp file
    
    bool canBeApplied(const RepPN& rep, const MY_ADS&);

    Move< RepPN , MY_ADS  >& apply(RepPN& rep, MY_ADS&);
    
    MoveCost* cost(const Evaluation<  >&, const RepPN& rep, const MY_ADS& ads);
};



class NSIteratorSwap: public NSIterator< RepPN , MY_ADS  >
{
private:
    // ITERATOR PARAMETERS
    int i,j;
    const RepPN& rep;
    ProblemInstance& pPN;

public:
    NSIteratorSwap(ProblemInstance& _pPN, const RepPN& _rep) :
		rep(_rep), pPN(_pPN)
    {
    }

    virtual ~NSIteratorSwap()
    {
    }
    
    // Implement these methods in the .cpp file

    void first();
    void next();
    bool isDone();
    Move< RepPN , MY_ADS  >& current();
};



class NSSeqSwap: public NSSeq< RepPN , MY_ADS  >
{
private:
    // YOU MAY REMOVE THESE PARAMETERS IF YOU DON'T NEED (BUT PROBABLY WILL...)
    ProblemInstance& pPN; // problem instance data
    RandGen& rg;                // random number generator

public:

    using NSSeq< RepPN , MY_ADS  >::move; // prevents name hiding

    // YOU MAY REMOVE THESE PARAMETERS IF YOU DON'T NEED (BUT PROBABLY WILL...)
    NSSeqSwap(ProblemInstance& _pPN, RandGen& _rg):
        pPN(_pPN), rg(_rg)
    {
    }

    virtual ~NSSeqSwap()
    {
    }
    
    void print() const
    {
        cout << "NSSeqSwap" << endl;
    }
    
    string id() const
    {
        return NSSeq<RepPN , MY_ADS >::idComponent().append(":NSSeqSwap");
    }
    
    NSIterator<RepPN , MY_ADS >& getIterator(const RepPN& rep, const MY_ADS&)
    {
        // return an iterator to the neighbors of 'rep' 
        return * new NSIteratorSwap(pPN, rep);  // ADD POSSIBLE ITERATOR PARAMETERS
    }
        
    // Implement this method in the .cpp file

    Move<RepPN , MY_ADS >& move(const RepPN& rep, const MY_ADS&);

    Move<RepPN , MY_ADS >* validMove(const RepPN& rep, const MY_ADS& ads)
    {
	for(unsigned i=0; i<100; i++)
	{
		Move<RepPN , MY_ADS >& m = move(rep, ads);
		if(m.canBeApplied(rep, ads))
			return &m;
		else
			delete &m;
	}
	return NULL;
    }

};

}

#endif /*PN_NSSEQSwap_H_*/
