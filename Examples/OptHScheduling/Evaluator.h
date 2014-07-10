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

#ifndef OptHS_EVALUATOR_HPP_
#define OptHS_EVALUATOR_HPP_

//#include <cmath>
#include <stdlib.h>
#include <math.h>

#include "../../OptFrame/Evaluation.hpp"
#include "../../OptFrame/Evaluator.hpp"

#include "Representation.h"
#include "Memory.h"
#include "Solution.h"
#include "Evaluation.h"

#include "ProblemInstance.h"

#define OptHS_EPSILON 0.0001

using namespace optframe;

namespace OptHS
{

class OptHSEvaluator: public Evaluator<RepOptHS, OPTFRAME_DEFAULT_ADS, MemOptHS>
{
private:
	ProblemInstance& p;

public:

	using Evaluator<RepOptHS, OPTFRAME_DEFAULT_ADS, MemOptHS>::evaluate; // prevents name hiding

	OptHSEvaluator(ProblemInstance& _p) :
			p(_p)
	{
	}

	Evaluation<MemOptHS>& evaluate(const RepOptHS& r)
	{
		int fo_inv = 0; // Infeasible Value

		vector<int> lastClean(p.members.size(), -1);
		//cout << "MEMBERS: " << p.members << endl;

		// first MUST be 'Y' and 'M'
		if (r.size() > 3)
		{
			if ((r[0].first != 'Y') || (r[0].second != 'Z'))
				fo_inv += 1000;
			if ((r[1].first != 'E') || (r[1].second != 'J'))
				fo_inv += 1000;
			if ((r[2].first != 'M') || (r[2].second != 'B'))
				fo_inv += 1000;

		}

		for (unsigned i = 0; i < r.size(); i++)
		{
			//cout << "ITER " << i << ": " << lastClean << endl;
			for (unsigned m = 0; m < p.members.size(); m++)
			{
				if ((r[i].first == p.members[m]) || (r[i].second == p.members[m]))
				{
					if (lastClean[m] > -1)
					{
						int diff = i - lastClean[m];

						if (diff < 2)
						{
							fo_inv += diff;
							//cout << "INF for member: " << p.members[m] << " (at week " << i << ")" << endl;
						}
					}

					lastClean[m] = i;
				}
			}
			//cout << "ITER (FINISH)" << i << ": " << lastClean << endl;
		}

		MemOptHS mem = 0;

		return *new Evaluation<MemOptHS>(0, fo_inv, mem);
	}


	virtual bool betterThan(double f1, double f2)
	{
		// MINIMIZATION
		return (f1 < (f2 - OptHS_EPSILON));
	}

	virtual bool isMinimization() const
	{
		return true;
	}


	virtual string id() const
	{
		string pai = Evaluator<RepOptHS, OPTFRAME_DEFAULT_ADS, MemOptHS>::idComponent();
		pai.append(":OptHSEvaluator");
		return pai;
	}

};

}

#endif /*OptHS_EVALUATOR_HPP_*/
