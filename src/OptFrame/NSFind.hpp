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

#ifndef OPTFRAME_NSFIND_HPP_
#define OPTFRAME_NSFIND_HPP_

#include "Move.hpp"
#include "Solution.hpp"

#include "Component.hpp"
#include "Action.hpp"

#include "GeneralEvaluator.hpp" // included for Neighborhood Exploration

#include "NS.hpp"

namespace optframe
{

template<XESolution XES, XEvaluation XEv = Evaluation<>, XESolution XSH = XES>
class NSFind: public NS<XES, XEv, XSH>
{
public:

	virtual ~NSFind()
	{
	}

public:

   // =======================================
   // find section (neighborhood exploration)
   // =======================================
   // findBest: returns move that greatly improves current solution 'se', according 'gev'
   // RETURNS: pair< uptr<Move<XES, XEv, XSH>>, op<XEv> >
   // NSFind is useful for exponential-sized neighborhoods, without requiring any iterator structure
   virtual pair< Move<XES, XEv, XSH>*, op<XEv> > findBest(GeneralEvaluator<XES>& gev, XES& se) = 0;


public:
	static string idComponent()
	{
		stringstream ss;
      ss << NS<XES, XEv>::idComponent() << ":NSFind";
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

#endif /*OPTFRAME_NSFIND_HPP_*/
