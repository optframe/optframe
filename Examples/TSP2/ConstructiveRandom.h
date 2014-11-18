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

#ifndef TSP2_CONSTRUCTIVE_RANDOM_H_
#define TSP2_CONSTRUCTIVE_RANDOM_H_

#include "../../OptFrame/Constructive.h"
#include "../../OptFrame/RandGen.hpp"

#include "ProblemInstance.h"
#include "Representation.h"

using namespace optframe;
using namespace std;

namespace TSP2
{

class ConstructiveRandom: public Constructive<RepTSP>
{
private:
	ProblemInstance& pTSP;
	RandGen& rg;

public:

	ConstructiveRandom(ProblemInstance& _pTSP, RandGen& _rg) :
			pTSP(_pTSP), rg(_rg)
	{
	}

	virtual ~ConstructiveRandom()
	{
	}

	Solution<RepTSP>& generateSolution()
	{
		RepTSP r(pTSP.n);

		for (unsigned int i = 0; i < r.size(); i++)
			r[i] = i;

		rg.shuffle(r); // shuffle elements of r

		return *new Solution<RepTSP>(r);
	}

	void print() const
	{
		cout << "Random Constructive" << endl;
	}

};

}

#endif /*TSP2_CONSTRUCTIVE_RANDOM_H_*/
