#pragma once

#include "NSSeqBase.hpp"

// my test
// vector<int>
// swap idea
// =================

class NSSeqTest : public NSSeq<std::vector<int>, MoveFuncPtrCopy<std::vector<int>>>
{
public:

   int nTSP{-1};

   NSSeqTest(int nTSP) : 
      nTSP{nTSP}
      {

      }

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
   virtual bool first(const std::vector<int>&) {
      commonState.first = 0;
      commonState.second = 1;
       /* whatever */ 
      return true;
   } //= 0;

   // returns false if finished
   virtual bool next() { 
      commonState.second++;
      if(commonState.second>=(nTSP-1))
      {
         commonState.first++;
         commonState.second = commonState.first+1;
      }
      return commonState.first < nTSP;
      } // = 0;

   // returns true if finished (TODO: we may remove this)
   virtual bool isDone() const { 
      return commonState.first >= (nTSP-1);
      /* whatever */ 
   } // = 0; 

   // returns current move, or nothing (if doesn't exist)
   //virtual op< MoveFuncPtrCopy<std::vector<int>> > current(){ /* whatever */ } // = 0;
   virtual op< MoveFuncPtrCopy<std::vector<int>> > current () {
      void(*ptr)(std::vector<int>&) = this->fApplyDo;
      //
      return  op< MoveFuncPtrCopy<std::vector<int>> > (MoveFuncPtrCopy<std::vector<int>> (
         ptr
      ));  // automatic implementation
   }
};

std::pair<int,int> NSSeqTest::commonState = std::pair<int,int>{};



