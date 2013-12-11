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

/*
 * ParetoDominance.hpp
 *
 * LGPLv3
 *
 */

#ifndef OPTFRAME_PARETODOMINANCE_HPP_
#define OPTFRAME_PARETODOMINANCE_HPP_

#include "Solution.hpp"
#include "Evaluation.hpp"
#include "Move.hpp"

#include <iostream>
#include <cmath>

using namespace std;

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ParetoDominance
{
protected:
	vector<Evaluator<R, ADS, DS>*> v_e;

public:

	ParetoDominance(vector<Evaluator<R, ADS, DS>*> _v_e) :
		v_e(_v_e)
	{

	}

	virtual ~ParetoDominance()
	{
	}

	void insertEvaluators(vector<Evaluator<R, ADS, DS>*> _v_e)
	{
		v_e = _v_e;
	}

	// true if 's1' dominates 's2'
	virtual bool dominates(const Solution<R, ADS>& s1, const Solution<R, ADS>& s2)
	{
		vector<Evaluation<DS>*> v1;
		vector<Evaluation<DS>*> v2;

		for (int e = 0; e < v_e.size(); e++)
		{
			v1.push_back(&v_e[e]->evaluate(s1));
			v2.push_back(&v_e[e]->evaluate(s2));
		}

		bool r = dominates(v1, v2);

		for (int e = 0; e < v_e.size(); e++)
		{
			delete v1[e];
			delete v2[e];
		}

		return r;
	}

	// true if 's1' dominates 's2'
	virtual bool dominates(const vector<Evaluation<DS>*> v1, const vector<Evaluation<DS>*> v2)
	{
		if (!((v_e.size() == v1.size()) && (v1.size() == v2.size())))
		{
			cout << "WARNING in ParetoDominance: different sizes." << endl;
			return false;
		}

		int better = 0;
		int equals = 0;

		for (int e = 0; e < v1.size(); e++)
		{
			if (v_e[e]->betterThan(*v1[e], *v2[e]))
				better++;

			if (abs(v1[e]->evaluation() - v2[e]->evaluation()) < 0.0001)
				equals++;
		}

		return ((better + equals == v_e.size()) && (better > 0));
	}

};

}

#endif /*OPTFRAME_PARETODOMINANCE_HPP_*/
