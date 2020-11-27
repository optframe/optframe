#include "NSSeqBitFlip.h"

using namespace KP;

// ============ MoveBitFlip ==============

bool MoveBitFlip::canBeApplied(const ESolutionKP& s)
{
    return true;
}

uptr<Move<ESolutionKP>> MoveBitFlip::apply(ESolutionKP& s)
{
   //RepKP& rep = s.first.getR();
   RepKP& rep = s.first;
    rep[item] = 1-rep[item];
    
    return uptr<Move<ESolutionKP>>(new MoveBitFlip(item)); 
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
	
uptr<Move<ESolutionKP>> NSIteratorBitFlip::current()
{
	return uptr<Move<ESolutionKP>>(new MoveBitFlip(item));
};




// ============ NSSeqBitFlip ==============


uptr<Move<ESolutionKP>> NSSeqBitFlip::randomMove(const ESolutionKP& s)
{
   int item = rand()%pKP.N;
   
   return uptr<Move<ESolutionKP>>(new MoveBitFlip(item)); 
}
