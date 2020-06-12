#pragma once

#include <iostream>

#include "NSSeq.hpp"

// ======================
// SMove (Singleton Move)
// stateless...
// ======================

template<class XES>
class SMoveFuncPtrCopy final
{
public:
   void (*fApplyDo)(XES&);
   void (*fApplyUndo)(XES&);

   SMoveFuncPtrCopy()
   {
      // TODO: remove! USED FOR coroutines... try to avoid this!
   }

   SMoveFuncPtrCopy(void (*_fApplyDo)(XES&))
     : fApplyDo{ _fApplyDo }
     , fApplyUndo{ _fApplyDo }
   {
   }

   void print()
   {
      std::cout << "MOVE!" << std::endl;
   }
};

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
