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

#ifndef OPTFRAME_GENERAL_CROSSOVER_HPP_
#define OPTFRAME_GENERAL_CROSSOVER_HPP_

#include <vector>

#include "../../Solution.hpp"
#include "../../Evaluation.hpp"

#include "EA.h"

#ifndef _OPTFRAME_DBG_CROSSOVER_
#   ifdef OPTFRAME_DEBUG
#       define _OPTFRAME_DBG_CROSSOVER_ 
#   else
#       define _OPTFRAME_DBG_CROSSOVER_ while(false)
#   endif /* OPTFRAME_DEBUG */
#endif /* _OPTFRAME_DBG_CROSSOVER_ */

namespace optframe
{

template<XESolution XES, XEvaluation XEv = Evaluation<>>
class Crossover: public Component, public EA
{
public:

	virtual ~Crossover()
	{
	}

	virtual pair<S*, S*> cross(const S&, const S&) = 0;

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (Component::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << ":" << EA::family() << ":Crossover";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

};

//temporary fix for the true basic genetic algorithm! I will revisit this in the future to perform a proper naming convention
template<XESolution XES, XEvaluation XEv = Evaluation<>>
class SimpleCrossover {
protected:
	using Individual = S;
    //using Chromossome = R;
    using Fitness = XEv*; //nullptr means there's no evaluation
    using Population = std::vector< pair<Individual, Fitness> >;

public:
	SimpleCrossover() = default;
	virtual ~SimpleCrossover() = default;

	virtual Population cross(const std::vector<Individual*>& parents) = 0;
};


/**********************/
/* CROSSOVER EXAMPLES */
/**********************/

//receives two parents to return offspring with user programmed operator
template<XESolution XES, XEvaluation XEv = Evaluation<>>
class TwoParentsCrossover : public SimpleCrossover<S, XEv> {
protected:
	using Individual = S;
    //using Chromossome = R;
    using Fitness = XEv*; //nullptr means there's no evaluation
    using Population = std::vector< std::pair<Individual, Fitness> >;

public:
	TwoParentsCrossover() = default;
	virtual ~TwoParentsCrossover() = default;

	virtual Population cross(const Individual& parent1, const Individual& parent2) = 0;

	virtual Population cross(const std::vector<Individual*>& parents){
		assert(parents.size() == 2);
		assert(parents[0] != nullptr);
		assert(parents[1] != nullptr);
		return cross(*parents[0], *parents[1]);
	}
};

}

#endif /*OPTFRAME_GENERAL_CROSSOVER_HPP_*/
