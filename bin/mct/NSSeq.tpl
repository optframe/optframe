#pragma once

#ifndef $project_NSSEQ$neighborhood_H_
#define $project_NSSEQ$neighborhood_H_

// Framework includes
#include <OptFrame/Core/Move.hpp>
// #include <OptFrame/MoveCost.hpp>
#include <OptFrame/Core/NSIterator.hpp>
#include <OptFrame/Core/NSSeq.hpp>
#include <OptFrame/Core/RandGen.hpp>

// Own includes
#include "ProblemInstance.h"
#include "Representation.h"

using namespace std;

namespace $project
{

class Move$neighborhood: public Move< ESolution$project >
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
    
    void print() const override
    {
        std::cout << id() << " with params: '" << "ADD MY PARAMETERS" << "'" << std::endl;
    }
    
    string id() const override
    {
        return Move< ESolution$project >::idComponent().append(":Move$neighborhood");
    }
    
    bool operator==(const Move< ESolution$project >& _m) const
    {
        const Move$neighborhood& m = (const Move$neighborhood&) _m;
        // COMPARE PARAMETERS AND RETURN TRUE IF EQUALS
        return false;
    }
    
    // Implement these methods in the .cpp file
    
    bool canBeApplied(const ESolution$project& rep) override;

    uptr<Move< ESolution$project >> apply(ESolution$project& rep) override;
    
    op< Evaluation$project > cost(const ESolution$project& se, bool allowEstimated) override;
};



class NSIterator$neighborhood: public NSIterator< ESolution$project >
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

    void first() override;
    void next() override;
    bool isDone() override;
    uptr<Move< ESolution$project >> current() override;
};



class NSSeq$neighborhood: public NSSeq< ESolution$project >
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
    
    void print() const override
    {
        std::cout << "NSSeq$neighborhood" << std::endl;
    }
    
    string id() const override
    {
        return NSSeq<ESolution$project >::idComponent().append(":NSSeq$neighborhood");
    }
    
    uptr<NSIterator<ESolution$project >> getIterator(const ESolution$project& se) override
    {
        // return an iterator to the neighbors of 'rep' 
        return uptr<NSIterator<ESolution$project >>(new NSIterator$neighborhood );  // ADD POSSIBLE ITERATOR PARAMETERS
    }
        
    // Implement this method in the .cpp file

    uptr<Move<ESolution$project >> randomMove(const ESolution$project& se) override;
    
    // Move<Solution$project >* validRandomMove(const ESolution$project& se) override;
};

}

#endif /*$project_NSSEQ$neighborhood_H_*/
