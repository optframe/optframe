#pragma once

#include <iostream>
#include <functional>

#include "NSSeq.hpp"

// ======================
// SMove (Singleton Move)
// stateless...
// ======================

template<class XES>
class MoveFPtr final
{
public:
   // no need for a method:  void apply(XES& se)
   void (*apply)(XES&);
   void (*undo)(XES&);

/*
   MoveFPtr() = delete;
   MoveFPtr(const MoveFPtr&) = delete;
   // Move semantics is allowed
   MoveFPtr(MoveFPtr&& m) noexcept :
      apply{m.apply},
      undo{m.undo}
   {
   }
*/

   // 'consteval' may cause some strange error with 'fcoroutines'
   //consteval 
   //
   MoveFPtr(void (*_apply)(XES&)) noexcept
     : apply{ _apply }
     , undo{ _apply }
   {
   }

/*
   consteval MoveFPtr(const MoveFPtr& mv) noexcept
     : apply{ mv.apply }
     , undo{ mv.apply }
   {
   }

   MoveFPtr(MoveFPtr&& mv) noexcept
     : apply{ mv.apply }
     , undo{ mv.apply }
   {
   }
*/

   MoveFPtr<XES>* operator->()
   {
      return this;
   }

   void print()
   {
      std::cout << "MOVE!" << std::endl;
   }
};

// std::vector<int>
static_assert(IsMove<MoveFPtr<std::vector<int>>, std::vector<int>>);

// ---------


template<class XES>
class MoveFRef final
{
public:
   std::function<void(XES&)>& apply;
   std::function<void(XES&)>& undo;

   consteval 
   //
   MoveFRef(std::function<void(XES&)>& _apply) noexcept
     : apply{ _apply }
     , undo{ _apply }
   {
   }


   MoveFPtr<XES>* operator->()
   {
      return this;
   }

   void print()
   {
      std::cout << "MOVE!" << std::endl;
   }
};

// std::vector<int>
static_assert(IsMove<MoveFRef<std::vector<int>>, std::vector<int>>);

// ============


template<class XES>
class MoveFCopy final
{
public:
   std::function<void(XES&)> apply;
   std::function<void(XES&)> undo;

   //consteval 
   //
   MoveFCopy(const std::function<void(XES&)>& _apply) noexcept
     : apply{ _apply }
     , undo{ _apply }
   {
   }


   MoveFPtr<XES>* operator->()
   {
      return this;
   }

   void print()
   {
      std::cout << "MOVE!" << std::endl;
   }
};

// std::vector<int>
static_assert(IsMove<MoveFCopy<std::vector<int>>, std::vector<int>>);

// ===========

#include "tl/function_ref.hpp"

template<class XES>
class MoveFTL final
{
public:
   tl::function_ref<void(XES&)> apply;
   tl::function_ref<void(XES&)> undo;

   //consteval 
   //
   MoveFTL(const tl::function_ref<void(XES&)> _apply) noexcept
     : apply{ _apply }
     , undo{ _apply }
   {
   }


   MoveFTL<XES>* operator->()
   {
      return this;
   }

   void print()
   {
      std::cout << "MOVE!" << std::endl;
   }
};

// std::vector<int>
static_assert(IsMove<MoveFTL<std::vector<int>>, std::vector<int>>);

// ---------

template<class XES, class MoveModel>
class SNSIteratorFuncPtrCopy final //: public NSIterator<XES, MoveModel>
{
public:
   bool (*fFirst)(const XES&);
   bool (*fNext)();
   bool (*fIsDone)();
   op<MoveModel> (*fCurrent)();

   SNSIteratorFuncPtrCopy(
     bool (*_fFirst)(const XES&),
     bool (*_fNext)(),
     bool (*_fIsDone)(),
     op<MoveModel> (*_fCurrent)())
     : fFirst{ _fFirst }
     , fNext{ _fNext }
     , fIsDone{ _fIsDone }
     , fCurrent{ _fCurrent }
   {
   }

   bool first(const XES& se) //override
   {
      return fFirst(se);
   }

   bool next() //override
   {
      return fNext();
   }

   bool isDone() const //override
   {
      return fIsDone();
   }

   op<MoveModel> current() //override
   {
      return fCurrent();
   }
};

// =====================
// TMove: stateful move
// no need for singleton
// =====================

template<class XES>
class TMoveStdFunc final
{
public:
   std::function<void(XES&)> fApplyDoUndo;

   TMoveStdFunc(std::function<void(XES&)> _fApplyDoUndo) noexcept
     : fApplyDoUndo{ _fApplyDoUndo }
   {
   }
};

// TODO: any default iterator here?
