#ifndef PN_NSSEQShift_H_
#define PN_NSSEQShift_H_

// Framework includes
#include <OptFrame/NSSeq.hpp>
#include <OptFrame/RandGen.hpp>

// Own includes
#include "DeltaStructure.h"
#include "ProblemInstance.h"
#include "Solution.h"

using namespace std;

namespace PN {

class MoveShift : public Move<RepPN, MY_ADS>
{
private:
   // MOVE PARAMETERS
   int i;
   ProblemInstance& pPN;

public:
   MoveShift(int _i, ProblemInstance& _pPN)
     : i(_i)
     , pPN(_pPN)
   {
   }

   virtual ~MoveShift()
   {
   }

   void print() const
   {
      cout << id() << " with params: '" << i << "'" << endl;
   }

   string id() const
   {
      return Move<RepPN, MY_ADS>::idComponent().append(":MoveShift");
   }

   bool operator==(const Move<RepPN, MY_ADS>& _m) const
   {
      const MoveShift& m = (const MoveShift&)_m;
      return (i == m.i);
   }

   // Implement these methods in the .cpp file

   bool canBeApplied(const RepPN& rep, const MY_ADS*) override;

   Move<RepPN, MY_ADS>* apply(RepPN& rep, MY_ADS*) override;

   op<EvaluationPN> cost(const Evaluation<>&, const RepPN& rep, const MY_ADS* ads, bool allowEstimate) override;
};

class NSIteratorShift : public NSIterator<RepPN, MY_ADS>
{
private:
   // ITERATOR PARAMETERS
   int i;
   const RepPN& rep;
   ProblemInstance& pPN;

public:
   NSIteratorShift(ProblemInstance& _pPN, const RepPN& _rep)
     : rep(_rep)
     , pPN(_pPN)
   {
   }

   virtual ~NSIteratorShift()
   {
   }

   // Implement these methods in the .cpp file

   void first() override;
   void next() override;
   bool isDone() override;
   Move<RepPN, MY_ADS>* current() override;
};

class NSSeqShift : public NSSeq<RepPN, MY_ADS>
{
private:
   // YOU MAY REMOVE THESE PARAMETERS IF YOU DON'T NEED (BUT PROBABLY WILL...)
   ProblemInstance& pPN; // problem instance data
   RandGen& rg;          // random number generator

public:
   // YOU MAY REMOVE THESE PARAMETERS IF YOU DON'T NEED (BUT PROBABLY WILL...)
   NSSeqShift(ProblemInstance& _pPN, RandGen& _rg)
     : pPN(_pPN)
     , rg(_rg)
   {
   }

   virtual ~NSSeqShift()
   {
   }

   void print() const
   {
      cout << "NSSeqShift" << endl;
   }

   string id() const
   {
      return NSSeq<RepPN, MY_ADS>::idComponent().append(":NSSeqShift");
   }

   NSIterator<RepPN, MY_ADS>& getIterator(const RepPN& rep, const MY_ADS&)
   {
      // return an iterator to the neighbors of 'rep'
      return *new NSIteratorShift(pPN, rep); // ADD POSSIBLE ITERATOR PARAMETERS
   }

   // Implement this method in the .cpp file

   Move<RepPN, MY_ADS>* randomMove(const RepPN& rep, const MY_ADS*) override;

   Move<RepPN, MY_ADS>* validMove(const RepPN& rep, const MY_ADS* ads)
   {
      for (unsigned i = 0; i < 100; i++) {
         Move<RepPN, MY_ADS>& m = *randomMove(rep, ads);
         if (m.canBeApplied(rep, ads))
            return &m;
         else
            delete &m;
      }
      return NULL;
   }
};
}

#endif /*PN_NSSEQShift_H_*/
