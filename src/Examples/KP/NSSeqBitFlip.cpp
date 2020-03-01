#include "NSSeqBitFlip.h"

using namespace KP;

// ============ MoveBitFlip ==============

bool MoveBitFlip::canBeApplied(const SolutionKP& s)
{
    return true;
}

uptr<Move<SolutionKP>> MoveBitFlip::apply(SolutionKP& s)
{
   RepKP& rep = s.getR();
    rep[item] = 1-rep[item];
    
    return uptr<Move<SolutionKP>>(new MoveBitFlip(item)); 
}

op<Evaluation<>> MoveBitFlip::cost(const pair<SolutionKP, Evaluation<>>& se, bool mayEstimate)
{
   return std::nullopt;
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
	
uptr<Move<SolutionKP>> NSIteratorBitFlip::current()
{
	return uptr<Move<SolutionKP>>(new MoveBitFlip(item));
};




// ============ NSSeqBitFlip ==============


uptr<Move<SolutionKP>> NSSeqBitFlip::randomMove(const SolutionKP& s)
{
   int item = rand()%pKP.N;
   
   return uptr<Move<SolutionKP>>(new MoveBitFlip(item)); 
}
