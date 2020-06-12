#pragma once

#include "NSFind.hpp"
#include "NSIterator.hpp"

template<class XES, class MoveModel = uptr<Move<XES>>, class IterModel = uptr<NSIterator<XES, MoveModel>> >
class NSSeq : public NSFind<XES, MoveModel>
{
public:
   // override
   //virtual op<MoveModel> randomMove(const XES&) = 0;

   // override
   //virtual op<MoveModel> findBest(const XES&) = 0;

   // override
   //svirtual op<MoveModel> findImproving(const XES&) = 0;

   virtual uptr<NSIterator<XES, MoveModel>> getIterator(const XES&) { /* whatever */ }  //= 0;

   virtual IterModel getIteratorCopy(const XES&) { /* whatever */ }  //= 0;


   // =================
   // embedded iterator
   // =================

    // returns false if finished
   virtual bool first(const XES&) = 0;

   // returns false if finished
   virtual bool next() = 0;

   // returns true if finished (TODO: we may remove this)
   virtual bool isDone() const = 0; 

   // returns current move, or nothing (if doesn't exist)
   virtual op<MoveModel> current() = 0;

};
