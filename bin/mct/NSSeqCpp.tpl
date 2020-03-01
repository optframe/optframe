#include "NSSeq$neighborhood.h"

using namespace $project;

// ============ Move$neighborhood ==============

bool Move$neighborhood::canBeApplied(const Solution$project& rep)
{
    return true;
}

uptr<Move< Solution$project >> Move$neighborhood::apply(Solution$project& rep)
{
    // apply this move to 'rep'
    // rep. (...) = (...)
    // return reverse move
    return uptr<Move< Solution$project >>(newMove$neighborhood ); 
}

op<Evaluation<>> Move$neighborhood::cost(const pair< Solution$project , Evaluation<>>& se, bool allowEstimated)
{
   return std::nullopt;
   // Implement if a more efficient evaluation of the move is available
   //double move_cost = ...;
   //double move_cost_infeasible = ...;
   //return new MoveCost<>(move_cost, move_cost_infeasible);
}



// ============ NSIterator$neighborhood ==============

void NSIterator$neighborhood::first(){};

void NSIterator$neighborhood::next(){};
	
bool NSIterator$neighborhood::isDone(){};
	
Move< Solution$project >* NSIterator$neighborhood::current(){};




// ============ NSSeq$neighborhood ==============


uptr<Move<Solution$project >> NSSeq$neighborhood::randomMove(const Solution$project& rep)
{
   // you can ignore 'ads' if you want.
   // return a random move (that is, a move operator that generates a neighbor solution of 'rep')
   // you may need to use the random number generator 'rg'
   
   return uptr<Move<Solution$project >>(newMove$neighborhood ); 
}
