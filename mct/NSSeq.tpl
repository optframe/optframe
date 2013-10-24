#ifndef $project_NSSEQ$neighborhood_H_
#define $project_NSSEQ$neighborhood_H_

// Framework includes
#include "../../OptFrame/NSSeq.hpp"
#include "../../OptFrame/RandGen.hpp"

// Own includes
#include "ProblemInstance.h"
#include "DeltaStructure.h"
#include "Solution.h"

using namespace std;

namespace $project
{

class Move$neighborhood: public Move< Rep$project , MY_ADS $commadproject >
{
private:
    // MOVE PARAMETERS

public:
    using Move< Rep$project , MY_ADS $commadproject >::apply; // prevents name hiding
    using Move< Rep$project , MY_ADS $commadproject >::canBeApplied; // prevents name hiding

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
        return Move<Rep$project , MY_ADS $commadproject>::idComponent().append(":Move$neighborhood");
    }
    
    bool operator==(const Move< Rep$project , MY_ADS $commadproject >& _m) const
    {
        const Move$neighborhood& m = (const Move$neighborhood&) _m;
        // COMPARE PARAMETERS AND RETURN TRUE IF EQUALS
        return false;
    }
    
    // Implement these methods in the .cpp file
    
    bool canBeApplied(const Rep$project& rep, const MY_ADS&);

    Move< Rep$project , MY_ADS $commadproject >& apply(Rep$project& rep, MY_ADS&);
    
    MoveCost* cost(const Evaluation< $deltaproject >&, const Rep$project& rep, const MY_ADS& ads);
};



class NSIterator$neighborhood: public NSIterator< Rep$project , MY_ADS $commadproject >
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
    Move< Rep$project , MY_ADS $commadproject >& current();
};



class NSSeq$neighborhood: public NSSeq< Rep$project , MY_ADS $commadproject >
{
private:
    // YOU MAY REMOVE THESE PARAMETERS IF YOU DON'T NEED (BUT PROBABLY WILL...)
    ProblemInstance& p$project; // problem instance data
    RandGen& rg;                // random number generator

public:

    using NSSeq< Rep$project , MY_ADS $commadproject >::move; // prevents name hiding

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
        return NSSeq<Rep$project , MY_ADS $commadproject>::idComponent().append(":NSSeq$neighborhood");
    }
    
    NSIterator<Rep$project , MY_ADS $commadproject>& getIterator(const Rep$project& rep, const MY_ADS&)
    {
        // return an iterator to the neighbors of 'rep' 
        return * new NSIterator$neighborhood;  // ADD POSSIBLE ITERATOR PARAMETERS
    }
        
    // Implement this method in the .cpp file

    Move<Rep$project , MY_ADS $commadproject>& move(const Rep$project& rep, const MY_ADS&);
};

}

#endif /*$project_NSSEQ$neighborhood_H_*/
