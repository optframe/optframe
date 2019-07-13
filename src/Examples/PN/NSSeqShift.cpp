#include "NSSeqShift.h"

using namespace PN;

// ============ MoveShift ==============

bool MoveShift::canBeApplied(const RepPN& rep, const MY_ADS&)
{
        if (i >= rep.size()-1 || i < 0)
		return false;
    return true;
}

Move< RepPN , MY_ADS  >& MoveShift::apply(RepPN& rep, MY_ADS&)
{
    rep[i] = !(rep[i]);

    return * new MoveShift(i, pPN);
}

MoveCost* MoveShift::cost(const Evaluation<  >&, const RepPN& rep, const MY_ADS& ads)
{
   return NULL;
   // Implement if a more efficient evaluation of the move is available
   //double move_cost = ...;
   //double move_cost_infeasible = ...;
   //return new MoveCost(move_cost, move_cost_infeasible);
}



// ============ NSIteratorShift ==============

void NSIteratorShift::first()
{
    i = 0;
};

void NSIteratorShift::next()
{
    i++;
};
	
bool NSIteratorShift::isDone()
{
    return (i > rep.size() - 1);
};
	
Move< RepPN , MY_ADS  >& NSIteratorShift::current()
{
    return *new MoveShift(i, pPN);
};




// ============ NSSeqShift ==============


Move<RepPN , MY_ADS >& NSSeqShift::move(const RepPN& rep, const MY_ADS&)
{
   int i1 = rg.rand(rep.size());
   
   return * new MoveShift(i1, pPN);
}
