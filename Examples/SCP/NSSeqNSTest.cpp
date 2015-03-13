#include "../../Examples/SCP/NSSeqNSTest.h"

using namespace SCP;

// ============ MoveNSTest ==============

bool MoveNSTest::canBeApplied(const RepSCP& rep, const MY_ADS&)
{
    return true;
}

Move< RepSCP , MY_ADS  >& MoveNSTest::apply(RepSCP& rep, MY_ADS&)
{
    // apply this move to 'rep'
    // rep. (...) = (...)
    // return reverse move
    return * new MoveNSTest; 
}

MoveCost* MoveNSTest::cost(const Evaluation<  >&, const RepSCP& rep, const MY_ADS& ads)
{
   return NULL;
   // Implement if a more efficient evaluation of the move is available
   //double move_cost = ...;
   //double move_cost_infeasible = ...;
   //return new MoveCost(move_cost, move_cost_infeasible);
}



// ============ NSIteratorNSTest ==============

void NSIteratorNSTest::first(){};

void NSIteratorNSTest::next(){};
	
bool NSIteratorNSTest::isDone(){};
	
Move< RepSCP , MY_ADS  >& NSIteratorNSTest::current(){};




// ============ NSSeqNSTest ==============


Move<RepSCP , MY_ADS >& NSSeqNSTest::move(const RepSCP& rep, const MY_ADS&)
{
   // return a random move (that is, a move operator that generates a neighbor solution of 'rep')
   // you may need to use the random number generator 'rg'
   
   return * new MoveNSTest; 
}
