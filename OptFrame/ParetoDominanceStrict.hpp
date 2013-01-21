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
 * ParetoDominanceStrict.hpp
 *
 * LGPLv3
 *
 */

#ifndef OPTFRAME_PARETODOMINANCE_STRICT_HPP_
#define OPTFRAME_PARETODOMINANCE_STRICT_HPP_

#include "ParetoDominance.hpp"

#include <iostream>


using namespace std;

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_EMEMORY>
class ParetoDominanceStrict: public ParetoDominance<R, ADS, DS>
{
public:

        using ParetoDominance<R, ADS, DS>::dominates;

	ParetoDominanceStrict(vector<Evaluator<R, ADS, DS>*> _v_e) :
		ParetoDominance<R, ADS, DS> (_v_e)
	{

	}

	ParetoDominanceStrict()
	{

	}

	virtual ~ParetoDominanceStrict()
	{
	}

	void insertEvaluators(vector<Evaluator<R, ADS, DS>*> _v_e)
	{
		ParetoDominance<R, ADS, DS>::v_e = _v_e;
	}

	// true if 's1' weakly dominates 's2'
	virtual bool dominates(const vector<Evaluation<DS>*> v1, const vector<Evaluation<DS>*> v2)
	{
		vector<Evaluator<R, ADS, DS>*>& v_e = ParetoDominance<R, ADS, DS>::v_e;

		if (!((v_e.size() == v1.size()) && (v1.size() == v2.size())))
		{
			cout << "WARNING in ParetoDominanceStrict: different sizes." << endl;
			return false;
		}

		int better = 0;
		//int equals = 0;

		for (int e = 0; e < v1.size(); e++)
		{
			if (v_e[e]->betterThan(*v1[e], *v2[e]))
				better++;

		}

		return (better == v_e.size());
	}

};

#endif /*OPTFRAME_PARETODOMINANCE_STRICT_HPP_*/
