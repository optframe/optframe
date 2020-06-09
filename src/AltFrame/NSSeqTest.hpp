#pragma once

#include "NSSeqBase.hpp"

// my test
// vector<int>
// swap idea
// =================

class NSSeqTest : public NSSeq<std::vector<int>, MoveFuncPtrCopy<std::vector<int>>>
{
public:

   // store move methods on NSSeq
   
   /*
   std::function<void(std::vector<int>&)> fApplyDo
   {
      [](std::vector<int>& v) -> void {
            int& i = commonState.first;
            int& j = commonState.second;
            // swap
            int aux = v[i];
            v[i] = v[j];
            v[j] = aux;
         }
   };
   */
   

  
  static void fApplyDo(std::vector<int>& v) 
   {
      // non-capture!
      //[](std::vector<int>& v) -> void {
            int& i = commonState.first;
            int& j = commonState.second;
            // swap
            int aux = v[i];
            v[i] = v[j];
            v[j] = aux;
      //   }
   };
   

   //std::function<void(X&)> fApplyUndo; // TODO: receive this correctly.. now only copy for 'undo'
   //
   //std::function<MoveByContextRef<X>(State&)> fGenMoves;
   //
   static std::pair<int,int> commonState;

   // must find solution to this!
   virtual MoveFuncPtrCopy<std::vector<int>> getStateMove () {
      void(*ptr)(std::vector<int>&) = this->fApplyDo;
      return MoveFuncPtrCopy<std::vector<int>> (
         ptr
      );  // automatic implementation
   }

   // ==================== EMBEDDED ITERATOR

   // returns false if finished
   virtual bool first(const std::vector<int>&) { /* whatever */ } //= 0;

   // returns false if finished
   virtual bool next() { /* whatever */ } // = 0;

   // returns true if finished (TODO: we may remove this)
   virtual bool isDone() const { /* whatever */ } // = 0; 

   // returns current move, or nothing (if doesn't exist)
   virtual op< MoveFuncPtrCopy<std::vector<int>> > current(){ /* whatever */ } // = 0;
};

std::pair<int,int> NSSeqTest::commonState = std::pair<int,int>{};



