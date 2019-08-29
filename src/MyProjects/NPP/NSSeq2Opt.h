#ifndef NPP_NSSEQ2Opt_H_
#define NPP_NSSEQ2Opt_H_

// Framework includes
#include "../../OptFrame/NSSeq.hpp"
#include "../../OptFrame/RandGen.hpp"

// Own includes
#include "ProblemInstance.h"
#include "Representation.h"

using namespace std;

namespace NPP
{

class Move2Opt: public Move< RepNPP >
{
private:
    int i, j;

public:

    Move2Opt(int _i, int _j) : i(_i), j(_j) // ADD PARAMETERS
    {
    }

    virtual ~Move2Opt()
    {
    }
    
    void print() const
    {
        cout << id() << " with params: '" << i << ", " << j << "'" << endl;
    }
    
    string id() const
    {
        return Move<RepNPP >::idComponent().append(":Move2Opt");
    }
    
    bool operator==(const Move< RepNPP >& _m) const
    {
        const Move2Opt& m = (const Move2Opt&) _m;
        // COMPARE PARAMETERS AND RETURN TRUE IF EQUALS
        return this->i == m.i && this->j == m.j;
    }
    
    // Implement these methods in the .cpp file
    
    bool canBeApplied(const RepNPP& rep, const OptFrameADS* ads);

    Move< RepNPP >* apply(RepNPP& rep, OptFrameADS* ads);
    
    MoveCost* cost(const Evaluation&, const RepNPP& rep, const OptFrameADS* ads, bool allowEstimated);
};



class NSIterator2Opt: public NSIterator< RepNPP >
{
private:
    // ITERATOR PARAMETERS
    ProblemInstance& pNPP;
    int i, j;

public:
    NSIterator2Opt(ProblemInstance& _pNPP) : pNPP(_pNPP) // ADD ITERATOR PARAMETERS
    {
    }

    virtual ~NSIterator2Opt()
    {
    }
    
    // Implement these methods in the .cpp file

    void first();
    void next();
    bool isDone();
    Move< RepNPP >* current();
};



class NSSeq2Opt: public NSSeq< RepNPP >
{
private:
    // YOU MAY REMOVE THESE PARAMETERS IF YOU DON'T NEED (BUT PROBABLY WILL...)
    ProblemInstance& pNPP; // problem instance data
    RandGen& rg;                // random number generator

public:

    // YOU MAY REMOVE THESE PARAMETERS IF YOU DON'T NEED (BUT PROBABLY WILL...)
    NSSeq2Opt(ProblemInstance& _pNPP, RandGen& _rg):
        pNPP(_pNPP), rg(_rg)
    {
    }

    virtual ~NSSeq2Opt()
    {
    }
    
    void print() const
    {
        cout << "NSSeq2Opt" << endl;
    }
    
    string id() const
    {
        return NSSeq<RepNPP >::idComponent().append(":NSSeq2Opt");
    }
    
    NSIterator<RepNPP >* getIterator(const RepNPP& rep, const OptFrameADS* ads)
    {
        // return an iterator to the neighbors of 'rep' 
        return new NSIterator2Opt(pNPP);  // ADD POSSIBLE ITERATOR PARAMETERS
    }
        
    // Implement this method in the .cpp file

    Move<RepNPP >* randomMove(const RepNPP& rep, const OptFrameADS* ads);
    
    // Move<RepNPP >* validRandomMove(const RepNPP& rep, const OptFrameADS* ads);
};

}

#endif /*NPP_NSSEQ2Opt_H_*/
