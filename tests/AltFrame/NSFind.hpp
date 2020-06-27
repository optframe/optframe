#pragma once

#include "NS.hpp"

template<class XES, class MoveModel = uptr<Move<XES>> >
class NSFind : public NS<XES, MoveModel>
{
public:
   // override
   //virtual op<MoveModel> randomMove(const XES&) = 0;

   virtual op<MoveModel> findBest(const XES&) { /* whatever */ } // = 0;

   virtual op<MoveModel> findImproving(const XES&) { /* whatever */ } // = 0;

};
