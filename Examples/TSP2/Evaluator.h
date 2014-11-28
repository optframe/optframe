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

#ifndef TSP2_EVALUATOR_HPP_
#define TSP2_EVALUATOR_HPP_

//#include <cmath>
#include <stdlib.h>
#include <math.h>

#include "../../OptFrame/Evaluation.hpp"
#include "../../OptFrame/Evaluator.hpp"

#include "Representation.h"
#include "Evaluation.h"

#include "ProblemInstance.h"

#define TSP_EPSILON 0.0001

namespace TSP2
{

class MyEvaluator: public Evaluator<RepTSP>
{
private:
	ProblemInstance& pTSP;

public:

	long long evaluations;

	using Evaluator<RepTSP>::evaluate; // prevents name hiding

	MyEvaluator(ProblemInstance& _pTSP) :
			pTSP(_pTSP)
	{
		evaluations = 0;
	}

	Evaluation<>& evaluate(const RepTSP& r)
	{
		evaluations++;
		double fo = 0; // Evaluation Function Value

		for (int i = 0; i < ((int) r.size()) - 1; i++)
		{
			int j = r.at(i);
			int z = r.at(i + 1);

			double val = pTSP.dist(j, z);
			fo += val;
		}

		int k = r.at(((int) r.size()) - 1);
		int l = r.at(0);

		double val = pTSP.dist(k, l);
		fo += val;

		return *new Evaluation<>(fo);
	}

	virtual bool betterThan(double f1, double f2)
	{
		return (f1 < (f2 - TSP_EPSILON));
	}

	virtual bool isMinimization() const
	{
		return true;
	}

	virtual string id() const
	{
		return "OptFrame:Evaluator:MyEvaluator";
	}

	void print() const
	{
		cout << "TSP evaluation function" << endl;
	}

};

}

#endif /*TSP2_EVALUATOR_HPP_*/
