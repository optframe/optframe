#pragma once

#include<iostream>

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

   void print()
   {
      std::cout << "MOVE!" << std::endl;
   }
};

// TODO: any default iterator here?

