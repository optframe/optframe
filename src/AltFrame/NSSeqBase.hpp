#pragma once

#include "NSSeq.hpp"


template<class XES>
class MoveFuncPtrCopy final
{
public:
   void(*fApplyDo)(XES&); 

   MoveFuncPtrCopy(void(*_fApplyDo)(XES&))
     : fApplyDo{_fApplyDo}
   {
   }
};

// TODO: any default iterator here?

