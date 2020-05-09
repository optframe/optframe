#include "NSSeq$neighborhood.h"

using namespace $project;

// ============ Move$neighborhood ==============

bool Move$neighborhood::canBeApplied(const ESolution$project& rep)
{
    return true;
}

uptr<Move< ESolution$project >> Move$neighborhood::apply(ESolution$project& rep)
{
    // apply this move to 'rep'
    // rep. (...) = (...)
    // return reverse move
    return uptr<Move< ESolution$project >>(new Move$neighborhood ); 
}

op<Evaluation<>> Move$neighborhood::cost(const ESolution$project & se, bool allowEstimated)
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
	
uptr<Move< ESolution$project >> NSIterator$neighborhood::current(){};




// ============ NSSeq$neighborhood ==============


uptr<Move<ESolution$project >> NSSeq$neighborhood::randomMove(const ESolution$project& se)
{
   // return a random move (that is, a move operator that generates a neighbor solution of 'se')
   // you may need to use the random number generator 'rg'
   
   return uptr<Move<ESolution$project >>(new Move$neighborhood ); 
}
