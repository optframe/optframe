#include "NSSeqSwap.h"

using namespace PN;

// ============ MoveSwap ==============

bool MoveSwap::canBeApplied(const RepPN& rep, const MY_ADS&)
{
	if (i >= rep.size()-1 || i < 0)
		return false;
	if (j >= rep.size()-1 || j < 0)
		return false;
        if (rep[i] == rep[j])
                return false;
    return true;
}

Move< RepPN , MY_ADS  >& MoveSwap::apply(RepPN& rep, MY_ADS&)
{	
    int aux;

    aux = rep[i];
    rep[i] = rep[j];
    rep[j] = aux;    

    return * new MoveSwap(j,i,pPN); 
}

MoveCost* MoveSwap::cost(const Evaluation<  >&, const RepPN& rep, const MY_ADS& ads)
{
   return NULL;
   // Implement if a more efficient evaluation of the move is available
   //double move_cost = ...;
   //double move_cost_infeasible = ...;
   //return new MoveCost(move_cost, move_cost_infeasible);
}



// ============ NSIteratorSwap ==============

void NSIteratorSwap::first()
{
    i = 0;
    j = 1;
};

void NSIteratorSwap::next()
{
    j++;
    if (j > rep.size()-1)
    {
	i++;
	j = 1;
    }

};
	
bool NSIteratorSwap::isDone()
{
    return (i >= rep.size() - 1);
};
	
Move< RepPN , MY_ADS  >& NSIteratorSwap::current()
{
    return *new MoveSwap(i, j, pPN);
};




// ============ NSSeqSwap ==============


Move<RepPN , MY_ADS >& NSSeqSwap::move(const RepPN& rep, const MY_ADS&)
{
   int i1 = rg.rand(rep.size()-1);
   int j1 = rg.rand(rep.size()-1);
   
   return * new MoveSwap(i1, j1, pPN); 
}
