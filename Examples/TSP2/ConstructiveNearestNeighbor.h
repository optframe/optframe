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

#ifndef TSP_NN_CONSTRUCTIVE_HPP_
#define TSP_NN_CONSTRUCTIVE_HPP_

#include "../../OptFrame/Constructive.h"
#include "../../OptFrame/Util/TestSolution.hpp"

#include "ProblemInstance.h"

#include "Representation.h"

#include "Evaluator.h"
#include "../../OptFrame/RandGen.hpp"
#include <list>

#include <algorithm>
#include <stdlib.h>

using namespace std;

namespace TSP2
{

class ConstructiveNearestNeighbor: public Constructive<RepTSP>
{
private:
	ProblemInstance& pTSP;
	RandGen& rg;

	static bool compare(const pair<double, int>& p1, const pair<double, int>& p2)
	{
		return p1.first < p2.first;
	}

public:

	ConstructiveNearestNeighbor(ProblemInstance& _pTSP, RandGen& _rg) :
			pTSP(_pTSP), rg(_rg)
	{
	}
	;

	virtual ~ConstructiveNearestNeighbor()
	{
	}

	Solution<RepTSP>& generateSolution()
	{
		//cout << "Generating solution" << endl;
		RepTSP newRep;
		vector<bool> used(pTSP.n, false);

		int first = rg.rand(pTSP.n);

		newRep.push_back(first);
		used[first] = true;
		//cout << "first is " << first << endl;

		while (((int) newRep.size()) < pTSP.n - 1)
		{
			vector<pair<double, int> > candidates;

			for (unsigned i = 0; i < used.size(); i++)
				if (!used[i])
					candidates.push_back(make_pair((pTSP.dist)(i, newRep.at(newRep.size() - 1)), i));

			//cout << "before sort: " << newRep << endl;
			sort(candidates.begin(), candidates.end(), compare);

			newRep.push_back(candidates[0].second);
			used[candidates[0].second] = true;

			//cout << "after sort: " << newRep << endl;
		}

		// add last
		for (unsigned i = 0; i < used.size(); i++)
			if (!used[i])
			{
				newRep.push_back(i);
				used[i] = true;
				break;
			}

		return *new Solution<RepTSP>(newRep);
	}

	void print() const
	{
		cout << "Constructive heuristic nearest neighbor" << endl;
	}

};

}

#endif /*TSP_NN_CONSTRUCTIVE_HPP_*/
