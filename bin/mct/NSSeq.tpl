#pragma once

#ifndef $project_NSSEQ$neighborhood_H_
#define $project_NSSEQ$neighborhood_H_

// Framework includes
#include "../../OptFrame/Move.hpp"
#include "../../OptFrame/MoveCost.hpp"
#include "../../OptFrame/NSIterator.hpp"
#include "../../OptFrame/NSSeq.hpp"
#include "../../OptFrame/RandGen.hpp"

// Own includes
#include "ProblemInstance.h"
#include "Representation.h"

using namespace std;

namespace $project
{

class Move$neighborhood: public Move< Rep$project >
{
private:
    // MOVE PARAMETERS

public:

    Move$neighborhood() // ADD PARAMETERS
    {
    }

    virtual ~Move$neighborhood()
    {
    }
    
    void print() const
    {
        cout << id() << " with params: '" << "ADD MY PARAMETERS" << "'" << endl;
    }
    
    string id() const
    {
        return Move<Rep$project >::idComponent().append(":Move$neighborhood");
    }
    
    bool operator==(const Move< Rep$project >& _m) const
    {
        const Move$neighborhood& m = (const Move$neighborhood&) _m;
        // COMPARE PARAMETERS AND RETURN TRUE IF EQUALS
        return false;
    }
    
    // Implement these methods in the .cpp file
    
    bool canBeApplied(const Rep$project& rep, const OptFrameADS* ads);

    Move< Rep$project >* apply(Rep$project& rep, OptFrameADS* ads);
    
    MoveCost* cost(const Evaluation&, const Rep$project& rep, const OptFrameADS* ads, bool allowEstimated);
};



class NSIterator$neighborhood: public NSIterator< Rep$project >
{
private:
    // ITERATOR PARAMETERS

public:
    NSIterator$neighborhood() // ADD ITERATOR PARAMETERS
    {
    }

    virtual ~NSIterator$neighborhood()
    {
    }
    
    // Implement these methods in the .cpp file

    void first();
    void next();
    bool isDone();
    Move< Rep$project >* current();
};



class NSSeq$neighborhood: public NSSeq< Rep$project >
{
private:
    // YOU MAY REMOVE THESE PARAMETERS IF YOU DON'T NEED (BUT PROBABLY WILL...)
    ProblemInstance& p$project; // problem instance data
    RandGen& rg;                // random number generator

public:

    // YOU MAY REMOVE THESE PARAMETERS IF YOU DON'T NEED (BUT PROBABLY WILL...)
    NSSeq$neighborhood(ProblemInstance& _p$project, RandGen& _rg):
        p$project(_p$project), rg(_rg)
    {
    }

    virtual ~NSSeq$neighborhood()
    {
    }
    
    void print() const
    {
        cout << "NSSeq$neighborhood" << endl;
    }
    
    string id() const
    {
        return NSSeq<Rep$project >::idComponent().append(":NSSeq$neighborhood");
    }
    
    NSIterator<Rep$project >* getIterator(const Rep$project& rep, const OptFrameADS* ads)
    {
        // return an iterator to the neighbors of 'rep' 
        return new NSIterator$neighborhood;  // ADD POSSIBLE ITERATOR PARAMETERS
    }
        
    // Implement this method in the .cpp file

    Move<Rep$project >* randomMove(const Rep$project& rep, const OptFrameADS* ads);
    
    // Move<Rep$project >* validRandomMove(const Rep$project& rep, const OptFrameADS* ads);
};

}

#endif /*$project_NSSEQ$neighborhood_H_*/
