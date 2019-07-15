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

class MoveBitFlip : public Move<RepKP, MY_ADS>
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
      return Move<RepKP, MY_ADS>::idComponent().append(":MoveBitFlip");
   }

   bool operator==(const Move<RepKP, MY_ADS>& _m) const
   {
      const MoveBitFlip& m = (const MoveBitFlip&)_m;
      return item == m.item;
   }

   // Implement these methods in the .cpp file

   bool canBeApplied(const RepKP& rep, const MY_ADS*) override;

   Move<RepKP, MY_ADS>* apply(RepKP& rep, MY_ADS*) override;

   MoveCost* cost(const Evaluation&, const RepKP& rep, const MY_ADS* ads, bool mayEstimate) override;
};

class NSIteratorBitFlip : public NSIterator<RepKP, MY_ADS>
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
   Move<RepKP, MY_ADS>* current() override;
};

class NSSeqBitFlip : public NSSeq<RepKP, MY_ADS>
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
      return NSSeq<RepKP, MY_ADS>::idComponent().append(":NSSeqBitFlip");
   }

   NSIterator<RepKP, MY_ADS>* getIterator(const RepKP& rep, const MY_ADS*) override
   {
      // return an iterator to the neighbors of 'rep'
      return new NSIteratorBitFlip(pKP.N);
   }

   // Implement this method in the .cpp file

   Move<RepKP, MY_ADS>* randomMove(const RepKP& rep, const MY_ADS*) override;
};
}

#endif /*KP_NSSEQBitFlip_H_*/
