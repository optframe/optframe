#include "NSSeq2Opt.h"

using namespace NPP;

// ============ Move2Opt ==============

bool Move2Opt::canBeApplied(const RepNPP& rep, const OptFrameADS* ads)
{
    return true;
}

Move< RepNPP >* Move2Opt::apply(RepNPP& rep, OptFrameADS* ads)
{
    // apply this move to 'rep'
    // rep. (...) = (...)
    // return reverse move
    for (int x = this->i, y = this->j; x < y; ++x, --y) {
        swap(rep[x], rep[y]);
    }
    return new Move2Opt(this->i, this->j);
}

MoveCost* Move2Opt::cost(const Evaluation&, const RepNPP& rep, const OptFrameADS* ads, bool allowEstimated)
{
   return NULL;
   // Implement if a more efficient evaluation of the move is available
   //double move_cost = ...;
   //double move_cost_infeasible = ...;
   //return new MoveCost(move_cost, move_cost_infeasible);
}



// ============ NSIterator2Opt ==============

void NSIterator2Opt::first(){
    this->i = 1;
    this->j = 2;
//   cout << "first " << i << ", " << j << endl;
};

void NSIterator2Opt::next(){
//   cout << "next " << i << ", " << j << " -> ";
    this->j++;
    if (this->j == this->pNPP.getSize()) {
        this->i++;
        this->j = this->i + 2;
    }
//   cout  << i << ", " << j << endl;
};

bool NSIterator2Opt::isDone(){
    return this->i == this->pNPP.getSize() - 3 && this->j == this->pNPP.getSize() - 1;
};

Move< RepNPP >* NSIterator2Opt::current(){
//   cout << "current " << i << ", " << j << endl;
    return new Move2Opt(this->i, this->j);
};




// ============ NSSeq2Opt ==============


Move<RepNPP >* NSSeq2Opt::randomMove(const RepNPP& rep, const OptFrameADS* ads)
{
   // you can ignore 'ads' if you want.
   // return a random move (that is, a move operator that generates a neighbor solution of 'rep')
   // you may need to use the random number generator 'rg'
//   cout << "randomMove " << flush;
   int i = this->rg.rand(this->pNPP.getSize() - 2) + 1;
   int j = this->rg.rand(this->pNPP.getSize());
   while (i >= j + 1) {
       j = this->rg.rand(this->pNPP.getSize());
   }
//   cout << i << ", " << j << endl;
   return new Move2Opt(i, j);
}
