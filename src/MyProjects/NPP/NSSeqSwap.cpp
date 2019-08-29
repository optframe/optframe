#include "NSSeqSwap.h"

using namespace NPP;

// ============ MoveSwap ==============

bool MoveSwap::canBeApplied(const RepNPP& rep, const OptFrameADS* ads)
{
    return true;
}

Move< RepNPP >* MoveSwap::apply(RepNPP& rep, OptFrameADS* ads)
{
    // apply this move to 'rep'
    // rep. (...) = (...)
    // return reverse move
    swap(rep[this->i], rep[this->j]);

    return new MoveSwap(this->i, this->j);
}

MoveCost* MoveSwap::cost(const Evaluation&, const RepNPP& rep, const OptFrameADS* ads, bool allowEstimated)
{
   return NULL;
   // Implement if a more efficient evaluation of the move is available
   //double move_cost = ...;
   //double move_cost_infeasible = ...;
   //return new MoveCost(move_cost, move_cost_infeasible);
}



// ============ NSIteratorSwap ==============

void NSIteratorSwap::first(){
    this->i = 0;
    this->j = 1;
};

void NSIteratorSwap::next(){
    this->j++;
    if (this->j == this->pNPP.getSize()) {
        this->i++;
        this->j = this->i + 1;
    }
};
	
bool NSIteratorSwap::isDone(){
    return this->i == this->pNPP.getSize() - 2 && this->j == this->pNPP.getSize() - 1;
};
	
Move< RepNPP >* NSIteratorSwap::current(){
    return new MoveSwap(this->i, this->j);
};




// ============ NSSeqSwap ==============


Move<RepNPP >* NSSeqSwap::randomMove(const RepNPP& rep, const OptFrameADS* ads)
{
   // you can ignore 'ads' if you want.
   // return a random move (that is, a move operator that generates a neighbor solution of 'rep')
   // you may need to use the random number generator 'rg'

    int i = this->rg.rand(this->pNPP.getSize() - 1);
    int j = this->rg.rand(this->pNPP.getSize());
    while (i >= j) {
        j = this->rg.rand(this->pNPP.getSize());
    }
    return new MoveSwap(i, j);
}
