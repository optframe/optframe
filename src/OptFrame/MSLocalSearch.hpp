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

#ifndef OPTFRAME_MS_LOCAL_SEARCH_HPP_
#define OPTFRAME_MS_LOCAL_SEARCH_HPP_

#include <iostream>
#include <vector>

using namespace std;

#include "Component.hpp"
#include "ComponentBuilder.h"
#include "MultiObjSearch.hpp"
#include "Solution.hpp"
#include "Evaluation.hpp"
#include "ParetoManager.hpp"

// Local Search defined for "MultiSolution Space" (2^S)

namespace optframe
{

template<XSolution S, XEvaluation XEv, X2ESolution<S, XEv> X2ES>
class MSLocalSearch: public Component
{

public:

	MSLocalSearch()
	{
	}

	virtual ~MSLocalSearch()
	{
	}

	// core methods

// core methods

    // 1
	virtual void msSearchFrom(X2ES& p, pair<S, XEv>& se, const StopCriteria<XEv>& stopCriteria) = 0;

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (Component::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << ":MSLocalSearch";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

};

}

#endif /* OPTFRAME_MS_LOCAL_SEARCH_HPP_ */
