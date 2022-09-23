// OptFrame 4.2 - Optimization Framework
// Copyright (C) 2009-2021 - MIT LICENSE
// https://github.com/optframe/optframe
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef OPTFRAME_NS_HPP_
#define OPTFRAME_NS_HPP_

#include "Move.hpp"
#include "Solution.hpp"

#include "Action.hpp"
#include "Component.hpp"

#include "GeneralEvaluator.hpp" // included for Neighborhood Exploration

namespace optframe {

template<XESolution XES, XEvaluation XEv = typename XES::second_type, XESolution XSH = XES>
class NS : public Component
{
public:
   virtual ~NS()
   {
   }

public:
   // returns 'true' if moves generated from this neighborhood are solution-independent
   // this means that a move generated for solution s1 could also be applied to solution s2
   // default is 'false' for safety precautions. if this is 'false', one should be VERY careful on canBeApplied tests
   virtual bool isSolutionIndependent() const
   {
      return false;
   }

   virtual bool supportsMoveIndependence() const
   {
      return false;
   }

   virtual uptr<Move<XES, XEv, XSH>> randomMove(const XES&) = 0;

   virtual uptr<Move<XES, XEv, XSH>> validRandomMove(const XES& se)
   {
      uptr<Move<XES, XEv, XSH>> moveValid = this->randomMove(se);
      if (moveValid && moveValid->canBeApplied(se))
         return moveValid;
      else
         return nullptr;
   }

   // neighborhood id
   id_type nid()
   {
      return 0;
   }

public:
   // =======================================
   // find section (neighborhood exploration)
   // =======================================
   // findAny: returns any move that strictly improves current solution 'se', according 'gev'
   // RETURNS: pair< uptr<Move<XES, XEv, XSH>>, op<XEv> >
   // default implementation tries method 'validRandomMove' for a *single time* (not iterative)
   // note that 'se' is not const, since moves may need to change it (and revert)
   //   we could have "const_cast" here, or inside "moveCost", but at the moment let's fully respect "const"
   virtual pair<Move<XES, XEv, XSH>*, op<XEv>> findAny(GeneralEvaluator<XES, XEv, XSH>& gev, XES& se)
   {
      uptr<Move<XES, XEv, XSH>> pm = validRandomMove(se);
      if (!pm)
         return std::make_pair(nullptr, std::nullopt);
      Move<XES, XEv, XSH>& m = *pm;
      op<XEv> mvcost = gev.moveCost(m, se);
      // TODO: will we need 'non-strict' checks here
      if (!mvcost)
         return std::make_pair(nullptr, std::nullopt);
      if (gev.isStrictImprovement(*mvcost))
         return std::make_pair(pm.release(), mvcost);
      return std::make_pair(nullptr, std::nullopt);
   }

public:
   static string idComponent()
   {
      stringstream ss;
      ss << Component::idComponent() << ":NS";
      return ss.str();
   }

   std::string toString() const override
   {
      return id();
   }

   virtual string id() const
   {
      return idComponent();
   }

   virtual bool compatible(string s)
   {
      return (s == idComponent()) || (Component::compatible(s));
   }
};

}

#endif /*OPTFRAME_NS_HPP_*/
