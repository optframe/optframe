// OptFrame - Optimization Framework

// Copyright (C) 2009-2015
// http://optframe.sourceforge.net/
//
// This file is part of the OptFrame optimization framework. This framework
// is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License v3 as published by the
// Free Software Foundation.

// This framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License v3 for more details.

// You should have received a copy of the GNU Lesser General Public License v3
// along with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

#ifndef OPTFRAME_NS_HPP_
#define OPTFRAME_NS_HPP_

#include "Move.hpp"
#include "Solution.hpp"

#include "Component.hpp"
#include "Action.hpp"

#include "GeneralEvaluator.hpp" // included for Neighborhood Exploration

namespace optframe
{

template<XESolution XES, XEvaluation XEv = typename XES::second_type, XESolution XSH = XES>
class NS: public Component
{
public:

	virtual ~NS()
	{
	}

public:

	virtual uptr<Move<XES, XEv, XSH>> randomMove(const XES&) = 0;

	virtual uptr<Move<XES, XEv, XSH>> validRandomMove(const XES& se)
	{
		uptr<Move<XES, XEv, XSH>> moveValid = this->randomMove(se);
		if(moveValid && moveValid->canBeApplied(se))
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
   virtual pair< Move<XES, XEv, XSH>*, op<XEv> > findAny(GeneralEvaluator<XES, XEv, XSH>& gev, XES& se)
   {
      uptr<Move<XES, XEv, XSH>> pm = validRandomMove(se);
      if(!pm)
         return std::make_pair(nullptr, std::nullopt);
      Move<XES, XEv, XSH>& m = *pm;
      op<XEv> mvcost = gev.moveCost(m, se);
      // TODO: will we need 'non-strict' checks here
      if(!mvcost)
         return std::make_pair(nullptr, std::nullopt);
      if(gev.isStrictImprovement(*mvcost))
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
