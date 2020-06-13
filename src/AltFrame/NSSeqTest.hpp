#pragma once

#include "NSSeqBase.hpp"

#include <OptFCore/coro/Generator.hpp>

// my test
// vector<int>
// swap idea
// =================

class NSSeqTestStateless : public NSSeq<std::vector<int>, MoveFPtr<std::vector<int>>, SNSIteratorFuncPtrCopy<std::vector<int>, MoveFPtr<std::vector<int>>>>
{
public:
   using MyMove = MoveFPtr<std::vector<int>>;

public:
   int nTSP{ -1 };

   static int st_nTSP;

   NSSeqTestStateless(int nTSP)
     : nTSP{ nTSP }, gen{make_gen()}
   {
      // keep static problem data
      st_nTSP = nTSP;
      commonState = std::make_pair(0,1);
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
   static std::pair<int, int> commonState;

   /*
   // must find solution to this!
   virtual MoveFuncPtrCopy<std::vector<int>> getStateMove () {
      void(*ptr)(std::vector<int>&) = this->fApplyDo;
      return MoveFuncPtrCopy<std::vector<int>> (
         ptr
      );  // automatic implementation
   }
*/

   virtual uptr<NSIterator<std::vector<int>, MyMove>> getIterator(const std::vector<int>&);

   virtual SNSIteratorFuncPtrCopy<std::vector<int>, MyMove> getIteratorCopy(const std::vector<int>&)
   {
      return SNSIteratorFuncPtrCopy<std::vector<int>, MyMove>{
         fFirst,
         fNext,
         fIsDone,
         fCurrent
      };
   }

   // ============ COROUTINE APPROACH ============

   // for coroutines
   //op<optframe::Generator<MyMove>> op_gen;
   //
   //op<optframe::Generator<std::pair<int,int>>> op_gen;
   optframe::Generator<std::pair<int,int>> gen;
   //
   bool done;
   //

   //optframe::Generator<MyMove> make_gen(const std::vector<int>& v)
   optframe::Generator<std::pair<int,int>> make_gen()
   {
      //void (*ptr)(std::vector<int>&) = fApplyDo;
      //
      for (int i = 0; i < nTSP - 1; i++)
         for (int j = i + 1; j < nTSP; j++)
            //co_yield MoveFPtr<std::vector<int>>{ ptr };
            co_yield std::make_pair(i,j);
   }

   bool coroFirst(const std::vector<int>& v)
   {
      //std::cout << "FIRST!" << std::endl;
      gen = make_gen();
      done = !gen.next(); // advance and update bool
      return done;
   }

   bool coroNext()
   {
      done = !gen.next(); // advance and update bool
      return done;
   }

   bool coroIsDone()
   {
      return done;
   }

   ///static constexpr MoveFPtr<std::vector<int>> mv_coro{ fApplyDo };
      

   op<MyMove> coroCurrent()
   {
      //void (*ptr)(std::vector<int>&) = fApplyDo;
      //MoveFPtr<std::vector<int>> mv{ ptr };
      MoveFPtr<std::vector<int>> mv{ fApplyDo };
      commonState = gen.getValue();
      //return op<MyMove>{ std::move(mv) };
      return op<MyMove>{ mv };

      //return op<MyMove>(
      //  op_gen->getValue()); // automatic implementation
   }

   // ==================== REF STATIC ITER ================

   // missing '=' here breaks gcc...
   std::function<void(std::vector<int>&)> funcApply = [](std::vector<int>& v) -> void {
                        int& i = commonState.first;
                        int& j = commonState.second;
                        // swap
                        int aux = v[i];
                        v[i] = v[j];
                        v[j] = aux;
                     };

   /*
   g++-10.1 -g AltFrame.bench.cpp -std=c++20 -O3 -Wall -fcoroutines -fno-exceptions -fno-rtti -fno-omit-frame-pointer  -I../src  -isystem ../libs/gbenchmark/include -L../libs/gbenchmark/build/src -o build/AltFrame_bench -lbenchmark  -lpthread
]In file included from ./AltFrame-bench/NSTest.bench.cpp:7,
                 from AltFrame.bench.cpp:4:
../src/AltFrame/NSSeqTest.hpp:153:22: internal compiler error: in splice_late_return_type, at cp/pt.c:29095
  153 |                      };
      |                      ^
0x7facbb6f5a86 __libc_start_main
	../csu/libc-start.c:310
Please submit a full bug report,
with preprocessed source if appropriate.
Please include the complete backtrace with any bug report.
See <https://gcc.gnu.org/bugs/> for instructions.
*/

   static op<MoveFRef<std::vector<int>>> fCurrentRef()
   {
      auto myfuncDo = [](std::vector<int>& v) -> void {
                        int& i = commonState.first;
                        int& j = commonState.second;
                        // swap
                        int aux = v[i];
                        v[i] = v[j];
                        v[j] = aux;
                     };
      //std::function<void(std::vector<int>&)> fX { myfuncDo };
      return op<MoveFRef<std::vector<int>>>(
         //
        MoveFRef<std::vector<int>>(funcApply)
        // 
        ); 
   }

   // ==================== STATIC ITERATOR ====================

   static bool fFirst(const std::vector<int>&)
   {
      commonState.first = 0;
      commonState.second = 1;
      return true;
   }

   static bool fNext()
   {
      commonState.second++;
      if (commonState.second >= (st_nTSP - 1)) {
         commonState.first++;
         commonState.second = commonState.first + 1;
      }
      return commonState.first < st_nTSP;
   }

   static bool fIsDone()
   {
      return commonState.first >= (st_nTSP - 1);
   }

   static op<MyMove> fCurrent()
   {
      //void (*ptr)(std::vector<int>&) = fApplyDo;
      //
      return op<MyMove>(
        //MoveFPtr<std::vector<int>>(ptr)
        MoveFPtr<std::vector<int>>(fApplyDo)
        // automatic implementation
        ); 
   }

   // ==================== EMBEDDED ITERATOR

   // returns false if finished
   virtual bool first(const std::vector<int>&)
   {
      commonState.first = 0;
      commonState.second = 1;
      /* whatever */
      return true;
   } //= 0;

   // returns false if finished
   virtual bool next()
   {
      commonState.second++;
      if (commonState.second >= (nTSP - 1)) {
         commonState.first++;
         commonState.second = commonState.first + 1;
      }
      return commonState.first < nTSP;
   } // = 0;

   // returns true if finished (TODO: we may remove this)
   virtual bool isDone() const
   {
      return commonState.first >= (nTSP - 1);
      /* whatever */
   } // = 0;

   // returns current move, or nothing (if doesn't exist)
   //virtual op< MoveFuncPtrCopy<std::vector<int>> > current(){ /* whatever */ } // = 0;
   virtual op<MoveFPtr<std::vector<int>>> current()
   {
      //void (*ptr)(std::vector<int>&) = this->fApplyDo;
      //
      return op<MoveFPtr<std::vector<int>>>(
        //MoveFPtr<std::vector<int>>(ptr)
        MoveFPtr<std::vector<int>>(fApplyDo)
          // automatic implementation
          ); 
   }
};

int NSSeqTestStateless::st_nTSP = 0;

std::pair<int, int> NSSeqTestStateless::commonState = std::pair<int, int>{0,1};

class MyIter : public NSIterator<std::vector<int>, NSSeqTestStateless::MyMove>
{
public:
   int nTSP{ -1 };

   MyIter(int _nTSP)
     : nTSP{ _nTSP }
   {
   }

   // returns false if finished
   virtual bool first(const std::vector<int>&)
   {
      NSSeqTestStateless::commonState.first = 0;
      NSSeqTestStateless::commonState.second = 1;
      /* whatever */
      return true;
   } //= 0;

   // returns false if finished
   virtual bool next()
   {
      NSSeqTestStateless::commonState.second++;
      if (NSSeqTestStateless::commonState.second >= (nTSP - 1)) {
         NSSeqTestStateless::commonState.first++;
         NSSeqTestStateless::commonState.second = NSSeqTestStateless::commonState.first + 1;
      }
      return NSSeqTestStateless::commonState.first < nTSP;
   } // = 0;

   // returns true if finished (TODO: we may remove this)
   virtual bool isDone() const
   {
      return NSSeqTestStateless::commonState.first >= (nTSP - 1);
      /* whatever */
   } // = 0;

   // returns current move, or nothing (if doesn't exist)
   //virtual op< MoveFuncPtrCopy<std::vector<int>> > current(){ /* whatever */ } // = 0;
   virtual op<MoveFPtr<std::vector<int>>> current()
   {
      //void (*ptr)(std::vector<int>&) = NSSeqTestStateless::fApplyDo;
      //
      return op<MoveFPtr<std::vector<int>>>(
         //MoveFPtr<std::vector<int>>(ptr)
         MoveFPtr<std::vector<int>>(NSSeqTestStateless::fApplyDo)
         // automatic implementation
          ); 
   }
};

uptr<NSIterator<std::vector<int>, NSSeqTestStateless::MyMove>>
NSSeqTestStateless::getIterator(const std::vector<int>&)
{
   return uptr<NSIterator<std::vector<int>, NSSeqTestStateless::MyMove>>(new MyIter{ this->nTSP });
}