#include "NSSeq$neighborhood.h"

using namespace $project;

// ============ Move$neighborhood ==============

bool Move$neighborhood::canBeApplied(const Rep$project& rep, const OptFrameADS* ads)
{
    return true;
}

Move< Rep$project >* Move$neighborhood::apply(Rep$project& rep, OptFrameADS* ads)
{
    // apply this move to 'rep'
    // rep. (...) = (...)
    // return reverse move
    return new Move$neighborhood; 
}

MoveCost* Move$neighborhood::cost(const Evaluation&, const Rep$project& rep, const OptFrameADS* ads, bool allowEstimated)
{
   return nullptr;
   // Implement if a more efficient evaluation of the move is available
   //double move_cost = ...;
   //double move_cost_infeasible = ...;
   //return new MoveCost(move_cost, move_cost_infeasible);
}



// ============ NSIterator$neighborhood ==============

void NSIterator$neighborhood::first(){};

void NSIterator$neighborhood::next(){};
	
bool NSIterator$neighborhood::isDone(){};
	
Move< Rep$project >* NSIterator$neighborhood::current(){};




// ============ NSSeq$neighborhood ==============


Move<Rep$project >* NSSeq$neighborhood::randomMove(const Rep$project& rep, const OptFrameADS* ads)
{
   // you can ignore 'ads' if you want.
   // return a random move (that is, a move operator that generates a neighbor solution of 'rep')
   // you may need to use the random number generator 'rg'
   
   return new Move$neighborhood; 
}
