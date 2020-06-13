#pragma once

#include <iostream>

#include "NSSeq.hpp"

// ======================
// SMove (Singleton Move)
// stateless...
// ======================

template<class XES>
class MovePtr final
{
public:
   // no need for a method:  void apply(XES& se)
   void (*apply)(XES&);
   void (*undo)(XES&);

/*
   MovePtr() = delete;
   MovePtr(const MovePtr&) = delete;
   // Move semantics is allowed
   MovePtr(MovePtr&& m) noexcept :
      apply{m.apply},
      undo{m.undo}
   {
   }
*/

   MovePtr(void (*_apply)(XES&))
     : apply{ _apply }
     , undo{ _apply }
   {
   }

   MovePtr<XES>* operator->()
   {
      return this;
   }

   void print()
   {
      std::cout << "MOVE!" << std::endl;
   }
};

// std::vector<int>
static_assert(IsMove<MovePtr<std::vector<int>>, std::vector<int>>);

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
