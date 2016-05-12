#include "NSSeq$neighborhood.h"

using namespace $project;

// ============ Move$neighborhood ==============

bool Move$neighborhood::canBeApplied(const Rep$project& rep, const MY_ADS&)
{
    return true;
}

Move< Rep$project , MY_ADS  >* Move$neighborhood::apply(Rep$project& rep, MY_ADS&)
{
    // apply this move to 'rep'
    // rep. (...) = (...)
    // return reverse move
    return new Move$neighborhood; 
}

MoveCost* Move$neighborhood::cost(const Evaluation&, const Rep$project& rep, const MY_ADS& ads)
{
   return NULL;
   // Implement if a more efficient evaluation of the move is available
   //double move_cost = ...;
   //double move_cost_infeasible = ...;
   //return new MoveCost(move_cost, move_cost_infeasible);
}



// ============ NSIterator$neighborhood ==============

void NSIterator$neighborhood::first(){};

void NSIterator$neighborhood::next(){};
	
bool NSIterator$neighborhood::isDone(){};
	
Move< Rep$project , MY_ADS  >& NSIterator$neighborhood::current(){};




// ============ NSSeq$neighborhood ==============


Move<Rep$project , MY_ADS >& NSSeq$neighborhood::move(const Rep$project& rep, const MY_ADS&)
{
   // return a random move (that is, a move operator that generates a neighbor solution of 'rep')
   // you may need to use the random number generator 'rg'
   
   return * new Move$neighborhood; 
}
