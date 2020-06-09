#pragma once

#include "MoveModel.hpp"
#include "Move.hpp"

template<class XES, class MoveModel = uptr<Move<XES>> >
class NSIterator
{
public:

   // returns false if finished
   virtual bool first(const XES&) = 0;

   // returns false if finished
   virtual bool next() = 0;

   // returns true if finished (TODO: we may remove this)
   virtual bool isDone() const = 0; 

   // returns current move, or nothing (if doesn't exist)
   virtual op<MoveModel> current() = 0;

};
