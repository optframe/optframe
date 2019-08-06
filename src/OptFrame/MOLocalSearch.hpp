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

#ifndef OPTFRAME_MO_LOCAL_SEARCH_HPP_
#define OPTFRAME_MO_LOCAL_SEARCH_HPP_

#include <iostream>
#include <vector>

using namespace std;

#include "Component.hpp"
#include "ComponentBuilder.h"
#include "MultiObjSearch.hpp"
#include "Solution.hpp"
#include "Evaluation.hpp"

#include "Component.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class MOLocalSearch: public Component
{

public:

	MOLocalSearch()
	{
	}

	virtual ~MOLocalSearch()
	{
	}

	// core methods

	// no-optimization
//	Solution<R, ADS>& search(const Solution<R, ADS>& s, Pareto<R, ADS>& pf, double timelimit = 100000000, double target_f = 0)
//	{
//		Solution<R, ADS>& s2 = s.clone();
//		exec(s2, pf, timelimit, target_f);
//		return s2;
//	}
//
//	// optimizated version
//	pair<Solution<R, ADS>&, Evaluation&>& search(const Solution<R, ADS>& s, const Evaluation& e, Pareto<R, ADS>& pf, double timelimit = 100000000, double target_f = 0)
//	{
//		Solution<R, ADS>& s2 = s.clone();
//		Evaluation& e2 = e.clone();
//		exec(s2, e2, pf, timelimit, target_f);
//		return *new pair<Solution<R, ADS>&, Evaluation&>(s2, e2);
//	}

// core methods

    // 1
	virtual void exec(Pareto<R, ADS>& p, Solution<R, ADS>& s, paretoManager<R, ADS>& pManager, MOSC& stopCriteria) = 0;

	// 2
	virtual void exec(Pareto<R, ADS>& p, Solution<R, ADS>& s, MultiEvaluation& mev, paretoManager<R, ADS>& pManager, MOSC& stopCriteria) =0;

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (Component::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << ":MOLocalSearch";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

};

}

#endif /* OPTFRAME_MO_LOCAL_SEARCH_HPP_ */
