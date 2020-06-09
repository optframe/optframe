#pragma once

#include "MoveModel.hpp"
#include "Move.hpp"

template<class XES, class MoveModel = uptr<Move<XES>> >
class NS
{
public:
   // may not have any move
   virtual op<MoveModel> randomMove(const XES&)  { /* whatever */ } //= 0;


};
