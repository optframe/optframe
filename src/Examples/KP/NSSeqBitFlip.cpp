#include "NSSeqBitFlip.h"

using namespace KP;

// ============ MoveBitFlip ==============

bool MoveBitFlip::canBeApplied(const SolutionKP& s)
{
    return true;
}

Move<SolutionKP>* MoveBitFlip::apply(SolutionKP& s)
{
   RepKP& rep = s.getR();
    rep[item] = 1-rep[item];
    
    return new MoveBitFlip(item); 
}

MoveCost<>* MoveBitFlip::cost(const Evaluation<>&, const SolutionKP& s, bool mayEstimate)
{
   return NULL;
   // Implement if a more efficient evaluation of the move is available
   //double move_cost = ...;
   //double move_cost_infeasible = ...;
   //return new MoveCost<>(move_cost, move_cost_infeasible);
}



// ============ NSIteratorBitFlip ==============

void NSIteratorBitFlip::first()
{
   item = 0;	
};

void NSIteratorBitFlip::next()
{
	item=item+1;
};
	
bool NSIteratorBitFlip::isDone()
{
	return item == N;
};
	
Move<SolutionKP>* NSIteratorBitFlip::current()
{
	return new MoveBitFlip(item);
};




// ============ NSSeqBitFlip ==============


Move<SolutionKP>* NSSeqBitFlip::randomMove(const SolutionKP& s)
{
   int item = rand()%pKP.N;
   
   return new MoveBitFlip(item); 
}
