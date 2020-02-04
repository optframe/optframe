#include "NSSeqBitFlip.h"

using namespace KP;

// ============ MoveBitFlip ==============

bool MoveBitFlip::canBeApplied(const RepKP& rep, const MY_ADS*)
{
    return true;
}

Move< RepKP , MY_ADS  >* MoveBitFlip::apply(RepKP& rep, MY_ADS*)
{
    rep[item] = 1-rep[item];
    
    return new MoveBitFlip(item); 
}

MoveCost<>* MoveBitFlip::cost(const Evaluation<>&, const RepKP& rep, const MY_ADS* ads, bool mayEstimate)
{
   return NULL;
   // Implement if a more efficient evaluation of the move is available
   //double move_cost = ...;
   //double move_cost_infeasible = ...;
   //return new MoveCost(move_cost, move_cost_infeasible);
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
	
Move< RepKP , MY_ADS  >* NSIteratorBitFlip::current()
{
	return new MoveBitFlip(item);
};




// ============ NSSeqBitFlip ==============


Move<RepKP , MY_ADS >* NSSeqBitFlip::randomMove(const RepKP& rep, const MY_ADS*)
{
   int item = rand()%pKP.N;
   
   return new MoveBitFlip(item); 
}
