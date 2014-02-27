// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
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

#ifndef OptHS_INITIALSOLUTION_Random_HPP_
#define OptHS_INITIALSOLUTION_Random_HPP_

#include "../../OptFrame/Constructive.h"
#include "../../OptFrame/Util/TestSolution.hpp"

#include "ProblemInstance.h"

#include "Representation.h"
#include "Solution.h"

#include "Evaluator.h"
#include "../../OptFrame/RandGen.hpp"
#include <list>

#include <algorithm>
#include <stdlib.h>

using namespace std;
using namespace optframe;

namespace OptHS
{

class ConstructiveRandom: public Constructive<RepOptHS>
{
private:
	ProblemInstance& p;
	RandGen& rg;

public:

	ConstructiveRandom(ProblemInstance& _p, RandGen& _rg) :
			p(_p), rg(_rg)
	{
	}

	virtual ~ConstructiveRandom()
	{
	}

	SolutionOptHS& generateSolution()
	{
		RepOptHS newRep;

		for(unsigned i=0; i<p.members.size()-1; i++)
			for(unsigned j=i+1; j<p.members.size(); j++)
				newRep.push_back(make_pair(p.members[i], p.members[j]));

		rg.shuffle(newRep); // shuffle elements of newRep

		return *new TestSolution<RepOptHS>(newRep);
	}

};

}

#endif /*OptHS_INITIALSOLUTION_Random_HPP_*/
