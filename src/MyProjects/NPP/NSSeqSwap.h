#ifndef NPP_NSSEQSwap_H_
#define NPP_NSSEQSwap_H_

// Framework includes
#include "../../OptFrame/NSSeq.hpp"
#include "../../OptFrame/RandGen.hpp"

// Own includes
#include "ProblemInstance.h"
#include "Representation.h"

using namespace std;

namespace NPP
{

class MoveSwap: public Move< RepNPP >
{
private:
    int i, j;

public:

    MoveSwap(int _i, int _j) : i(_i), j(_j) // ADD PARAMETERS
    {
    }

    virtual ~MoveSwap()
    {
    }
    
    void print() const
    {
        cout << id() << " with params: '" << i << ", " << j << "'" << endl;
    }
    
    string id() const
    {
        return Move<RepNPP >::idComponent().append(":MoveSwap");
    }
    
    bool operator==(const Move< RepNPP >& _m) const
    {
        const MoveSwap& m = (const MoveSwap&) _m;
        // COMPARE PARAMETERS AND RETURN TRUE IF EQUALS
        return this->i == m.i && this->j == m.j;
    }
    
    // Implement these methods in the .cpp file
    
    bool canBeApplied(const RepNPP& rep, const OptFrameADS* ads);

    Move< RepNPP >* apply(RepNPP& rep, OptFrameADS* ads);
    
    MoveCost* cost(const Evaluation&, const RepNPP& rep, const OptFrameADS* ads, bool allowEstimated);
};



class NSIteratorSwap: public NSIterator< RepNPP >
{
private:
    // ITERATOR PARAMETERS
    ProblemInstance& pNPP;
    int i, j;

public:
    NSIteratorSwap(ProblemInstance& _pNPP) : pNPP(_pNPP) // ADD ITERATOR PARAMETERS
    {
    }

    virtual ~NSIteratorSwap()
    {
    }
    
    // Implement these methods in the .cpp file

    void first();
    void next();
    bool isDone();
    Move< RepNPP >* current();
};



class NSSeqSwap: public NSSeq< RepNPP >
{
private:
    // YOU MAY REMOVE THESE PARAMETERS IF YOU DON'T NEED (BUT PROBABLY WILL...)
    ProblemInstance& pNPP; // problem instance data
    RandGen& rg;                // random number generator

public:

    // YOU MAY REMOVE THESE PARAMETERS IF YOU DON'T NEED (BUT PROBABLY WILL...)
    NSSeqSwap(ProblemInstance& _pNPP, RandGen& _rg):
        pNPP(_pNPP), rg(_rg)
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
        return NSSeq<RepNPP >::idComponent().append(":NSSeqSwap");
    }
    
    NSIterator<RepNPP >* getIterator(const RepNPP& rep, const OptFrameADS* ads)
    {
        // return an iterator to the neighbors of 'rep' 
        return new NSIteratorSwap(pNPP);  // ADD POSSIBLE ITERATOR PARAMETERS
    }
        
    // Implement this method in the .cpp file

    Move<RepNPP >* randomMove(const RepNPP& rep, const OptFrameADS* ads);
    
    // Move<RepNPP >* validRandomMove(const RepNPP& rep, const OptFrameADS* ads);
};

}

#endif /*NPP_NSSEQSwap_H_*/
