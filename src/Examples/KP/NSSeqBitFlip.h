#ifndef KP_NSSEQBitFlip_H_
#define KP_NSSEQBitFlip_H_

// Framework includes
#include "../../OptFrame/NSSeq.hpp"
#include "../../OptFrame/RandGen.hpp"

// Own includes
#include "ProblemInstance.h"
#include "Solution.h"

using namespace std;

namespace KP {

class MoveBitFlip : public Move<ESolutionKP>
{
private:
   int item;

public:
   MoveBitFlip(int _item)
     : item(_item)
   {
   }

   virtual ~MoveBitFlip()
   {
   }

   void print() const
   {
      cout << id() << " with params: '"
           << "item=" << item << "'" << endl;
   }

   string id() const
   {
      return Move<SolutionKP>::idComponent().append(":MoveBitFlip");
   }

   bool operator==(const Move<SolutionKP>& _m) const
   {
      const MoveBitFlip& m = (const MoveBitFlip&)_m;
      return item == m.item;
   }

   // Implement these methods in the .cpp file

   bool canBeApplied(const SolutionKP& s) override;

   uptr<Move<SolutionKP>> apply(SolutionKP& s) override;

   op<Evaluation<>> cost(const pair<SolutionKP, Evaluation<>>& se, bool mayEstimate) override;
};

class NSIteratorBitFlip : public NSIterator<SolutionKP>
{
private:
   int N, item;

public:
   NSIteratorBitFlip(int _N)
     : N(_N)
   {
   }

   virtual ~NSIteratorBitFlip()
   {
   }

   // Implement these methods in the .cpp file

   void first() override;
   void next() override;
   bool isDone() override;
   uptr<Move<SolutionKP>> current() override;
};

class NSSeqBitFlip : public NSSeq<SolutionKP>
{
private:
   // YOU MAY REMOVE THESE PARAMETERS IF YOU DON'T NEED (BUT PROBABLY WILL...)
   ProblemInstance& pKP; // problem instance data
   RandGen& rg;          // random number generator

public:
   // YOU MAY REMOVE THESE PARAMETERS IF YOU DON'T NEED (BUT PROBABLY WILL...)
   NSSeqBitFlip(ProblemInstance& _pKP, RandGen& _rg)
     : pKP(_pKP)
     , rg(_rg)
   {
   }

   virtual ~NSSeqBitFlip()
   {
   }

   void print() const
   {
      cout << "NSSeqBitFlip" << endl;
   }

   string id() const
   {
      return NSSeq<SolutionKP>::idComponent().append(":NSSeqBitFlip");
   }

   uptr<NSIterator<SolutionKP>> getIterator(const SolutionKP& s) override
   {
      // return an iterator to the neighbors of 'rep'
      return uptr<NSIterator<SolutionKP>>(new NSIteratorBitFlip(pKP.N));
   }

   // Implement this method in the .cpp file

   uptr<Move<SolutionKP>> randomMove(const SolutionKP& s) override;
};
}

#endif /*KP_NSSEQBitFlip_H_*/
