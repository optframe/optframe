#pragma once

#include "MoveModel.hpp"

// abstract class "compatible" with OptFrame

template<class XES, class XEv = int>
class Move
{
public:
   // move id
   virtual id_type mid()
   {
      return 0;
   }

   virtual bool canApply(const XES& se) = 0;

   // need to ask for permission first... no possible "forgiveness" could fix that!
   virtual bool canUndo(const XES& se) = 0;

   virtual bool apply(XES& se) = 0;

   virtual bool undo(XES& se) = 0;

   virtual op<XEv> cost(const XES& se, bool allowEstimated)
   {
      return std::nullopt;
   }

   virtual bool equals(const Move<XES, XEv>& m) const = 0;

   virtual void print() const = 0;
};


